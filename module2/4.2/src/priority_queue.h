#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PRIORITY 255
#define MIN_PRIORITY 0
#define MAX_QUEUE_SIZE 1000

typedef struct {
    uint8_t priority;
    int data;
    uint64_t sequence;
} QueueElement;

typedef struct {
    QueueElement* elements[MAX_QUEUE_SIZE];
    int size;
    uint64_t next_sequence;
} PriorityQueue;

PriorityQueue* create_queue(void);
void destroy_queue(PriorityQueue* queue);
bool enqueue(PriorityQueue* queue, uint8_t priority, int data);

QueueElement* peek_front(const PriorityQueue* queue);
QueueElement* peek_by_priority(const PriorityQueue* queue, uint8_t priority);
QueueElement* peek_by_min_priority(const PriorityQueue* queue, uint8_t min_priority);

QueueElement* dequeue_front(PriorityQueue* queue);
QueueElement* dequeue_by_priority(PriorityQueue* queue, uint8_t priority);
QueueElement* dequeue_by_min_priority(PriorityQueue* queue, uint8_t min_priority);

bool is_empty(const PriorityQueue* queue);
int get_size(const PriorityQueue* queue);
void print_queue(const PriorityQueue* queue);

#endif