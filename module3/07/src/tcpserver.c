#include "tcpsock.h"

int main() {
    ServerState s;
    memset(&s, 0, sizeof(ServerState));

    if ((s.master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(s.master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    s.address.sin_family = AF_INET;
    s.address.sin_addr.s_addr = INADDR_ANY;
    s.address.sin_port = htons(BRDCS_PORT);
    s.addrlen = sizeof(s.address);

    if (bind(s.master_socket, (struct sockaddr *)&s.address, s.addrlen) < 0) {
        perror("Ошибка bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s.master_socket, 5) < 0) {
        perror("Ошибка listen");
        exit(EXIT_FAILURE);
    }

    printf("TCP Сервер [POLL] запущен на порту %d...\n", BRDCS_PORT);

    s.fds[0].fd = s.master_socket;
    s.fds[0].events = POLLIN;
    s.nfds = 1;

    while (1) {
        int activity = poll(s.fds, s.nfds, -1);

        if (activity < 0 && errno != EINTR) {
            perror("Ошибка poll");
            break;
        }

        int current_size = s.nfds;
        for (int i = 0; i < current_size; i++) {
            if (s.fds[i].revents == 0) continue;

            if (s.fds[i].revents & POLLIN) {
                if (s.fds[i].fd == s.master_socket) {
                    // 1. Новое подключение на главном сокете
                    int new_socket = accept(s.master_socket, (struct sockaddr *)&s.address, (socklen_t*)&s.addrlen);
                    if (new_socket >= 0) {
                        printf("Новое подключение: IP %s, Порт %d\n", inet_ntoa(s.address.sin_addr), ntohs(s.address.sin_port));
                        if (s.nfds < MAX_CLIENTS + 1) {
                            s.fds[s.nfds].fd = new_socket;
                            s.fds[s.nfds].events = POLLIN;
                            s.nfds++;
                        } else {
                            printf("Сервер перегружен. Сброс подключения.\n");
                            close(new_socket);
                        }
                    }
                } else {
                    // 2. Входящее сообщение от клиента
                    int valread = recv(s.fds[i].fd, s.buffer, BUFFER_SIZE - 1, 0);
                    
                    if (valread <= 0) {
                        // Клиент отключился
                        getpeername(s.fds[i].fd, (struct sockaddr*)&s.address, (socklen_t*)&s.addrlen);
                        printf("Отключение: IP %s, Порт %d\n", inet_ntoa(s.address.sin_addr), ntohs(s.address.sin_port));
                        close(s.fds[i].fd);
                        
                        s.fds[i] = s.fds[s.nfds - 1];
                        s.nfds--;
                        i--;
                        current_size--;
                    } else {
                        // Рассылка сообщения остальным клиентам
                        s.buffer[valread] = '\0';
                        for (int j = 1; j < s.nfds; j++) {
                            if (j != i) { // Не отправляем сообщение обратно отправителю
                                send(s.fds[j].fd, s.buffer, strlen(s.buffer), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}