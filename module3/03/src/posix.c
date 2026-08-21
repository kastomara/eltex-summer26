#include "posix.h"

int initChat(ChatInfo *inf, const char *base_name) {
    char queue1[128], queue2[128];
    struct mq_attr attr;

    if (base_name[0] == '/') {
        snprintf(queue1, sizeof(queue1), "%s_1", base_name);
        snprintf(queue2, sizeof(queue2), "%s_2", base_name);
    } else {
        snprintf(queue1, sizeof(queue1), "/%s_1", base_name);
        snprintf(queue2, sizeof(queue2), "/%s_2", base_name);
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;

    inf->created_queues = 0;
    inf->running = 1;
    inf->send_queue = (mqd_t)-1;
    inf->receive_queue = (mqd_t)-1;
    
    mqd_t q1 = mq_open(queue1, O_RDWR | O_CREAT | O_EXCL, 0666, &attr);
    
    if (q1 == (mqd_t)-1 && errno == EEXIST) {
        q1 = mq_open(queue1, O_RDWR);
        if (q1 == (mqd_t)-1) {
            perror("mq_open queue1");
            return -1;
        }
        
        mqd_t q2 = mq_open(queue2, O_RDWR);
        if (q2 == (mqd_t)-1) {
            perror("mq_open queue2");
            mq_close(q1);
            return -1;
        }
        
        inf->send_queue = q1;
        inf->receive_queue = q2;
        inf->created_queues = 0;
        
    } else if (q1 != (mqd_t)-1) {
        mqd_t q2 = mq_open(queue2, O_RDWR | O_CREAT | O_EXCL, 0666, &attr);
        
        if (q2 == (mqd_t)-1) {
            perror("mq_open queue2");
            mq_close(q1);
            mq_unlink(queue1);
            return -1;
        }
        
        inf->send_queue = q2;
        inf->receive_queue = q1;
        inf->created_queues = 1;
        
    } else {
        perror("mq_open");
        return -1;
    }

    strncpy(inf->name, base_name, sizeof(inf->name) - 1);
    inf->name[sizeof(inf->name) - 1] = '\0';
    
    return 0;
}

void cleanChat(ChatInfo *inf) {
    char queue1[128], queue2[128];
    
    if (inf->send_queue != (mqd_t)-1) {
        mq_close(inf->send_queue);
        inf->send_queue = (mqd_t)-1;
    }
    
    if (inf->receive_queue != (mqd_t)-1) {
        mq_close(inf->receive_queue);
        inf->receive_queue = (mqd_t)-1;
    }

    if (inf->created_queues) {
        if (inf->name[0] == '/') {
            snprintf(queue1, sizeof(queue1), "%s_1", inf->name);
            snprintf(queue2, sizeof(queue2), "%s_2", inf->name);
        } else {
            snprintf(queue1, sizeof(queue1), "/%s_1", inf->name);
            snprintf(queue2, sizeof(queue2), "/%s_2", inf->name);
        }
        
        mq_unlink(queue1);
        mq_unlink(queue2);
        inf->created_queues = 0;
    }
    
    inf->running = 0;
}

void sendMSG(ChatInfo *inf, const char *msg, unsigned int priority) {
    if (inf->send_queue == (mqd_t)-1) return;
    if (mq_send(inf->send_queue, msg, strlen(msg) + 1, priority) == -1) {
        perror("mq_send");
    }
}

int receiveMSG(ChatInfo *inf, char *buffer, unsigned int *priority) {
    if (inf->receive_queue == (mqd_t)-1) return -1;

    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;
    
    ssize_t bytes = mq_timedreceive(inf->receive_queue, buffer, MSG_SIZE, priority, &timeout);
    
    if (bytes == -1) {
        if (errno == ETIMEDOUT) {
            return 0;
        }
        if (errno != EBADF && errno != EINTR) {
            perror("mq_receive");
        }
        return -1;
    }
    
    buffer[bytes] = '\0';
    return (int)bytes;
}
