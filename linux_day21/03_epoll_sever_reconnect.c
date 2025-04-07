///01_epoll_sever.c
#include <func.h>

int main(int argc,char*argv[])
{
    // ./01_epoll_sever.c 127.0.0.1 1234
    ARGS_CHECK(argc,3);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    int reuse = 1;
    int ret = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    ERROR_CHECK(ret,-1,"setsockopt");

    ret = bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    ERROR_CHECK(ret,-1,"bind");
    listen(sockfd,50);

    int netfd = -1;
    char buf[4096]={0};
    int epfd = epoll_create(1);//创建epoll文件对象
    //设置监听
    struct epoll_event events; 
    events.events = EPOLLIN;
    events.data.fd = sockfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&events);//开始只监听sockfd判断有没有客户端连接

    while(1){
        struct epoll_event readyEvents[1024];
        int readyNum = epoll_wait(epfd,readyEvents,1024,-1);

        for(int i=0;i<readyNum;i++){
            if(readyEvents[i].data.fd==sockfd){
                netfd = accept(sockfd,NULL,NULL);
                printf("客户端连接！,netfd=%d\n",netfd);
                epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,NULL);//有客户端连接后只对该客户端服务不再接收别的客户端连接
                events.events = EPOLLIN;
                events.data.fd = STDIN_FILENO;
                epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&events);//添加读缓冲区监听
                events.events = EPOLLIN;
                events.data.fd = netfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,netfd,&events);//添加客户端文件描述符监听接收数据
            }else if(netfd != -1 && readyEvents[i].data.fd == netfd){
                bzero(buf,sizeof(buf));
                ssize_t sret = recv(netfd,buf,sizeof(buf),0);
                if(sret == 0){
                    epoll_ctl(epfd,EPOLL_CTL_DEL,netfd,NULL);
                    close(netfd);
                    netfd = -1;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,STDIN_FILENO,NULL);
                    events.events = EPOLLIN;
                    events.data.fd = sockfd;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&events);//开始只监听sockfd判断有没有客户端连接
                    break;
                }
                printf("来自对方的消息：%s",buf);
            }else if(netfd != -1 && readyEvents[i].data.fd == STDIN_FILENO){
                bzero(buf,sizeof(buf));
                ssize_t sret = read(STDIN_FILENO,buf,sizeof(buf));
                send(netfd,buf,strlen(buf),0);
            }
            
        }
    }
    close(sockfd);
    return 0;
}