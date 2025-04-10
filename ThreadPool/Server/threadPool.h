#ifndef __ThreadPool__
#define __ThreadPool__

#include<func.h>
#include"worker.h"
#include"taskQueue.h"
typedef struct threadPool_s{
    tidArr_t tidArr;//记录所有子线程信息
    taskQueue_t taskQueue;//任务队列
    pthread_mutex_t mutex;//锁
    pthread_cond_t cond;//条件变量
    int exitFlag;//退出标志位
}threadPool_t;
int threadPoolInit(threadPool_t *pthreadPool,int workerNum);
int makeWorker(threadPool_t *pthreadPool);
int tcpInit(const char *ip,const char* port,int *psockfd);
int epollAdd(int epfd,int fd);
int epollDel(int epfd,int fd);
int transFile(int netfd);
#endif