#include "process.hpp"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
char buf[1024];
int process(int fd) {
  for (;;) {
    int ret = read(fd, buf, 1024);
    if (-1 == ret) {
      break;
      switch (errno) {
      case EAGAIN: {
        break;
      }
      default:
        perror("read from fd");
      }
    }
    buf[ret - 1] = '\0';
    if (0 == strncmp("exit", buf, 4)) {
      return 1;
    } else if (0 == strncmp("ping", buf, 4)) {
      ret = write(fd, "pong", 4);
    } else if (0 == strncmp("hello", buf, 5)) {
      ret = write(fd, "world", 5);
    } else {
      printf("get weird message %s, just ignore\n", buf);
    }
  }
  return 0;
}
