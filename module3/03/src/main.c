#include "posix.h"
#include <pthread.h>

ChatInfo *global_inf = NULL;

void *threads(void *arg) {
    ChatInfo *inf = (ChatInfo *)arg;
    char buffer[MSG_SIZE];
    unsigned int priority = 0;
    
    while (inf->running) {
        int bytes = receiveMSG(inf, buffer, &priority);
        
        if (bytes > 0) {
            if (priority == EXIT_PRIORITY) {
                printf("\n\n[Собеседник завершил работу]\n");
                inf->running = 0;
                break;
            }
            printf("\n[Собеседник]: %s\n", buffer);
            printf("Вы: ");
            fflush(stdout);
        }
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_очереди>\n", argv[0]);
        fprintf(stderr, "Пример: %s mychat\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    ChatInfo inf;
    pthread_t thread;
    char input[MSG_SIZE];
    
    global_inf = &inf;
    signal(SIGINT, signals);
    
    if (initChat(&inf, argv[1]) == -1) {
        fprintf(stderr, "Ошибка инициализации очередей сообщений.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("\n=== P2P Чат запущен ===\n");
    printf("Имя очереди: %s\n", argv[1]);
    printf("Введите сообщение и нажмите Enter для отправки.\n");
    printf("Для выхода введите 'exit' или нажмите Ctrl+C.\n\n");
    
    if (pthread_create(&thread, NULL, threads, &inf) != 0) {
        perror("pthread_create");
        cleanChat(&inf);
        exit(EXIT_FAILURE);
    }
    
    while (inf.running) {
        printf("Вы: ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
        
        if (strcasecmp(input, "exit") == 0) {
            sendMSG(&inf, "EXIT", EXIT_PRIORITY);
            inf.running = 0;
            break;
        }
        
        if (strlen(input) > 0) {
            sendMSG(&inf, input, NORM_PRIORITY);
        }
    }
    
    pthread_join(thread, NULL);
    
    cleanChat(&inf);
    global_inf = NULL;
    
    printf("Чат завершен.\n");
    return 0;
}
