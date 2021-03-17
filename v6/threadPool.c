#include "threadPool.h"
#include "utils.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

void initThreadPool(struct threadPool *thdP) {
  if (thdP == NULL) {
    coloredPerror("thread pool null pointer");
    exit(1);
  }
  if (pthread_mutex_init(&(thdP->taskLock), NULL) != 0) {
    coloredPerror("error pthread mutex init");
    exit(1);
  }
  if (pthread_cond_init(&(thdP->taskReady), NULL) != 0) {
    coloredPerror("error pthread cond init");
    exit(1);
  }
  thdP->shutdown = 0;
  thdP->taskHead = NULL;
  for (int i = 0; i < thdP->threadNum; i++)
    pthread_create(&thdP->threadsId[i], NULL, routine, (void *)thdP);
}
void createTpool(struct threadPool *thdP, int threadNum, int maxThreadNum,
                 int shutdown) {
  if (thdP == NULL) {
    coloredPerror("thread pool null pointer");
    exit(1);
  }
  thdP->threadNum = threadNum;
  if (thdP->threadNum == 0)
    thdP->threadNum = get_nprocs();

  thdP->maxThreadNum = maxThreadNum;
  thdP->shutdown = shutdown;
  thdP->threadsId = (pthread_t *)malloc(sizeof(pthread_t) * thdP->threadNum);
  return;
}
void addTaskTpool(struct threadPool *thdP, void *(*routine)(void *),
                  void *arg) {
  if (thdP == NULL) {
    coloredPerror("thread pool null pointer");
    exit(1);
  }
  struct tasks *taskp = (struct tasks *)malloc(sizeof(struct tasks));
  taskp->func = routine;
  taskp->arg = arg;

  pthread_mutex_lock(&thdP->taskLock);

  taskp->next = thdP->taskHead;
  thdP->taskHead = taskp;

  pthread_cond_broadcast(&thdP->taskReady);
  pthread_mutex_unlock(&thdP->taskLock);
  return;
}
void *routine(void *arg) {
  struct threadPool *thdP = (struct threadPool *)arg;

  struct tasks *task = NULL;
  for (;;) {
    pthread_mutex_lock(&thdP->taskLock);
#ifdef DEBUG
    printf("thread %lu enter mutex\n", pthread_self());
#endif

    // if there is no task, suspend until get a task signal
    while (thdP->taskHead == NULL || thdP->shutdown == 1)
      pthread_cond_wait(&thdP->taskReady, &thdP->taskLock);

    if (thdP->shutdown == 1) {
#ifdef DEBUG
      printf("thread %lu leave mutex\n", pthread_self());
#endif
      pthread_mutex_unlock(&thdP->taskLock);
      pthread_exit(0);
    }

    task = thdP->taskHead;
    thdP->taskHead = task->next;

#ifdef DEBUG
    printf("thread %lu leave mutex\n", pthread_self());
#endif

    pthread_mutex_unlock(&thdP->taskLock);

    task->func(task->arg);
    free(task);
  }
  return NULL;
}
void destoryThreadPool(struct threadPool *thdP) {
  if (thdP == NULL) {
    coloredPerror("thread pool null pointer");
    exit(1);
  }

  thdP->shutdown = 1;

  pthread_mutex_lock(&thdP->taskLock);
  pthread_cond_broadcast(&thdP->taskReady);
  pthread_mutex_lock(&thdP->taskLock);

  for (int i = 0; i < thdP->threadNum; i++)
    pthread_join(thdP->threadsId[i], NULL);

  free(thdP->threadsId);
}
