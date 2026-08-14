#include "processPX.h"

int get_actual_count(data_block_t *block) {
    if (!block) return 0;
    return block->count & BLOCK_PROCESSED_MASK;
}

int is_last_block(data_block_t *block) {
    if (!block) return 0;
    return (block->count & BLOCK_LAST) != 0;
}

void cleanup_shared_resources() {
    printf("[CLEANUP] Очистка старых POSIX ресурсов...\n");
    
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
        printf("[CLEANUP] Удалена разделяемая память: %s\n", SHM_NAME);
    }
    
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem != SEM_FAILED) {
        sem_close(sem);
        sem_unlink(SEM_NAME);
        printf("[CLEANUP] Удален семафор: %s\n", SEM_NAME);
    }
    
    printf("[CLEANUP] Очистка завершена\n");
}

shared_memory_t* create_shared_memory() {
    cleanup_shared_resources();
    
    shared_memory_t *shm = malloc(sizeof(shared_memory_t));
    if (!shm) {
        perror("malloc");
        return NULL;
    }
    
    shm->shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
    if (shm->shm_fd == -1) {
        shm->shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm->shm_fd == -1) {
            perror("shm_open");
            free(shm);
            return NULL;
        }
        printf("[SHM] Использована существующая разделяемая память\n");
    } else {
        printf("[SHM] Создана новая разделяемая память\n");
    }
        if (ftruncate(shm->shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        close(shm->shm_fd);
        shm_unlink(SHM_NAME);
        free(shm);
        return NULL;
    }
        shm->shmaddr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_SHARED, shm->shm_fd, 0);
    if (shm->shmaddr == MAP_FAILED) {
        perror("mmap");
        close(shm->shm_fd);
        shm_unlink(SHM_NAME);
        free(shm);
        return NULL;
    }
        memset(shm->shmaddr, 0, SHM_SIZE);
        snprintf(shm->sem_name, sizeof(shm->sem_name), "%s_%d", SEM_NAME, getpid());
    
    shm->sem = sem_open(shm->sem_name, O_CREAT | O_EXCL, 0666, 1);
    if (shm->sem == SEM_FAILED) {        shm->sem = sem_open(shm->sem_name, O_CREAT, 0666, 1);
        if (shm->sem == SEM_FAILED) {
            perror("sem_open");
            munmap(shm->shmaddr, SHM_SIZE);
            close(shm->shm_fd);
            shm_unlink(SHM_NAME);
            free(shm);
            return NULL;
        }
        printf("[SEM] Использован существующий семафор: %s\n", shm->sem_name);
    } else {
        printf("[SEM] Создан новый семафор: %s\n", shm->sem_name);
    }
    
    return shm;
}

void destroy_shared_memory(shared_memory_t *shm) {
    if (!shm) return;
    if (shm->shmaddr && shm->shmaddr != MAP_FAILED) {
        munmap(shm->shmaddr, SHM_SIZE);
    }
    if (shm->shm_fd != -1) {
        close(shm->shm_fd);
    }
    shm_unlink(SHM_NAME);
    if (shm->sem && shm->sem != SEM_FAILED) {
        sem_close(shm->sem);
        sem_unlink(shm->sem_name);
    }
    
    free(shm);
}

void semaphore_wait(sem_t *sem) {
    if (!sem) return;
    
    while (sem_wait(sem) == -1) {
        if (errno == EINTR) continue;
        perror("sem_wait");
        break;
    }
}

void semaphore_signal(sem_t *sem) {
    if (!sem) return;
    
    while (sem_post(sem) == -1) {
        if (errno == EINTR) continue;
        perror("sem_post");
        break;
    }
}

int generate_data_block(data_block_t *block, int ncount) {
    if (!block || ncount <= 0) return 0;
    
    block->count = ncount;
    block->next_offset = 0;
    
    for (int i = 0; i < ncount; i++) {
        block->numbers[i] = rand() % 1000;
    }
    
    return ncount;
}

int find_unprocessed_block(shared_memory_t *shm, int start_offset, data_block_t **block_ptr, int *offset_ptr) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = start_offset;
    int max_offset = SHM_SIZE - sizeof(data_block_t);
    int visited = 0;
    int max_visited = SHM_SIZE / sizeof(data_block_t) + 10;
    
    while (offset < SHM_SIZE && offset < max_offset && visited < max_visited) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        visited++;
        
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
        int is_last = (block->count & BLOCK_LAST) != 0;
        block->count = is_last ? BLOCK_LAST : 0;
    }
}

int has_unprocessed_blocks(shared_memory_t *shm) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = sizeof(int);
    int max_offset = SHM_SIZE - sizeof(data_block_t);
    int visited = 0;
    int max_visited = SHM_SIZE / sizeof(data_block_t) + 10;
    
    while (offset < SHM_SIZE && offset < max_offset && visited < max_visited) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        visited++;
        
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

void mark_last_block(shared_memory_t *shm, int last_block_offset) {
    if (!shm || !shm->shmaddr || last_block_offset < 0) return;
    if (last_block_offset >= SHM_SIZE) return;
    
    data_block_t *block = (data_block_t*)((char*)shm->shmaddr + last_block_offset);
    if (block) {
        int actual_count = get_actual_count(block);
        block->count = actual_count | BLOCK_LAST;
        block->next_offset = 0;
    }
}