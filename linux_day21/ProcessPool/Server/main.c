#include"head.h"

int main(int argc,char* argv[]){
    // ./server 192.168.111.128 1234 3
    ARGS_CHECK(argc,4);
    int workerNum = atoi(argv[3]);
    workerdata_t* workerArr = (workerdata_t*)calloc(workerNum,sizeof(workerdata_t));
    makeWorker(workerNum,workerArr);
    int sockfd;
    tcpInit(argv[1],argv[2],&sockfd);
    //初始化epoll
    int epfd = epoll_create(1);
    epollAdd(epfd,sockfd);//监听有无客户端接入
    for(int i=0;i<workerNum;i++){
        epollAdd(epfd,workerArr[i].pipeSockfd);//监听父子进程之间的管道，用于监听子进程发来任务完成的信息
    }
    while(1){
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd,readySet,1024,-1);
        for(int i=0;i<readyNum;i++){
            if(readySet[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                printf("one client connected!\n");
                //顺序查找一个空闲子进程
                for(int j=0;j<workerNum;j++){
                    if(workerArr[j].status == FREE){
                        send_fd(workerArr[j].pipeSockfd,netfd);
                        workerArr[j].status = BUSY;
                        break;
                    }
                }
                close(netfd);
            }else{
                //某个子进程完成任务
                int pipesockfd = readySet[i].data.fd;
                //使用顺序查找从pipsockfd找到工人进程
                for(int j=0;j<workerNum;++j){
                    if(workerArr[j].pipeSockfd == pipesockfd){
                        pid_t pid;
                        recv(pipesockfd,&pid,sizeof(pid),0);
                        printf("%d,worker,pid = %d\n",j,pid);
                        workerArr[j].status = FREE;
                        break;
                    }
                }
            }
        }
    }
    return 0;
}