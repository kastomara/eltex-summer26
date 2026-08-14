#include "processPX.h"

shared_memory_t *global_shm = NULL;
volatile sig_atomic_t shutdown_requested = 0;

void signal_handler(int sig) {
    shutdown_requested = 1;
    printf("\n[SIGNAL] Получен сигнал %d, завершение программы...\n", sig);
    
    if (global_shm) {
        destroy_shared_memory(global_shm);
    }
    exit(0);
}

void producer_function(shared_memory_t *shm) {
    srand(time(NULL) ^ getpid());
    
    int current_offset = sizeof(int);
    int block_count = 0;
    int total_numbers = 0;
    int last_block_offset = -1;
    
    printf("[PRODUCER PID=%d] Начало работы\n", getpid());
    
    usleep(100000);
    
    while (1) {
        if (shutdown_requested) {
            printf("[PRODUCER] Получен сигнал завершения\n");
            break;
        }
        
        if (current_offset + sizeof(data_block_t) >= SHM_SIZE) {
            printf("[PRODUCER] Разделяемая память заполнена (offset=%d)\n", current_offset);
            break;
        }
        
        int count = (rand() % MAX_NUMBERS) + 1;
        int block_size = sizeof(data_block_t) + (count - 1) * sizeof(int);
        
        if (current_offset + block_size >= SHM_SIZE) {
            printf("[PRODUCER] Недостаточно места для блока с %d числами\n", count);
            break;
        }
        
        semaphore_wait(shm->sem);
        
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + current_offset);
        int generated = generate_data_block(block, count);
        
        int next_offset = current_offset + block_size;
        block->next_offset = next_offset;
        
        last_block_offset = current_offset;
        current_offset = next_offset;
        block_count++;
        total_numbers += generated;
        
        semaphore_signal(shm->sem);
        
        printf("[PRODUCER] Блок #%d: %d чисел, смещение=%d\n", 
               block_count, generated, current_offset);
        
        usleep((rand() % 500000) + 100000);
    }
    
    semaphore_wait(shm->sem);
    
    if (last_block_offset >= 0) {
        mark_last_block(shm, last_block_offset);
        printf("[PRODUCER] Последний блок помечен (offset=%d)\n", last_block_offset);
    }
    
    int *producer_done = (int*)shm->shmaddr;
    *producer_done = 1;
    
    semaphore_signal(shm->sem);
    
    printf("[PRODUCER] Завершение работы. Всего блоков: %d, чисел: %d\n", 
           block_count, total_numbers);
}

void consumer_function(shared_memory_t *shm, int id) {
    srand(time(NULL) ^ getpid() ^ (id * 777));
    
    int processed_blocks = 0;
    int processed_numbers = 0;
    int idle_cycles = 0;
    int max_idle_cycles = 10;
    
    printf("[CONSUMER #%d PID=%d] Начало работы\n", id, getpid());
    
    usleep(100000 * id);
    
    while (1) {
        if (shutdown_requested) {
            printf("[CONSUMER #%d] Получен сигнал завершения\n", id);
            break;
        }
        
        semaphore_wait(shm->sem);
        
        data_block_t *block = NULL;
        int offset = 0;
        
        if (find_unprocessed_block(shm, sizeof(int), &block, &offset)) {
            int min_val, max_val;
            if (get_block_stats(block, &min_val, &max_val)) {
                int actual_count = get_actual_count(block);
                printf("[CONSUMER #%d] Блок: мин=%d, макс=%d, чисел=%d, offset=%d\n", 
                       id, min_val, max_val, actual_count, offset);
                processed_numbers += actual_count;
            }
            
            mark_block_processed(block);
            processed_blocks++;
            idle_cycles = 0;
            
            semaphore_signal(shm->sem);
            
            int sleep_time = (rand() % MAX_WAIT_TIME) + 1;
            printf("[CONSUMER #%d] Обработано блоков: %d, засыпает на %d сек\n", 
                   id, processed_blocks, sleep_time);
            sleep(sleep_time);
        } else {
            int producer_finished = *((int*)shm->shmaddr);
            semaphore_signal(shm->sem);
            
            if (producer_finished) {
                semaphore_wait(shm->sem);
                int has_data = has_unprocessed_blocks(shm);
                semaphore_signal(shm->sem);
                
                if (!has_data) {
                    printf("[CONSUMER #%d] Производитель завершен, все блоки обработаны\n", id);
                    break;
                }
            }
            
            idle_cycles++;
            printf("[CONSUMER #%d] Нет данных, ожидание... (цикл %d)\n", 
                   id, idle_cycles);
            
            if (idle_cycles >= max_idle_cycles) {
                semaphore_wait(shm->sem);
                int final_check = has_unprocessed_blocks(shm);
                int prod_finished = *((int*)shm->shmaddr);
                semaphore_signal(shm->sem);
                
                if (prod_finished && !final_check) {
                    printf("[CONSUMER #%d] Завершаю работу (превышено время ожидания)\n", id);
                    break;
                }
                idle_cycles = 0;
            }
            
            sleep(1);
        }
    }
    
    printf("[CONSUMER #%d] Завершение работы. Обработано блоков: %d, чисел: %d\n", 
           id, processed_blocks, processed_numbers);
}

int main(int argc, char *argv[]) {
    int num_consumers = 2;
    
    if (argc > 1) {
        num_consumers = atoi(argv[1]);
        if (num_consumers < 1) num_consumers = 1;
        if (num_consumers > 20) num_consumers = 20;
    }
    
    printf("========================================\n");
    printf("ЗАПУСК ПРОГРАММЫ\n");
    printf("Количество потребителей: %d\n", num_consumers);
    printf("Размер разделяемой памяти: %d байт\n", SHM_SIZE);
    printf("========================================\n\n");
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    global_shm = create_shared_memory();
    if (!global_shm) {
        fprintf(stderr, "ОШИБКА: Не удалось создать разделяемую память\n");
        exit(1);
    }
    
    int *producer_done = (int*)global_shm->shmaddr;
    *producer_done = 0;
    
    printf("[MAIN] Создана разделяемая память: %s\n", SHM_NAME);
    printf("[MAIN] Создан семафор: %s\n", global_shm->sem_name);
    printf("[MAIN] Адрес памяти: %p\n", global_shm->shmaddr);
    
    pid_t producer_pid = fork();
    if (producer_pid == -1) {
        perror("fork producer");
        destroy_shared_memory(global_shm);
        exit(1);
    }
    
    if (producer_pid == 0) {
        producer_function(global_shm);
        exit(0);
    }
    
    printf("[MAIN] Создан PRODUCER: PID=%d\n", producer_pid);
    
    sleep(1);
    
    pid_t consumer_pids[num_consumers];
    for (int i = 0; i < num_consumers; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork consumer");
            for (int j = 0; j < i; j++) {
                kill(consumer_pids[j], SIGTERM);
            }
            kill(producer_pid, SIGTERM);
            destroy_shared_memory(global_shm);
            exit(1);
        }
        
        if (pid == 0) {
            consumer_function(global_shm, i + 1);
            exit(0);
        }
        
        consumer_pids[i] = pid;
        printf("[MAIN] Создан CONSUMER #%d: PID=%d\n", i + 1, pid);
    }
    
    printf("\n[MAIN] Все процессы созданы. Ожидание завершения...\n");
    printf("========================================\n\n");
    
    int status;
    waitpid(producer_pid, &status, 0);
    printf("\n[MAIN] PRODUCER (PID=%d) завершился\n", producer_pid);
    
    for (int i = 0; i < num_consumers; i++) {
        waitpid(consumer_pids[i], &status, 0);
        printf("[MAIN] CONSUMER #%d (PID=%d) завершился\n", i + 1, consumer_pids[i]);
    }
    
    printf("\n[MAIN] Удаление разделяемой памяти...\n");
    destroy_shared_memory(global_shm);
    global_shm = NULL;
    
    printf("========================================\n");
    printf("ПРОГРАММА ЗАВЕРШЕНА\n");
    printf("========================================\n");
    
    return 0;
}