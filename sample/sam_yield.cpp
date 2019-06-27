/***********************************************************************
#   > File Name   : sam_yield.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-13 09:19:57
***********************************************************************/
#include "coroutine.hpp"
#include <iostream>

using namespace std;

/*
 * 输出1 3 5 2 4 6
 */
int main(int argc, char **argv) {
  go [] {
    cout << "1" << endl;
    co_yield;
    cout << "2" << endl;
  };

  go [] {
    cout << "3" << endl;
    co_yield;
    cout << "4" << endl;
  };

  go [] {
    cout << "5" << endl;
    co_yield;
    cout << "6" << endl;
  };

  co_sched.start();

  return 0;
}
