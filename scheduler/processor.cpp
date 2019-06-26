/***********************************************************************
#   > File Name   : processor.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 21:41:50
***********************************************************************/
#include "processor.hpp"

namespace co {

Processor::Processor(Scheduler *scheduler, int id) : scheduler_(scheduler), id_(id) {
    
}

void Process* & Processor::get_current_process() {
  static thread_local Processor* proc = nullptr;
  return proc;
}

void Scheduler* Processor::get_current_scheduler() {
  auto proc = get_current_process();
  if (proc) {
    return proc->scheduler_;  
  } else {
    return nullptr;
  }
}

void Processor::static_co_yield() {
  auto proc = get_current_process();
  proc->co_yield();
}

void Processor::co_yield() {
  auto task;
  task->swap_out();
}

// 调用add_task的是生产者线程
void Processor::add_task(Task *task) {
  std::unique_lock<> lock(new_queue_.lock_ref());
  task->state_ = TaskState::init;
  new_queue_.push_without_lock(task);

  // 如果执行process的线程在等待协程任务，则唤醒它
  if (waiting_) {
    cv_.notify_all();
  }
}

// 调用process的是消费者线程
void Processor::process() {
  get_current_process() = this;

  while (!scheduler_->stop()) {
    runnable_queue_.front(runnable_task_);
    if (!runnable_task_) {
      if (add_new_queue()) {
        continue;  
      } else {
        wait_condition();
        add_new_queue();
        continue;
      }
    }

    // 此循环将从头至尾遍历runnable_queue并依次执行协程
    while (runnable_task_ && !scheduler_->stop()) {
      if (task->state_ == TaskState::init) {
        task->state_ = TaskState::runnable;
        task->proc_ = this;
      }

      ++switch_cout_;
      runnable_task_->swap_in();

      switch (runnable_task_->state_) {
        case TaskState::runnable:
          auto next = runnable_task_->next_;
          runnable_task_ = next_;
          break;
        case TaskState::wait:
          break;
        case TaskState::done:
        default:
          break;
      }

    }  // end while
  }  // end while
}

// 等待有新协程任务加入到new_queue中
void Processor::wait_condition() {
  // 无协程可执行时做一些垃圾回收工作
  gc();
  std::unique_lock<> lock(new_queue_.lock_ref());
  waiting_ = true;
  cv_.wait();
  // 线程被唤醒后重新持有锁
  waiting = false;
  // 函数结束后锁被释放（RAII）
}

}  // namespace co
