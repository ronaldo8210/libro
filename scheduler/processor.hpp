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
 private:
  Scheduler *scheduler_;

  // 线程id
  int id_;

  // 线程安全Task队列
  typedef TSQueue<Task, true> TS_TaskQueue;

  TS_TaskQueue runnable_queue_;
  TS_TaskQueue wait_queue_;
  TS_TaskQueue new_queue_;
  TS_TaskQueue gc_queue_;

 // for friend class Scheduler 
 private:
  Processor(Scheduler *scheduler_, int id);

  void process();
};

}  // namespace co
