#include"head.h"

int send_fd(int sockfd, int fd_to_send) {
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