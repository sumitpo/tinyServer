#ifndef THREADPOOL_H
#define THREADPOOL_H
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct tasks {
  void *(*func)(void *);
  void *arg;
  struct tasks *next;
};
struct threadPool {
  int threadNum;
  int maxThreadNum;
  int shutdown;
  pthread_t *threadsId;
  struct tasks *taskHead;
  struct tasks *taskTail;
  pthread_mutex_t taskLock;
  pthread_cond_t taskReady;
};
void initThreadPool(struct threadPool *thdP);
void createTpool(struct threadPool *thdP, int threadNum, int maxThreadNum,
                 int shutdown);
void addTaskTpool(struct threadPool *thdP, void *(*routine)(void *), void *arg);
void *routine(void *arg);
void destoryThreadPool(struct threadPool *thdP);
