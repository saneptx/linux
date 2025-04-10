#include"taskQueue.h"
#include<func.h>
int taskQueueInit(taskQueue_t* ptaskqueue){
    bzero(ptaskqueue,sizeof(taskQueue_t));
    return 0;
}
int enQueue(taskQueue_t* ptaskqueue,int netfd){
    node_t *pNew = (node_t*)calloc(1,sizeof(node_t));
    pNew->netfd = netfd;
    if(ptaskqueue->queueSize == 0){
        ptaskqueue->pFront = pNew;
        ptaskqueue->pRear = pNew;
    }else{
        ptaskqueue->pRear->next = pNew;
        ptaskqueue->pRear = pNew;
    }
    ptaskqueue->queueSize++;
    return 0;
}
int deQueue(taskQueue_t* ptaskqueue){
    node_t *pCur = ptaskqueue->pFront;
    ptaskqueue->pFront = pCur->next;
    if(ptaskqueue->queueSize == 1){
        ptaskqueue->pRear = NULL;
    }
    free(pCur);
    ptaskqueue->queueSize--;
    return 0;
}