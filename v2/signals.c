#include "signals.h"
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
  // pid_t pid;
  int stat;

  // Establishing a signal handler and calling wait from that handler are
  // insufficient for preventing zombies. The problem is that all five signals
  // are generated before the signal handler is executed, and the signal handler
  // is executed only one time because Unix signals are normally not queued.

  /*
  pid = wait(&stat);
  */

  // so wait is replaced by waitpid, the following code handle SIGCHLD correctly
  // This version works because waitpid is called within a loop, fetching the
  // status of any of children that have terminated. the WNOHANG must be
  // specified: This tells waitpid not to block if there are running children
  // that have not yet terminated. but for wait, it can't be called within a
  // loop, for wait will block if there is running child process

  while (waitpid(-1, &stat, WNOHANG) > 0) {
    write(1, "child process exited, signal catch\n", 35);
  }
}
