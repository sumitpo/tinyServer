#ifndef SINGALS_H
#define SINGALS_H
#endif

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

typedef void sigFunc(int);
sigFunc * signal(int signo, sigFunc *func);
void sigChild(int signo);
