/***********************************************************************
#   > File Name   : context.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-14 08:44:52
***********************************************************************/
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <ucontext.h>

namespace co {

class Context {
 public:
  typedef void (*fn_t)(intptr_t);

  Context(fn_t fn, intptr_t task_ptr, uint32_t stack_size) : 
      fn_(fn), task_ptr_(task_ptr), stack_size_(stack_size) {
    // 可能会出现内存不足导致malloc失败的情况，应该用RAII保证异常安全性
    stack_ = (char*)malloc(stack_size_);

    getcontext(&ctx_); 
    ctx_.uc_stack.ss_sp = stack_;
    ctx_.uc_stack.ss_size = stack_size_;
    ctx_.uc_stack.ss_flags = 0;
    ctx_.uc_link = &get_tls_context();

    makecontext(&ctx_, (void (*)(void))fn_, 1, task_ptr_);
  }

  ~Context() {
    if (stack_) {
      free(stack_);
    }
  }

  inline void swap_in() {
    swapcontext(&get_tls_context(), &ctx_);
  }

  inline void swap_to() {

  }

  inline void swap_out() {
    swapcontext(&ctx_, &get_tls_context());
  }

  // 不可以是inline，必须有函数调用过程
  // 放在processor里更好？
  ucontext_t& get_tls_context() {
    // 每个processor线程唯一的base ctx，每个协程调用yield交出cpu后回到的指定执行点
    static thread_local ucontext_t tls_ctx;
    return tls_ctx;
  }

 private:
  fn_t fn_;
  intptr_t task_ptr_;
  ucontext_t ctx_;
  char *stack_;
  uint32_t stack_size_;
};

}  // namespace co
