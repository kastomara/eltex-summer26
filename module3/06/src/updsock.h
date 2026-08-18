#ifndef UPDSOCK_H
#define UPDSOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>    

#define MAX_BUFFER 1024
#define MAX_USER_LEN 32
#define BRDCS_PORT 40000
#define BRDCS_ADDR "255.255.255.255"

typedef struct {
    char username{MAX_USER_LEN};
    char message{MAX_BUFFER};
    char timestamp[20];
} Message;

typedef struct {
    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    char username{MAX_USER_LEN};
    int running;
    pthread_t recieve_thread;
} Client;

void get_current_time(char* buffer, size_t size);
void chat_set_callbacks(Client* client, void (*msg_cb)(const char*, const char*, const char*), void (*status_cb)(const char*));
void chat_set_running(Client* client, int running);
int chat_is_running(Client* client);
void chat_cleanup(Cleint* client);
void chat_destroy(Client* client);


#endif 