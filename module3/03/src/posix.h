#ifndef POSIX_H
#define POSIX_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <mqueue.h>

#define MSG_SIZE 256
#define NORM_PRIORITY 1
#define EXIT_PRIORITY 10

typedef struct {
    char name[64];
    mqd_t receive_queue;
    mqd_t send_queue;
    int created_queues;
    volatile sig_atomic_t running;
} ChatInfo;

int initChat(ChatInfo *inf, const char *base_name);
void cleanChat(ChatInfo *inf);
void sendMSG(ChatInfo *inf, const char *msg, unsigned int priority);
int receiveMSG(ChatInfo *inf, char *buffer, unsigned int *priority);
void signals(int sig);
void *threads(void *arg);

#endif
