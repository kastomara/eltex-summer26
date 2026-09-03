#include "raw.h"

int sock;
struct sockaddr_in server_addr;
uint16_t local_port;

void handle_sigint(int sig) {
    printf("\n[Client] Sending close signal to server and shutting down...\n");
    
    char send_buf[BUFFER_SIZE];
    memset(send_buf, 0, BUFFER_SIZE);
    
    struct udphdr *udph = (struct udphdr *)send_buf;
    char *payload = send_buf + sizeof(struct udphdr);
    
    strcpy(payload, CLOSE_CMD);
    
    udph->source = htons(local_port);
    udph->dest = server_addr.sin_port;
    udph->len = htons(sizeof(struct udphdr) + strlen(payload));
    udph->check = 0;

    sendto(sock, send_buf, sizeof(struct udphdr) + strlen(payload), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    close(sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <local_port>\n", argv[0]);
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    local_port = atoi(argv[3]);

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed (did you run as root?)");
        exit(1);
    }

    signal(SIGINT, handle_sigint);

    printf("Client started. Bound to local port %d. Type your messages (Ctrl+C to exit):\n", local_port);
    
    char input[MAX_PAYLOAD];
    while (fgets(input, MAX_PAYLOAD, stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;

        char send_buf[BUFFER_SIZE];
        memset(send_buf, 0, BUFFER_SIZE);
        
        struct udphdr *udph = (struct udphdr *)send_buf;
        char *payload = send_buf + sizeof(struct udphdr);
        strcpy(payload, input);
        
        udph->source = htons(local_port);
        udph->dest = server_addr.sin_port;
        udph->len = htons(sizeof(struct udphdr) + strlen(payload));
        udph->check = 0;

        sendto(sock, send_buf, sizeof(struct udphdr) + strlen(payload), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        char recv_buf[BUFFER_SIZE];
        while(1) {
            memset(recv_buf, 0, BUFFER_SIZE);
            struct sockaddr_in src_addr;
            socklen_t addr_len = sizeof(src_addr);
            
            ssize_t data_size = recvfrom(sock, recv_buf, BUFFER_SIZE, 0, (struct sockaddr *)&src_addr, &addr_len);
            if (data_size < 0) continue;

            struct iphdr *iph = (struct iphdr *)recv_buf;
            int iphdrlen = iph->ihl * 4;
            struct udphdr *recv_udph = (struct udphdr *)(recv_buf + iphdrlen);

            if (ntohs(recv_udph->dest) == local_port && iph->saddr == server_addr.sin_addr.s_addr) {
                char *recv_payload = recv_buf + iphdrlen + sizeof(struct udphdr);
                int payload_len = ntohs(recv_udph->len) - sizeof(struct udphdr);
                recv_payload[payload_len] = '\0';
                
                printf("Server reply: %s\n", recv_payload);
                break;
            }
        }
    }

    return 0;
}