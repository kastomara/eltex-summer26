#include "message.h"

volatile sig_atomic_t signal_received = 0;

int get_message_queue(key_t key, int flags) {
    return msgget(key, flags);
}

void remove_message_queue(int msgid) {
    if (msgid >= 0) msgctl(msgid, IPC_RMID, NULL);
}

int send_message(int msgid, long mtype, const char* text) {
    Message msg;
    msg.mtype = mtype;
    strncpy(msg.mtext, text, MAX_TEXT_LEN - 1);
    msg.mtext[MAX_TEXT_LEN - 1] = '\0';
    return msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1 ? -1 : 0;
}

int receive_message(int msgid, long mtype, char* buffer, size_t buf_size) {
    Message msg;
    if (msgrcv(msgid, &msg, sizeof(msg.mtext), mtype, 0) == -1) return -1;
    strncpy(buffer, msg.mtext, buf_size - 1);
    buffer[buf_size - 1] = '\0';
    return 0;
}

void init_broker_state(BrokerState* state) {
    memset(state, 0, sizeof(BrokerState));
    state->running = true;
    state->msg_queue_id = -1;
}

static int find_subscriber_index(BrokerState* state, pid_t pid) {
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid == pid) return i;
    }
    return -1;
}

static int find_topic_index(Subscriber* sub, const char* topic) {
    for (int i = 0; i < sub->topic_count; i++) {
        if (strcmp(sub->topics[i], topic) == 0) return i;
    }
    return -1;
}

int add_subscriber(BrokerState* state, pid_t pid, const char* topic) {
    if (!state || !topic || !*topic) return -1;
    
    int idx = find_subscriber_index(state, pid);
    Subscriber* sub;
    
    if (idx == -1) {
        if (state->subscriber_count >= MAX_SUBSCRIBERS) return -1;
        sub = &state->subscribers[state->subscriber_count++];
        sub->pid = pid;
        sub->topic_count = 0;
    } else {
        sub = &state->subscribers[idx];
    }
    
    if (find_topic_index(sub, topic) != -1) return 0;
    if (sub->topic_count >= MAX_TOPICS_PER_SUB) return -1;
    
    strncpy(sub->topics[sub->topic_count], topic, MAX_TOPIC_LEN - 1);
    sub->topics[sub->topic_count][MAX_TOPIC_LEN - 1] = '\0';
    sub->topic_count++;
    return 0;
}

int remove_subscriber(BrokerState* state, pid_t pid, const char* topic) {
    if (!state || !topic) return -1;
    
    int idx = find_subscriber_index(state, pid);
    if (idx == -1) return -1;
    
    Subscriber* sub = &state->subscribers[idx];
    int topic_idx = find_topic_index(sub, topic);
    if (topic_idx == -1) return -1;
    
    for (int k = topic_idx; k < sub->topic_count - 1; k++) {
        strcpy(sub->topics[k], sub->topics[k + 1]);
    }
    sub->topic_count--;
    
    if (sub->topic_count == 0) {
        for (int k = idx; k < state->subscriber_count - 1; k++) {
            state->subscribers[k] = state->subscribers[k + 1];
        }
        state->subscriber_count--;
    }
    return 0;
}

void update_publisher(BrokerState* state, pid_t pid, bool add) {
    if (!state || pid <= 0) return;
    
    if (add) {
        for (int i = 0; i < state->publisher_count; i++) {
            if (state->publishers[i].pid == pid) return;
        }
        if (state->publisher_count < MAX_PUBLISHERS) {
            state->publishers[state->publisher_count++].pid = pid;
        }
    } else {
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
}

bool is_subscribed(BrokerState* state, pid_t pid, const char* topic) {
    if (!state || !topic) return false;
    
    int idx = find_subscriber_index(state, pid);
    if (idx == -1) return false;
    
    return find_topic_index(&state->subscribers[idx], topic) != -1;
}

int broadcast_message(BrokerState* state, const char* topic, const char* payload) {
    if (!state || !topic || !payload) return 0;
    
    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "send,%s,%s", topic, payload);
    
    int sent_count = 0;
    for (int i = 0; i < state->subscriber_count; i++) {
        if (is_subscribed(state, state->subscribers[i].pid, topic)) {
            if (send_message(state->msg_queue_id, (long)state->subscribers[i].pid, message) == 0) {
                sent_count++;
            }
        }
    }
    return sent_count;
}

void cleanup_broker(BrokerState* state) {
    if (!state) return;
    
    for (int i = 0; i < state->publisher_count; i++) {
        if (state->publishers[i].pid > 0) kill(state->publishers[i].pid, SIGINT);
    }
    for (int i = 0; i < state->subscriber_count; i++) {
        if (state->subscribers[i].pid > 0) kill(state->subscribers[i].pid, SIGINT);
    }
    
    if (state->msg_queue_id >= 0) {
        struct msqid_ds buf;
        for (int i = 0; i < 10; i++) {
            if (msgctl(state->msg_queue_id, IPC_STAT, &buf) == -1 || buf.msg_qnum == 0) break;
            usleep(50000);
        }
        remove_message_queue(state->msg_queue_id);
    }
    
    state->running = false;
}

int publisher_send_message(int msgid, const char* topic, const char* payload) {
    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "send,%d,%s,%s", getpid(), topic, payload);
    return send_message(msgid, 1, message);
}

int subscriber_change_subscription(int msgid, pid_t pid, const char* topic, const char* action) {
    char message[MAX_TEXT_LEN];
    snprintf(message, sizeof(message), "%s,%d,%s", action, pid, topic);
    return send_message(msgid, 1, message);
}

int subscriber_receive_messages(int msgid, pid_t pid, char* buffer, size_t buf_size) {
    return receive_message(msgid, (long)pid, buffer, buf_size);
}

void parse_message(const char* text, char* command, pid_t* pid, char* topic, char* payload) {
    if (!text) {
        if (command) command[0] = '\0';
        if (pid) *pid = 0;
        if (topic) topic[0] = '\0';
        if (payload) payload[0] = '\0';
        return;
    }
    
    char* copy = strdup(text);
    char* token = strtok(copy, ",");
    
    if (command && token) {
        strncpy(command, token, 19);
        command[19] = '\0';
    }
    
    token = strtok(NULL, ",");
    if (pid && token) *pid = (pid_t)atoi(token);
    
    token = strtok(NULL, ",");
    if (topic && token) {
        strncpy(topic, token, MAX_TOPIC_LEN - 1);
        topic[MAX_TOPIC_LEN - 1] = '\0';
    }
    
    token = strtok(NULL, ",");
    if (payload && token) {
        strncpy(payload, token, MAX_PAYLOAD_LEN - 1);
        payload[MAX_PAYLOAD_LEN - 1] = '\0';
    }
    
    free(copy);
}

void signal_handler(int sig) {
    if (sig == SIGINT) signal_received = 1;
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}