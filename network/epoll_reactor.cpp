/***********************************************************************
#   > File Name   : epoll_reactor.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 16:26:03
***********************************************************************/
#include "epoll_reactor.hpp"

#include "ev_util.hpp"
#include "fd_context.hpp"
#include "fd_mgr.hpp"
#include "sys/epoll.h"
#include <thread>

namespace co {

EpollReactor::EpollReactor() {
  epfd_ = epoll_create(1024);
  init_loop_thread();
}

void EpollReactor::run() {
  const int max_events = 1024;
  struct epoll_event evs[max_events];

  int n = call_without_intr<int>(::epoll_wait, epfd_, evs, max_events, 1);
  for (int i = 0; i < n; ++i) {
    struct epoll_event &ev = evs[i];
    int fd = ev.data.fd;
    FdContextPtr ctx = FdMgr::getInstance().get_fd_context(fd);
    if (!ctx) {
      continue;    
    }
    ctx->trigger(this, event_reactor_to_poll(ev.events));
  }
}

bool EpollReactor::add_event(int fd, short int add_event, short int promise_event) {
  struct epoll_event ev;
  ev.events = event_poll_to_reactor(promise_event) | EPOLLET;
  ev.data.fd = fd;
  int op = add_event == promise_event ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  int res = call_without_intr<int>(::epoll_ctl, epfd_, op, fd, &ev);
  return res == 0;
}

bool EpollReactor::del_event(int fd, short int del_event, short int promise_event) {
  struct epoll_event ev;
  ev.events = event_poll_to_reactor(promise_event) | EPOLLET;
  ev.data.fd = fd;
  int op = 0 == promise_event ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
  int res = call_without_intr<int>(::epoll_ctl, epfd_, op, fd, &ev);
  return res == 0;
}

}  // namespace co
