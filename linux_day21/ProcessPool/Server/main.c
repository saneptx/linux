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
    epollAdd(epfd,sockfd);
    while(1){
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd,readySet,1024,-1);
        for(int i=0;i<readyNum;i++){
            if(readySet[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                printf("one client connected!\n");
                //...to do
            }
        }
    }
    return 0;
}