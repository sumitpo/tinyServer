#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  char logBuf[1025];
  int client[FD_SETSIZE];
  for (int i = 0; i < FD_SETSIZE; i++) {
    client[i] = -1;
  }
  int maxClient = 0;

  int maxfd = listenFd;
  fd_set allset, interestSet;

  FD_ZERO(&allset);
  FD_SET(listenFd, &allset);
  interestSet = allset;

  struct sockaddr_in clientAddr;
  socklen_t slen = sizeof(clientAddr);
  char addr[INET_ADDRSTRLEN];
  int nready = 0;

  for (;;) {
    // When we call the function, we specify the values of the descriptors that
    // we are interested in, and on return, the result indicates which descriptors
    // are ready. Any descriptor that is not ready on return will have its
    // corresponding bit cleared in the descriptor set. To handle this, we turn
    // on all the bits in which we are interested in all the descriptor sets each
    // time we call select.

    allset = interestSet;
    nready = select(maxfd + 1, &allset, NULL, NULL, NULL);
    if (FD_ISSET(listenFd, &allset)) { // new client connection
      int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &slen);
      int i;
      for (i = 0; i < FD_SETSIZE; i++) {
        if (client[i] == -1) {
          client[i] = connFd;
          break;
        }
      }

      if (i == FD_SETSIZE) {
        coloredPerror("too many clients");
        exit(1);
      }

      inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
      snprintf(logBuf, sizeof(logBuf),
               ANSI_COLOR_GREEN "client from %s:%d connected" ANSI_COLOR_RESET,
               addr, clientAddr.sin_port);
      puts(logBuf);

      FD_SET(connFd, &interestSet);
      if (maxfd < connFd)
        maxfd = connFd;
      if (i > maxClient)
        maxClient = i;

      nready--;
      if (nready <= 0)
        continue;
    }
    for (int i = 0; i <= maxClient; i++) {
      if (client[i] == -1)
        continue;
      if (FD_ISSET(client[i], &allset)) {
        int rdn;
        if((rdn=read(client[i], sendBuff, 1024))==0){

          getpeername(client[i], (struct sockaddr *)&clientAddr, &slen);
          inet_ntop(AF_INET, &(clientAddr.sin_addr), addr, INET_ADDRSTRLEN);
          snprintf(logBuf, sizeof(logBuf),
                   ANSI_COLOR_BLUE "client from %s:%d disconnected" ANSI_COLOR_RESET,
                   addr, clientAddr.sin_port);
          puts(logBuf);

          close(client[i]);
          FD_CLR(client[i], &interestSet);
          client[i]=-1;
        }
        else{
          write(client[i], sendBuff, rdn);
        }
        nready--;
        if(nready<=0){
          break;
        }
      }
    }
  }

  return 0;
}
