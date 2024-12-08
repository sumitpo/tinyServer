#include "log.hpp"
#include "net.hpp"
#include <random>
#include <sched.h>
#include <unistd.h>

long getCPUCount() {
  long num_cores = sysconf(_SC_NPROCESSORS_ONLN);

  // Check if sysconf returned an error
  if (num_cores == -1) {
    log_error("Error: Unable to get the number of CPU cores.");
    return 1;
  }
  log_debug("%d cpus in this computer", num_cores);
  return num_cores;
}

int getRandomCPU() {
  // Initialize random number generator
  std::random_device rd;  // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator

  // Define the range for the random number
  std::uniform_int_distribution<> dis(0, getCPUCount()); // Range: 0 to 99

  // Generate a random number
  return dis(gen);
}

int stick2OneCPU() {
  // Define the CPU core to which the process should be bound
  const int cpu_core = getRandomCPU();

  // Create a CPU set to specify which CPU cores the process can run on
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);          // Clear the CPU set
  CPU_SET(cpu_core, &cpu_set); // Add the specified CPU core to the set

  // Get the PID of the current process
  pid_t pid = getpid();

  // Set the CPU affinity for the current process
  if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) == -1) {
    perror("sched_setaffinity");
    return 1;
  }
  log_info("running on cpu %d", sched_getcpu());
  return 0;
}

int main() {
  log_set_level(LOG_INFO);
  stick2OneCPU();
  tcpConn conn;
  conn.run();
  printf("hello world\n");
}
