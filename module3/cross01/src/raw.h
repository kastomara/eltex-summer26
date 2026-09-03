#ifndef RAW_H
#define RAW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>

#define MAX_PAYLOAD 1024
#define BUFFER_SIZE 4096
#define CLOSE_CMD "__CLOSE_CONNECTION__"
#define MAX_CLIENTS 100

typedef struct {
    uint32_t ip;
    uint16_t port;
    int counter;
    int active;
} ClientState;

#endif