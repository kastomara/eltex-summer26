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
    if (msgid >= 0) {
        msgctl(msgid, IPC_RMID, NULL);
    }
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
    state->msg_queue_id = -1;
    global_broker_state = state;
}

int add_subscriber(BrokerState* state, pid_t pid, const char* topic) {
    if (!state || !topic || strlen(topic) == 0) return -1;

    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid == pid) {
            for (int j = 0; j < state->subscribers[i].topic_count; j++) {
                if (strcmp(state->subscribers[i].topics[j], topic) == 0) {
                    return 0;
                }
            }
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
    if (!state || !topic) return -1;

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
    if (!state || pid <= 0) return;

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
    if (!state) return;

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
    if (!state || !topic) return false;

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
    if (!state || !topic || !payload) return 0;

    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "send,%s,%s", topic, payload);
    
    int sent_count = 0;
    for (int i = 0; i < state->subscriber_count; i++) {
        if (is_subscribed(state, state->subscribers[i].pid, topic)) {
            long mtype = (long)state->subscribers[i].pid;
            if (send_message(state->msg_queue_id, mtype, message) == 0) {
                sent_count++;
            }
        }
    }
    return sent_count;
}

void cleanup_broker(BrokerState* state) {
    if (!state) return;
    
    for (int i = 0; i < state->publisher_count; i++) {
        if (state->publishers[i].pid > 0) {
            kill(state->publishers[i].pid, SIGINT);
        }
    }
    
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid > 0) {
            kill(state->subscribers[i].pid, SIGINT);
        }
    }
    
    if (state->msg_queue_id >= 0) {
        struct msqid_ds buf;
        int max_checks = 10;
        while (max_checks-- > 0) {
            if (msgctl(state->msg_queue_id, IPC_STAT, &buf) == -1) {
                break;
            }
            if (buf.msg_qnum == 0) {
                break;
            }
            usleep(50000);
        }

        remove_message_queue(state->msg_queue_id);
        state->msg_queue_id = -1;
    }
    
    state->running = false;
}

int publisher_send_message(int msgid, const char* topic, const char* payload) {
    char message[MAX_TEXT_LEN];
    pid_t pid = getpid();
    snprintf(message, sizeof(message), "send,%d,%s,%s", pid, topic, payload);
    return send_message(msgid, 1, message);
}

int subscriber_subscribe(int msgid, pid_t pid, const char* topic) {
    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "subscribe,%d,%s", pid, topic);
    return send_message(msgid, 1, message);
}

int subscriber_unsubscribe(int msgid, pid_t pid, const char* topic) {
    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "unsubscribe,%d,%s", pid, topic);
    return send_message(msgid, 1, message);
}

int subscriber_receive_messages(int msgid, pid_t pid, char* buffer, size_t buf_size) {
    return receive_message(msgid, (long)pid, buffer, buf_size);
}

void parse_message(const char* text, char* command, pid_t* pid, char* topic, char* payload) {
    if (command) command[0] = '\0';
    if (pid) *pid = 0;
    if (topic) topic[0] = '\0';
    if (payload) payload[0] = '\0';

    if (!text) return;

    const char* p1 = strchr(text, ',');
    if (!p1) {
        if (command) strncpy(command, text, 19);
        return;
    }
    if (command) {
        size_t len = p1 - text;
        if (len >= 20) len = 19;
        strncpy(command, text, len);
        command[len] = '\0';
    }

    const char* p2 = strchr(p1 + 1, ',');
    if (!p2) {
        if (pid) *pid = (pid_t)atoi(p1 + 1);
        return;
    }
    if (pid) {
        char pid_buf[32];
        size_t len = p2 - (p1 + 1);
        if (len >= sizeof(pid_buf)) len = sizeof(pid_buf) - 1;
        strncpy(pid_buf, p1 + 1, len);
        pid_buf[len] = '\0';
        *pid = (pid_t)atoi(pid_buf);
    }

    const char* p3 = strchr(p2 + 1, ',');
    if (!p3) {
        if (topic) {
            strncpy(topic, p2 + 1, MAX_TOPIC_LEN - 1);
            topic[MAX_TOPIC_LEN - 1] = '\0';
        }
        return;
    }
    
    if (topic) {
        size_t len = p3 - (p2 + 1);
        if (len >= MAX_TOPIC_LEN) len = MAX_TOPIC_LEN - 1;
        strncpy(topic, p2 + 1, len);
        topic[len] = '\0';
    }

    if (payload) {
        strncpy(payload, p3 + 1, MAX_PAYLOAD_LEN - 1);
        payload[MAX_PAYLOAD_LEN - 1] = '\0';
    }
}

void parse_subscriber_message(const char* text, char* command, char* topic, char* payload) {
    if (command) command[0] = '\0';
    if (topic) topic[0] = '\0';
    if (payload) payload[0] = '\0';

    if (!text) return;

    const char* p1 = strchr(text, ',');
    if (!p1) return;

    if (command) {
        size_t len = p1 - text;
        if (len >= 20) len = 19;
        strncpy(command, text, len);
        command[len] = '\0';
    }

    const char* p2 = strchr(p1 + 1, ',');
    if (!p2) {
        if (topic) {
            strncpy(topic, p1 + 1, MAX_TOPIC_LEN - 1);
            topic[MAX_TOPIC_LEN - 1] = '\0';
        }
        return;
    }

    if (topic) {
        size_t len = p2 - (p1 + 1);
        if (len >= MAX_TOPIC_LEN) len = MAX_TOPIC_LEN - 1;
        strncpy(topic, p1 + 1, len);
        topic[len] = '\0';
    }

    if (payload) {
        strncpy(payload, p2 + 1, MAX_PAYLOAD_LEN - 1);
        payload[MAX_PAYLOAD_LEN - 1] = '\0';
    }
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        signal_received = 1;
    }
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}
