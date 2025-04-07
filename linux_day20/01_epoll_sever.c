///01_epoll_sever.c
#include <func.h>

int main(int argc,char*argv[])
{
    // ./01_epoll_sever.c 127.0.0.1 1234
    ARGS_CHECK(argc,3);
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1],&serverAddr.sin_addr);
    int ret = bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    ERROR_CHECK(ret,-1,"bind");
    //服务端recvfrom
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    char buf[4096] = {0};
    recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&clientAddr,&clientAddrSize);
    //fd_set rdset;
    //创建epoll文件对象
    int epfd = epoll_create(1);
    //设置监听
    struct epoll_event events;//什么情况就绪？就绪了怎么办？
    events.events = EPOLLIN;//设置可读时就绪
    events.data.fd = STDIN_FILENO;//设置就绪时将STDIN_FILENO放入就绪队列
    epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&events);
    events.events = EPOLLIN;//设置可读时就绪
    events.data.fd = sockfd;//设置就绪时将sockfd放入就绪队列
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&events);
    while(1){
        // FD_ZERO(&rdset);
        // FD_SET(STDIN_FILENO,&rdset);
        // FD_SET(sockfd,&rdset);
        // select(sockfd+1,&rdset,NULL,NULL,NULL);
        struct epoll_event readyset[2];
        int readyNum = epoll_wait(epfd,readyset,2,-1);
        for(int i=0;i<readyNum;i++){
            if(readyset[i].data.fd==STDIN_FILENO){
                bzero(buf,sizeof(buf));
                ssize_t sret = read(STDIN_FILENO,buf,sizeof(buf));
                if(sret == 0){
                    sendto(sockfd,buf,0,0,(struct sockaddr*)&clientAddr,clientAddrSize);
                    break;
                }
                sendto(sockfd,buf,strlen(buf),0,
                    (struct sockaddr*)&clientAddr,clientAddrSize);
            }
            else if(readyset[i].data.fd==sockfd){
                bzero(buf,sizeof(buf));
                ssize_t sret = recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
                if(sret == 0){
                    break;
                }
                printf("来自对方的消息：%s",buf); 
            }
        }
        // if(FD_ISSET(STDIN_FILENO,&rdset)){
        //     bzero(buf,sizeof(buf));
        //     ssize_t sret = read(STDIN_FILENO,buf,sizeof(buf));
        //     if(sret == 0){
        //         sendto(sockfd,buf,0,0,(struct sockaddr*)&clientAddr,clientAddrSize);
        //         break;
        //     }
        //     sendto(sockfd,buf,strlen(buf),0,
        //            (struct sockaddr*)&clientAddr,clientAddrSize);
        // }
        // if(FD_ISSET(sockfd,&rdset)){
        //     bzero(buf,sizeof(buf));
        //     ssize_t sret = recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
        //     if(sret == 0){
        //         break;
        //     }
        //     printf("来自对方的消息：%s",buf);
        // }

    }
    close(sockfd);
    return 0;
}