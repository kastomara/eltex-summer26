#include "ipgen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

uint32_t ip_to_uint32(ip_address_t ip) {
    return ((uint32_t)ip.octet1 << 24) |
           ((uint32_t)ip.octet2 << 16) |
           ((uint32_t)ip.octet3 << 8) |
           (uint32_t)ip.octet4;
}

ip_address_t uint32_to_ip(uint32_t ip) {
    ip_address_t result;
    result.octet1 = (ip >> 24) & 0xFF;
    result.octet2 = (ip >> 16) & 0xFF;
    result.octet3 = (ip >> 8) & 0xFF;
    result.octet4 = ip & 0xFF;
    return result;
}

uint32_t get_network_address(uint32_t ip, uint32_t mask) {
    return ip & mask;
}

uint32_t get_broadcast_address(uint32_t network, uint32_t mask) {
    return network | ~mask;
}

bool is_ip_in_subnet(uint32_t ip, uint32_t network, uint32_t mask) {
    return (ip & mask) == network;
}

ip_address_t generate_random_ip(void) {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    ip_address_t ip;
    ip.octet1 = rand() % 256;
    ip.octet2 = rand() % 256;
    ip.octet3 = rand() % 256;
    ip.octet4 = rand() % 256;
    return ip;
}

bool parse_ip(const char* str, ip_address_t* ip) {
    unsigned int o1, o2, o3, o4;
    if (sscanf(str, "%u.%u.%u.%u", &o1, &o2, &o3, &o4) != 4) {
        return false;
    }
    
    if (o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255) {
        return false;
    }
    
    ip->octet1 = (uint8_t)o1;
    ip->octet2 = (uint8_t)o2;
    ip->octet3 = (uint8_t)o3;
    ip->octet4 = (uint8_t)o4;
    return true;
}

bool parse_mask(const char* str, uint32_t* mask) {
    if (str[0] == '/') {
        int prefix;
        if (sscanf(str, "/%d", &prefix) != 1) {
            return false;
        }
        if (prefix < 0 || prefix > 32) {
            return false;
        }
        *mask = prefix_to_mask(prefix);
        return true;
    }
    
    ip_address_t ip;
    if (!parse_ip(str, &ip)) {
        return false;
    }
    
    *mask = ip_to_uint32(ip);
    return true;
}

int mask_to_prefix(uint32_t mask) {
    int prefix = 0;
    uint32_t temp_mask = mask;
    while (temp_mask & 0x80000000) {
        prefix++;
        temp_mask <<= 1;
    }
    return prefix;
}

uint32_t prefix_to_mask(int prefix) {
    return prefix ? (0xFFFFFFFF << (32 - prefix)) : 0;
}

char* ip_to_string(ip_address_t ip, char* buffer) {
    sprintf(buffer, "%d.%d.%d.%d", ip.octet1, ip.octet2, ip.octet3, ip.octet4);
    return buffer;
}

bool is_private_ip(ip_address_t ip) {
    if (ip.octet1 == 10) return true;
    if (ip.octet1 == 172 && ip.octet2 >= 16 && ip.octet2 <= 31) return true;
    if (ip.octet1 == 192 && ip.octet2 == 168) return true;
    if (ip.octet1 == 127) return true;
    return false;
}

void sort_ips(ip_address_t* ips, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            uint32_t ip1 = ip_to_uint32(ips[i]);
            uint32_t ip2 = ip_to_uint32(ips[j]);
            if (ip1 > ip2) {
                ip_address_t temp = ips[i];
                ips[i] = ips[j];
                ips[j] = temp;
            }
        }
    }
}