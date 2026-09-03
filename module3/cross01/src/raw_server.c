#include "raw.h"

ClientState clients[MAX_CLIENTS];

int process_client_msg(uint32_t ip, uint16_t port) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].ip == ip && clients[i].port == port) {
            return ++clients[i].counter;
        }
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].active = 1;
            clients[i].ip = ip;
            clients[i].port = port;
            clients[i].counter = 1;
            return 1;
        }
    }
    return -1; 
}

// Функция для сброса счетчиков при получении сигнала о закрытии
void reset_client(uint32_t ip, uint16_t port) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].ip == ip && clients[i].port == port) {
            clients[i].active = 0;
            clients[i].counter = 0;
            printf("[Server] Counters reset for client %s:%d\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listen_port>\n", argv[0]);
        exit(1);
    }

    uint16_t server_port = atoi(argv[1]);
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed (did you run as root?)");
        exit(1);
    }

    printf("Server listening on raw UDP port %d...\n", server_port);
    char buffer[BUFFER_SIZE];

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        memset(buffer, 0, BUFFER_SIZE);

        ssize_t data_size = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (data_size < 0) {
            perror("recvfrom error");
            continue;
        }

        struct iphdr *iph = (struct iphdr *)buffer;
        int iphdrlen = iph->ihl * 4;
        struct udphdr *udph = (struct udphdr *)(buffer + iphdrlen);

        if (ntohs(udph->dest) != server_port) {
            continue;
        }

        char *payload = buffer + iphdrlen + sizeof(struct udphdr);
        int payload_len = ntohs(udph->len) - sizeof(struct udphdr);
        
        if (payload_len < 0 || payload_len > MAX_PAYLOAD) continue;
        payload[payload_len] = '\0';

        if (strncmp(payload, CLOSE_CMD, strlen(CLOSE_CMD)) == 0) {
            reset_client(iph->saddr, udph->source);
            continue;
        }

        int count = process_client_msg(iph->saddr, udph->source);
        printf("Received from %s:%d -> %s\n", inet_ntoa(*(struct in_addr*)&iph->saddr), ntohs(udph->source), payload);
        char send_buf[BUFFER_SIZE];
        memset(send_buf, 0, BUFFER_SIZE);
        
        struct udphdr *resp_udph = (struct udphdr *)send_buf;
        char *resp_payload = send_buf + sizeof(struct udphdr);
        sprintf(resp_payload, "%s %d", payload, count);
        
        resp_udph->source = htons(server_port);
        resp_udph->dest = udph->source;
        resp_udph->len = htons(sizeof(struct udphdr) + strlen(resp_payload));
        resp_udph->check = 0;

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = udph->source;
        dest_addr.sin_addr.s_addr = iph->saddr;

        sendto(sock, send_buf, sizeof(struct udphdr) + strlen(resp_payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    close(sock);
    return 0;
}