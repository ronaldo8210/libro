/***********************************************************************
#   > File Name   : processor.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 16:02:29
***********************************************************************/
#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include "../common/clock.hpp"
#include "../common/ts_queue.hpp"
#include "../task/task.hpp"

namespace co {

class Scheduler;

class Processor {
 friend class Scheduler;

 public:
  static Processor* & get_current_processor();

  static Scheduler* get_current_scheduler();

  static Task* get_current_task();

  // 正在执行的协程主动让出cpu
  // 频繁调用，置为inline类型
  inline static void static_co_yield();

  struct SuspendEntry {
    std::shared_ptr<Task> tkPtr_;
    uint64_t id_;
    
    bool operator==(const SuspendEntry &rhs) {
      return /* tkPtr_ == rhs.tkPtr_ && */ id_ == rhs.id_;
    }

    bool is_expire() const {
      return Processor::is_expire(*this);
    }
  };

  // 一般在协程任务内部调用，挂起当前正在执行的协程
  // 仅是将协程任务从runnable_queue移到wait_queue
  static SuspendEntry suspend();

  static SuspendEntry suspend(FastSteadyClock::duration dur);

  static SuspendEntry suspend(FastSteadyClock::time_point tp);

  // 由时间轮timerloop线程调用
  // 将协程任务从wait_queue重新压入runnable_queue
  static void wakeup(const SuspendEntry& entry, const std::function<void()> &functor = nullptr);

  static bool is_expire(const SuspendEntry &entry);

 private:
  // 将新增协程任务加入new_queue
  void add_new_task(Task *task);

  // 将new_queue中的协程全部移入runnable_queue
  bool move_new_queue();

  bool gc();

  // 频繁调用，置为inline类型
  inline void co_yield();

  // 用于添加协程任务线程与处理协程任务线程的同步
  void wait_condition();

  void notify_condition();

  SuspendEntry suspend_by_coroutine_self(Task *task);

  void wakeup_by_self(std::shared_ptr<Task> taskPtr, uint64_t id, const std::function<void()> &functor);
      
  // 线程安全Task队列
  typedef TSQueue<Task, true> TS_TaskQueue;

  TS_TaskQueue runnable_queue_;

  TS_TaskQueue wait_queue_;

  TS_TaskQueue new_queue_;

  TSQueue<Task, false> gc_queue_;

  Task *runnable_task_ = nullptr;

  Task *next_task_ = nullptr;

  std::condition_variable_any cv_;

  bool waiting_;

  Scheduler *scheduler_;

  // 执行process方法的线程的线程id
  int id_;

  uint64_t switch_count_ = 0;

 // for friend class Scheduler 
 private:
  Processor(Scheduler *scheduler_, int id);

  void process();
};

inline void Processor::static_co_yield() {
  auto proc = get_current_processor();
  proc->co_yield();
}

inline void Processor::co_yield() {
  Task *task = get_current_task();
  ++(task->yield_cnt_);
  task->swap_out();
}

}  // namespace co
