#include "include/util.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <include/web_func.h>

void Perror(const char* err) {
    perror(err);
    exit(-1);
}

void* work_thread_func(void* arg) {
    int work_thread_index = *(int* )(arg);
    delete arg;
    work_thread_arr[work_thread_index]->work();
}

void create_work_threads() {
    pthread_t tid;
    for (int i = 0; i < work_threads_num; i ++) {
        int* p;
        p = new int(i);
        if (pthread_create(&tid, NULL, work_thread_func, p) != 0) {
            delete [] work_thread_arr;
            Perror("Fail to create thread\n");
        }
        if (pthread_detach(tid) != 0) {
            delete [] work_thread_arr;
            Perror("Fail to detach thread\n");
        }
        printf("Successfully create the thread %d\n", tid);
    }
}

int openListenFd(char* ip, char* port_) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int port = atoi(port_);
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof sock_addr);

    sock_addr.sin_family = AF_INET; 
    inet_pton(AF_INET,  ip, &sock_addr.sin_addr);
    sock_addr.sin_port = htons(port);
    
    int ret = bind(fd, (struct sockaddr*) &sock_addr, sizeof sock_addr);
    assert(ret != -1);
    
    ret = listen(fd, MX_LISTEN_FD);
    assert(ret != -1);

    return fd;
}   

void setnonblocking(int fd) {
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

void addfd(int efd, int fd, bool oneshot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (oneshot) {
        event.events |= EPOLLONESHOT;
    }
    int err = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
    
    assert(err == -1);

    setnonblocking(fd);
}
