#include "process.h"

// Создание разделяемой памяти и семафора
shared_memory_t* create_shared_memory() {
    shared_memory_t *shm = malloc(sizeof(shared_memory_t));
    if (!shm) {
        return NULL;
    }
    
    // Создание разделяемой памяти
    shm->shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm->shmid == -1) {
        free(shm);
        return NULL;
    }
    
    // Присоединение к разделяемой памяти
    shm->shmaddr = shmat(shm->shmid, NULL, 0);
    if (shm->shmaddr == (void*)-1) {
        shmctl(shm->shmid, IPC_RMID, NULL);
        free(shm);
        return NULL;
    }
    
    // Инициализация разделяемой памяти нулями
    memset(shm->shmaddr, 0, SHM_SIZE);
    
    // Создание семафора
    shm->semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (shm->semid == -1) {
        shmdt(shm->shmaddr);
        shmctl(shm->shmid, IPC_RMID, NULL);
        free(shm);
        return NULL;
    }
    
    // Инициализация семафора значением 1 (доступен)
    if (semctl(shm->semid, 0, SETVAL, 1) == -1) {
        shmdt(shm->shmaddr);
        shmctl(shm->shmid, IPC_RMID, NULL);
        semctl(shm->semid, 0, IPC_RMID);
        free(shm);
        return NULL;
    }
    
    return shm;
}

// Удаление разделяемой памяти и семафора
void destroy_shared_memory(shared_memory_t *shm) {
    if (!shm) return;
    
    if (shm->shmaddr) {
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

// Ожидание семафора (P-операция)
void semaphore_wait(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    
    if (semop(semid, &sb, 1) == -1) {
        perror("semop wait");
    }
}

// Освобождение семафора (V-операция)
void semaphore_signal(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    
    if (semop(semid, &sb, 1) == -1) {
        perror("semop signal");
    }
}

// Генерация блока данных (возвращает количество сгенерированных чисел)
int generate_data_block(data_block_t *block, int max_count) {
    if (!block) return 0;
    
    int count = (rand() % max_count) + 1;
    block->count = count;
    
    for (int i = 0; i < count; i++) {
        block->numbers[i] = rand() % 1000;
    }
    
    return count;
}

// Поиск первого необработанного блока
int find_unprocessed_block(shared_memory_t *shm, int start_offset, 
                          data_block_t **block_ptr, int *offset_ptr) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = start_offset;
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        
        if (block->count > 0) {
            *block_ptr = block;
            *offset_ptr = offset;
            return 1;
        }
        
        if (block->next_offset == 0) {
            break;
        }
        offset = block->next_offset;
    }
    
    *block_ptr = NULL;
    *offset_ptr = -1;
    return 0;
}

// Пометить блок как обработанный
void mark_block_processed(data_block_t *block) {
    if (block) {
        block->count = 0;
    }
}

// Проверка наличия необработанных блоков
int has_unprocessed_blocks(shared_memory_t *shm) {
    if (!shm || !shm->shmaddr) return 0;
    
    int offset = 0;
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        
        if (block->count > 0) {
            return 1;
        }
        
        if (block->next_offset == 0) {
            break;
        }
        offset = block->next_offset;
    }
    
    return 0;
}

// Получение статистики блока (мин и макс)
int get_block_stats(data_block_t *block, int *min_val, int *max_val) {
    if (!block || block->count <= 0) return 0;
    
    *min_val = block->numbers[0];
    *max_val = block->numbers[0];
    
    for (int i = 1; i < block->count; i++) {
        if (block->numbers[i] < *min_val) *min_val = block->numbers[i];
        if (block->numbers[i] > *max_val) *max_val = block->numbers[i];
    }
    
    return 1;
}

// Пометить последний блок (установить next_offset = 0)
void mark_last_block(shared_memory_t *shm) {
    if (!shm || !shm->shmaddr) return;
    
    int offset = 0;
    int last_offset = -1;
    
    while (offset < SHM_SIZE) {
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + offset);
        if (block->count == 0) break;
        last_offset = offset;
        if (block->next_offset == 0) break;
        offset = block->next_offset;
    }
    
    if (last_offset >= 0) {
        data_block_t *last_block = (data_block_t*)((char*)shm->shmaddr + last_offset);
        last_block->next_offset = 0;
    }
}