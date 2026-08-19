#ifndef UPDSOCK_H
#define UPDSOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define MAX_USERNAME_LEN 32
#define MAX_BUFFER_SIZE 1024
#define BRDCS_PORT 40000
#define BRDCS_ADDR "255.255.255.255"

typedef struct {
    char username[MAX_USERNAME_LEN];
    char message[MAX_BUFFER_SIZE];
    char timestamp[20];
} Message;

typedef struct {
    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    char username[MAX_USERNAME_LEN];
    int running;
    pthread_t receive_thread;
    void (*message_callback)(const char*, const char*, const char*);
    void (*status_callback)(const char*);
} Client;

Client* chat_create(const char* username);
void get_current_time(char* buffer, size_t size);
void chat_set_callbacks(Client* client, 
                       void (*msg_cb)(const char*, const char*, const char*), 
                       void (*status_cb)(const char*));
void chat_set_running(Client* client, int running);
int chat_is_running(Client* client);
void chat_cleanup(Client* client);
void chat_destroy(Client* client);
int chat_send_message(Client* client, const char* message);
void* chat_receive_thread(void* arg);
int chat_run(Client* client);

#endif