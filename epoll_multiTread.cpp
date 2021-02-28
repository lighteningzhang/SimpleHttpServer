#include "include/util.h"
#include <sys/epoll.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
const int MX_EVENTS = 10;
const int MX_BUF_SZ = 1000;
int main() {
    // bind
    // listen
    std::cout << "Program starts ...." << std::endl;
    const char* SERVER_ADDR =  "127.0.0.1";
    const char* SERVER_PORT = "1234";
    int listenfd = openListenFd(const_cast<char*>(SERVER_ADDR), const_cast<char*>(SERVER_PORT));
    char buf[MX_BUF_SZ];

    struct epoll_event event;
    struct epoll_event recv_events[MX_EVENTS];
    struct sockaddr_in client_addr;
    
    int efd = epoll_create(MX_EVENTS);

    event.events = EPOLLIN;

    socklen_t client_addr_len = sizeof(client_addr);
    printf("Waiting for connection .... \n");
    
    event.data.fd = listenfd;
    addfd(efd, listenfd, true);

    create_work_threads();
    // accept
    while (1) {
        printf("epoll wait begin ... \n");
        int num_active_events = epoll_wait(efd, recv_events, MX_EVENTS, -1);

        int len;
        for (int i = 0; i < num_active_events; i ++) {
            if (!(recv_events[i].events & EPOLLIN)) {
                continue;
            }
        
            if (recv_events[i].data.fd == listenfd) {
                int connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_addr_len);
                event.data.fd = connfd;
                
                int err = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);
                if (err == -1) {
                    perror("epoll_ctl error\n");
                    exit(-1);
                }
            } else {
                int cur_fd = recv_events[i].data.fd;
                int len = read(cur_fd, buf, MX_BUF_SZ / 2);
                if (len == 0) {
                    int err = epoll_ctl(efd, EPOLL_CTL_DEL, cur_fd, NULL);
                    if (err == -1) {
                        perror("epoll_ctl del error\n");
                        exit(-1);
                    }
                    close(cur_fd);
                    printf("client %d closed connection\n", cur_fd);
                } else if (len < 0) {
                    perror("Read error\n");
                    exit(-1);
                } else {
                    for (int i = 0; i < len; i ++) {
                        buf[i] = toupper(buf[i]);
                    }
                    write(STDOUT_FILENO, buf, len);
                    write(cur_fd, buf, len);
                }
            } 
        }
        // }
    }   
    // read
    close(listenfd);
    close(efd);
    return 0;
}