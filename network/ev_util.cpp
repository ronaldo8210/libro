/***********************************************************************
#   > File Name   : ev_util.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-04 14:37:13
***********************************************************************/
#include "ev_util.hpp"

#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

extern "C" {

uint32_t event_poll_to_reactor(short int poll_event) {
  uint32_t reactor_event = 0;
  if (poll_event & POLLIN) {
    reactor_event |= EPOLLIN;
  }
  if (poll_event & POLLOUT) {
    reactor_event |= EPOLLOUT;
  }
  if (poll_event & POLLERR) {
    reactor_event |= EPOLLERR;
  }
  if (poll_event & POLLHUP) {
    reactor_event |= EPOLLHUP;
  }

  return reactor_event;
}

short int event_reactor_to_poll(uint32_t reactor_event) {
  short int poll_event = 0;
  if (reactor_event & EPOLLIN) {
    poll_event |= POLLIN;
  }
  if (reactor_event & EPOLLOUT) {
    poll_event |= POLLOUT;
  }
  if (reactor_event & EPOLLERR) {
    poll_event |= POLLERR;
  }
  if (reactor_event & EPOLLHUP) {
    poll_event |= POLLHUP;
  }

  return poll_event;
}

}  // extern "C"
