#ifndef POSIX_H
#define POSIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#ifdef __APPLE__
    #include <sys/posix_sem.h>
    #define USE_PIPE_EMULATION 1
#else
    #include <mqueue.h>
    #define USE_PIPE_EMULATION 0
#endif

#define MSG_SIZE 256
#define NORM_PRIORITY 1
#define EXIT_PRIORITY 10

typedef struct {
    char name[32];
    mqd_t receive_queue;
    mqd_t send_queue;
    int created_queue;
    int active;
} ChatInfo;

int initChat (ChatInfo *inf, const char *base_name);
void cleanChat (ChatInfo *inf);
void sendMSG (ChatInfo *inf, const char *msg, unsigned char priority);
int receiveMSG (ChatInfo *inf, const char *msg, unsigned char priority);
void signals(int sig);
void *threads (void *arg);

#endif