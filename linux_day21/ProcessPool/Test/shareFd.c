#include<func.h>

int send_fd(int sockfd, int fd_to_send) {
    /*
        struct msghdr {
            void         *msg_name;       // 目标地址（对 socketpair/UNIX 域通常为 NULL）
            socklen_t     msg_namelen;    // 地址长度通常为0
            struct iovec *msg_iov;        // 数据缓冲区数组
            int           msg_iovlen;     // 数据缓冲区数量
            void         *msg_control;    // 控制消息缓冲区（用于传递文件描述符）
            socklen_t     msg_controllen; // 控制消息长度
            int           msg_flags;      // 接收标志（recvmsg 时用）
        };
        struct iovec{
        	void *iov_base;
        	size_t lov_len;
        }
        struct cmsghdr {
            size_t cmsg_len;	//整个结构体的实际长度
            int    cmsg_level;  //发送文件对象时cmsg_level=SOL_SOCKET
            int    cmsg_type;   //发送文件对象时type=SCM_RIGHTS
            unsigned char cmsg_data[]; //若干个int，存放文件对象的文件描述符
        };

    */
    struct msghdr msg;
    struct iovec iov[1];
    char buf[1];  // 需要发送至少1字节数据
    
    // 设置要发送的数据
    buf[0] = '0';  // 任意值，必须有内容
    iov[0].iov_base = buf;
    iov[0].iov_len = 1;
    
    // 设置消息头
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    
    // 准备发送文件描述符的控制信息
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    
    struct cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmptr)) = fd_to_send;
    
    
    return sendmsg(sockfd, &msg, 0);
}
int recv_fd(int sockfd) {
    struct msghdr msg;
    struct iovec iov[1];
    char buf[1];
    int fd = -1;
    
    // 设置接收缓冲区
    iov[0].iov_base = buf;
    iov[0].iov_len = 1;
    
    // 设置消息头
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    
    // 准备接收控制信息
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    
    // 接收消息
    if (recvmsg(sockfd, &msg, 0) <= 0) {
        return -1;
    }
    
    // 处理控制信息
    struct cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
    if (cmptr != NULL && 
        cmptr->cmsg_len == CMSG_LEN(sizeof(int)) &&
        cmptr->cmsg_level == SOL_SOCKET && 
        cmptr->cmsg_type == SCM_RIGHTS) {
        fd = *((int *)CMSG_DATA(cmptr));
    } else {
        fd = -1;
    }
    
    return fd;
}
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
        send_fd(fds[1],fdfile);
        wait(NULL);
    }else{
        //子进程
        close(fds[1]);
        int fdfile = recv_fd(fds[0]);
        printf("child fdfile = %d\n",fdfile);
        ssize_t sret = write(fdfile,"world",5);
        printf("sret = %ld\n",sret);
    }
    return 0;
}