#include <arpa/inet.h> // For sockaddr_in, inet_addr(), etc.
#include <cstring>
#include <iostream>
#include <random>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // For close()

char *message[] = {
    "ping",
    "hello",
    "exit",
};

int main(int argc, char *argv[]) {
  char *server_ip = "127.0.0.1";
  int port = 8888;
  if (argc >= 2) {
    printf("the ip addr is %s\n", argv[1]);
    server_ip = argv[1];
  }
  if (argc >= 3) {
    printf("the port is %s\n", argv[2]);
    port = atoi(argv[2]);
  }
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[1024] = {0};

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
  printf("the port is %d\n", server_addr.sin_port);
  printf("the addr is %d\n", server_addr.sin_addr);

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("connect failed\n");
    return -1;
  }

  // Create a random device and seed the generator
  std::random_device rd;  // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator

  // Define the range for the random numbers
  std::uniform_int_distribution<> dis(1, 100); // [1, 100]
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

  // Close the socket
  close(sockfd);

  return 0;
}
