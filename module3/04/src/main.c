#include "process.h"

// Глобальные переменные для обработки сигналов
shared_memory_t *global_shm = NULL;

void signal_handler(int sig) {
    (void)sig;
    if (global_shm) {
        destroy_shared_memory(global_shm);
    }
    exit(0);
}

// Функция производителя
void producer_function(shared_memory_t *shm) {
    srand(time(NULL) ^ getpid());
    
    int current_offset = sizeof(int); // Пропускаем первый int (флаг завершения)
    int block_count = 0;
    int total_numbers = 0;
    
    printf("[ПРОИЗВОДИТЕЛЬ PID=%d] Начало работы\n", getpid());
    
    while (1) {
        // Проверяем место в памяти
        if (current_offset + sizeof(data_block_t) >= SHM_SIZE) {
            printf("[ПРОИЗВОДИТЕЛЬ] Разделяемая память заполнена (offset=%d)\n", current_offset);
            break;
        }
        
        // Генерируем данные
        int count = (rand() % MAX_NUMBERS) + 1;
        int block_size = sizeof(data_block_t) + (count - 1) * sizeof(int);
        
        if (current_offset + block_size >= SHM_SIZE) {
            printf("[ПРОИЗВОДИТЕЛЬ] Недостаточно места для блока с %d числами\n", count);
            break;
        }
        
        // Захватываем семафор
        semaphore_wait(shm->semid);
        
        // Записываем блок
        data_block_t *block = (data_block_t*)((char*)shm->shmaddr + current_offset);
        int generated = generate_data_block(block, MAX_NUMBERS);
        block->next_offset = current_offset + block_size;
        
        current_offset = block->next_offset;
        block_count++;
        total_numbers += generated;
        
        // Освобождаем семафор
        semaphore_signal(shm->semid);
        
        printf("[ПРОИЗВОДИТЕЛЬ] Блок #%d: %d чисел, смещение=%d\n", 
               block_count, generated, current_offset);
        
        // Задержка
        usleep((rand() % 500000) + 100000);
    }
    
    // Помечаем последний блок
    semaphore_wait(shm->semid);
    mark_last_block(shm);
    
    // Устанавливаем флаг, что производитель завершил работу
    int *producer_done = (int*)shm->shmaddr;
    *producer_done = 1;
    
    semaphore_signal(shm->semid);
    
    printf("[ПРОИЗВОДИТЕЛЬ] Завершение работы. Всего блоков: %d, чисел: %d\n", 
           block_count, total_numbers);
}

// Функция потребителя
void consumer_function(shared_memory_t *shm, int id) {
    srand(time(NULL) ^ getpid() ^ (id * 777));
    
    int processed_blocks = 0;
    int processed_numbers = 0;
    int idle_cycles = 0;
    int max_idle_cycles = 5; // Максимальное количество циклов ожидания
    
    printf("[ПОТРЕБИТЕЛЬ #%d PID=%d] Начало работы\n", id, getpid());
    
    while (1) {
        semaphore_wait(shm->semid);
        
        data_block_t *block = NULL;
        int offset = 0;
        
        if (find_unprocessed_block(shm, sizeof(int), &block, &offset)) {
            // Получаем статистику
            int min_val, max_val;
            if (get_block_stats(block, &min_val, &max_val)) {
                printf("[ПОТРЕБИТЕЛЬ #%d] Блок: мин=%d, макс=%d, чисел=%d\n", 
                       id, min_val, max_val, block->count);
                processed_numbers += block->count;
            }
            
            // Помечаем как обработанный
            mark_block_processed(block);
            processed_blocks++;
            idle_cycles = 0;  // Сброс счетчика
            
            semaphore_signal(shm->semid);
            
            // Засыпаем
            int sleep_time = (rand() % MAX_WAIT_TIME) + 1;
            printf("[ПОТРЕБИТЕЛЬ #%d] Засыпает на %d сек\n", id, sleep_time);
            sleep(sleep_time);
        } else {
            // Нет необработанных блоков
            semaphore_signal(shm->semid);
            
            // Проверяем, завершил ли производитель работу
            semaphore_wait(shm->semid);
            int producer_finished = *((int*)shm->shmaddr);
            semaphore_signal(shm->semid);
            
            if (producer_finished) {
                // Проверяем, есть ли еще необработанные блоки
                semaphore_wait(shm->semid);
                int has_data = has_unprocessed_blocks(shm);
                semaphore_signal(shm->semid);
                
                if (!has_data) {
                    printf("[ПОТРЕБИТЕЛЬ #%d] Производитель завершен, все блоки обработаны\n", id);
                    break;
                }
            }
            
            idle_cycles++;
            printf("[ПОТРЕБИТЕЛЬ #%d] Нет необработанных блоков, ожидание... (%d/%d)\n", 
                   id, idle_cycles, max_idle_cycles);
            
            // Если слишком много циклов без данных и производитель завершен - выходим
            if (producer_finished && idle_cycles >= max_idle_cycles) {
                printf("[ПОТРЕБИТЕЛЬ #%d] Слишком много циклов ожидания, завершаю работу\n", id);
                break;
            }
            
            sleep(1);
        }
    }
    
    printf("[ПОТРЕБИТЕЛЬ #%d] Завершение работы. Обработано блоков: %d, чисел: %d\n", 
           id, processed_blocks, processed_numbers);
}

int main(int argc, char *argv[]) {
    int num_consumers = 2;
    
    // Обработка аргументов командной строки
    if (argc > 1) {
        num_consumers = atoi(argv[1]);
        if (num_consumers < 1) num_consumers = 1;
        if (num_consumers > 10) num_consumers = 10;
    }
    
    printf("========================================\n");
    printf("ЗАПУСК ПРОГРАММЫ\n");
    printf("Количество потребителей: %d\n", num_consumers);
    printf("Размер разделяемой памяти: %d байт\n", SHM_SIZE);
    printf("========================================\n\n");
    
    // Установка обработчика сигналов
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Создаем разделяемую память
    global_shm = create_shared_memory();
    if (!global_shm) {
        fprintf(stderr, "ОШИБКА: Не удалось создать разделяемую память\n");
        exit(1);
    }
    
    // Инициализируем флаг завершения производителя (первый int в памяти)
    int *producer_done = (int*)global_shm->shmaddr;
    *producer_done = 0;
    
    printf("[MAIN] Создана разделяемая память: shmid=%d, semid=%d\n", 
           global_shm->shmid, global_shm->semid);
    
    // Создаем производителя
    pid_t producer_pid = fork();
    if (producer_pid == -1) {
        perror("fork producer");
        destroy_shared_memory(global_shm);
        exit(1);
    }
    
    if (producer_pid == 0) {
        // Дочерний процесс - производитель
        producer_function(global_shm);
        exit(0);
    }
    
    printf("[MAIN] Создан производитель: PID=%d\n", producer_pid);
    
    // Создаем потребителей
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
            // Дочерний процесс - потребитель
            consumer_function(global_shm, i + 1);
            exit(0);
        }
        
        consumer_pids[i] = pid;
        printf("[MAIN] Создан потребитель #%d: PID=%d\n", i + 1, pid);
    }
    
    printf("\n[MAIN] Все процессы созданы. Ожидание завершения...\n");
    printf("========================================\n\n");
    
    // Ожидаем завершения производителя
    int status;
    waitpid(producer_pid, &status, 0);
    printf("\n[MAIN] Производитель (PID=%d) завершился\n", producer_pid);
    
    // Ожидаем завершения потребителей
    for (int i = 0; i < num_consumers; i++) {
        waitpid(consumer_pids[i], &status, 0);
        printf("[MAIN] Потребитель #%d (PID=%d) завершился\n", i + 1, consumer_pids[i]);
    }
    
    // Удаляем разделяемую память ТОЛЬКО здесь
    printf("\n[MAIN] Удаление разделяемой памяти...\n");
    destroy_shared_memory(global_shm);
    global_shm = NULL;
    
    printf("========================================\n");
    printf("ПРОГРАММА ЗАВЕРШЕНА\n");
    printf("========================================\n");
    
    return 0;
}