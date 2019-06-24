/***********************************************************************
#   > File Name   : ts_queue.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-14 15:51:13
***********************************************************************/
#pragma once

#include <type_traits>
#include <assert.h>
#include "spinlock.hpp"

namespace co {

// 侵入式数据结构节点基类
struct TSQueueHook {
  TSQueueHook() : prev_(nullptr), next_(nullptr), check_(nullptr) {}

  // 将一个给定节点链入本节点尾部
  inline void link(TSQueueHook *node) {
    assert(next_ == nullptr);
    assert(node->prev_ == nullptr);
    next_ = node;
    node->prev_ = this;
  }

  // 断开本节点与后方节点的链接关系
  inline void unlink(TSQueueHook *node) {
    assert(next_ == node);
    assert(node->prev_ == this);
    next_ = nullptr;
    node->prev_ = nullptr;
  }

  // 编译器不一定支持类内初始值
  TSQueueHook *prev_ /* = nullptr */;
  TSQueueHook *next_ /* = nullptr */;
  void *check_ /* = nullptr */;
};

template<typename T, bool ThreadSafe = true>
class TSQueue {
 public:

  typedef typename std::conditional<ThreadSafe, SpinLock, FakeLock>::type lock_t;

  TSQueue() {
    head_ = tail_ = new TSQueueHook();
    count_ = 0;
    lock_ = &owner_lock_;
  }

  ~TSQueue() {
    while (head_ != tail_) {
      TSQueueHook *prev = tail_->prev_;
      delete (T*)tail_;
      tail_ = prev;
    }
  }

  inline void front(T*& out) {  // 指针的引用
    out = (T*)head_->next_;
  }

  inline std::size_t push(T *element) {
    return push_without_lock(element);
  }

  inline std::size_t push_without_lock(T *element) {
    TSQueueHook *node = static_cast<TSQueueHook*>(element);
    assert(node->prev_ == nullptr);
    assert(node->next_ == nullptr);

    tail_->link(node);
    node->next_ = nullptr;
    tail_ = node;
    
    ++count_;
    return count_;
  }

  inline bool empty() {
    return !count_;
  }

  inline size_t size() {
    return count_;  
  }

 public:
  TSQueueHook *head_;
  TSQueueHook *tail_;
  std::size_t count_;
  lock_t owner_lock_;
  lock_t *lock_;
};

}  // namespace co
