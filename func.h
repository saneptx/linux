#ifndef __WD_FUNC_H
#define __WD_FUNC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <strings.h>
#include <sys/epoll.h>
#define ARGS_CHECK(argc, n) {               \
   if (argc != n) {                        \
          fprintf(stderr, "ERROR: expected %d arguments.\n", n);      \
          exit(1);                            \
      }                                       \
}
#define ERROR_CHECK(retval, val, msg)   {   \
   if (retval == val) {                    \
          perror(msg);                        \
          exit(1);                            \
      }                                       \
}
#define SIZE(a) (sizeof(a)/sizeof(a[0]))
#define THREAD_ERROR_CHECK(ret,msg){if(ret!=0){fprintf(stderr,"%s:%s\n",msg,strerror(ret));}}
#endif

