#include "message.h"

#define MSG_KEY 12345

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование:\n");
        fprintf(stderr, "  Брокер:     %s -b\n", argv[0]);
        fprintf(stderr, "  Издатель:   %s -p <topic>\n", argv[0]);
        fprintf(stderr, "  Подписчик:  %s -s <topic1> [topic2] ...\n", argv[0]);
        exit(1);
    }

    setup_signal_handlers();

    if (strcmp(argv[1], "-b") == 0) {
        printf("[Broker PID=%d]: Запуск брокера сообщений...\n", getpid());
        
        int msgid = get_message_queue(MSG_KEY, IPC_CREAT | IPC_EXCL | 0666);
        if (msgid == -1) {
            printf("[Broker]: Очередь сообщений с ключом %d уже существует (другой брокер уже запущен). Завершение работы.\n", MSG_KEY);
            exit(1);
        }   
        
        printf("[Broker]: Очередь сообщений успешно создана (ID очереди: %d, Key: %d)\n", msgid, MSG_KEY);
        
        BrokerState state;
        init_broker_state(&state);
        state.msg_queue_id = msgid;
        
        printf("[Broker]: Работает в штатном режиме, ожидание сообщений (mtype=1)...\n");
        
        while (state.running && !signal_received) {
            char buffer[MAX_TEXT_LEN];
            if (receive_message(msgid, 1, buffer, sizeof(buffer)) == -1) {
                if (signal_received || errno == EINTR || errno == EIDRM || errno == EINVAL) break;
                continue;
            }
            
            char command[20], topic[MAX_TOPIC_LEN], payload[MAX_PAYLOAD_LEN];
            pid_t sender_pid = 0;
            parse_message(buffer, command, &sender_pid, topic, payload);
            
            if (strcmp(command, "subscribe") == 0) {
                if (strlen(topic) == 0 || add_subscriber(&state, sender_pid, topic) != 0) {
                    printf("[Broker]: %s подписки PID %d на тему '%s'\n", 
                           strlen(topic) == 0 ? "Ошибка: пустая тема" : "Не удалось", sender_pid, topic);
                } else {
                    printf("[Broker]: Подписчик PID %d успешно подписан на тему '%s'\n", sender_pid, topic);
                }
            } else if (strcmp(command, "unsubscribe") == 0) {
                if (remove_subscriber(&state, sender_pid, topic) == 0) {
                    printf("[Broker]: Подписчик PID %d отписался от темы '%s'\n", sender_pid, topic);
                } else {
                    printf("[Broker]: Подписчик PID %d не был подписан на тему '%s'\n", sender_pid, topic);
                }
            } else if (strcmp(command, "send") == 0) {
                update_publisher(&state, sender_pid, true);
                int sent = broadcast_message(&state, topic, payload);
                printf("[Broker]: Рассылка по теме '%s' от PID %d -> %d подписчикам: \"%s\"\n", 
                       topic, sender_pid, sent, payload);
            } else {
                printf("[Broker]: Неизвестная команда: '%s' от PID %d\n", command, sender_pid);
            }
        }
        
        cleanup_broker(&state);
        printf("[Broker]: Очередь удалена, работа брокера корректно завершена.\n");
    } else if (strcmp(argv[1], "-p") == 0) {
        if (argc < 3) {
            fprintf(stderr, "[Publisher]: Ошибка: необходимо указать тему (topic)\n");
            fprintf(stderr, "Использование: %s -p <topic>\n", argv[0]);
            exit(1);
        }
        
        char* topic = argv[2];
        pid_t pid = getpid();
        
        int msgid = get_message_queue(MSG_KEY, 0666);
        if (msgid == -1) {
            printf("[Publisher PID=%d]: Брокер не запущен (очередь сообщений не найдена). Завершение.\n", pid);
            exit(1);
        }
        
        printf("[Publisher PID=%d]: Подключен к очереди брокера (ID: %d), тема публикаций: '%s'\n", pid, msgid, topic);
        
        int counter = 0;
        while (!signal_received) {
            char payload[MAX_PAYLOAD_LEN];
            time_t now = time(NULL);
            char time_str[32];
            struct tm *tm_info = localtime(&now);
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

            snprintf(payload, sizeof(payload), "Сообщение #%d от издателя [PID=%d] в %s", 
                    ++counter, pid, time_str);
            
            if (publisher_send_message(msgid, topic, payload) == 0) {
                printf("[Publisher PID=%d]: Отправлено сообщение #%d в тему '%s': \"%s\"\n", 
                       pid, counter, topic, payload);
            } else {
                if (errno == EIDRM || errno == EINVAL) {
                    printf("[Publisher PID=%d]: Очередь сообщений была удалена брокером. Завершение работы...\n", pid);
                    break;
                }
                if (signal_received || errno == EINTR) break;
                perror("[Publisher]: Ошибка отправки сообщения");
            }
            
            for (int step = 0; step < 20 && !signal_received; step++) {
                usleep(100000);
            }
        }
        
        printf("[Publisher PID=%d]: Завершение работы выполнено.\n", pid);
    } else if (strcmp(argv[1], "-s") == 0) {
        if (argc < 3) {
            fprintf(stderr, "[Subscriber]: Ошибка: укажите хотя бы одну тему для подписки\n");
            fprintf(stderr, "Использование: %s -s <topic1> [topic2] ...\n", argv[0]);
            exit(1);
        }
        
        pid_t pid = getpid();
        printf("[Subscriber PID=%d]: Запуск подписчика...\n", pid);
        
        int msgid = get_message_queue(MSG_KEY, 0666);
        if (msgid == -1) {
            printf("[Subscriber PID=%d]: Брокер не запущен (очередь сообщений не найдена). Завершение.\n", pid);
            exit(1);
        }
        
        printf("[Subscriber PID=%d]: Подключен к очереди брокера (ID: %d)\n", pid, msgid);
        
        for (int i = 2; i < argc; i++) {
            if (subscriber_change_subscription(msgid, pid, argv[i], "subscribe") == 0) {
                printf("[Subscriber PID=%d]: Оформлена подписка на тему '%s'\n", pid, argv[i]);
            } else {
                printf("[Subscriber PID=%d]: Ошибка подписки на тему '%s'\n", pid, argv[i]);
            }
        }
        
        printf("[Subscriber PID=%d]: Ожидание входящих сообщений (mtype=%d)...\n", pid, pid);
        
        char buffer[MAX_TEXT_LEN];
        while (!signal_received) {
            if (subscriber_receive_messages(msgid, pid, buffer, sizeof(buffer)) == 0) {
                char command[20], topic[MAX_TOPIC_LEN], payload[MAX_PAYLOAD_LEN];
                parse_message(buffer, command, NULL, topic, payload);
                
                if (strcmp(command, "send") == 0) {
                    printf("[Subscriber PID=%d] -> [ПОЛУЧЕНО] Тема: '%s' | Данные: \"%s\"\n", 
                           pid, topic, payload);
                }
            } else {
                if (signal_received || errno == EINTR) {
                    printf("\n[Subscriber PID=%d]: Получен сигнал SIGINT (завершение от брокера или пользователя)\n", pid);
                    break;
                } else if (errno == EIDRM || errno == EINVAL) {
                    printf("\n[Subscriber PID=%d]: Очередь сообщений удалена брокером. Завершение работы...\n", pid);
                    break;
                } else {
                    perror("[Subscriber]: Ошибка msgrcv");
                    break;
                }
            }
        }
        
        printf("\n[Subscriber PID=%d]: Завершение по сигналу SIGINT. Отправка сообщений об отписке...\n", pid);
        for (int i = 2; i < argc; i++) {
            if (subscriber_change_subscription(msgid, pid, argv[i], "unsubscribe") == 0) {
                printf("[Subscriber PID=%d]: Отправлено уведомление об отписке от темы '%s' (unsubscribe)\n", pid, argv[i]);
            } else {
                if (errno == EIDRM || errno == EINVAL) {
                    printf("[Subscriber PID=%d]: Очередь недоступна, отправка отписки не требуется.\n", pid);
                    break;
                }
            }
        }
        
        printf("[Subscriber PID=%d]: Завершение работы выполнено.\n", pid);
    } else {
        fprintf(stderr, "Неизвестный параметр: %s\n", argv[1]);
        fprintf(stderr, "Допустимые ключи: -b (брокер), -p (издатель), -s (подписчик)\n");
        exit(1);
    }
    
    return 0;
}