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
#include <pthread.h>

#include "color.h"
#include "utils.h"

#define xxx

#define MAXLINE 1024
static inline void usage() { puts("./server [port]\n"); }
void* strEcho(void *sockfd) {
  struct sockaddr_in clientAddr;
  socklen_t slen = sizeof(clientAddr);

  int sockFd;
  sockFd = *(int *)sockfd;
  free(sockfd);

  getpeername(sockFd, (struct sockaddr *)&clientAddr, &slen);

  char logBuf[1025];
  char addr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
  snprintf(logBuf, sizeof(logBuf),
           ANSI_COLOR_GREEN "client from %s:%d connected" ANSI_COLOR_RESET,
           addr, clientAddr.sin_port);
  puts(logBuf);

  pthread_detach(pthread_self());

  char buf[MAXLINE];
  for (;;) {
    ssize_t n;
    while ((n = read(sockFd, buf, MAXLINE)) > 0){
#ifdef DEBUG
#endif
      write(0, "client input: ", 14);
      write(0, buf, n);
      for(int i=0;i<n;i++){
        if(buf[i]<='z'&& buf[i]>='a')
          buf[i]=buf[i]-'a'+'A';
        else if(buf[i]<='Z'&& buf[i]>='A')
          buf[i]=buf[i]-'A'+'a';
      }
      write(sockFd, buf, n);
    }
    if (n < 0 && errno == EINTR)
      continue;
    else if (n < 0) {
      coloredPerror("strEcho: read error");
    }
    break;
  }
  snprintf(logBuf, sizeof(logBuf),
           ANSI_COLOR_BLUE
           "client from %s:%d disconnected" ANSI_COLOR_RESET,
           addr, clientAddr.sin_port);
  puts(logBuf);
  return NULL;
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

  char logBuf[1025];
  for (;;) {
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

    int *connFd=(int *)malloc(sizeof(int));
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

      *connFd = accept(listenFd, NULL, NULL);
      if (*connFd < 0) {
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
    // use thread instead of process
    pthread_t pthid;
    pthread_create(&pthid, NULL, strEcho, connFd);

  }

  return 0;
}
