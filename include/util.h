#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/epoll.h>
/*
int socket(int domain, int type, int protocol);
*/
#define MX_LISTEN_FD 2048

int openListenFd(char* ip, char* port_);
void addfd(int efd, int fd, bool oneshot);
void Perror(const char* err);