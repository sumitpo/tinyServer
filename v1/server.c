#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  char logBuf[1025];
  for (;;) {
    struct sockaddr_in clientAddr;
    socklen_t slen = sizeof(clientAddr);
    // accept is called by a TCP server to return the next completed connection
    // from the front of the completed connection queue. If accept is
    // successful, its return value is a brand-new descriptor automatically
    // created by the kernel. This new descriptor refers to the TCP connection
    // with the client. When discussing accept, we call the first argument to
    // accept the listening socket (the descriptor created by socket and then
    // used as the first argument to both bind and listen), and we call the
    // return value from accept the connected socket. It is important to
    // differentiate between these two sockets. A given server normally creates
    // only one listening socket, which then exists for the lifetime of the
    // server. The kernel creates one connected socket for each client
    // connection that is accepted (i.e., for which the TCP three-way handshake
    // completes). When the server is finished serving a given client, the
    // connected socket is closed.

    int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &slen);

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
    snprintf(logBuf, sizeof(logBuf),
             ANSI_COLOR_GREEN "client from %s:%d connected" ANSI_COLOR_RESET,
             addr, clientAddr.sin_port);
    puts(logBuf);

    ticks = time(NULL);
    snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
    write(connFd, sendBuff, strlen(sendBuff));
    close(connFd);
  }

  return 0;
}
