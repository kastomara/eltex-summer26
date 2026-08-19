#include "updsock.h"

void get_current_time(char* buffer, size_t size) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, size, "%H:%M:%S", timeinfo);
}

void chat_set_callbacks(Client* client, 
                       void (*msg_cb)(const char*, const char*, const char*), 
                       void (*status_cb)(const char*)) {
    if (client) {
        client->message_callback = msg_cb;
        client->status_callback = status_cb;
    }
}

void chat_set_running(Client* client, int running) {
    if (client) client->running = running;
}

int chat_is_running(Client* client) {
    return client ? client->running : 0;
}

// ИЗМЕНЕНО: добавляем параметр port
Client* chat_create(const char* username) {
    Client* client = (Client*)malloc(sizeof(Client));
    if (!client) return NULL;

    memset(client, 0, sizeof(Client));
    strncpy(client->username, username, MAX_USERNAME_LEN - 1);
    client->username[MAX_USERNAME_LEN - 1] = '\0';

    if ((client->sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        free(client);
        return NULL;
    }

    // Включить режим broadcast
    int broadcast_enable = 1;
    if (setsockopt(client->sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, 
                   sizeof(broadcast_enable)) < 0) {
        perror("setsockopt SO_BROADCAST");
        close(client->sockfd);
        free(client);
        return NULL;
    }

    // КЛЮЧЕВОЕ ИЗМЕНЕНИЕ: Включаем SO_REUSEADDR для совместного использования порта
    int reuse = 1;
    if (setsockopt(client->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        close(client->sockfd);
        free(client);
        return NULL;
    }

    // Включаем SO_REUSEPORT (если доступен) для лучшей совместимости
    #ifdef SO_REUSEPORT
    int reuseport = 1;
    setsockopt(client->sockfd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));
    #endif

    // ВАЖНО: Все клиенты bind на ОДИНАКОВЫЙ порт
    bzero(&client->cliaddr, sizeof(client->cliaddr));
    client->cliaddr.sin_family = AF_INET;
    client->cliaddr.sin_port = htons(BRDCS_PORT);  // Все на порт 40000
    client->cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(client->sockfd, (struct sockaddr*)&client->cliaddr, sizeof(client->cliaddr)) < 0) {
        perror("bind");
        close(client->sockfd);
        free(client);
        return NULL;
    }

    // Настройка адреса для отправки broadcast сообщений
    bzero(&client->servaddr, sizeof(client->servaddr));
    client->servaddr.sin_family = AF_INET;
    client->servaddr.sin_port = htons(BRDCS_PORT);
    if (inet_aton(BRDCS_ADDR, &client->servaddr.sin_addr) == 0) {
        perror("inet_aton");
        close(client->sockfd);
        free(client);
        return NULL;
    }
    
    client->running = 1;
    return client;
}

// Остальной код без изменений
int chat_send_message(Client* client, const char* message) {
    if (!client || !client->running) return -1;
    
    Message chat_msg;
    bzero(&chat_msg, sizeof(chat_msg));
    
    strncpy(chat_msg.username, client->username, MAX_USERNAME_LEN - 1);
    strncpy(chat_msg.message, message, MAX_BUFFER_SIZE - 1);
    get_current_time(chat_msg.timestamp, sizeof(chat_msg.timestamp));
    
    if (sendto(client->sockfd, &chat_msg, sizeof(chat_msg), 0,
               (struct sockaddr*)&client->servaddr, 
               sizeof(client->servaddr)) < 0) {
        return -1;
    }
    
    return 0;
}

void* chat_receive_thread(void* arg) {
    Client* client = (Client*)arg;
    Message received_msg;
    struct sockaddr_in sender_addr;
    socklen_t clilen = sizeof(sender_addr);
    int n;
    
    struct timeval tv = {1, 0};
    setsockopt(client->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    while (client->running) {
        bzero(&received_msg, sizeof(received_msg));
        bzero(&sender_addr, sizeof(sender_addr));
        clilen = sizeof(sender_addr);
        
        n = recvfrom(client->sockfd, &received_msg, sizeof(received_msg), 0,
                     (struct sockaddr*)&sender_addr, &clilen);
        
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            perror("recvfrom");
            break;
        }
        
        if (strcmp(received_msg.username, client->username) != 0 && client->message_callback) {
            client->message_callback(received_msg.username, 
                                    received_msg.message, 
                                    received_msg.timestamp);
        }
    }
    
    return NULL;
}

int chat_run(Client* client) {
    if (!client) return -1;
    
    if (pthread_create(&client->receive_thread, NULL, chat_receive_thread, client) != 0) {
        return -1;
    }
    
    char join_msg[MAX_BUFFER_SIZE];
    snprintf(join_msg, sizeof(join_msg), "🟢 %s подключился к чату", client->username);
    chat_send_message(client, join_msg);
    
    return 0;
}

void chat_cleanup(Client* client) {
    if (!client) return;

    client->running = 0;
    if (client->receive_thread) {
        pthread_join(client->receive_thread, NULL);
    }
    
    if (client->sockfd >= 0) {
        close(client->sockfd);
    }
}

void chat_destroy(Client* client) {
    if (!client) return;
    chat_cleanup(client);
    free(client);
}