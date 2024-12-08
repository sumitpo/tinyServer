#ifndef NET_HPP
#define NET_HPP
#include <string>
class tcpConn {
public:
  tcpConn(const std::string serverAddr, int port);
  tcpConn();
  ~tcpConn();
  int run();

private:
  // private function
  int _initSvr();

private:
  // private data
  std::string _server_addr;
  int _port;

  int _listenFd;
  bool _inited;
  int _epFd;
  int _maxConn;
  int _currentConn;
};
#endif
