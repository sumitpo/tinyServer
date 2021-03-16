#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "color.h"
#include "utils.h"
static inline void usage() { puts("./server [port]\n"); }
int main(int argc, char *argv[]) {
  if (argc < 1)
    usage();

  char sendBuff[1025];
  time_t ticks;
  int listenFd;
  struct sockaddr_in server;

  int port = atoi(argv[1]);

#ifdef DEBUG
  printf("Server listen on port is at %d\n", port);
#endif

  listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd == -1)
    coloredPerror("socket error");

  memset((char *)&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  // htonl host to network long, convert the endian
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  if (bind(listenFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    coloredPerror("bind error");
    return 1;
  }

  // the linux kernel maintains two queues for any listening socket:
  //
  // incomplete connection queue: client have send the SYN, and this socket
  // is in SYN_RCVD and waiting for the completion of three-way handshake
  // the socket is created when client's SYN arrived, this entry will remain
  // on the incomplete queue until the third segment of the three-way handshake
  // arrives (the client's ACK of the server's SYN), or until the entry times
  // out (Berkeley-derived implementations have a timeout of 75 seconds for
  // these incomplete entries)
  //
  // completed connection queue: client have send the ack and complete the
  // three-way handshake, the sockets in this queue is in ESTABLISHED.
  listen(listenFd, 10);
  puts("Server is on line -_-");

  struct epoll_event ev, events[20];

  // create new epoll file descriptor
  int epfd = epoll_create(256);

  ev.data.fd = listenFd;
  ev.events = EPOLLET | EPOLLIN;
  // add listening file descriptor to epoll object
  epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &ev);

  struct sockaddr_in clientAddr;
  socklen_t slen = sizeof(clientAddr);

  char logBuf[1025];
  char addr[INET_ADDRSTRLEN];

  for (;;) {
    int rdyfds = epoll_wait(epfd, events, 20, 500);
    for (int i = 0; i < rdyfds; i++) {
      if (listenFd == events[i].data.fd) {
          int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &slen);

          inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
          snprintf(logBuf, sizeof(logBuf),
                   ANSI_COLOR_GREEN "client from %s:%d connected" ANSI_COLOR_RESET,
                   addr, clientAddr.sin_port);
          puts(logBuf);

          ev.data.fd=connFd;
          ev.events = EPOLLET | EPOLLIN;
          epoll_ctl(epfd, EPOLL_CTL_ADD, connFd, &ev);

      } else {
        int rdn;
        if ((rdn = read(events[i].data.fd, sendBuff, 1024)) == 0) {

          getpeername(events[i].data.fd, (struct sockaddr *)&clientAddr, &slen);
          inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
          snprintf(logBuf, sizeof(logBuf),
                   ANSI_COLOR_BLUE
                   "client from %s:%d disconnected" ANSI_COLOR_RESET,
                   addr, clientAddr.sin_port);
          puts(logBuf);

          close(events[i].data.fd);
          epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);

        }
        else{
          write(events[i].data.fd, sendBuff, rdn);
        }
      }
    }
  }

  return 0;
}
