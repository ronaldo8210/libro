/***********************************************************************
#   > File Name   : task.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 20:30:04
***********************************************************************/
#include "task.hpp"
#include "../scheduler/processor.hpp"

namespace co {

Task::Task(const TaskF &fn, size_t stack_size) : 
    ctx_(&Task::static_run, (intptr_t)this, stack_size), fn_(fn) {

}

Task::~Task() {
  // Task对象析构的时候必须保证对象已不在队列中
  assert(!this->prev_);
  assert(!this->next_);
}

void Task::static_run(intptr_t ptr) {
  Task *tk = (Task *)ptr;
  tk->run();
}

void Task::run() {
  fn_();

  // 协程执行完后析构functional对象

  state_ = TaskState::done;
  Processor::static_co_yield();
}

}  // namespace co
