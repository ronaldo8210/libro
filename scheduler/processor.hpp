/***********************************************************************
#   > File Name   : processor.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 16:02:29
***********************************************************************/
#pragma once

#include <condition_variable>
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

  // 频繁调用，置为inline类型
  inline static void static_co_yield();

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
