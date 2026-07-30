#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void display_menu(void) {
    printf("\n========== МЕНЮ ОЧЕРЕДИ С ПРИОРИТЕТАМИ ==========\n");
    printf("Приоритет: 0 = наивысший, 255 = наинизший\n");
    printf("1. Добавить элемент в очередь\n");
    printf("2. Извлечь первый элемент (наивысший приоритет)\n");
    printf("3. Извлечь элемент по конкретному приоритету\n");
    printf("4. Извлечь элемент с приоритетом >= значению (наивысший)\n");
    printf("5. Показать очередь\n");
    printf("6. Получить размер очереди\n");
    printf("7. Сгенерировать случайные сообщения\n");
    printf("8. Очистить очередь\n");
    printf("9. Выйти\n");
    printf("==================================================\n");
    printf("Выберите действие: ");
}

int choose_action(const char* operation_name) {
    int choice;
    printf("\n--- %s ---\n", operation_name);
    printf("1. Просмотреть (без удаления)\n");
    printf("2. Извлечь (с удалением)\n");
    printf("Выберите действие (1 или 2): ");
    
    if (scanf("%d", &choice) != 1) {
        printf("Ошибка ввода!\n");
        while (getchar() != '\n');
        return 0;
    }
    
    while (getchar() != '\n');
    
    if (choice != 1 && choice != 2) {
        printf("Неверный выбор! Действие отменено.\n");
        return 0;
    }
    return choice;
}

void add_element(PriorityQueue* queue) {
    uint8_t priority;
    int data;
    
    printf("Введите приоритет (0-255, 0 - наивысший): ");
    if (scanf("%hhu", &priority) != 1) {
        printf("Ошибка ввода!\n");
        while (getchar() != '\n');
        return;
    }
    
    printf("Введите данные (целое число): ");
    if (scanf("%d", &data) != 1) {
        printf("Ошибка ввода!\n");
        while (getchar() != '\n');
        return;
    }
    
    if (enqueue(queue, priority, data)) {
        printf("Элемент успешно добавлен!\n");
        printf("Приоритет: %d (0 - наивысший), Данные: %d\n", priority, data);
    } else {
        printf("Не удалось добавить элемент! Очередь переполнена.\n");
    }
}

void handle_front_element(PriorityQueue* queue) {
    if (is_empty(queue)) {
        printf("Очередь пуста!\n");
        return;
    }
    
    int action = choose_action("Работа с первым элементом");
    if (action == 0) return;
    
    if (action == 1) {
        QueueElement* elem = peek_front(queue);
        if (elem != NULL) {
            printf("\nПЕРВЫЙ ЭЛЕМЕНТ (наивысший приоритет):\n");
            printf("  Приоритет: %d\n", elem->priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент НЕ удалён из очереди\n");
        }
    } else if (action == 2) {
        QueueElement* elem = dequeue_front(queue);
        if (elem != NULL) {
            printf("\nИЗВЛЕЧЁННЫЙ ЭЛЕМЕНТ (наивысший приоритет):\n");
            printf("  Приоритет: %d\n", elem->priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент УДАЛЁН из очереди\n");
            free(elem);
        }
    }
}

void handle_by_priority(PriorityQueue* queue) {
    if (is_empty(queue)) {
        printf("Очередь пуста!\n");
        return;
    }
    
    uint8_t priority;
    printf("Введите приоритет (0-255, 0 - наивысший): ");
    if (scanf("%hhu", &priority) != 1) {
        printf("Ошибка ввода!\n");
        while (getchar() != '\n');
        return;
    }
    
    int action = choose_action("Работа с элементом по приоритету");
    if (action == 0) return;
    
    if (action == 1) {
        QueueElement* elem = peek_by_priority(queue, priority);
        if (elem != NULL) {
            printf("\nЭЛЕМЕНТ С ПРИОРИТЕТОМ %d:\n", priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент НЕ удалён из очереди\n");
        } else {
            printf("Элемент с приоритетом %d не найден\n", priority);
        }
    } else if (action == 2) {
        QueueElement* elem = dequeue_by_priority(queue, priority);
        if (elem != NULL) {
            printf("\nИЗВЛЕЧЁННЫЙ ЭЛЕМЕНТ С ПРИОРИТЕТОМ %d:\n", priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент УДАЛЁН из очереди\n");
            free(elem);
        } else {
            printf("Элемент с приоритетом %d не найден\n", priority);
        }
    }
}

void handle_by_min_priority(PriorityQueue* queue) {
    if (is_empty(queue)) {
        printf("Очередь пуста!\n");
        return;
    }
    
    uint8_t min_priority;
    printf("Введите минимальный приоритет (0-255): ");
    if (scanf("%hhu", &min_priority) != 1) {
        printf("Ошибка ввода!\n");
        while (getchar() != '\n');
        return;
    }
    
    int action = choose_action("Работа с элементом по минимальному приоритету");
    if (action == 0) return;
    
    if (action == 1) {
        QueueElement* elem = peek_by_min_priority(queue, min_priority);
        if (elem != NULL) {
            printf("\nЭЛЕМЕНТ С ПРИОРИТЕТОМ >= %d (наивысший из подходящих):\n", min_priority);
            printf("  Приоритет: %d\n", elem->priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент НЕ удалён из очереди\n");
        } else {
            printf("Элемент с приоритетом >= %d не найден\n", min_priority);
        }
    } else if (action == 2) {
        QueueElement* elem = dequeue_by_min_priority(queue, min_priority);
        if (elem != NULL) {
            printf("\nИЗВЛЕЧЁННЫЙ ЭЛЕМЕНТ С ПРИОРИТЕТОМ >= %d (наивысший из подходящих):\n", min_priority);
            printf("  Приоритет: %d\n", elem->priority);
            printf("  Данные: %d\n", elem->data);
            printf("  Номер: %llu\n", elem->sequence);
            printf("Статус: Элемент УДАЛЁН из очереди\n");
            free(elem);
        } else {
            printf("Элемент с приоритетом >= %d не найден\n", min_priority);
        }
    }
}

void generate_random_messages(PriorityQueue* queue) {
    int count;
    
    printf("Сколько случайных сообщений сгенерировать? ");
    if (scanf("%d", &count) != 1 || count <= 0) {
        printf("Ошибка ввода! Введите положительное число.\n");
        while (getchar() != '\n');
        return;
    }
    
    srand(time(NULL));
    int added = 0;
    
    printf("\nГенерация %d случайных сообщений...\n", count);
    for (int i = 0; i < count; i++) {
        uint8_t priority = rand() % 256;
        int data = rand() % 10000;
        
        if (enqueue(queue, priority, data)) {
            added++;
            printf("Добавлено: Приоритет=%d, Данные=%d\n", priority, data);
        } else {
            printf("Очередь заполнена! Остановлено на %d сообщениях.\n", i);
            break;
        }
    }
    
    printf("\nУспешно добавлено %d сообщений в очередь.\n", added);
}

void clear_queue(PriorityQueue* queue) {
    int count = 0;
    while (!is_empty(queue)) {
        QueueElement* elem = dequeue_front(queue);
        if (elem != NULL) {
            free(elem);
            count++;
        }
    }
    printf("Очищено %d элементов из очереди.\n", count);
}

int main(void) {
    
    PriorityQueue* queue = create_queue();
    if (queue == NULL) {
        printf("Не удалось создать очередь!\n");
        return 1;
    }
    
    printf("Система очереди с приоритетами\n");
    printf("================================\n");
    printf("Очередь успешно создана.\n");
    printf("Максимальный размер очереди: %d\n", MAX_QUEUE_SIZE);
    printf("Приоритет: 0 = наивысший, %d = наинизший\n\n", MAX_PRIORITY);
    
    int choice;
    int running = 1;
    
    while (running) {
        display_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода! Введите число.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                add_element(queue);
                break;
                
            case 2:
                handle_front_element(queue);
                break;
                
            case 3:
                handle_by_priority(queue);
                break;
                
            case 4:
                handle_by_min_priority(queue);
                break;
                
            case 5:
                printf("\nТекущее состояние очереди:\n");
                print_queue(queue);
                break;
                
            case 6:
                printf("Размер очереди: %d\n", get_size(queue));
                break;
                
            case 7:
                generate_random_messages(queue);
                break;
                
            case 8:
                clear_queue(queue);
                break;
                
            case 9:
                printf("Выход из программы...\n");
                running = 0;
                break;
                
            default:
                printf("Неверный выбор! Попробуйте снова.\n");
                break;
        }
    }

    clear_queue(queue);
    destroy_queue(queue);
    printf("Очередь уничтожена. До свидания!\n");
    
    return 0;
}