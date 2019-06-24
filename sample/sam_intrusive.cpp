/***********************************************************************
#   > File Name   : sam_intrusive.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-24 18:34:56
***********************************************************************/

#include <iostream>
#include "../common/ts_queue.hpp"

class Foo : public co::TSQueueHook {
 public:
  Foo(int n) : n_(n) {}
  inline int get() { return n_; }

 private:
  int n_;
};

int main(int argc, char **argv) {
  co::TSQueue<Foo, true> queue;

  Foo foo1(5);
  Foo foo2(10);

  queue.push(&foo1);
  queue.push(&foo2);
  std::cout << queue.size() << std::endl;  // 2

  Foo *ptr = nullptr;
  queue.front(ptr);
  std::cout << ptr->get() << std::endl;  // 5

  return 0;
}
