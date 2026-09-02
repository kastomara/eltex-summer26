#ifndef TCPSOCK_H
#define TCPSOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define MAX_USERNAME_LEN 32
#define MAX_CLIENTS 50
#define BRDCS_PORT 40000

typedef struct {
    int master_socket;
    struct sockaddr_in address;
    int addrlen;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds;                           
} ServerState;

typedef struct {
    int sock;
    int max_sd;
    int running;
    char username[MAX_USERNAME_LEN];
    char msg_buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    fd_set readfds;
} ClientState;

#endif