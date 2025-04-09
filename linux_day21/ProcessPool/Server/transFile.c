#include "head.h"
//1.0
// int transFile(int netfd){
//     char filename[] = "file1";
//     send(netfd,filename,5,0);
//     int fd = open(filename,O_RDWR);
//     char buf[1000] = {0};
//     size_t sret = read(fd,buf,sizeof(buf));
//     send(netfd,buf,sret,0);
//     return 0; 
// }

//2.0
typedef struct train_s{
    int length;
    char data[1000];
}train_t;
// int transFile(int netfd){
//     train_t train;
//     char filename[] = "file1";
//     train.length = strlen(filename);
//     memcpy(train.data,filename,train.length);
//     send(netfd,&train.length,sizeof(train.length),0);
//     send(netfd,train.data,train.length,0);

//     int fd = open(filename,O_RDWR);
//     size_t sret = read(fd,train.data,sizeof(train.data));
//     train.length = sret;
//     send(netfd,&train.length,sizeof(train.length),0);
//     send(netfd,train.data,train.length,0);
//     close(fd);
//     return 0; 
// }

//3.0
// int transFile(int netfd){
//     train_t train;
//     char filename[] = "file1";
//     train.length = strlen(filename);
//     memcpy(train.data,filename,train.length);
//     send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
//     send(netfd,train.data,train.length,MSG_NOSIGNAL);

//     int fd = open(filename,O_RDWR);
//     while(1){
//         size_t sret = read(fd,train.data,sizeof(train.data));
//         train.length = sret;
//         if(send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL)==-1){
//             break;
//         }
//         if(send(netfd,train.data,train.length,MSG_NOSIGNAL)==-1){
//             break;
//         }
//         sleep(1);
//         if(sret == 0){
//             break;
//         }
//     }
    
//     close(fd);
//     return 0; 
// }

//4.0 添加进度条
// int transFile(int netfd){
//     train_t train;
//     //发送文件名
//     char filename[] = "file1";
//     train.length = strlen(filename);
//     memcpy(train.data,filename,train.length);
//     send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
//     send(netfd,train.data,train.length,MSG_NOSIGNAL);
//     //发送文件大小
//     int fd = open(filename,O_RDWR);
//     struct stat statbuf;
//     fstat(fd,&statbuf);
//     train.length = sizeof(off_t);
//     memcpy(train.data,&statbuf.st_size,train.length);
//     send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
//     send(netfd,train.data,train.length,MSG_NOSIGNAL);
//     //发送文件
//     while(1){
//         size_t sret = read(fd,train.data,sizeof(train.data));
//         train.length = sret;
//         if(send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL)==-1){
//             break;
//         }
//         if(send(netfd,train.data,train.length,MSG_NOSIGNAL)==-1){
//             break;
//         }
//         usleep(100);
//         if(sret == 0){
//             break;
//         }
//     }
    
//     close(fd);
//     return 0; 
// }

//4.1 零拷贝优化
int transFile(int netfd){
    train_t train;
    //发送文件名
    char filename[] = "file1";
    train.length = strlen(filename);
    memcpy(train.data,filename,train.length);
    send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
    send(netfd,train.data,train.length,MSG_NOSIGNAL);
    //发送文件大小
    int fd = open(filename,O_RDWR);//open必须用O_RDWR
    struct stat statbuf;
    fstat(fd,&statbuf);
    train.length = sizeof(off_t);
    memcpy(train.data,&statbuf.st_size,train.length);
    send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
    send(netfd,train.data,train.length,MSG_NOSIGNAL);
    //发送文件
    char *p = (char*)mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    ERROR_CHECK(p,MAP_FAILED,"mmap");
    off_t cursize = 0;
    while(1){
        if(cursize >= statbuf.st_size)
            break;
        if(statbuf.st_size - cursize >= 1000){
            train.length = 1000;
        }else{
            train.length = statbuf.st_size - cursize;
        }
        send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
        send(netfd,p+cursize,train.length,MSG_NOSIGNAL);//data部分的数据都是从映射区发出的
        usleep(100);
        cursize += train.length;
    }
    train.length = 0;
    send(netfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
    munmap(p,statbuf.st_size);
    close(fd);
    return 0; 
}