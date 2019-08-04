/***********************************************************************
#   > File Name   : reactor_element.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-02 14:55:00
***********************************************************************/
#pragma once

#include "../scheduler/processor.hpp"

#include <mutex>
#include <vector>

namespace co {

class Reactor;
class ReactorElement {
 public:
  struct Entry {
    Processor::SuspendEntry suspend_entry_;
    short int revents_;
    int id_;

    Entry(const Processor::SuspendEntry &suspend_entry, short int revent, int id) 
        : suspend_entry_(suspend_entry), 
        revents_(revent), 
        id_(id) {

    }

    bool operator==(const Entry &rhs) {
      return suspend_entry_ == rhs.suspend_entry_ 
          && revents_ == rhs.revents_ 
          && id_ == rhs.id_;
    }
  };

  typedef std::vector<Entry> EntryList;

  explicit ReactorElement(int fd);

  bool add(Reactor *reactor, short int poll_event, const Entry &entry);

  void trigger(Reactor *reactor, short int poll_event);

 protected:

  EntryList& select_list(short int poll_event);

  void trigger_list_without_lock(short int revent, EntryList &list);

  void roll_back(EntryList &list, const Entry &entry);

  void check_expire(EntryList &list);

  void close();

 private:

  // 如下队列是多个协程读写同一fd的场景使用，如果是一个协程独占tcp连接池中的一个连接，不会用到这些队列
  EntryList in_;

  EntryList out_;

  EntryList in_out_;

  EntryList err_;

  int fd_;

  // fd上当前关注的事件集合
  short int event_;

  std::mutex mtx_;
};

}  // namespace co
