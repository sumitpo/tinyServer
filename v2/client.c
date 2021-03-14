#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "color.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    coloredPerror("usage: client [server address] [server port]");
    return 1;
  }
  int port = atoi(argv[2]);
  int connFd;

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

  connFd = socket(AF_INET, SOCK_STREAM, 0);
  if (connFd == -1) {
    coloredPerror("socket error");
    return 1;
  }
  if (connect(connFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    coloredPerror("connect to server failed");
    return 1;
  }
  char wtBuf[1025];
  char rdBuf[1025];
  int rdn;
  while ((rdn = read(0, wtBuf, 1024)) > 0) {
    int start = 0;
    int len = 0;
    for (int i = 0; i < rdn; i++) {
      if (wtBuf[i] == '\n') {
        write(connFd, &wtBuf[start], len + 1);
        rdn = read(connFd, rdBuf, 1024);
        if(rdn<0){
          break;
        }
        write(1, rdBuf, rdn);
        len = 0;
        start = i + 1;
      }
      len++;
    }
  }
  close(connFd);
  return 0;
}
