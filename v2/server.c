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
#include "signals.h"
#include "utils.h"

#define xxx

#define MAXLINE 1024
static inline void usage() { puts("./server [port]\n"); }
static void strEcho(int sockfd) {
  char buf[MAXLINE];
  for (;;) {
    ssize_t n;
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
      write(sockfd, buf, n);
    if (n < 0 && errno == EINTR)
      continue;
    else if (n < 0) {
      coloredPerror("strEcho: read error");
    }
    break;
  }
}
int main(int argc, char *argv[]) {
  if (argc < 2)
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
  puts(ANSI_COLOR_GRAY "Server is on line -_-" ANSI_COLOR_RESET);

  // The purpose of the zombie state is to maintain information about the child
  // for the parent to fetch at some later time. This information includes the
  // process ID of the child, its termination status, and information on the
  // resource utilization of the child. Zombie processes take up space in the
  // kernel and eventually can run out of processes. So after fork children,
  // the parent process must wait for them to prevent them from becoming
  // zombies. To do this, a signal handler is established to catch SIGCHLD,
  // and within the handler, wait is called.

  signal(SIGCHLD, sigChild);

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

    int connFd;
    for (;;) {
      // when client disconnected from the server, the child process exit and
      // SIGCHLD is delivered, at the same time, parent is blocked in its call
      // to accept, Since the signal was caught by the parent while the parent
      // was blocked in a slow system call (accept), the kernel causes the
      // accept to return an error of EINTR (interrupted system call). The
      // parent does not handle this error, so it aborts.
      //
      //  when writing network programs that catch signals, we must be cognizant
      //  of interrupted system calls, and we must handle them. in the signal
      //  function in signals.c, EINTR is handled and accept is restarted afer
      //  encounter interrupted system call.

      connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &slen);
      if (connFd < 0) {
#ifdef EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
          continue;
        else {
          coloredPerror("accept error");
          exit(1);
        }
      } else
        break;
    }

    pid_t pid;
    if ((pid = fork()) == 0) {
      if (close(listenFd) == -1) {
        coloredPerror("listen close error");
      }

      char addr[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
      snprintf(logBuf, sizeof(logBuf),
               ANSI_COLOR_GREEN "client from %s:%d connected" ANSI_COLOR_RESET,
               addr, clientAddr.sin_port);
      puts(logBuf);

      /*
      ticks = time(NULL);
      snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
      write(connFd, sendBuff, strlen(sendBuff));
      */
      strEcho(connFd);
      if (close(connFd) == -1) {
        coloredPerror("connected close error");
      }

      snprintf(logBuf, sizeof(logBuf),
               ANSI_COLOR_BLUE
               "client from %s:%d disconnected" ANSI_COLOR_RESET,
               addr, clientAddr.sin_port);
      puts(logBuf);

      // WARNING!! REMEMBER TO EXIT.
      exit(0);
    }
    // Why doesn't the close of connfd by the parent terminate
    // its connection with the client? To understand what's happening,
    // we must understand that every file or socket has a reference
    // count. The reference count is maintained in the file table entry
    // This is a count of the number of descriptors that are currently
    // open that refer to this file or socket. After accept returns,
    // the file table entry associated with connfd has a reference
    // count of 1. But, after fork returns, both descriptors are shared
    // (i.e., duplicated) between the parent and child, so the file
    // table entries associated with both sockets now have a reference
    // count of 2. Therefore, when the parent closes connfd, it just
    // decrements the reference count from 2 to 1 and that is all.
    // The actual cleanup and de-allocation of the socket does not happen
    // until the reference count reaches 0. This will occur at some
    // time later when the child closes connfd.
    close(connFd);
  }

  return 0;
}
