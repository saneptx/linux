#include"worker.h"
#include"threadPool.h"

int tidArrInit(tidArr_t *ptidArr,int workerNum){
    ptidArr->arr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    ptidArr->workerNum = workerNum;
    return 0;
}
void cleanUp(void *arg){
    threadPool_t * pthreadPool = (threadPool_t*)arg;
    printf("clean up!\n");
    pthread_mutex_unlock(&pthreadPool->mutex);
}
void *threadFunc(void* arg){
    threadPool_t * pthreadPool = (threadPool_t*)arg;
    while(1){
        int netfd;
        //上锁
        pthread_mutex_lock(&pthreadPool->mutex);
        //pthread_cleanup_push(cleanUp,pthreadPool);
        while(pthreadPool->exitFlag==0 && pthreadPool->taskQueue.queueSize <= 0){
            pthread_cond_wait(&pthreadPool->cond,&pthreadPool->mutex);
        }
        if(pthreadPool->exitFlag == 1){
            printf("worker going to exit!\n");
            pthread_mutex_unlock(&pthreadPool->mutex);
            pthread_exit(NULL);
        }
        netfd = pthreadPool->taskQueue.pFront->netfd;
        printf("worker got netfd = %d\n",netfd);
        deQueue(&pthreadPool->taskQueue);
        pthread_mutex_unlock(&pthreadPool->mutex);
        //pthread_cleanup_pop(1);
        //执行业务
        transFile(netfd);
        close(netfd);
    }
    return NULL;
}