#include "tcpsock.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Использование: %s <IP сервера> <Имя>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ClientState c;
    memset(&c, 0, sizeof(ClientState));
    
    strncpy(c.username, argv[2], MAX_USERNAME_LEN - 1);
    
    c.sock = socket(AF_INET, SOCK_STREAM, 0);
    c.server_addr.sin_family = AF_INET;
    c.server_addr.sin_port = htons(BRDCS_PORT);
    
    if (inet_pton(AF_INET, argv[1], &c.server_addr.sin_addr) <= 0) {
        perror("Неверный IP адрес");
        return EXIT_FAILURE;
    }

    if (connect(c.sock, (struct sockaddr *)&c.server_addr, sizeof(c.server_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        return EXIT_FAILURE;
    }

    snprintf(c.msg_buffer, sizeof(c.msg_buffer), "%s присоединился к чату", c.username);
    send(c.sock, c.msg_buffer, strlen(c.msg_buffer), 0);

    printf("Добро пожаловать, %s! Команда 'quit' для выхода.\n", c.username);
    printf("Введите сообщение: ");
    fflush(stdout);

    c.max_sd = (c.sock > STDIN_FILENO) ? c.sock : STDIN_FILENO;
    c.running = 1;

    while (c.running) {
        FD_ZERO(&c.readfds);
        FD_SET(STDIN_FILENO, &c.readfds);
        FD_SET(c.sock, &c.readfds);

        int activity = select(c.max_sd + 1, &c.readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("Ошибка select");
            break;
        }

        // 1. Пришли данные из сети
        if (FD_ISSET(c.sock, &c.readfds)) {
            int valread = recv(c.sock, c.msg_buffer, sizeof(c.msg_buffer) - 1, 0);
            if (valread == 0) {
                printf("\nСервер разорвал соединение.\n");
                break;
            } else if (valread > 0) {
                c.msg_buffer[valread] = '\0';
                printf("\r%*s\r", 80, "");
                printf("%s\n", c.msg_buffer);
                printf("Введите сообщение: ");
                fflush(stdout);
            }
        }

        // 2. Введен текст с клавиатуры
        if (FD_ISSET(STDIN_FILENO, &c.readfds)) {
            if (fgets(c.msg_buffer, sizeof(c.msg_buffer), stdin) == NULL) break;
            
            c.msg_buffer[strcspn(c.msg_buffer, "\n")] = '\0';

            if (strcmp(c.msg_buffer, "quit") == 0) {
                c.running = 0;
                break;
            }

            if (strlen(c.msg_buffer) > 0) {
                char formatted_msg[BUFFER_SIZE];
                snprintf(formatted_msg, sizeof(formatted_msg), "[%s]: %s", c.username, c.msg_buffer);
                send(c.sock, formatted_msg, strlen(formatted_msg), 0);
                
                printf("Введите сообщение: ");
                fflush(stdout);
            }
        }
    }

    snprintf(c.msg_buffer, sizeof(c.msg_buffer), "%s покинул чат", c.username);
    send(c.sock, c.msg_buffer, strlen(c.msg_buffer), 0);
    close(c.sock);
    
    return 0;
}