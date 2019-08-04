/***********************************************************************
#   > File Name   : reactor.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-07-30 22:35:27
***********************************************************************/
#pragma once

#include "reactor_element.hpp"

#include <vector>

namespace co {

class Reactor {
 public:
  typedef ReactorElement::Entry Entry;

  Reactor();

  static Reactor& select(int fd);

  static void init_reactor_cnt(uint8_t n);

  static int reactor_thread_cnt();

  bool add(int fd, short int poll_event, const Entry& entry);

  virtual void run() = 0;

  virtual bool add_event(int fd, short int add_event, short int promise_event) = 0;

  virtual bool del_event(int fd, short int del_event, short int promise_event) = 0;

 protected:
  void init_loop_thread();

 private:
  static std::vector<Reactor*> reactors_;
  static uint8_t reactor_cnt_;
};

}  // namespace co
