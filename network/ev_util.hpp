/***********************************************************************
#   > File Name   : ev_util.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-04 14:37:13
***********************************************************************/
#pragma once

#include <stdint.h>

extern "C" {

uint32_t event_poll_to_reactor(short int poll_event); 

short int event_reactor_to_poll(uint32_t reactor_event); 

/*
template <typename R, typename F, typename ... Args>
static R call_without_intr(F f, Args && ... args) {
retry:
  R res = f(std::forward<Args>(args)...);
  if (res == -1 && errno == EINTR) {
    goto retry;
  }
  return res;
} */

}  // extern "C"
