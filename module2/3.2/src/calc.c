#include "calc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Преобразование IP-адреса в 32-битное число
uint32_t ip_to_uint32(ip_address_t ip) {
    return ((uint32_t)ip.octet1 << 24) |
           ((uint32_t)ip.octet2 << 16) |
           ((uint32_t)ip.octet3 << 8) |
           (uint32_t)ip.octet4;
}

// Преобразование 32-битного числа в IP-адрес
ip_address_t uint32_to_ip(uint32_t ip) {
    ip_address_t result;
    result.octet1 = (ip >> 24) & 0xFF;
    result.octet2 = (ip >> 16) & 0xFF;
    result.octet3 = (ip >> 8) & 0xFF;
    result.octet4 = ip & 0xFF;
    return result;
}

// Получение сетевого адреса (IP & маска)
uint32_t get_network_address(uint32_t ip, uint32_t mask) {
    return ip & mask;
}

// Проверка принадлежности IP-адреса подсети
bool is_ip_in_subnet(uint32_t ip, uint32_t network, uint32_t mask) {
    return (ip & mask) == network;
}

// Генерация случайного IP-адреса
ip_address_t generate_random_ip(void) {
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    
    ip_address_t ip;
    ip.octet1 = rand() % 256;
    ip.octet2 = rand() % 256;
    ip.octet3 = rand() % 256;
    ip.octet4 = rand() % 256;
    return ip;
}

// Парсинг строки IP-адреса
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

// Парсинг маски подсети
bool parse_mask(const char* str, uint32_t* mask) {
    // Проверяем формат /24
    if (str[0] == '/') {
        int prefix;
        if (sscanf(str, "/%d", &prefix) != 1) {
            return false;
        }
        if (prefix < 0 || prefix > 32) {
            return false;
        }
        *mask = prefix ? (0xFFFFFFFF << (32 - prefix)) : 0;
        return true;
    }
    
    // Проверяем формат xxx.xxx.xxx.xxx
    ip_address_t ip;
    if (!parse_ip(str, &ip)) {
        return false;
    }
    
    *mask = ip_to_uint32(ip);
    return true;
}