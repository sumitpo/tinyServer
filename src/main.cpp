#include "log.hpp"
#include "net.hpp"

int main() {
  log_set_level(LOG_WARN);
  tcpConn conn;
  conn.run();
  printf("hello world\n");
}
