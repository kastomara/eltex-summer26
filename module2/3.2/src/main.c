#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"

// Функция для очистки буфера ввода
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Функция для интерактивного ввода
void interactive_mode() {
    char ip_str[20];
    char mask_str[20];
    char count_str[20];
    ip_address_t subnet_ip;
    uint32_t mask;
    int packet_count;
    
    printf("\n=== Интерактивный режим ===\n");
    
    // Ввод IP-адреса подсети
    do {
        printf("Введите IP-адрес подсети (например, 192.168.1.0): ");
        if (fgets(ip_str, sizeof(ip_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        ip_str[strcspn(ip_str, "\n")] = '\0'; // Удаляем символ новой строки
        
        if (!parse_ip(ip_str, &subnet_ip)) {
            printf("Ошибка: Неверный формат IP-адреса. Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    // Ввод маски подсети
    do {
        printf("Введите маску подсети (/24 или 255.255.255.0): ");
        if (fgets(mask_str, sizeof(mask_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        mask_str[strcspn(mask_str, "\n")] = '\0';
        
        if (!parse_mask(mask_str, &mask)) {
            printf("Ошибка: Неверный формат маски. Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    // Ввод количества пакетов
    do {
        printf("Введите количество пакетов (положительное число): ");
        if (fgets(count_str, sizeof(count_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        count_str[strcspn(count_str, "\n")] = '\0';
        
        packet_count = atoi(count_str);
        if (packet_count <= 0) {
            printf("Ошибка: Количество пакетов должно быть положительным числом. Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    // Выполняем расчеты
    process_packets(subnet_ip, mask, packet_count);
}

// Функция для обработки пакетов и вывода статистики
void process_packets(ip_address_t subnet_ip, uint32_t mask, int packet_count) {
    // Преобразование IP подсети в 32-битное число
    uint32_t subnet_uint = ip_to_uint32(subnet_ip);
    uint32_t network_address = get_network_address(subnet_uint, mask);
    
    // Вывод информации о подсети
    ip_address_t network_ip = uint32_to_ip(network_address);
    printf("\n=== Информация о подсети ===\n");
    printf("IP подсети: %d.%d.%d.%d\n", subnet_ip.octet1, subnet_ip.octet2, 
           subnet_ip.octet3, subnet_ip.octet4);
    printf("Маска: %d.%d.%d.%d\n", ((mask >> 24) & 0xFF), ((mask >> 16) & 0xFF),
           ((mask >> 8) & 0xFF), (mask & 0xFF));
    
    // Вычисляем префикс маски
    int prefix = 0;
    uint32_t temp_mask = mask;
    while (temp_mask & 0x80000000) {
        prefix++;
        temp_mask <<= 1;
    }
    printf("Маска (префикс): /%d\n", prefix);
    
    printf("Сетевой адрес: %d.%d.%d.%d\n", network_ip.octet1, network_ip.octet2,
           network_ip.octet3, network_ip.octet4);
    
    // Вычисляем broadcast адрес
    uint32_t broadcast = network_address | ~mask;
    ip_address_t broadcast_ip = uint32_to_ip(broadcast);
    printf("Broadcast адрес: %d.%d.%d.%d\n", broadcast_ip.octet1, broadcast_ip.octet2,
           broadcast_ip.octet3, broadcast_ip.octet4);
    
    printf("Количество пакетов: %d\n\n", packet_count);
    
    // Генерация и проверка пакетов
    int in_subnet = 0;
    ip_address_t packet_ip;
    uint32_t packet_uint;
    
    printf("=== Результаты проверки пакетов ===\n");
    for (int i = 0; i < packet_count; i++) {
        packet_ip = generate_random_ip();
        packet_uint = ip_to_uint32(packet_ip);
        
        bool is_in = is_ip_in_subnet(packet_uint, network_address, mask);
        if (is_in) {
            in_subnet++;
        }
        
        printf("Пакет %4d: %3d.%3d.%3d.%3d - %s\n", 
               i + 1,
               packet_ip.octet1, packet_ip.octet2, 
               packet_ip.octet3, packet_ip.octet4,
               is_in ? "В подсети" : "Вне подсети");
    }
    
    // Статистика
    double percent = (double)in_subnet / packet_count * 100.0;
    printf("\n=== Статистика ===\n");
    printf("Всего пакетов: %d\n", packet_count);
    printf("В подсети: %d (%.2f%%)\n", in_subnet, percent);
    printf("Вне подсети: %d (%.2f%%)\n", packet_count - in_subnet, 100.0 - percent);
}

// Функция для вывода справки
void print_help(const char* program_name) {
    printf("Использование:\n");
    printf("  1. Интерактивный режим: %s\n", program_name);
    printf("  2. Режим командной строки: %s <IP_подсети> <маска> <количество_пакетов>\n", program_name);
    printf("\nПримеры:\n");
    printf("  %s 192.168.1.0 /24 100\n", program_name);
    printf("  %s 10.0.0.0 255.0.0.0 50\n", program_name);
    printf("  %s 172.16.0.0 /16 20\n", program_name);
    printf("\nМаска может быть в формате:\n");
    printf("  - /24 (префиксная нотация)\n");
    printf("  - 255.255.255.0 (точечная десятичная нотация)\n");
}

int main(int argc, char* argv[]) {
    // Если передан ключ -h или --help, выводим справку
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help(argv[0]);
        return 0;
    }
    
    // Если аргументов нет - запускаем интерактивный режим
    if (argc == 1) {
        interactive_mode();
        return 0;
    }
    
    // Режим командной строки
    if (argc == 4) {
        ip_address_t subnet_ip;
        uint32_t mask;
        int packet_count;
        
        // Парсинг IP-адреса подсети
        if (!parse_ip(argv[1], &subnet_ip)) {
            printf("Ошибка: Неверный формат IP-адреса: %s\n", argv[1]);
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        // Парсинг маски подсети
        if (!parse_mask(argv[2], &mask)) {
            printf("Ошибка: Неверный формат маски: %s\n", argv[2]);
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        // Парсинг количества пакетов
        packet_count = atoi(argv[3]);
        if (packet_count <= 0) {
            printf("Ошибка: Количество пакетов должно быть положительным числом\n");
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        // Обработка пакетов
        process_packets(subnet_ip, mask, packet_count);
        return 0;
    }
    
    // Неправильное количество аргументов
    printf("Ошибка: Неправильное количество аргументов\n");
    print_help(argv[0]);
    return 1;
}