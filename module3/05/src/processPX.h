#ifndef PROCESSPX_H
#define PROCESSPX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdatomic.h>

#define SHM_NAME "/producer_consumer_shm"
#define SEM_NAME "/producer_consumer_sem"
#define SHM_SIZE 4096
#define MAX_NUMBERS 100
#define MAX_WAIT_TIME 3

#define BLOCK_LAST 0x80000000
#define BLOCK_PROCESSED_MASK 0x7FFFFFFF

typedef struct data_block {
    int count;
    int next_offset;
    int numbers[1];
} data_block_t;

typedef struct shared_memory {
    int shm_fd;
    void *shmaddr;
    sem_t *sem;
    char sem_name[64];
} shared_memory_t;

shared_memory_t* create_shared_memory();
void destroy_shared_memory(shared_memory_t *shm);
void cleanup_shared_resources();
void semaphore_wait(sem_t *sem);
void semaphore_signal(sem_t *sem);

int generate_data_block(data_block_t *block, int ncount);
int find_unprocessed_block(shared_memory_t *shm, int start_offset, data_block_t **block_ptr, int *offset_ptr);
void mark_block_processed(data_block_t *block);
int has_unprocessed_blocks(shared_memory_t *shm);
int get_block_stats(data_block_t *block, int *min_val, int *max_val);
void mark_last_block(shared_memory_t *shm, int last_block_offset);
int is_last_block(data_block_t *block);
int get_actual_count(data_block_t *block);

#endif