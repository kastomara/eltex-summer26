#ifndef CALC_H
#define CALC_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
} ip_address_t;

uint32_t ip_to_uint32(ip_address_t ip);
ip_address_t uint32_to_ip(uint32_t ip);
uint32_t get_network_address(uint32_t ip, uint32_t mask);
uint32_t get_broadcast_address(uint32_t network, uint32_t mask);
bool is_ip_in_subnet(uint32_t ip, uint32_t network, uint32_t mask);
ip_address_t generate_random_ip(void);
bool parse_ip(const char* str, ip_address_t* ip);
bool parse_mask(const char* str, uint32_t* mask);
int mask_to_prefix(uint32_t mask);
uint32_t prefix_to_mask(int prefix);
char* ip_to_string(ip_address_t ip, char* buffer);
bool is_private_ip(ip_address_t ip);
void sort_ips(ip_address_t* ips, int count);

#endif