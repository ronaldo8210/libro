/***********************************************************************
#   > File Name   : processor.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 21:41:50
***********************************************************************/
#include "processor.hpp"
#include "scheduler.hpp"

namespace co {

Processor::Processor(Scheduler *scheduler, int id) : scheduler_(scheduler), id_(id) {
    
}

Processor* & Processor::get_current_processor() {
  static thread_local Processor* proc = nullptr;
  return proc;
}

Scheduler* Processor::get_current_scheduler() {
  auto proc = get_current_processor();
  if (proc) {
    return proc->scheduler_;  
  } else {
    return nullptr;
  }
}

Task* Processor::get_current_task() {
  auto proc = get_current_processor();
  if (proc) {
    return proc->runnable_task_;
  } else {
    return nullptr;  
  }
}

// 调用add_new_task的是生产者线程
void Processor::add_new_task(Task *task) {
  std::unique_lock<TS_TaskQueue::lock_t> lock(new_queue_.lock_ref());
  //new_queue_.push_without_lock(task);
  runnable_queue_.push_without_lock(task);  // 临时测试

  // 如果执行process的线程在等待协程任务，则唤醒它
  if (waiting_) {
    cv_.notify_all();
  }
}

bool Processor::move_new_queue() {
  return true;
}

// 调用process的是消费者线程
void Processor::process() {
  get_current_processor() = this;

  while (!scheduler_->is_stop()) {
    runnable_queue_.front(runnable_task_);
    if (!runnable_task_) {
      // runnable_queue为空，尝试将new_queue的所有协程移入runnable_queue
      // 移入操作只是移动指针，没有开销
      if (move_new_queue()) {
        continue;  
      } else {
        // 线程挂起，等待有新协程加入new_queue
        wait_condition();
        move_new_queue();
        continue;
      }
    }

    // 此循环将从头至尾遍历当前的runnable_queue并依次执行协程
    // 主动执行yield让出cpu的协程在runnable_queue中的位置不动，协程函数执行完毕后才从runnable_queue中移出
    while (runnable_task_ && !scheduler_->is_stop()) {
      if (runnable_task_->state_ == TaskState::init) {
        runnable_task_->state_ = TaskState::runnable;
        runnable_task_->proc_ = this;
      }

      ++switch_count_;
      runnable_task_->swap_in();

      switch (runnable_task_->state_) {
        case TaskState::runnable:
          {
            std::unique_lock<TS_TaskQueue::lock_t> lock(runnable_queue_.lock_ref());
            auto next = (Task*)runnable_task_->next_;
            if (next) {
              runnable_task_ = next;
            } else {
              runnable_task_ = nullptr;
            }
          }
          break;
        case TaskState::block:
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
  std::unique_lock<TS_TaskQueue::lock_t> lock(new_queue_.lock_ref());
  waiting_ = true;
  cv_.wait(lock);
  // 线程被唤醒后重新持有锁
  waiting_ = false;
  // 函数结束后锁被释放（RAII）
}

void Processor::notify_condition() {

}

bool Processor::gc() {
  return true;  
}

}  // namespace co
