#include "message.h"

BrokerState* global_broker_state = NULL;
volatile sig_atomic_t signal_received = 0;

int create_message_queue(key_t key) {
    int msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    return msgid;
}

int get_message_queue(key_t key) {
    int msgid = msgget(key, 0666);
    return msgid;
}

void remove_message_queue(int msgid) {
    msgctl(msgid, IPC_RMID, NULL);
}

int send_message(int msgid, long mtype, const char* text) {
    Message msg;
    msg.mtype = mtype;
    strncpy(msg.mtext, text, MAX_TEXT_LEN - 1);
    msg.mtext[MAX_TEXT_LEN - 1] = '\0';
    
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        return -1;
    }
    return 0;
}

int receive_message(int msgid, long mtype, char* buffer, size_t buf_size) {
    Message msg;
    ssize_t bytes = msgrcv(msgid, &msg, sizeof(msg.mtext), mtype, 0);
    if (bytes == -1) {
        return -1;
    }
    strncpy(buffer, msg.mtext, buf_size - 1);
    buffer[buf_size - 1] = '\0';
    return 0;
}

void init_broker_state(BrokerState* state) {
    memset(state, 0, sizeof(BrokerState));
    state->running = true;
    global_broker_state = state;
}

int add_subscriber(BrokerState* state, pid_t pid, const char* topic) {
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid == pid) {
            if (state->subscribers[i].topic_count < MAX_TOPICS_PER_SUB) {
                strncpy(state->subscribers[i].topics[state->subscribers[i].topic_count], 
                       topic, MAX_TOPIC_LEN - 1);
                state->subscribers[i].topics[state->subscribers[i].topic_count][MAX_TOPIC_LEN - 1] = '\0';
                state->subscribers[i].topic_count++;
                return 0;
            }
            return -1;
        }
    }
    
    if (state->subscriber_count < MAX_SUBSCRIBERS) {
        state->subscribers[state->subscriber_count].pid = pid;
        strncpy(state->subscribers[state->subscriber_count].topics[0], 
               topic, MAX_TOPIC_LEN - 1);
        state->subscribers[state->subscriber_count].topics[0][MAX_TOPIC_LEN - 1] = '\0';
        state->subscribers[state->subscriber_count].topic_count = 1;
        state->subscriber_count++;
        return 0;
    }
    return -1;
}

int remove_subscriber(BrokerState* state, pid_t pid, const char* topic) {
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid == pid) {
            for (int j = 0; j < state->subscribers[i].topic_count; j++) {
                if (strcmp(state->subscribers[i].topics[j], topic) == 0) {
                    for (int k = j; k < state->subscribers[i].topic_count - 1; k++) {
                        strcpy(state->subscribers[i].topics[k], 
                               state->subscribers[i].topics[k + 1]);
                    }
                    state->subscribers[i].topic_count--;
                    
                    if (state->subscribers[i].topic_count == 0) {
                        for (int k = i; k < state->subscriber_count - 1; k++) {
                            state->subscribers[k] = state->subscribers[k + 1];
                        }
                        state->subscriber_count--;
                    }
                    return 0;
                }
            }
        }
    }
    return -1;
}

void add_publisher(BrokerState* state, pid_t pid) {
    for (int i = 0; i < state->publisher_count; i++) {
        if (state->publishers[i].pid == pid) {
            return;
        }
    }
    if (state->publisher_count < MAX_PUBLISHERS) {
        state->publishers[state->publisher_count].pid = pid;
        state->publisher_count++;
    }
}

void remove_publisher(BrokerState* state, pid_t pid) {
    for (int i = 0; i < state->publisher_count; i++) {
        if (state->publishers[i].pid == pid) {
            for (int k = i; k < state->publisher_count - 1; k++) {
                state->publishers[k] = state->publishers[k + 1];
            }
            state->publisher_count--;
            return;
        }
    }
}

bool is_subscribed(BrokerState* state, pid_t pid, const char* topic) {
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid == pid) {
            for (int j = 0; j < state->subscribers[i].topic_count; j++) {
                if (strcmp(state->subscribers[i].topics[j], topic) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

int broadcast_message(BrokerState* state, const char* topic, const char* payload) {
    char message[MAX_TEXT_LEN];
    snprintf(message, MAX_TEXT_LEN, "send,%s,%s", topic, payload);
    
    int sent_count = 0;
    for (int i = 0; i < state->subscriber_count; i++) {
        if (is_subscribed(state, state->subscribers[i].pid, topic)) {
            long mtype = state->subscribers[i].pid;
            if (send_message(state->msg_queue_id, mtype, message) == 0) {
                sent_count++;
            }
        }
    }
    return sent_count;
}

void cleanup_broker(BrokerState* state) {
    for (int i = 0; i < state->publisher_count; i++) {
        kill(state->publishers[i].pid, SIGINT);
    }
    
    for (int i = 0; i < state->subscriber_count; i++) {
        kill(state->subscribers[i].pid, SIGINT);
    }
    
    if (state->msg_queue_id >= 0) {
        remove_message_queue(state->msg_queue_id);
    }
    
    state->running = false;
}

int publisher_send_message(int msgid, const char* topic, const char* payload) {
    char message[MAX_TEXT_LEN];
    pid_t pid = getpid();
    snprintf(message, MAX_TEXT_LEN, "send,%d,%s,%s", pid, topic, payload);
    return send_message(msgid, 1, message);
}

int subscriber_subscribe(int msgid, pid_t pid, const char* topic) {
    char message[MAX_TEXT_LEN];
    snprintf(message, MAX_TEXT_LEN, "subscribe,%d,%s", pid, topic);
    return send_message(msgid, 1, message);
}

int subscriber_unsubscribe(int msgid, pid_t pid, const char* topic) {
    char message[MAX_TEXT_LEN];
    snprintf(message, MAX_TEXT_LEN, "unsubscribe,%d,%s", pid, topic);
    return send_message(msgid, 1, message);
}

int subscriber_receive_messages(int msgid, pid_t pid, char* buffer, size_t buf_size) {
    return receive_message(msgid, pid, buffer, buf_size);
}

void parse_message(const char* text, char* command, pid_t* pid, char* topic) {
    char buffer[MAX_TEXT_LEN];
    strcpy(buffer, text);
    
    char* token = strtok(buffer, ",");
    if (token) strcpy(command, token);
    
    token = strtok(NULL, ",");
    if (token) *pid = atoi(token);
    
    token = strtok(NULL, ",");
    if (token && topic) strcpy(topic, token);
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        signal_received = 1;
        if (global_broker_state) {
            cleanup_broker(global_broker_state);
        }
        exit(0);
    }
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}