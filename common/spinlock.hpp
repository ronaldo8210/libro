/***********************************************************************
#   > File Name   : spinlock.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-24 19:36:17
***********************************************************************/
#pragma once

#include <atomic>

namespace co {

// using atomic_flag
class SpinLock {
 public:
  SpinLock() : flag(ATOMIC_FLAG_INIT) {}
  
  inline void lock() {
    while (flag.test_and_set(std::memory_order_acquire)) {
      // do nothing
    }
  }

  inline void unlock() {
    flag.clear(std::memory_order_release);
  }

 private:
  std::atomic_flag flag;
};

// using atomic<bool>
class SpinLock_2 {
 public:
  SpinLock_2() : flag(ATOMIC_VAR_INIT(false)) {}
  
  void lock() {
    bool expect = false;
    while (!flag.compare_exchange_strong(expect, true)) {
      expect = false;  
    }
  }

  void unlock() {
    flag.store(false);
  }

 private:
  std::atomic<bool> flag;
};

// using Release-Acquire to enhance performance
class SpinLock_3 {
 public:
  SpinLock_3() : flag(ATOMIC_VAR_INIT(false)) {}
  
  void lock() {
    bool expect = false;
    while (!flag.compare_exchange_strong(expect, true, 
           std::memory_order_acquire, std::memory_order_relaxed)) {
      expect = false;  
    }
  }

  void unlock() {
    flag.store(false, std::memory_order_release);
  }

 private:
  std::atomic<bool> flag;
};

struct FakeLock {
  void lock() {}
  bool is_lock() { return false; }
  bool try_lock() { return true; }
  void unlock() {}
};

}  // namespace co
