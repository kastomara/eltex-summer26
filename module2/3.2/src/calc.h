#ifndef CALC_H
#define CALC_H

#include <stdint.h>
#include <stdbool.h>

// Структура для хранения IP-адреса
typedef struct {
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
} ip_address_t;

// Преобразование IP-адреса в 32-битное число
uint32_t ip_to_uint32(ip_address_t ip);

// Преобразование 32-битного числа в IP-адрес
ip_address_t uint32_to_ip(uint32_t ip);

// Получение сетевого адреса (IP & маска)
uint32_t get_network_address(uint32_t ip, uint32_t mask);

// Проверка принадлежности IP-адреса подсети
bool is_ip_in_subnet(uint32_t ip, uint32_t network, uint32_t mask);

// Генерация случайного IP-адреса
ip_address_t generate_random_ip(void);

// Парсинг строки IP-адреса
bool parse_ip(const char* str, ip_address_t* ip);

// Парсинг маски подсети (в формате /24 или xxx.xxx.xxx.xxx)
bool parse_mask(const char* str, uint32_t* mask);

#endif // CALC_H