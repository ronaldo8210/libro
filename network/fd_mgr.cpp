/***********************************************************************
#   > File Name   : fd_mgr.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 11:49:26
***********************************************************************/
#include "fd_mgr.hpp"

namespace co {

FdMgr& FdMgr::getInstance() {
  static FdMgr instance;
  return instance;
}
  
FdContextPtr FdMgr::get_fd_context(int fd) {
  FdSlotPtr slot = get_slot(fd);
  if (!slot) {
    return FdContextPtr();  
  }
  
  std::unique_lock<SpinLock> lock(slot->lock_);
  FdContextPtr ctx(slot->ctx_);
  return ctx;
}

void FdMgr::create(int fd, bool is_nonblocking) {
  FdContextPtr ctx(new FdContext(fd, is_nonblocking));
  insert(fd, ctx);
}

FdMgr::FdSlotPtr FdMgr::get_slot(int fd) {
  int bucket_idx = fd & bucket_count;
  std::unique_lock<std::mutex> lock(bucket_mtx_[bucket_idx]);

  auto &bucket = buckets_[bucket_idx];
  auto iter = bucket.find(fd);
  if (iter == bucket.end()) {
    return FdSlotPtr();
  }
  return iter->second;
}

void FdMgr::insert(int fd, FdContextPtr ctx) {
  // 先做散列，把fd分散到不同的bucket，让锁的粒度小些  
  int bucket_idx = fd & bucket_count;
  std::unique_lock<std::mutex> lock(bucket_mtx_[bucket_idx]);

  auto &bucket = buckets_[bucket_idx];
  FdSlotPtr &slot = bucket[fd];
  if (!slot) {
    slot.reset(new FdSlot());
  }
}

}  // namespace co
