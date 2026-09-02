#ifndef UDP_SNIFFER_H
#define UDP_SNIFFER_H

#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/time.h>

typedef enum {
    FILTER_NONE = 0,
    FILTER_UDP_CHAT = 1,
    FILTER_DNS = 2
} FilterType;

void start_sniffer(FilterType filter, int chat_port, int packets_to_capture, const char* out_filename);
void print_mac(unsigned char* mac, char* buffer);

#endif