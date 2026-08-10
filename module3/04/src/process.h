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

#define SHM_SIZE 4096
#define MAX_NUMBERS 100
#define MAX_WAIT_TIME 3

#define BLOCK_UNPROCESSED 1
#define BLOCK_PROCESSED 0
#define BLOCK_LAST 0x80000000

typedef struct data_block {
    int count;
    int next_offset;
    int numbers[1];
} data_block_t;

typedef struct shared_memory {
    int shmid;
    void *shmaddr;
    int semid;
} shared_memory_t;

shared_memory_t* create_shared_memory();
void destroy_shared_memory(shared_memory_t *shm);
void semaphore_wait(int semid);
void semaphore_signal(int semid);

int generate_data_block(data_block_t *block, int max_count);
int find_unprocessed_block(shared_memory_t *shm, int start_offset, data_block_t **block_ptr, int *offset_ptr);
void mark_block_processed(data_block_t *block);
int has_unprocessed_blocks(shared_memory_t *shm);
int get_block_stats(data_block_t *block, int *min_val, int *max_val);
void mark_last_block(shared_memory_t *shm);
int is_last_block(data_block_t *block);
int get_actual_count(data_block_t *block);

#endif