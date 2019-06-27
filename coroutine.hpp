/***********************************************************************
#   > File Name   : coroutine.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-12 21:17:11
***********************************************************************/
#pragma once

#include "common/syntax_helper.hpp"
#include "scheduler/scheduler.hpp"
#include "scheduler/processor.hpp"

#define go ::co::__go()-

#define co_yield ::co::Processor::static_co_yield()

#define co_sched ::co::Scheduler::getInstance()

