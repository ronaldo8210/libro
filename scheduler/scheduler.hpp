/***********************************************************************
#   > File Name   : scheduler.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 09:36:40
***********************************************************************/
#pragma once

#include <deque>
#include <functional>

namespace co {

class Processor;
struct TaskOpt;
class Task;

class Scheduler {
 public:
  typedef std::function<void()> TaskF;

  inline static Scheduler& getInstance();

  void create_task(const TaskF &task_fn, const TaskOpt &task_opt);

  void start(int min_thread_cnt = 1, int max_thread_cnt = 1);
  void stop();

  // 当前是否没有协程可执行
  bool empty();

  // 当前协程总数量（可执行数+挂起数）
  uint64_t task_count();

  // 当前协程ID，协程ID从1开始计数
  uint64_t current_task_id();

  // 当前协程切换总次数
  uint64_t current_yield_task_count();

 private:
  // 禁止从外部直接构造该类型实例
  Scheduler();
  ~Scheduler();

  // 阻止拷贝和移动操作
  Scheduler(const Scheduler&) = delete;
  Scheduler(Scheduler&&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;
  Scheduler& operator=(Scheduler&&) = delete;

  // 将一个协程加入可执行队列中
  void add_runnable_task(Task *task);

  void create_process_thread();

  int min_thread_cnt_;
  int max_thread_cnt_;

  uint64_t task_cnt_; 

  std::deque<Processor*> processors_;

  bool stop_;
};

inline Scheduler& Scheduler::getInstance() {
  // 函数第一次被调用时才生成静态的全局唯一Scheduler实例  
  static Scheduler obj;
  return obj;
}

}  // namespace co
