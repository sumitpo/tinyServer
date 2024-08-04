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
      perror("read from fd");
      if (errno == EAGAIN) {
        write(fd, "error", 5);
        return 2;
      }
    } else if (0 == ret) {
      if (errno == EAGAIN) {
        printf("ret 0: errno is eagain\n");
        break;
      } else if (errno == EWOULDBLOCK) {
        printf("ret 0: errno is ewouldblock\n");
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
      printf("get weird message %s, just ignore\n", buf);
    }
  }
  return 0;
}
