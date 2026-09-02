#include "udp_sniffer.h"

void print_mac(unsigned char* mac, char* buffer) {
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void start_sniffer(FilterType filter, int chat_port, int packets_to_capture, const char* out_filename) {
    int raw_socket;
    unsigned char buffer[65536];
    FILE *logfile = NULL;
    struct timeval start_time, current_time;

    if (out_filename != NULL) {
        logfile = fopen(out_filename, "w");
        if (logfile == NULL) {
            perror("Ошибка открытия файла для записи");
        }
    }

    // Создаем сырой сокет (требует sudo)
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        perror("Ошибка создания сокета (Вы запустили с sudo?)");
        exit(1);
    }

    printf("\nНачат перехват пакетов. Ожидание...\n");
    gettimeofday(&start_time, NULL);
    
    int captured_count = 0;

    while (captured_count < packets_to_capture) {
        struct sockaddr saddr;
        socklen_t saddr_len = sizeof(saddr);
        
        // Получаем пакет
        int data_size = recvfrom(raw_socket, buffer, 65536, 0, &saddr, &saddr_len);
        if (data_size < 0) {
            perror("Ошибка получения пакета");
            break;
        }

        // Парсинг Ethernet-заголовка
        struct ethhdr *eth_header = (struct ethhdr *)buffer;

        if (ntohs(eth_header->h_proto) != ETH_P_IP) {
            continue;
        }

        // Парсинг IP
        struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        if (ip_header->protocol != IPPROTO_UDP) {
            continue;
        }

        unsigned short iphdrlen = ip_header->ihl * 4;
        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);

        uint16_t src_port = ntohs(udp_header->source);
        uint16_t dst_port = ntohs(udp_header->dest);

        // Применение фильтра
        int match = 0;
        if (filter == FILTER_UDP_CHAT) {
            if (src_port == chat_port || dst_port == chat_port) match = 1;
        } else if (filter == FILTER_DNS) {
            if (src_port == 53 || dst_port == 53) match = 1;
        } else {
            match = 1;
        }

        if (match) {
            gettimeofday(&current_time, NULL);
            double elapsed = (current_time.tv_sec - start_time.tv_sec) + 
                             (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

            char src_mac[18], dst_mac[18];
            print_mac(eth_header->h_source, src_mac);
            print_mac(eth_header->h_dest, dst_mac);

            struct in_addr src_ip, dst_ip;
            src_ip.s_addr = ip_header->saddr;
            dst_ip.s_addr = ip_header->daddr;

            char output_str[1024];
            snprintf(output_str, sizeof(output_str),
                     "[%0.6f с] UDP Пакет\n"
                     "  MAC: %s -> %s\n"
                     "  IP:  %s -> %s\n"
                     "  Порт: %d -> %d\n"
                     "  Длина: %d байт\n"
                     "------------------------------------------\n",
                     elapsed,
                     src_mac, dst_mac,
                     inet_ntoa(src_ip), inet_ntoa(dst_ip),
                     src_port, dst_port,
                     ntohs(udp_header->len));

            printf("%s", output_str);

            if (logfile) {
                fprintf(logfile, "%s", output_str);
                fflush(logfile);
            }

            captured_count++;
        }
    }

    printf("\nЗахват %d пакетов завершен.\n", captured_count);
    close(raw_socket);
    if (logfile) fclose(logfile);
}