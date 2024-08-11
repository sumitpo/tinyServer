#include "log.hpp"
#include <arpa/inet.h> // For sockaddr_in, inet_addr(), etc.
#include <cstring>
#include <iostream>
#include <random>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

const char *message[] = {
    "ping",
    "hello",
    "exit",
};
int conn(const char *server_ip, int port) {
  int sockfd;
  struct sockaddr_in server_addr;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error\n";
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported\n";
    return -1;
  }
  /*
  printf("the port is %d\n", server_addr.sin_port);
  printf("the addr is %d\n", server_addr.sin_addr);
  */

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    log_error("connect failed: %s", strerror(errno));
    return -1;
  }
  return sockfd;
}

int sendMsg(int sockfd) {
  char buffer[1024] = {0};
  // Create a random device and seed the generator
  std::random_device rd;  // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator

  // Define the range for the random numbers
  std::uniform_int_distribution<> dis(4, 20); // [1, 100]
  int round = dis(gen);
  for (int i = 0; i < round; i++) {
    int index = dis(gen);

    // Send message to server
    write(sockfd, message[index % 2], strlen(message[index % 2]));
    std::cout << "Message sent to server\n";

    // Receive response from server
    int valread = read(sockfd, buffer, 1024);
    std::cout << "Server response: " << std::string(buffer, valread) << "\n";
  }
  send(sockfd, "exit", 4, 0);
  return 0;
}

int client(int id, const char *server_ip, int port) {
  printf("begin the client %d\n", id);
  int sockfd = conn(server_ip, port);
  if (sockfd == -1) {
    return 0;
  }
  sendMsg(sockfd);
  // Close the socket
  close(sockfd);
  printf("end the client %d\n", id);
  return 0;
}

int launchMultiClient(int nClient, const char *server_ip, int port) {
  std::vector<std::thread> clients;
  for (int i = 0; i < nClient; i++) {
    clients.emplace_back(client, i, server_ip, port);
  }
  for (auto &th : clients) {
    if (th.joinable()) {
      th.join();
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  const char *server_ip = "127.0.0.1";
  int port = 8888;
  int nclient = 1;
  if (argc >= 2) {
    printf("the ip addr is %s\n", argv[1]);
    server_ip = argv[1];
  }
  if (argc >= 3) {
    printf("the port is %s\n", argv[2]);
    port = atoi(argv[2]);
  }
  if (argc >= 4) {
    printf("the client number is %s\n", argv[3]);
    nclient = atoi(argv[3]);
  }
  // client(server_ip, port);
  launchMultiClient(nclient, server_ip, port);

  return 0;
}
