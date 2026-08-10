#include "message.h"

#define MSG_KEY 12345

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  Broker:   %s -b\n", argv[0]);
        fprintf(stderr, "  Publisher: %s -p <topic>\n", argv[0]);
        fprintf(stderr, "  Subscriber: %s -s <topic1> [topic2] ...\n", argv[0]);
        exit(1);
    }

    setup_signal_handlers();

    if (strcmp(argv[1], "-b") == 0) {
        printf("Starting broker...\n");
        
        int msgid = create_message_queue(MSG_KEY);
        if (msgid == -1) {
            printf("Broker: message queue already exists, exiting...\n");
            exit(1);
        }   
        
        printf("Broker: message queue created with ID %d\n", msgid);
        
        BrokerState state;
        init_broker_state(&state);
        state.msg_queue_id = msgid;
        
        printf("Broker: running, waiting for messages...\n");
        
        while (state.running) {
            char buffer[MAX_TEXT_LEN];
            
            if (receive_message(msgid, 1, buffer, sizeof(buffer)) == -1) {
                if (signal_received) {
                    break;
                }
                continue;
            }
            
            char command[20];
            pid_t sender_pid;
            char topic[MAX_TOPIC_LEN];
            char payload[MAX_PAYLOAD_LEN];
            
            parse_message(buffer, command, &sender_pid, topic);
            
            if (strcmp(command, "subscribe") == 0) {
                if (add_subscriber(&state, sender_pid, topic) == 0) {
                    if (topic == -1) {
                        printf("Broker: topic '%s' do not exists\n", sender_pid, topic);
                    }
                    else printf("Broker: subscriber %d subscribed to topic '%s'\n", sender_pid, topic);
                }
            } 
            else if (strcmp(command, "unsubscribe") == 0) {
                if (remove_subscriber(&state, sender_pid, topic) == 0) {
                    printf("Broker: subscriber %d unsubscribed from topic '%s'\n", sender_pid, topic);
                }
            } 
            else if (strcmp(command, "send") == 0) {
                char* payload_start = strstr(buffer, ",");
                if (payload_start) {
                    payload_start = strstr(payload_start + 1, ",");
                    if (payload_start) {
                        strcpy(payload, payload_start + 1);
                    }
                }
                
                add_publisher(&state, sender_pid);
                
                int sent = broadcast_message(&state, topic, payload);
                printf("Broker: broadcasted message on topic '%s' from publisher %d to %d subscribers\n", 
                       topic, sender_pid, sent);
            }
        }
        
        cleanup_broker(&state);
        printf("Broker: shutdown complete\n");
    }
    else if (strcmp(argv[1], "-p") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Publisher: topic required\n");
            exit(1);
        }
        
        char* topic = argv[2];
        
        int msgid = get_message_queue(MSG_KEY);
        if (msgid == -1) {
            printf("Publisher: broker not running, exiting...\n");
            exit(1);
        }
        
        printf("Publisher (PID=%d): connected to queue, topic: %s\n", getpid(), topic);
        
        int counter = 0;
        while (!signal_received) {
            char payload[MAX_PAYLOAD_LEN];
            snprintf(payload, MAX_PAYLOAD_LEN, "Message #%d from publisher %d at %ld", 
                    ++counter, getpid(), time(NULL));
            
            if (publisher_send_message(msgid, topic, payload) == 0) {
                printf("Publisher: sent message with topic '%s'\n", topic);
            } else {
                printf("Publisher: failed to send message\n");
            }
            
            sleep(2);
        }
        
        printf("Publisher (PID=%d): shutting down\n", getpid());
    }
    else if (strcmp(argv[1], "-s") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Subscriber: at least one topic required\n");
            exit(1);
        }
        
        pid_t pid = getpid();
        printf("Subscriber (PID=%d): starting...\n", pid);
        
        int msgid = get_message_queue(MSG_KEY);
        if (msgid == -1) {
            printf("Subscriber: broker not running, exiting...\n");
            exit(1);
        }
        
        printf("Subscriber (PID=%d): connected to queue\n", pid);
        
        for (int i = 2; i < argc; i++) {
            if (subscriber_subscribe(msgid, pid, argv[i]) == 0) {
                printf("Subscriber: subscribed to topic '%s'\n", argv[i]);
            } else {
                printf("Subscriber: failed to subscribe to topic '%s'\n", argv[i]);
            }
            sleep(1);
        }
        
        printf("Subscriber (PID=%d): waiting for messages...\n", pid);
        
        char buffer[MAX_TEXT_LEN];
        while (!signal_received) {
            if (subscriber_receive_messages(msgid, pid, buffer, sizeof(buffer)) == 0) {
                char command[20];
                char topic[MAX_TOPIC_LEN];
                char payload[MAX_PAYLOAD_LEN];
                
                char* token = strtok(buffer, ",");
                if (token) strcpy(command, token);
                
                token = strtok(NULL, ",");
                if (token) strcpy(topic, token);
                
                token = strtok(NULL, ",");
                if (token) strcpy(payload, token);
                
                if (strcmp(command, "send") == 0) {
                    printf("Subscriber (PID=%d): Received message - Topic: %s, Payload: %s\n", 
                           pid, topic, payload);
                }
            } else {
                break;
            }
        }
        
        for (int i = 2; i < argc; i++) {
            if (subscriber_unsubscribe(msgid, pid, argv[i]) == 0) {
                printf("Subscriber: unsubscribed from topic '%s'\n", argv[i]);
            }
        }
        
        printf("Subscriber (PID=%d): shutting down\n", pid);
    } 
    else {
        fprintf(stderr, "Unknown option: %s\n", argv[1]);
        fprintf(stderr, "Use -b (broker), -p (publisher), or -s (subscriber)\n");
        exit(1);
    }
    
    return 0;
}