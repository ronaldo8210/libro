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
  // wait_queue和runnable_queue共用一把锁
  // 因为协程任务挂起的时候需要操作runnable_queue+wait_queue，时间轮timerloop线程唤醒挂起的协程时也要
  // 操作wait_queue+runnable_queue
  wait_queue_.set_lock(&runnable_queue_.lock_ref());  
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
    while (runnable_task_ && !scheduler_->is_stop()) {

      // 正在执行的协程可能是从new_queue中转移来的，也可能是从wait_queue中转移来的
      /*
      if (runnable_task_->state_ == TaskState::init || runnable_task_->state_ == TaskState::block) {
        runnable_task_->state_ = TaskState::runnable;
        runnable_task_->proc_ = this;
      } */

      runnable_task_->state_ = TaskState::runnable;
      runnable_task_->proc_ = this;

      ++switch_count_;
      runnable_task_->swap_in();

      switch (runnable_task_->state_) {
        case TaskState::runnable:
          // 主动执行yield让出cpu的协程在runnable_queue中的位置不动，协程函数执行完毕后才从runnable_queue中移出
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
          // 广告系统业务中，一个协程负责访问DMP服务、索引服务、算法服务
          // 相当于执行三组阻塞式的{write+read} IO
          // 每次write/read都要挂起协程，协程在runnable_queue和wait_queue间不断转移，直到状态为done
          {
            std::unique_lock<TS_TaskQueue::lock_t> lock(runnable_queue_.lock_ref());
            // 挂起的协程已经移入wait_queue，直接执行下一个可执行的协程
            runnable_task_ = next_task_;
            next_task_ = nullptr;
          }
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

Processor::SuspendEntry Processor::suspend() {
  Task *task = get_current_task();
  assert(task);
  assert(task->proc_);
  return task->proc_->suspend_by_coroutine_self(task);
}

Processor::SuspendEntry Processor::suspend(FastSteadyClock::duration dur) {
  SuspendEntry entry = suspend();
  get_current_scheduler()->timer().start_timer(dur, 
          [entry] () {
              Processor::wakeup(entry);
          });
  return entry;
}

Processor::SuspendEntry Processor::suspend(FastSteadyClock::time_point tp) {
  SuspendEntry entry = suspend();
  get_current_scheduler()->timer().start_timer(tp,
          [entry] () {
              Processor::wakeup(entry);
          });
  return entry;
}

Processor::SuspendEntry Processor::suspend_by_coroutine_self(Task *task) {
  assert(task == runnable_task_);
  assert(task->state_ == TaskState::runnable);

  task->state_ = TaskState::block;
  uint64_t id = 0;  // TODO

  std::unique_lock<TS_TaskQueue::lock_t> lock(runnable_queue_.lock_ref());
  runnable_queue_.next_without_lock(runnable_task_, next_task_);
  runnable_queue_.erase_without_lock(runnable_task_);
  wait_queue_.push_without_lock(runnable_task_);

  return {std::shared_ptr<Task>(task), id};
}

void Processor::wakeup(const SuspendEntry &entry, const std::function<void()> &functor) {
  std::shared_ptr<Task> tkPtr = entry.tkPtr_;
  auto proc = tkPtr->proc_;
  proc->wakeup_by_timer(tkPtr, entry.id_, functor);
}

void Processor::wakeup_by_timer(std::shared_ptr<Task> taskPtr, uint64_t id, const std::function<void()> &functor) {
  Task *task = taskPtr.get();

  if (functor) {
    functor();
  }

  std::unique_lock<TS_TaskQueue::lock_t> lock(wait_queue_.lock_ref());
  wait_queue_.erase_without_lock(task);
  runnable_queue_.push_without_lock(task);
}

}  // namespace co
