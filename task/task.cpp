/***********************************************************************
#   > File Name   : task.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 20:30:04
***********************************************************************/
#include "task.hpp"

namespace co {

Task::Task(const TaskF &fn, size_t stack_size) : 
    ctx_(&Task::static_run, (intptr_t)this, stack_size), fn_(fn) {

}

Task::~Task() {

}

void Task::static_run(intptr_t ptr) {
  Task *tk = (Task *)ptr;
  tk->run();
}

void Task::run() {
  fn_();

  state_ = TaskState::done;
  // yield
}

}  // namespace co
