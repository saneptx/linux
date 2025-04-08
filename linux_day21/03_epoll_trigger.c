///03_epoll_trigger.c
#include <func.h>

int main(int argc,char*argv[])
{
    // ./03_epoll_trigger 127.0.0.1 1234
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

    int netfd = accept(sockfd,NULL,NULL);
    printf("connected!\n");

    char buf[3]={0};
    int epfd = epoll_create(1);//创建epoll文件对象
    //设置监听
    struct epoll_event events; //什么情况就绪？就绪如何处理？
    events.events = EPOLLIN;//读就绪
    events.data.fd = STDIN_FILENO;//设置就绪时将STDIN_FILENO放入就绪队列
    epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&events);
    events.events = EPOLLIN|EPOLLET;//设置边缘触发
    events.data.fd = netfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,netfd,&events);

    while(1){
        struct epoll_event readyEvents[2];
        int readyNum = epoll_wait(epfd,readyEvents,2,-1);
        printf("epoll ready!\n");
        for(int i=0;i<readyNum;i++){
            if(readyEvents[i].data.fd==STDIN_FILENO){
                bzero(buf,sizeof(buf));
                ssize_t sret = read(STDIN_FILENO,buf,sizeof(buf));
                if(sret == 0){
                    send(netfd,buf,strlen(buf),0);
                    break;
                }
                send(netfd,buf,strlen(buf),0);
            }else if(readyEvents[i].data.fd==netfd){
                bzero(buf,sizeof(buf));
                while(1){
                    ssize_t sret = recv(netfd,buf,2,MSG_DONTWAIT);
                    if(sret==-1||sret==0){
                        //sret==-1缓冲区读完
                        //sret==0客户端断开连接
                        break;
                    }
                    printf("recv:%s\n",buf);
                }
                
            }
        }
    }
    close(sockfd);
    return 0;
}