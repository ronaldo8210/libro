/***********************************************************************
#   > File Name   : scheduler.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 09:36:40
***********************************************************************/
#pragma once

#include <deque>
#include <functional>
#include "common/timer.hpp"

namespace co {

class Processor;
class RefObject;
class Task;
struct TaskOpt;

class Scheduler {
 public:
  typedef std::function<void()> TaskF;

  typedef Timer<std::function<void()>> TimerType;

  inline static Scheduler& getInstance();

  void create_task(const TaskF &task_fn, const TaskOpt &task_opt);

  void start(int min_thread_cnt = 1, int max_thread_cnt = 1);

  void stop();

  inline bool is_stop() {
    return stop_;  
  }

  // 当前是否没有协程可执行
  bool empty();

  // 当前协程总数量（可执行数+挂起数）
  uint64_t task_count();

  // 当前协程ID，协程ID从1开始计数
  uint64_t current_task_id();

  // 当前协程切换总次数
  uint64_t current_yield_task_count();

  inline TimerType & timer() {
    return *timer_;  // TODO
  }

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
  void add_new_task(Task *task);

  // 析构一个协程，必须保证协程对象引用计数是1时才执行
  static void delete_task(RefObject *task, void *arg);

  void create_process_thread();

  int min_thread_cnt_;

  int max_thread_cnt_;

  uint64_t task_cnt_; 

  std::deque<Processor*> processors_;

  TimerType *timer_ = nullptr;

  bool stop_ = false;
};

inline Scheduler& Scheduler::getInstance() {
  // 函数第一次被调用时才生成静态的全局唯一Scheduler实例  
  static Scheduler obj;
  return obj;
}

}  // namespace co
