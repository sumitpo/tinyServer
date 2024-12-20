#include "net.hpp"
#include "log.hpp"
#include "process.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

bool setSocketBlockingEnabled(int fd, bool blocking) {
  if (fd < 0)
    return false;

  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    return false;
  flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
  return (fcntl(fd, F_SETFL, flags) == 0);
}

tcpConn::tcpConn() {
  _server_addr = "127.0.0.1";
  _port = 8888;
  _initSvr();
}
tcpConn::tcpConn(const std::string serverAddr, int port) {
  _server_addr = serverAddr;
  _port = port;
  _initSvr();
}
tcpConn::~tcpConn() {
  if (!_inited) {
    log_warn("tcp not inited");
  } else {
    close(_listenFd);
    close(_epFd);
  }
}
int tcpConn::_initSvr() {
  _maxConn = 512;
  _currentConn = 0;
  // create socket and convert it to a listen fd
  if ((_listenFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    log_warn("socket failed: %s", strerror(errno));
    return 1;
  }
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  // inet_pton(AF_INET, _server_addr.c_str(), &(address.sin_addr));
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(_port);

  if (bind(_listenFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    log_warn("bind failed: %s", strerror(errno));
    return 1;
  }
  if (listen(_listenFd, 15) < 0) {
    log_warn("listen failed: %s", strerror(errno));
    return 1;
  }

  // epoll create and add listen fd to the epoll fds
  _epFd = epoll_create(256);

  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  // DO NOT enable edge triggered on listen fd
  // or accepted connection may be losted
  // ev.events = EPOLLIN | EPOLLET;
  ev.events = EPOLLIN;
  ev.data.fd = _listenFd;
  epoll_ctl(_epFd, EPOLL_CTL_ADD, _listenFd, &ev);
  log_debug("inited success");
  _inited = true;
  return 0;
}

int tcpConn::run() {
  if (!_inited) {
    log_warn("not inited, exit now");
    return 0;
  }
  struct epoll_event events[256], ev;
  int nfds;
  for (;;) {
    nfds = epoll_wait(_epFd, events, 256, -1);
    for (int i = 0; i < nfds; i++) {
      int fd = events[i].data.fd;
      if (fd == _listenFd && (events[i].events & EPOLLIN)) {
        int connFd = accept(fd, nullptr, nullptr);
        if (-1 == connFd) {
          log_error("accept: %d", strerror(errno));
          continue;
        }
        if (_currentConn >= _maxConn) {
          log_warn("closing connection, reach max");
          close(connFd);
          continue;
        }
        setSocketBlockingEnabled(connFd, false);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = connFd;
        int ret = epoll_ctl(_epFd, EPOLL_CTL_ADD, connFd, &ev);
        if (0 != ret) {
          // on error for adding fd to epoll fds, just close the connection
          log_warn("epoll_ctl failed: %s", strerror(errno));
          close(connFd);
        }
        _currentConn += 1;
      } else if (events[i].events & EPOLLIN) {
        int ret = process(events[i].data.fd);
        log_debug("process return %d", ret);
        if (1 == ret) {
          epoll_ctl(_epFd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
          log_debug("closing connected fd %d", events[i].data.fd);
          close(events[i].data.fd);
          _currentConn -= 1;
        }
      }
    }
  }
  return 0;
}
