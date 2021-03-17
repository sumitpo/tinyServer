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
#include "threadPool.h"
#include "utils.h"

struct processInfo {
  int sockFd;
  int epollFd;
};
void *strEcho(void *arg) {
  struct processInfo *proInfop = (struct processInfo *)arg;
  int sockfd = proInfop->sockFd;
  int epfd = proInfop->epollFd;
  free(proInfop);
  char buf[2048];

  int rdn = read(sockfd, buf, 2047);
  //printf("read %d\n", rdn);
  if (rdn == 0) {
    struct sockaddr_in clientAddr;
    socklen_t slen;
    char addr[INET_ADDRSTRLEN];
    char logBuf[128];

    getpeername(sockfd, (struct sockaddr *)&clientAddr, &slen);
    inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
    snprintf(logBuf, sizeof(logBuf),
             ANSI_COLOR_BLUE "client from %s:%d disconnected" ANSI_COLOR_RESET,
             addr, clientAddr.sin_port);
    puts(logBuf);

    close(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
    return NULL;
  } else {
    write(sockfd, buf, rdn);
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLET | EPOLLIN | EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
  }
  return NULL;
}
static inline void usage() {
  puts("./server [port]\n");
  return;
}
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

  struct threadPool thdp;
  memset(&thdp, 0, sizeof(struct threadPool));

  createTpool(&thdp, 4, 6, 0);
  initThreadPool(&thdp);

  for (;;) {
    int rdyfds = epoll_wait(epfd, events, 20, 500);
    for (int i = 0; i < rdyfds; i++) {
      if (listenFd == events[i].data.fd) {
        int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &slen);

        inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
        snprintf(logBuf, sizeof(logBuf),
                 ANSI_COLOR_GREEN
                 "client from %s:%d connected" ANSI_COLOR_RESET,
                 addr, clientAddr.sin_port);
        puts(logBuf);

        ev.data.fd = connFd;
        ev.events = EPOLLET | EPOLLIN | EPOLLONESHOT;
        epoll_ctl(epfd, EPOLL_CTL_ADD, connFd, &ev);

      } else {

        struct processInfo *proInfop =
            (struct processInfo *)malloc(sizeof(struct processInfo));
        proInfop->sockFd = events[i].data.fd;
        proInfop->epollFd = epfd;

        addTaskTpool(&thdp, strEcho, proInfop);
      }
    }
  }

  return 0;
}
