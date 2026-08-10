#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_TOPIC_LEN 50
#define MAX_PAYLOAD_LEN 256
#define MAX_TEXT_LEN 300
#define MAX_SUBSCRIBERS 100
#define MAX_PUBLISHERS 100
#define MAX_TOPICS_PER_SUB 10

typedef struct {
    long mtype;
    char mtext[MAX_TEXT_LEN];
} Message;

typedef struct {
    pid_t pid;
    char topics[MAX_TOPICS_PER_SUB][MAX_TOPIC_LEN];
    int topic_count;
} Subscriber;

typedef struct {
    pid_t pid;
} Publisher;

typedef struct {
    int msg_queue_id;
    Subscriber subscribers[MAX_SUBSCRIBERS];
    int subscriber_count;
    Publisher publishers[MAX_PUBLISHERS];
    int publisher_count;
    bool running;
} BrokerState;

extern volatile sig_atomic_t signal_received;

int create_message_queue(key_t key);
int get_message_queue(key_t key);
void remove_message_queue(int msgid);
int send_message(int msgid, long mtype, const char* text);
int receive_message(int msgid, long mtype, char* buffer, size_t buf_size);
    
void init_broker_state(BrokerState* state);
int add_subscriber(BrokerState* state, pid_t pid, const char* topic);
int remove_subscriber(BrokerState* state, pid_t pid, const char* topic);
void add_publisher(BrokerState* state, pid_t pid);
void remove_publisher(BrokerState* state, pid_t pid);
bool is_subscribed(BrokerState* state, pid_t pid, const char* topic);
int broadcast_message(BrokerState* state, const char* topic, const char* payload);
void cleanup_broker(BrokerState* state);

int publisher_send_message(int msgid, const char* topic, const char* payload);
int subscriber_subscribe(int msgid, pid_t pid, const char* topic);
int subscriber_unsubscribe(int msgid, pid_t pid, const char* topic);
int subscriber_receive_messages(int msgid, pid_t pid, char* buffer, size_t buf_size);

void parse_message(const char* text, char* command, pid_t* pid, char* topic);
void signal_handler(int sig);
void setup_signal_handlers(void);

#endif