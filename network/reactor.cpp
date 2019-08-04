/***********************************************************************
#   > File Name   : reactor.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-07-30 22:35:42
***********************************************************************/
#include "reactor.hpp"

#include "epoll_reactor.hpp"
#include <thread>

namespace co {

std::vector<Reactor*> Reactor::reactors_;
uint8_t Reactor::reactor_cnt_;

Reactor::Reactor() {

}

Reactor& Reactor::select(int fd) {
  return *reactors_[fd % reactors_.size()];
}

void Reactor::init_reactor_cnt(uint8_t n) {
  if (!reactors_.empty()) {
    return;  
  }

  reactors_.reserve(n);
  for (uint8_t i = 0; i < n; ++i) {
    reactors_.push_back(new EpollReactor());
  }
}

int Reactor::reactor_thread_cnt() {
  return reactors_.size();
}

bool Reactor::add(int fd, short int poll_event, const Entry& entry) {
  // TODO
  return true;
}

void Reactor::init_loop_thread() {
  std::thread thrd([this] {
            for (;;) this->run();
          });
  thrd.detach();
}

}  // namespace co
