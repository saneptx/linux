#ifndef __TASKQUEUE__
#define __TASKQUEUE__

typedef struct node_s{
    int netfd;
    struct node_s *next;
}node_t;
typedef struct taskQueue_s{
    node_t * pFront;
    node_t * pRear;
    int queueSize;
}taskQueue_t;
int taskQueueInit(taskQueue_t* ptaskqueue);
int enQueue(taskQueue_t* ptaskqueue,int netfd);
int deQueue(taskQueue_t* ptaskqueue);
#endif