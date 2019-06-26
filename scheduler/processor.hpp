/***********************************************************************
#   > File Name   : processor.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 16:02:29
***********************************************************************/
#pragma once

#include "../common/ts_queue.hpp"

namespace co {

class Task;
class Scheduler;
class TaskQueue;

class Processor {
 friend class Scheduler;

 public:
  static Process* get_current_process();

  static Scheduler* get_current_scheduler();

  static void static_co_yield();

 private:
  Scheduler *scheduler_;

  // 执行process方法的线程的线程id
  int id_;

  // 线程安全Task队列
  typedef TSQueue<Task, true> TS_TaskQueue;

  TS_TaskQueue runnable_queue_;
  TS_TaskQueue wait_queue_;
  TS_TaskQueue new_queue_;
  TSQueue<Task, false> gc_queue_;

  Task* runnable_task_ = nullptr;

  // 用于添加协程任务线程与处理协程任务线程的同步
  bool wait_condition();
  std::condition_variable_any cv_;
  bool waiting_;

  // 将新增协程任务加入new_queue
  bool add_task(Task *task);

  // 将new_queue中的协程全部移入runnable_queue
  bool add_new_queue();

  bool gc();

  void co_yiled();

 // for friend class Scheduler 
 private:
  Processor(Scheduler *scheduler_, int id);

  void process();
};

}  // namespace co
