/***********************************************************************
#   > File Name   : clock.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-07-03 09:06:15
***********************************************************************/
#pragma once

#include <chrono>

namespace co {

class FastSteadyClock : public std::chrono::steady_clock {

};

}  // namespace co
