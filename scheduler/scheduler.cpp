/***********************************************************************
#   > File Name   : scheduler.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 09:40:43
***********************************************************************/
#include "scheduler.hpp"

#include "../task/task.hpp"
#include "processor.hpp"

namespace co {

Scheduler::Scheduler() : min_thread_cnt_(1), max_thread_cnt_(1), task_cnt_(0) {
  // 构造函数执行完毕之前this指针是否有效？
  processors_.push_back(new Processor(this, 0));
}

Scheduler::~Scheduler() {
  stop();
}

void Scheduler::create_task(const TaskF &task_fn, const TaskOpt &task_opt) {
  Task *task = new Task(task_fn, task_opt.stack_size_ ? task_opt.stack_size_ : 1 * 1024);
  task->set_deleter(Deleter(Scheduler::delete_task, this));
  task->id_ = 0;
  ++task_cnt_;

  add_new_task(task);
}

void Scheduler::start(int min_thread_cnt, int max_thread_cnt) {
  if (min_thread_cnt < 1) {
    min_thread_cnt = std::thread::hardware_concurrency();
  }

  if (max_thread_cnt < min_thread_cnt) {
    max_thread_cnt = min_thread_cnt;
  }

  min_thread_cnt_ = min_thread_cnt;
  max_thread_cnt_ = max_thread_cnt;

  for (int i = 0; i < min_thread_cnt_ - 1; ++i) {
    create_process_thread();
  }
  
  auto main_proc = processors_[0];
  main_proc->process();
}

void Scheduler::stop() {
  if (stop_) {
    return;  
  }

  stop_ = true;

  size_t n = processors_.size();
  for (size_t idx = 0; idx < n; ++idx) {
    if (processors_[idx]) {
      //processors_[idx]->notify_condition();
      delete processors_[idx];
      processors_[idx] = NULL;
    }
  }
}

uint64_t Scheduler::task_count() {
  return task_cnt_;
}

uint64_t Scheduler::current_task_id() {
  Task *tk = Processor::get_current_task();
  return tk ? tk->id_ : 0;
}

uint64_t Scheduler::current_yield_task_count() {
  Task *tk = Processor::get_current_task();
  return tk ? tk->yield_cnt_ : 0;
}

// 会多线程调用吗？
void Scheduler::add_new_task(Task *task) {
  std::size_t pcount = processors_.size();
  // TODO
  std::size_t idx = task->id_ % pcount;
  auto proc = processors_[idx];
  proc->add_new_task(task);
}

// 协程任务析构和任务统计相对应，统一聚集在Scheduler中实现
void Scheduler::delete_task(RefObject* task, void *arg) {
  Scheduler *self = (Scheduler*)arg;
  delete task;
  --(self->task_cnt_);
}

void Scheduler::create_process_thread() {
  auto p = new Processor(this, processors_.size());
  std::thread thrd([this, p] {
          p->process();
          });
  thrd.detach();
  processors_.push_back(p);
}

}  // namespace co
