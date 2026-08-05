#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define SHM_SIZE 4096        // Размер разделяемой памяти
#define MAX_NUMBERS 100      // Максимальное количество чисел в массиве
#define MAX_WAIT_TIME 3      // Максимальное время ожидания (сек)

// Структура блока данных в разделяемой памяти
typedef struct data_block {
    int count;               // Количество элементов (0 - блок обработан)
    int next_offset;         // Смещение до следующего блока (0 - последний)
    int numbers[1];          // Массив чисел (фактический размер определяется count)
} data_block_t;

// Структура для управления разделяемой памятью
typedef struct shared_memory {
    int shmid;               // ID разделяемой памяти
    void *shmaddr;           // Адрес разделяемой памяти
    int semid;               // ID семафора
} shared_memory_t;

// Прототипы функций для работы с разделяемой памятью
shared_memory_t* create_shared_memory();
void destroy_shared_memory(shared_memory_t *shm);
void semaphore_wait(int semid);
void semaphore_signal(int semid);

// Функции для работы с данными (без вывода на экран)
int generate_data_block(data_block_t *block, int max_count);
int find_unprocessed_block(shared_memory_t *shm, int start_offset, 
                          data_block_t **block_ptr, int *offset_ptr);
void mark_block_processed(data_block_t *block);
int has_unprocessed_blocks(shared_memory_t *shm);
int get_block_stats(data_block_t *block, int *min_val, int *max_val);
void mark_last_block(shared_memory_t *shm);

#endif