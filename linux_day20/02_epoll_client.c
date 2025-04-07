///02_epoll_client.c
#include <func.h>

int main(int argc,char*argv[])
{
    // ./02_epoll_client 127.0.0.1 1234
    ARGS_CHECK(argc,3);
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1],&serverAddr.sin_addr);
    //客户端先发消息
    sendto(sockfd,"hello",5,0,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    fd_set rdset;
    char buf[4096]={0};
    while(1){
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(sockfd,&rdset);
        select(sockfd+1,&rdset,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&rdset)){
            bzero(buf,sizeof(buf));
            ssize_t sret = read(STDIN_FILENO,buf,sizeof(buf));
            if(sret == 0){
                sendto(sockfd,buf,0,0,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
                break;
            }
        
            sendto(sockfd,buf,strlen(buf),0,
                   (struct sockaddr*)&serverAddr,sizeof(serverAddr));
        }
        if(FD_ISSET(sockfd,&rdset)){
            bzero(buf,sizeof(buf));
            ssize_t sret = recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
            if(sret == 0){
                break;
            }
            printf("来自对方的消息：%s",buf);
        }
    }
    close(sockfd);
    return 0;
}