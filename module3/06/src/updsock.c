#include "updsock.h"

static Client* global_client = NULL;

void get_current_time(char* buffer, size_t size){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, size, "%H:%M:%S", timeinfo);
}

void chat_set_callbacks(Client* client, void (*msg_cb)(const char*, const char*, const char*), void (*status_cb)(const char*)){
    if (client){
        client->message_callback = msg_cb;
        client->status_vallback = status_cb;
    }
}

void chat_set_running(Client* client, int running){
    if (client){
        client-> running = running;
    }
}

int chat_is_running(Client* client){
    return client ? client->running : 0;
}

Client* chat_create(const char* username) {
    Client* client = (Client*)malloc(sizeof(Client));
    if (!client) {
        return NULL;
    }

    memset(client, 0, sizeof(Client));
}

void chat_cleanup(Cleint* client){
    if (!client) return;

    client->running = 0;
    if (client->recieve_thread) {
        pthread_join(client->recieve_thread, NULL);
    }
    
    if (client->sockfd >= 0) {
        close(client->sockfd);
    }
}

void chat_destroy(Client* client) {
    if (!client) return;
    chat_cleanup(client);
    free(client);
}