///01_chatServer.c
#include <func.h>
typedef struct Conn_s{
    int isAlive;
    int netfd;
    time_t lastActivate;
}Conn_t;
int main(int argc,char*argv[])
{
    // ./01_chatServer.c 127.0.0.1 1234
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

    int epfd = epoll_create(1);
    struct epoll_event events;
    events.events = EPOLLIN;
    events.data.fd = sockfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&events);
    char buf[4096];
    Conn_t conn[1024];
    time_t now;
    for(int i=0;i<1024;i++){
        conn[i].isAlive=0;
    }
    int fdtoidx[1024];// fdtoidx[fd]-->conn_t[i]
    for(int i=0;i<1024;i++){
        fdtoidx[i]=-1;
    }
    int curidx = 0;
    while(1){
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd,readySet,1024,1000);
        now = time(NULL);
        printf("now = %s\n",ctime(&now));
        for(int i=0;i<readyNum;i++){
            if(readySet[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                printf("id = %d,netfd = %d\n",curidx,netfd);
                conn[curidx].isAlive=1;
                conn[curidx].netfd = netfd;
                conn[curidx].lastActivate = time(NULL);
                fdtoidx[netfd] = curidx;
                events.events = EPOLLIN;
                events.data.fd = netfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,netfd,&events);
                curidx++;
            }else{
                int netfd = readySet[i].data.fd;
                bzero(buf,sizeof(buf));
                ssize_t sret = recv(netfd,buf,sizeof(buf),0);
                if(sret == 0){//断线处理
                    printf("one client is closed!\n");
                    epoll_ctl(epfd,EPOLL_CTL_DEL,netfd,NULL);
                    int idx = fdtoidx[netfd];
                    conn[idx].isAlive = 0;
                    fdtoidx[netfd] = -1;
                    close(netfd);
                    continue;
                }
                int idx = fdtoidx[netfd];
                conn[idx].lastActivate = time(NULL);//活跃时间重置
                for(int j=0;j<curidx;j++){
                    if(conn[j].isAlive==1&&conn[j].netfd!=netfd){
                        send(conn[j].netfd,buf,strlen(buf),0);
                    }
                }
            }
        }
        for(int i=0;i<curidx;++i){
            if(conn[i].isAlive == 1 && now - conn[i].lastActivate > 10){
                epoll_ctl(epfd,EPOLL_CTL_DEL,conn[i].netfd,NULL);
                close(conn[i].netfd);
                conn[i].isAlive = 0;
                fdtoidx[conn[i].netfd] = -1;
            }
        }
    }
    return 0;
}