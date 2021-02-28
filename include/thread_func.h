#include <sys/epoll.h>
#include <sys/socket.h>
#include "util.h"
#include "web_func.h"
const int INIT_EVENT_NUM = 10, MX_USER_NUM = 1000, MX_EVENTS_NUM = 65535, MX_BUFFER_SZ = 1024;
work_thread<http_conn> work_threads;
template<typename T> 
class work_thread {
public: 
    /***
     * 初始化工作进程，需要考虑用户数
     *
     ***/
    work_thread() : user_num(0) {
        efd = epoll_create(INIT_EVENT_NUM);
        assert(efd != -1);

        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
        assert(ret != -1);
        /***
         * ?? 这里为什么要设置为非阻塞
         ***/
        // setnonblocking(pipefd[1]); 
        addfd(efd, pipefd[0]);
        users = new T[MX_USER_NUM];
        for (int i = 0; i < MX_USER_NUM; i ++) {
            free_list.push_back(i);
        }
    }

    void work();

private:
    int user_num;
    int efd;
    epoll_event events[MX_EVENTS_NUM];
    int pipefd[2];
    T* users[MX_USER_NUM];
    int listenfd;
    std::list<int> free_list;
};

template<typename T> void  work_thread<T>::work() {
    // new connection
    int conn = accept()
    while (1) {
        int ready_events = epoll_wait(efd, events, MX_EVENTS_NUM, -1);
        if (ready_events == -1) {
            Perror("epoll wait error\n");
        }
        for (int i = 0; i < ready_events; i ++) {
            int ready_event_fd = events[i].data.fd;
            epoll_event ev = events[i];
            if ((ready_event_fd == pipefd[0]) && (ev.events && EPOLLIN)) {
                char info[MX_BUFFER_SZ];
                int recv_info_lens = recv(ev.data.fd, info, sizeof info, 0);
                if (recv_infor_lens == -1) {
                    Perror("recv error\n");
                } else {
                    for (int j = 0; j < ret; j ++) {
                        switch (info[j]) {
                            case NEW_CONN:
                            {
                                struct sockaddr_in client_addr;
                                int connfd = accept(listenfd, (struct sockaddr*)& client_addr, sizeof client_addr);
                                if (!free_list.empty()) {
                                    int assign_idx = free_list.front();
                                    free_list.pop_front();

                                    users[assign_idx].init();
                                    
                                }
                            }
                            default:
                                break;
                        }
                    }
                }

                
            }
        }
    }
}