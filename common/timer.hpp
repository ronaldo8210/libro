/***********************************************************************
#   > File Name   : timer.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-07-02 19:31:48
***********************************************************************/
#pragma once

#include <chrono>
#include <thread>
#include "clock.hpp"
#include "spinlock.hpp"
#include "ts_queue.hpp"

namespace co {

/*
 * 多级时间轮定时事件处理算法
 * 1、每个执行周期需要前进M个时间精度，由第0级齿轮推动，0级齿轮转动M个刻度，1~N级齿轮进行相应的进位转动
 * 2、新增定时事件时：
 *    与当前的时间轮point进行对比，算出需要前进多少个时间精度（0级齿轮的刻度）；
 *    新增定时事件只安装在最高级别的齿轮上
 * 3、处理定时事件时：
 *    如果事件在0级齿轮上，则立即调用回调函数；
 *    如果在1~N级齿轮上，则算出事件的剩余timeout，并将事件重新安装在低一级的（轮转更快的）齿轮的相应刻度上
 */
template<typename F>    
class Timer {
 public:
  class Element : public TSQueueHook {
   public:
    inline void init(const F &cb, FastSteadyClock::time_point tp) {  // time_point内部实现为内置类型，直接拷贝，不用引用
      cb_ = cb;
      tp_ = tp;
    }

    inline void call() {
      slot_ = nullptr;
      cb_();
    }

   private:
    F cb_;
    FastSteadyClock::time_point tp_;
    void *slot_ = nullptr;  // 时间轮某一级齿轮的某个slot的指针
  };

  typedef TSQueue<Element> Slot;
  typedef TSQueue<Element> Pool;

 public:

  Timer();

  ~Timer();

  void thread_run();

  void run_once();

  // 由外部调用
  void stop();

  void start_timer(FastSteadyClock::duration dur, const F &cb);

  void start_timer(FastSteadyClock::time_point tp, const F &cb);

  void set_pool_size(int max);

  std::size_t get_pool_size();

  template<typename Rep, typename Period>
  void set_precision(std::chrono::duration<Rep, Period> precision);

 private:

  Element* new_element();

  void delete_element(Element *elem);

  // timerloop表示新增定时事件的动作是在时间轮主线程中完成，还是在外部线程（如处理协程任务的process线程）完成
  void dispatch(Element *elem, bool timerloop);

 private:
  bool stop_ = false;

  int max_pool_size_ = 0;

  Pool pool_;

  // 8级时间轮，每级齿轮含256个刻度
  Slot wheel_[8][256];

  union Point {
    uint64_t p64;
    // uint8_t为无符号8bit长度类型，2^8=256，每个bit用来表示每级齿轮上的一个刻度
    uint8_t p8[8];
  };

  // 时间轮当前位置
  Point pos_;

  // 时间轮精度，即第0级齿轮的每刻度表示的时间长度
  FastSteadyClock::duration precision_;

  FastSteadyClock::time_point start_time_;

  // 需要立即执行的事件集合
  Slot complete_slot_;
};

template<typename F>
Timer<F>::Timer() {
  precision_ = std::chrono::microseconds(100);
  pos_ = 0;
  start_time_ = FastSteadyClock::now();
}

template<typename F>
Timer<F>::~Timer() {

}

template<typename F>
void Timer<F>::thread_run() {
  while (!stop_) {
    run_once();
    std::this_thread::sleep_for(precision_);
  }
}

template<typename F>
void Timer<F>::run_once() {

}

template<typename F>
void Timer<F>::stop() {
  stop_ = true;
}

template<typename F>
void Timer<F>::start_timer(FastSteadyClock::duration dur, const F &cb) {
  start_timer(FastSteadyClock::now() + dur, cb);
}

template<typename F>
void Timer<F>::start_timer(FastSteadyClock::time_point tp, const F &cb) {
  Element *elem = new_element();
  elem->init(cb, tp);
  // start_timer由外部线程调用，这里timerloop=false
  dispatch(elem, false);
}

template<typename F>
void Timer<F>::set_pool_size(int max) {
  max_pool_size_ = max;
}

template<typename F>
std::size_t Timer<F>::get_pool_size() {
  return pool_.size();
}

template<typename F>
template<typename Rep, typename Period>
void Timer<F>::set_precision(std::chrono::duration<Rep, Period> precision) {
  precision_ = std::chrono::duration_cast<FastSteadyClock::duration>(precision);
}

template<typename F>
typename Timer<F>::Element* Timer<F>::new_element() {
  Element *elem = nullptr;
  elem = new Element();
  return elem;
}

template<typename F>
void Timer<F>::delete_element(Element *elem) {

}

template<typename F>
void Timer<F>::dispatch(Element *elem, bool timerloop) {

}

}  // namespace co
