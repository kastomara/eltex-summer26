#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD
#include "ipgen.h"
=======
#include "calc.h"
>>>>>>> a2f58e574dc0213e328b25463792a31c11913793

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void print_subnet_info(ip_address_t subnet_ip, uint32_t mask) {
    uint32_t subnet_uint = ip_to_uint32(subnet_ip);
    uint32_t network_address = get_network_address(subnet_uint, mask);
    uint32_t broadcast = get_broadcast_address(network_address, mask);
    
    ip_address_t network_ip = uint32_to_ip(network_address);
    ip_address_t broadcast_ip = uint32_to_ip(broadcast);
    
    int prefix = mask_to_prefix(mask);
    
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║           ИНФОРМАЦИЯ О ПОДСЕТИ                      ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║ IP подсети:        %d.%d.%d.%d                     ║\n", 
           subnet_ip.octet1, subnet_ip.octet2, subnet_ip.octet3, subnet_ip.octet4);
    printf("║ Маска:             %d.%d.%d.%d                     ║\n",
           ((mask >> 24) & 0xFF), ((mask >> 16) & 0xFF),
           ((mask >> 8) & 0xFF), (mask & 0xFF));
    printf("║ Префикс:           /%-27d ║\n", prefix);
    printf("║ Сетевой адрес:     %d.%d.%d.%d                     ║\n", 
           network_ip.octet1, network_ip.octet2, network_ip.octet3, network_ip.octet4);
    printf("║ Broadcast адрес:   %d.%d.%d.%d                     ║\n", 
           broadcast_ip.octet1, broadcast_ip.octet2, broadcast_ip.octet3, broadcast_ip.octet4);
    
    uint32_t host_count = ~mask;
    if (host_count != 0xFFFFFFFF) {
        uint32_t available_hosts = host_count - 1;
        if (available_hosts > 0) {
            printf("║ Доступно хостов:   %-27u ║\n", available_hosts);
        } else {
            printf("║ Доступно хостов:   %-27s ║\n", "0 (сеть из одного хоста)");
        }
    }
    printf("╚══════════════════════════════════════════════════════╝\n\n");
}

void print_packet_results(ip_address_t* packets, bool* results, int packet_count) {
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                       РЕЗУЛЬТАТЫ ПРОВЕРКИ                          ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ №   IP-адрес              Статус         Тип сети                   ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    
    int in_subnet = 0;
    int private_count = 0;
    ip_address_t sorted_ips[packet_count];
    
    for (int i = 0; i < packet_count; i++) {
        sorted_ips[i] = packets[i];
        if (results[i]) {
            in_subnet++;
        }
        if (is_private_ip(packets[i])) {
            private_count++;
        }
    }
    
    sort_ips(sorted_ips, packet_count);
    
    for (int i = 0; i < packet_count; i++) {
        char ip_str[20];
        ip_to_string(sorted_ips[i], ip_str);
        
        bool is_in = false;
        for (int j = 0; j < packet_count; j++) {
            if (ip_to_uint32(sorted_ips[i]) == ip_to_uint32(packets[j])) {
                is_in = results[j];
                break;
            }
        }
        
        printf("║ %-3d %-19s %-12s %-30s ║\n",
               i + 1,
               ip_str,
               is_in ? "В подсети" : "Вне подсети",
               is_private_ip(sorted_ips[i]) ? "Частный" : "Публичный");
    }
    
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n\n");
    
    double percent_in = (double)in_subnet / packet_count * 100.0;
    double percent_private = (double)private_count / packet_count * 100.0;
    
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         СТАТИСТИКА                                 ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Всего пакетов:        %-40d ║\n", packet_count);
    printf("║ В подсети:            %-40d (%.2f%%) ║\n", in_subnet, percent_in);
    printf("║ Вне подсети:          %-40d (%.2f%%) ║\n", packet_count - in_subnet, 100.0 - percent_in);
    printf("║ Частные IP:           %-40d (%.2f%%) ║\n", private_count, percent_private);
    printf("║ Публичные IP:         %-40d (%.2f%%) ║\n", packet_count - private_count, 100.0 - percent_private);
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
}

void process_packets(ip_address_t subnet_ip, uint32_t mask, int packet_count) {
    uint32_t subnet_uint = ip_to_uint32(subnet_ip);
    uint32_t network_address = get_network_address(subnet_uint, mask);
    
    print_subnet_info(subnet_ip, mask);
    
    ip_address_t* packets = malloc(packet_count * sizeof(ip_address_t));
    bool* results = malloc(packet_count * sizeof(bool));
    
    if (!packets || !results) {
        printf("Ошибка выделения памяти\n");
        free(packets);
        free(results);
        return;
    }
    
    for (int i = 0; i < packet_count; i++) {
        packets[i] = generate_random_ip();
        uint32_t packet_uint = ip_to_uint32(packets[i]);
        results[i] = is_ip_in_subnet(packet_uint, network_address, mask);
    }
    
    print_packet_results(packets, results, packet_count);
    
    free(packets);
    free(results);
}

void interactive_mode() {
    char ip_str[20];
    char mask_str[20];
    char count_str[20];
    ip_address_t subnet_ip;
    uint32_t mask;
    int packet_count;
    
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║         ИНТЕРАКТИВНЫЙ РЕЖИМ                     ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    do {
        printf("Введите IP-адрес подсети (например, 192.168.1.0): ");
        if (fgets(ip_str, sizeof(ip_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        ip_str[strcspn(ip_str, "\n")] = '\0';
        
        if (!parse_ip(ip_str, &subnet_ip)) {
            printf("✗ Ошибка: Неверный формат IP-адреса. Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    do {
        printf("Введите маску подсети (/24 или 255.255.255.0): ");
        if (fgets(mask_str, sizeof(mask_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        mask_str[strcspn(mask_str, "\n")] = '\0';
        
        if (!parse_mask(mask_str, &mask)) {
            printf("✗ Ошибка: Неверный формат маски. Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    do {
        printf("Введите количество пакетов (положительное число, до 1000): ");
        if (fgets(count_str, sizeof(count_str), stdin) == NULL) {
            printf("Ошибка ввода\n");
            return;
        }
        count_str[strcspn(count_str, "\n")] = '\0';
        
        packet_count = atoi(count_str);
        if (packet_count <= 0) {
            printf("✗ Ошибка: Количество пакетов должно быть положительным числом. Попробуйте снова.\n");
        } else if (packet_count > 1000) {
            printf("✗ Ошибка: Слишком много пакетов (максимум 1000). Попробуйте снова.\n");
        } else {
            break;
        }
    } while (1);
    
    process_packets(subnet_ip, mask, packet_count);
}

void print_help(const char* program_name) {
    printf("\n╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         ПОМОЩЬ                                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Использование:                                                     ║\n");
    printf("║   1. Интерактивный режим: %-41s ║\n", program_name);
    printf("║   2. Режим командной строки: %s <IP_подсети> <маска> <кол-во> ║\n", program_name);
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Примеры:                                                           ║\n");
    printf("║   %s 192.168.1.0 /24 100                         ║\n", program_name);
    printf("║   %s 10.0.0.0 255.0.0.0 50                       ║\n", program_name);
    printf("║   %s 172.16.0.0 /16 20                           ║\n", program_name);
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Форматы маски:                                                    ║\n");
    printf("║   - /24 (префиксная нотация)                                      ║\n");
    printf("║   - 255.255.255.0 (точечная десятичная нотация)                   ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Дополнительные функции:                                            ║\n");
    printf("║   - Классификация IP (частный/публичный)                          ║\n");
    printf("║   - Сортировка IP-адресов                                        ║\n");
    printf("║   - Подробная информация о подсети                                ║\n");
    printf("║   - Красивый вывод с рамками                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n\n");
}

int main(int argc, char* argv[]) {
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help(argv[0]);
        return 0;
    }
    
    if (argc == 1) {
        interactive_mode();
        return 0;
    }
    
    if (argc == 4) {
        ip_address_t subnet_ip;
        uint32_t mask;
        int packet_count;
        
        if (!parse_ip(argv[1], &subnet_ip)) {
            printf("✗ Ошибка: Неверный формат IP-адреса: %s\n", argv[1]);
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        if (!parse_mask(argv[2], &mask)) {
            printf("✗ Ошибка: Неверный формат маски: %s\n", argv[2]);
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        packet_count = atoi(argv[3]);
        if (packet_count <= 0 || packet_count > 1000) {
            printf("✗ Ошибка: Количество пакетов должно быть от 1 до 1000\n");
            printf("Для справки используйте: %s -h\n", argv[0]);
            return 1;
        }
        
        process_packets(subnet_ip, mask, packet_count);
        return 0;
    }
    
    printf("✗ Ошибка: Неправильное количество аргументов\n");
    print_help(argv[0]);
    return 1;
}