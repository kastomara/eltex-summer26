#include "updsock.h"
#include <ctype.h>

static Client* global_client = NULL;

void on_message_received(const char* username, const char* message, const char* timestamp) {
    printf("\r%*s\r", 80, "");
    printf("[%s] %s: %s\n", timestamp, username, message);
    printf("Введите сообщение: ");
    fflush(stdout);
}

void on_status(const char* message) {
    printf("\n%s\n", message);
    printf("Введите сообщение: ");
    fflush(stdout);
}

void signal_handler(int sig) {
    if (sig == SIGINT && global_client) {
        global_client->running = 0;
    }
}

void input_loop(Client* client) {
    char sendline[MAX_BUFFER];
    
    while (client->running) {
        if (fgets(sendline, MAX_BUFFER, stdin) == NULL) break;
        
        sendline[strcspn(sendline, "\n")] = '\0';
        
        if (strcmp(sendline, "quit") == 0) {
            client->running = 0;
            break;
        }
        
        char *start = sendline;
        char *end = sendline + strlen(sendline) - 1;
        while (isspace(*start)) start++;
        while (end > start && isspace(*end)) end--;
        *(end + 1) = '\0';
        
        if (strlen(start) > 0) {
            chat_send_message(client, start);
        }
        
        if (client->running) {
            printf("Введите сообщение: ");
            fflush(stdout);
        }
        
    }
    char leave_msg[MAX_BUFFER];
    snprintf(leave_msg, sizeof(leave_msg), "%s вышел из чата", client->username);
    chat_send_message(client, leave_msg);
}

int main(int argc, char **argv) {
    char username[MAX_USERNAME_LEN];
    
    printf("\n===================================\n");
    printf("    ГРУППОВОЙ ЧАТ (UDP Broadcast)\n");
    printf("===================================\n\n");
    
    if (argc > 1) {
        strncpy(username, argv[1], MAX_USERNAME_LEN - 1);
        username[MAX_USERNAME_LEN - 1] = '\0';
    } else {
        srand(time(NULL));
        snprintf(username, MAX_USERNAME_LEN, "User_%d", rand() % 10000);
    }
    
    printf("Имя пользователя: %s\n", username);
    printf("Порт: %d | Broadcast адрес: %s\n", BRDCS_PORT, BRDCS_ADDR);
    printf("Команды: 'quit' или Ctrl+C для выхода\n");
    printf("Инициализация сокета...\n\n");
    
    Client* client = chat_create(username);
    if (!client) {
        printf("ОШИБКА: Не удалось создать клиент чата\n");
        return EXIT_FAILURE;
    }
    
    global_client = client;
    chat_set_callbacks(client, on_message_received, on_status);
    signal(SIGINT, signal_handler);
    
    if (chat_run(client) != 0) {
        printf("ОШИБКА: Не удалось запустить клиент чата\n");
        chat_destroy(client);
        return EXIT_FAILURE;
    }
    
    printf("Добро пожаловать, %s!\n", username);
    printf("Введите сообщение: ");
    fflush(stdout);
    
    input_loop(client);
    
    Client* temp_client = global_client;
    global_client = NULL;
    chat_destroy(temp_client);
    
    printf("\n===================================\n");
    printf("Чат завершен. До свидания!\n");
    printf("===================================\n");
    
    return EXIT_SUCCESS;
}