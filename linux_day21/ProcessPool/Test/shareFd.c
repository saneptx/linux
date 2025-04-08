#include<func.h>

int main(){
    int fds[2];
    //pipe(fds);//fds[0]:read,fds[1]:write
    socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
    if(fork()){
        //父进程
        close(fds[0]);
        int fdfile = open("file1",O_RDWR);
        printf("parent fdfile = %d\n",fdfile);
        write(fdfile,"hello",5);
        send(fds[1],&fdfile,sizeof(int),0);
        wait(NULL);
    }else{
        //子进程
        close(fds[1]);
        int fdfile;
        recv(fds[0],&fdfile,sizeof(int),0);
        printf("child fdfile = %d\n",fdfile);
        ssize_t sret = write(fdfile,"world",5);
        printf("sret = %ld\n",sret);
    }
    return 0;
}