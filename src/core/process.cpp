#include "process.hpp"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "log.hpp"

char buf[1024];
int process(int fd) {
  for (;;) {
    int ret = read(fd, buf, 1024);
    if (-1 == ret) {
      if (errno == EAGAIN) {
        log_debug("read from fd %d: %s", fd, strerror(errno));
        // write(fd, "error", 5);
        return 2;
      }
    } else if (0 == ret) {
      if (errno == EAGAIN) {
        log_warn("read from fd %d: %s", fd, strerror(errno));
        break;
      } else if (errno == EWOULDBLOCK) {
        log_warn("read from fd %d: %s", fd, strerror(errno));
        break;
      }
    }
    buf[ret] = '\0';

    if (0 == strncmp("exit", buf, 4)) {
      return 1;
    } else if (0 == strncmp("ping", buf, 4)) {
      ret = write(fd, "pong", 4);
    } else if (0 == strncmp("hello", buf, 5)) {
      ret = write(fd, "world", 5);
    } else {
      log_warn("get weird message %s, just ignore", buf);
    }
  }
  return 0;
}
