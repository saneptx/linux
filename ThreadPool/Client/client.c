#include<func.h>
//1.0
// int recvFile(int sockfd){
//     char filename[4096] = {0};
//     recv(sockfd,filename,sizeof(filename),0);
//     int fd = open(filename,O_CREAT|O_RDWR,0666);
//     char buf[1000] = {0};
//     ssize_t sret = recv(sockfd,buf,sizeof(buf),0);
//     write(fd,buf,sret);
//     return 0;
// }

//2.0
typedef struct train_s{
    int length;
    char data[1000];
}train_t;
// int recvFile(int sockfd){
//     char filename[4096] = {0};
//     train_t train;
//     recv(sockfd,&train.length,sizeof(train.length),0);
//     recv(sockfd,train.data,train.length,0);
//     memcpy(filename,train.data,train.length);

//     int fd = open(filename,O_CREAT|O_RDWR,0666);
//     recv(sockfd,&train.length,sizeof(train.length),0);
//     recv(sockfd,train.data,train.length,0);
//     if(write(fd,train.data,train.length)!=-1){
//         printf("Download Success!\n");
//     }
//     close(fd);
//     return 0;
// }

//3.0
// int recvFile(int sockfd){
//     char filename[4096] = {0};
//     train_t train;
//     recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL);
//     recv(sockfd,train.data,train.length,MSG_WAITALL);
//     memcpy(filename,train.data,train.length);

//     int fd = open(filename,O_CREAT|O_RDWR|O_TRUNC,0666);
//     while(1){
//         recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL);
//         if(train.length == 0){
//             break;
//         }   
//         recv(sockfd,train.data,train.length,MSG_WAITALL);
//         if(write(fd,train.data,train.length)==-1){
//             break;
//         }
//     }
//     printf("Download Success!\n");
//     close(fd);
//     return 0;
// }

//4.0 添加进度条 将recv()改为recvn()
int recvn(int sockfd,void *buf,long total){//防止粘包
    char *p = (char*)buf;
    long cursize = 0;
    while(cursize<total){
        size_t sret = recv(sockfd,p+cursize,total-cursize,0);
        cursize+=sret;
    }
    return 0;
}
// int recvFile(int sockfd){
//     //获取文件名
//     char filename[4096] = {0};
//     train_t train;
//     recvn(sockfd,&train.length,sizeof(train.length));
//     recvn(sockfd,train.data,train.length);
//     memcpy(filename,train.data,train.length);
//     //获取文件大小
//     off_t filesize;
//     recvn(sockfd,&train.length,sizeof(train.length));
//     recvn(sockfd,train.data,train.length);
//     memcpy(&filesize,train.data,train.length);
//     // printf("filesize=%ld\n",filesize);
//     off_t cursize = 0;
//     off_t lastsize = 0;//上次printf的时候的文件大小
//     off_t slice = filesize/10000;
//     //接收并写入文件
//     int fd = open(filename,O_CREAT|O_RDWR|O_TRUNC,0666);
//     while(1){
//         recvn(sockfd,&train.length,sizeof(train.length));
//         if(train.length == 0){
//             break;
//         }
//         cursize += train.length;
//         recvn(sockfd,train.data,train.length);
//         if(write(fd,train.data,train.length)==-1){
//             break;
//         }
//         if(cursize-lastsize > slice){
//             printf("%5.2lf%%\r",cursize*100.0/filesize);
//             lastsize = cursize;
//         }
//         fflush(stdout);
//     }
//     printf("100.00%%\n");
//     printf("Download Success!\n");
//     close(fd);
//     return 0;
// }

//5.0 接收大火车
int recvFile(int sockfd){
    //获取文件名
    char filename[4096] = {0};
    train_t train;
    recvn(sockfd,&train.length,sizeof(train.length));
    recvn(sockfd,train.data,train.length);
    memcpy(filename,train.data,train.length);
    //获取文件大小
    off_t filesize;
    recvn(sockfd,&train.length,sizeof(train.length));
    recvn(sockfd,train.data,train.length);
    memcpy(&filesize,train.data,train.length);
    
    //接收并写入文件
    int fd = open(filename,O_CREAT|O_RDWR|O_TRUNC,0666);
    ftruncate(fd,filesize);
    char *p = (char*)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    ERROR_CHECK(p,MAP_FAILED,"mmap");
    recvn(sockfd,p,filesize);
    munmap(p,filesize);
    printf("Download Success!\n");
    close(fd);
    return 0;
}
int main(int argc,char* argv[]){
    //./client 192.168.111.128 1234
    ARGS_CHECK(argc,3);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    ERROR_CHECK(ret,-1,"connect");
    printf("connected!\n");
    recvFile(sockfd);
    return 0;
}