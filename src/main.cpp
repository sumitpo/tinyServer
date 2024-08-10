#include "net.hpp"
#include "log.hpp"

int main() {
  log_set_level(LOG_WARN);
  tcpConn conn;
  conn.run();
  printf("hello world\n");
}
