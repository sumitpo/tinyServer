#include "signals.h"
#include "color.h"
sigFunc *signal(int signo, sigFunc *func) {
  struct sigaction act, oact;

  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
  } else {
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART; /* SVR4, 44BSD */
#endif
  }
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return (oact.sa_handler);
}

void sigChild(int signo) {
  pid_t pid;
  int stat;
  pid = wait(&stat);
  wait(&stat);
  write(1, ANSI_COLOR_CYAN, 5);
  write(1, "[log] >>> child process exited, signal catch\n", 45);
  write(1, ANSI_COLOR_RESET, 4);
}
