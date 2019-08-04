/***********************************************************************
#   > File Name   : epoll_reactor.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 16:25:09
***********************************************************************/
#pragma once

#include "reactor.hpp"

namespace co {

class EpollReactor : public Reactor {
 public:
  EpollReactor();

  void run() override;

  bool add_event(int fd, short int add_event, short int promise_event) override;

  bool del_event(int fd, short int del_event, short int promise_event) override;

 private:
  int epfd_;
};

}  // namespace co
