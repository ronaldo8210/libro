/***********************************************************************
#   > File Name   : fd_mgr.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 11:49:00
***********************************************************************/
#pragma once

#include "../common/spinlock.hpp"
#include "fd_context.hpp"
#include <map>
#include <memory>
#include <mutex>

namespace co {

class FdMgr {
 public:
  struct FdSlot {
    FdContextPtr ctx_;
    SpinLock lock_;
  };

  typedef std::shared_ptr<FdSlot> FdSlotPtr;

  static FdMgr& getInstance();
  
  FdContextPtr get_fd_context(int fd);

  void create(int fd, bool is_nonblocking = false);
  
 private:
  static const int fd_size = 128;

  static const int bucket_shift = 10;

  static const int bucket_count = 1 << bucket_shift;

  FdSlotPtr get_slot(int fd);

  void insert(int fd, FdContextPtr ctx);

  typedef std::map<int, FdSlotPtr> Slots;

  Slots buckets_[bucket_count];

  std::mutex bucket_mtx_[bucket_count];
};

}  // namespace co
