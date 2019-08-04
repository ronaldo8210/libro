/***********************************************************************
#   > File Name   : reactor_element.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-02 17:36:53
***********************************************************************/
#include "reactor_element.hpp"

#include "../scheduler/processor.hpp"
#include "reactor.hpp"
#include <algorithm>
#include <poll.h>

namespace co {

ReactorElement::ReactorElement(int fd) : fd_(fd) {

}

// 被协程所在线程调用
bool ReactorElement::add(Reactor *reactor, short int poll_event, const Entry &entry) {
  // 以下是多个协程同时使用一条tcp连接的场景
  std::unique_lock<std::mutex> lock(mtx_);

  EntryList& list = select_list(poll_event);
  list.push_back(entry);

  short int add_event = poll_event & (POLLIN | POLLOUT);
  if (add_event == 0) {
    return false;    
  }
  
  short int promise_event = event_ | add_event;
  add_event = promise_event & ~event_;

  if (promise_event != event_) {
    if (!reactor->add_event(fd_, add_event, promise_event)) {
      roll_back(list, entry);
      return false;
    }

    event_ = promise_event;
    return true;
  }

  // 关注的事件集合与上一次的值一致，不需要重复向epoll中添加
  return true;
}

// 被reactor线程调用
void ReactorElement::trigger(Reactor *reactor, short int poll_event) {
  std::unique_lock<std::mutex> lock(mtx_);

  // 当一个socket出现错误时（如连接断开/拒绝/超时），epoll会返回POLLERR加上注册时的POLLIN/POLLOUT事件
  // 如果fd监听的是POLLOUT，epoll_wait返回POLLOUT|POLLERR
  // 如果fd监听的是POLLIN，epoll_wait返回POLLIN|POLLERR
  short int err_event = POLLERR | POLLHUP | POLLNVAL;
  short int promise_event = 0;

  short int check = POLLIN | err_event;
  if (poll_event & check) {
    if (!in_.empty()) {
      trigger_list_without_lock(poll_event & check, in_);
    }
  } else if (!in_.empty()) {
    promise_event |= POLLIN;
  }

  check = POLLOUT | err_event;
  if (poll_event & check) {
    if (!out_.empty()) {
      trigger_list_without_lock(poll_event & check, out_);
    }
  } else if (!out_.empty()) {
    promise_event |= POLLOUT;
  }

  check = POLLIN | POLLOUT | err_event;
  if (poll_event & check) {
    if (!in_out_.empty()) {
      trigger_list_without_lock(poll_event & check, in_out_);
    }
  } else if (!in_out_.empty()) {
    promise_event |= (POLLIN | POLLOUT);
  }

  check = err_event;
  if (poll_event & check) {
    if (!err_.empty()) {
      trigger_list_without_lock(poll_event & check, err_);
    }
  } else if (!err_.empty()) {
    promise_event |= POLLERR;
  }
}

ReactorElement::EntryList& ReactorElement::select_list(short int poll_event) {
  if (poll_event & POLLIN) {
    return in_;
  } else if (poll_event & POLLOUT) {
    return out_;
  } else if ((poll_event & POLLIN) && (poll_event & POLLOUT)) {
    return in_out_;
  } else {
    return err_;
  }
}

void ReactorElement::trigger_list_without_lock(short int revent, EntryList &list) {
  // 如果是多个协程读写同一tcp连接，这里应该进行tcp拆包，解析出属于各个协程的数据，写入各协程的输入缓冲中
  // 协程重入执行后，应该是从各自的输入缓冲读取数据
  
  // TODO

  for (Entry &entry : list) {
    entry.revents_ = revent;
    Processor::wakeup(entry.suspend_entry_);
  }
  list.clear();
}

void ReactorElement::roll_back(EntryList &list, const Entry &entry) {
  auto iter = std::find(list.begin(), list.end(), entry);
  if (iter != list.end()) {
    list.erase(iter);
  }
}

void ReactorElement::check_expire(EntryList &list) {
  // TODO
}

void ReactorElement::close() {

}

}  // namespace co
