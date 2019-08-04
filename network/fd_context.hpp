/***********************************************************************
#   > File Name   : fd_context.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 11:44:00
***********************************************************************/
#pragma once

#include "reactor_element.hpp"
#include <memory>

namespace co {

// TODO
template <typename R, typename F, typename ... Args>
static R call_without_intr(F f, Args && ... args) {
retry:
  R res = f(std::forward<Args>(args)...);
  if (res == -1 && errno == EINTR) {
    goto retry;
  }
  return res;
}

class FdContext : public ReactorElement {
 public:
  FdContext(int fd, bool is_nonblocking);

  bool set_nonblocking(bool is_nonblocking);

  bool is_nonblocking();

  void set_conn_timeout(int microseconds);

  int get_conn_timeout();

  void set_sock_timeout(int type, int microseconds);

  int get_sock_timeout(int type);

 private:
  int fd_;

  bool is_nonblocking_;

  int conn_timeout_;

  int send_timeout_;

  int recv_timeout_;
};

typedef std::shared_ptr<FdContext> FdContextPtr;

}  // namespace co
