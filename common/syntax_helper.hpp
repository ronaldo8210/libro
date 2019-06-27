/***********************************************************************
#   > File Name   : syntax_helper.h
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-14 11:09:20
***********************************************************************/

#include "../scheduler/scheduler.hpp"
#include "../scheduler/processor.hpp"

namespace co {

struct __go
{
    __go() {
        scheduler_ = nullptr;
    }

    template <typename Function>
    inline void operator-(Function const& f) {
        if (!scheduler_) scheduler_ = Processor::get_current_scheduler();
        if (!scheduler_) scheduler_ = &Scheduler::getInstance();
        scheduler_->create_task(f, opt_);
    }

    TaskOpt opt_;
    Scheduler* scheduler_;
};

}  // namespace co
