#include<func.h>
enum{
    FREE,
    BUSY
};
// 主进程用于保存工作进程信息
typedef struct workerdata_s
{
    pid_t pid;
    int status;
    int pipeSockfd;//父子进程通信的socketpair的fd
}workerdata_t;
int makeWorker(int workerNum,workerdata_t* workerArr);
int tcpInit(const char *ip,const char* port,int *psockfd);
int epollAdd(int epfd,int fd);
int epollDel(int epfd,int fd);
int send_fd(int sockfd, int fd_to_send,int exitFlag);
int recv_fd(int sockfd,int* pexitFlag);
int workLoop(int sockfd);
int transFile(int netfd);