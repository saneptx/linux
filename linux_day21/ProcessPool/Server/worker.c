#include"head.h"
int makeWorker(int workerNum,workerdata_t* workerArr){
    for(int i=0;i<workerNum;i++){
        int pipefd[2];
        socketpair(AF_LOCAL,SOCK_STREAM,0,pipefd);
        pid_t pid = fork();
        if(pid == 0){
            close(pipefd[1]);
            workLoop(pipefd[0]);
        }
        close(pipefd[0]);
        workerArr[i].status = FREE;
        workerArr[i].pid = pid;
        workerArr[i].pipeSockfd = pipefd[1];
        printf("i =%d,pid = %d,pipefd = %d\n",i,pid,pipefd[1]);
    }
}
int workLoop(int pipesockfd){
    while(1){
        int netfd;
        netfd = recv_fd(pipesockfd);//接受任务
        printf("begin work!\n");
        transFile(netfd);
        close(netfd);
        printf("work complete!\n");
        pid_t pid = getpid();//任务完成后向主进程发送自己的pid
        send(pipesockfd,&pid,sizeof(pid),0);
    }
}