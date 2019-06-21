/***********************************************************************
#   > File Name   : task.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 09:27:09
***********************************************************************/
#pragma once

#include <functional>
#include <stdlib.h>
#include <stdint.h>
#include "../context/context.hpp"

namespace co {

enum TaskState {
  runnable,
  block,
  done
};

struct TaskOpt {
  bool affinity_ = false;
  size_t stack_size_;
};

class Processor;

class Task {
 public:
  typedef std::function<void()> TaskF;
  Task(const TaskF &fn, size_t stack_size);
  ~Task();

  TaskState state_;
  uint64_t id_;
  Context ctx_;
  Processor *proc_;
  TaskF fn_;
  std::exception_ptr eptr_;

  uint64_t yield_cnt_;
  uint64_t suspend_id_;

  inline void swap_in() {
      
  }

  /*
  inline void swap_to(Task *dst_tk) {

  } */

  inline void swap_out() {
  
  }
  
 private:
  // task实例不能被拷贝、被移动，禁止编译器合成拷贝构造函数和移动构造函数
  Task(const Task &) = delete;
  Task(Task &&) = delete;

  // static函数相当于C的全局函数
  static void static_run(intptr_t ptr);
  void run();
};

}  // namespace co
