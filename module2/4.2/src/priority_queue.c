#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PriorityQueue* create_queue(void) {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (queue == NULL) {
        return NULL;
    }
    queue->size = 0;
    queue->next_sequence = 0;
    memset(queue->elements, 0, sizeof(queue->elements));
    return queue;
}

void destroy_queue(PriorityQueue* queue) {
    if (queue == NULL) return;
    
    for (int i = 0; i < queue->size; i++) {
        free(queue->elements[i]);
    }
    free(queue);
}

bool enqueue(PriorityQueue* queue, uint8_t priority, int data) {
    if (queue == NULL || queue->size >= MAX_QUEUE_SIZE) {
        return false;
    }
    
    QueueElement* new_elem = (QueueElement*)malloc(sizeof(QueueElement));
    if (new_elem == NULL) {
        return false;
    }
    
    new_elem->priority = priority;
    new_elem->data = data;
    new_elem->sequence = queue->next_sequence++;
    
    int pos = queue->size;
    while (pos > 0 && queue->elements[pos - 1]->priority > priority) {
        pos--;
    }
    
    for (int i = queue->size; i > pos; i--) {
        queue->elements[i] = queue->elements[i - 1];
    }
    
    queue->elements[pos] = new_elem;
    queue->size++;
    return true;
}

QueueElement* peek_front(const PriorityQueue* queue) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }
    return queue->elements[0];
}

QueueElement* peek_by_priority(const PriorityQueue* queue, uint8_t priority) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }
    
    for (int i = 0; i < queue->size; i++) {
        if (queue->elements[i]->priority == priority) {
            return queue->elements[i];
        }
    }
    return NULL;
}

QueueElement* peek_by_min_priority(const PriorityQueue* queue, uint8_t min_priority) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }

    int pos = -1;
    uint8_t highest_priority = 255;
    
    for (int i = 0; i < queue->size; i++) {
        if (queue->elements[i]->priority >= min_priority) {
            if (queue->elements[i]->priority < highest_priority) {
                highest_priority = queue->elements[i]->priority;
                pos = i;
            }
        }
    }
    
    if (pos == -1) {
        return NULL;
    }
    
    return queue->elements[pos];
}

QueueElement* dequeue_front(PriorityQueue* queue) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }
    
    QueueElement* result = queue->elements[0];
    
    for (int i = 0; i < queue->size - 1; i++) {
        queue->elements[i] = queue->elements[i + 1];
    }
    queue->size--;
    return result;
}

QueueElement* dequeue_by_priority(PriorityQueue* queue, uint8_t priority) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }
    
    int pos = -1;
    for (int i = 0; i < queue->size; i++) {
        if (queue->elements[i]->priority == priority) {
            pos = i;
            break;
        }
    }
    
    if (pos == -1) {
        return NULL;
    }
    
    QueueElement* result = queue->elements[pos];

    for (int i = pos; i < queue->size - 1; i++) {
        queue->elements[i] = queue->elements[i + 1];
    }
    queue->size--;
    return result;
}

QueueElement* dequeue_by_min_priority(PriorityQueue* queue, uint8_t min_priority) {
    if (queue == NULL || queue->size == 0) {
        return NULL;
    }

    int pos = -1;
    uint8_t highest_priority = 255;
    
    for (int i = 0; i < queue->size; i++) {
        if (queue->elements[i]->priority >= min_priority) {
            if (queue->elements[i]->priority < highest_priority) {
                highest_priority = queue->elements[i]->priority;
                pos = i;
            }
        }
    }
    
    if (pos == -1) {
        return NULL;
    }
    
    QueueElement* result = queue->elements[pos];
    
    for (int i = pos; i < queue->size - 1; i++) {
        queue->elements[i] = queue->elements[i + 1];
    }
    queue->size--;
    
    return result;
}

bool is_empty(const PriorityQueue* queue) {
    return queue == NULL || queue->size == 0;
}

int get_size(const PriorityQueue* queue) {
    return queue == NULL ? 0 : queue->size;
}

void print_queue(const PriorityQueue* queue) {
    if (queue == NULL) {
        printf("Очередь не инициализирована\n");
        return;
    }
    
    printf("Размер очереди: %d\n", queue->size);
    printf("Элементы (приоритет, данные, номер):\n");
    for (int i = 0; i < queue->size; i++) {
        printf("  [%d] Приор: %d, Данные: %d, Номер: %llu\n", 
               i, queue->elements[i]->priority, 
               queue->elements[i]->data, 
               queue->elements[i]->sequence);
    }
}