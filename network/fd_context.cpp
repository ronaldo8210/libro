/***********************************************************************
#   > File Name   : fd_context.cpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-08-03 11:44:53
***********************************************************************/
#include "fd_context.hpp"

#include "ev_util.hpp"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace co {

FdContext::FdContext(int fd, bool is_nonblocking) : ReactorElement(fd) {
  fd_ = fd;
  is_nonblocking_ = is_nonblocking;
}

bool FdContext::set_nonblocking(bool is_nonblocking) {
  int flags = call_without_intr<int>(fcntl, fd_, F_GETFL, 0);
  bool old = flags & O_NONBLOCK;
  if (is_nonblocking == old) {
    return old;    
  }

  call_without_intr<int>(fcntl, fd_, F_SETFL, 
          is_nonblocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
  is_nonblocking_ = is_nonblocking;
}

bool FdContext::is_nonblocking() {
  return is_nonblocking_;
}

void FdContext::set_conn_timeout(int microseconds) {
  conn_timeout_ = microseconds;
}

int FdContext::get_conn_timeout() {
  return conn_timeout_;
}

void FdContext::set_sock_timeout(int type, int microseconds) {
  switch (type) {
    case SO_RCVTIMEO:
      recv_timeout_ = microseconds;
      break;
    case SO_SNDTIMEO:
      send_timeout_ = microseconds;
      break;
  }
}

int FdContext::get_sock_timeout(int type) {
  switch (type) {
    case SO_RCVTIMEO:
      return recv_timeout_;
      break;
    case SO_SNDTIMEO:
      return send_timeout_;
      break;
  }
}

}  // namespace co
