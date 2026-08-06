#include "process.h"

int get_actual_count(data_block_t *block) {
    if (!block) return 0;
    return block->count & 0x7FFFFFFF;
}

int is_last_block(data_block_t *block) {
    if (!block) return 0;
    return (block->count & BLOCK_LAST) != 0;
}

shared_memory_t* create_shared_memory() {
    shared_memory_t *shm = malloc(sizeof(shared_memory_t));
    if (!shm) {
        return NULL;
    }
    
    shm->shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm->shmid == -1) {
        perror("shmget");
        free(shm);
        return NULL;
    }
    
    shm->shmaddr = shmat(shm->shmid, NULL, 0);
    if (shm->shmaddr == (void*)-1) {
        perror("shmat");
        shmctl(shm->shmid, IPC_RMID, NULL);
        free(shm);
        return NULL;
    }
    
    memset(shm->shmaddr, 0, SHM_SIZE);
    
    shm->semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (shm->semid == -1) {
        perror("semget");
        shmdt(shm->shmaddr);
        shmctl(shm->shmid, IPC_RMID, NULL);
        free(shm);
        return NULL;
    }
    
    if (semctl(shm->semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        shmdt(shm->shmaddr);
        shmctl(shm->shmid, IPC_RMID, NULL);
        semctl(shm->semid, 0, IPC_RMID);
        free(shm);
        return NULL;
    }
    
    return shm;
}

void destroy_shared_memory(shared_memory_t *shm) {
    if (!shm) return;
    
    if (shm->shmaddr && shm->shmaddr != (void*)-1) {
        shmdt(shm->shmaddr);
    }
    if (shm->shmid != -1) {
        shmctl(shm->shmid, IPC_RMID, NULL);
    }
    if (shm->semid != -1) {
        semctl(shm->semid, 0, IPC_RMID);
    }
    free(shm);
}

void semaphore_wait(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    
    while (semop(semid, &sb, 1) == -1) {
        if (errno == EINTR) continue;
        perror("semop wait");
        break;
    }
}

void semaphore_signal(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    
    while (semop(semid, &sb, 1) == -1) {
        if (errno == EINTR) continue;
        perror("semop signal");
        break;
    }
}

int generate_data_block(data_block_t *block, int max_count) {
    if (!block) return 0;
    
    int count = (rand() % max_count) + 1;
    block->count = count;
    block->next_offset = 0;
    
    for (int i = 0; i < count; i++) {
        block->numbers[i] = rand() % 1000;
    }
    
    return count;
}

int find_unprocessed_block(shared_memory_t *shm, int start_offset, 
                          data_block_t **block_ptr, int *offset_ptr) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = start_offset;
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        
        int actual_count = get_actual_count(block);
        
        if (actual_count > 0) {
            *block_ptr = block;
            *offset_ptr = offset;
            return 1;
        }
        
        if (is_last_block(block) || block->next_offset == 0) {
            break;
        }
        
        offset = block->next_offset;
        
        if (offset <= 0 || offset >= SHM_SIZE) {
            break;
        }
    }
    
    *block_ptr = NULL;
    *offset_ptr = -1;
    return 0;
}

void mark_block_processed(data_block_t *block) {
    if (block) {
        int is_last = block->count & BLOCK_LAST;
        block->count = is_last;
    }
}

int has_unprocessed_blocks(shared_memory_t *shm) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = sizeof(int);
    
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        
        int actual_count = get_actual_count(block);
        
        if (actual_count > 0) {
            return 1;
        }
        
        if (is_last_block(block) || block->next_offset == 0) {
            break;
        }
        
        offset = block->next_offset;
        
        if (offset <= 0 || offset >= SHM_SIZE) {
            break;
        }
    }
    
    return 0;
}

int get_block_stats(data_block_t *block, int *min_val, int *max_val) {
    if (!block) return 0;
    
    int actual_count = get_actual_count(block);
    if (actual_count <= 0) return 0;
    
    *min_val = block->numbers[0];
    *max_val = block->numbers[0];
    
    for (int i = 1; i < actual_count; i++) {
        if (block->numbers[i] < *min_val) *min_val = block->numbers[i];
        if (block->numbers[i] > *max_val) *max_val = block->numbers[i];
    }
    
    return 1;
}

void mark_last_block(shared_memory_t *shm) {
    if (!shm || !shm->shmaddr) return;
    
    int offset = sizeof(int);
    int last_offset = -1;
    data_block_t *last_block = NULL;
    
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        int actual_count = get_actual_count(block);
        
        if (actual_count == 0 && block->next_offset == 0) {
            break;
        }
        
        last_offset = offset;
        last_block = block;
        
        if (block->next_offset == 0) {
            break;
        }
        
        offset = block->next_offset;
        
        if (offset <= 0 || offset >= SHM_SIZE) {
            break;
        }
    }
    
    if (last_offset >= 0 && last_block) {
        int actual_count = get_actual_count(last_block);
        last_block->count = actual_count | BLOCK_LAST;
        last_block->next_offset = 0;
    }
}