#include "calc.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int test_counter = 0;

void setUp(void) {
}

void tearDown(void) {
}


void print_test_header(const char* test_name) {
    test_counter++;
    printf("\n┌─────────────────────────────────────────────────┐\n");
    printf("│ ТЕСТ #%d: %-39s │\n", test_counter, test_name);
    printf("└─────────────────────────────────────────────────┘\n");
}

void print_success(const char* message) {
    printf("  ✅ %s\n", message);
}

void print_failure(const char* message) {
    printf("  ❌ %s\n", message);
}

void print_info(const char* message) {
    printf("  ℹ️  %s\n", message);
}

void print_queue_state(const PriorityQueue* queue, const char* title) {
    printf("\n  📋 %s:\n", title);
    if (queue == NULL || queue->size == 0) {
        printf("     Очередь пуста\n");
        return;
    }
    printf("     Размер: %d\n", queue->size);
    for (int i = 0; i < queue->size && i < 10; i++) {
        printf("     [%d] Приор=%d, Данные=%d, Номер=%llu\n", 
               i, queue->elements[i]->priority, 
               queue->elements[i]->data,
               queue->elements[i]->sequence);
    }
    if (queue->size > 10) {
        printf("     ... и ещё %d элементов\n", queue->size - 10);
    }
}

void test_create_and_destroy_queue(void) {
    print_test_header("Создание и уничтожение очереди");
    
    PriorityQueue* queue = create_queue();
    TEST_ASSERT_NOT_NULL(queue);
    print_success("Очередь создана");
    
    TEST_ASSERT_EQUAL_INT(0, queue->size);
    print_success("Размер очереди = 0");
    
    TEST_ASSERT_EQUAL_UINT64(0, queue->next_sequence);
    print_success("Счётчик последовательности = 0");
    
    destroy_queue(queue);
    print_success("Очередь уничтожена");
}

void test_enqueue_single_element(void) {
    print_test_header("Добавление одного элемента");
    
    PriorityQueue* queue = create_queue();
    
    bool result = enqueue(queue, 50, 100);
    TEST_ASSERT_TRUE(result);
    print_success("Элемент добавлен");
    
    TEST_ASSERT_EQUAL_INT(1, queue->size);
    print_success("Размер очереди = 1");
    
    TEST_ASSERT_EQUAL_INT(50, queue->elements[0]->priority);
    TEST_ASSERT_EQUAL_INT(100, queue->elements[0]->data);
    TEST_ASSERT_EQUAL_UINT64(0, queue->elements[0]->sequence);
    print_success("Данные элемента: Приор=50, Данные=100, Номер=0");
    
    print_queue_state(queue, "Состояние очереди");
    
    destroy_queue(queue);
}

void test_enqueue_multiple_elements(void) {
    print_test_header("Добавление нескольких элементов");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    enqueue(queue, 50, 400);
    
    TEST_ASSERT_EQUAL_INT(4, queue->size);
    print_success("Добавлено 4 элемента");
    
    TEST_ASSERT_EQUAL_INT(30, queue->elements[0]->priority);
    TEST_ASSERT_EQUAL_INT(50, queue->elements[1]->priority);
    TEST_ASSERT_EQUAL_INT(50, queue->elements[2]->priority);
    TEST_ASSERT_EQUAL_INT(70, queue->elements[3]->priority);
    print_success("Очередь отсортирована по приоритету (0 - наивысший)");
    
    print_queue_state(queue, "Состояние очереди");
    
    destroy_queue(queue);
}

void test_enqueue_priority_ordering(void) {
    print_test_header("FIFO для одинаковых приоритетов");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 50, 200);
    enqueue(queue, 50, 300);
    
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    
    TEST_ASSERT_EQUAL_INT(100, queue->elements[0]->data);
    TEST_ASSERT_EQUAL_INT(200, queue->elements[1]->data);
    TEST_ASSERT_EQUAL_INT(300, queue->elements[2]->data);
    print_success("Порядок FIFO соблюдён: 100 → 200 → 300");
    
    print_queue_state(queue, "Состояние очереди");
    
    destroy_queue(queue);
}

void test_enqueue_queue_full(void) {
    print_test_header("Заполнение очереди до предела");
    
    PriorityQueue* queue = create_queue();
    
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        bool result = enqueue(queue, i % 256, i);
        TEST_ASSERT_TRUE(result);
    }
    print_success("Очередь заполнена до предела");
    TEST_ASSERT_EQUAL_INT(MAX_QUEUE_SIZE, queue->size);
    
    bool result = enqueue(queue, 50, 999);
    TEST_ASSERT_FALSE(result);
    print_success("Попытка добавить в полную очередь отклонена");
    
    destroy_queue(queue);
}

void test_peek_front(void) {
    print_test_header("Просмотр первого элемента (без удаления)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    
    print_queue_state(queue, "До просмотра");
    
    QueueElement* elem = peek_front(queue);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(30, elem->priority);
    TEST_ASSERT_EQUAL_INT(200, elem->data);
    print_success("Найден элемент: Приор=30, Данные=200");
    
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    print_success("Размер очереди не изменился (3)");
    
    print_queue_state(queue, "После просмотра");
    
    destroy_queue(queue);
}

void test_peek_by_priority(void) {
    print_test_header("Просмотр по приоритету (без удаления)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 50, 300);
    enqueue(queue, 70, 400);
    
    print_queue_state(queue, "До просмотра");
    
    QueueElement* elem = peek_by_priority(queue, 50);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(50, elem->priority);
    TEST_ASSERT_EQUAL_INT(100, elem->data);
    print_success("Найден элемент с приор=50: Данные=100");
    
    TEST_ASSERT_EQUAL_INT(4, queue->size);
    print_success("Размер очереди не изменился (4)");
    
    print_queue_state(queue, "После просмотра");
    
    destroy_queue(queue);
}

void test_peek_by_min_priority(void) {
    print_test_header("Просмотр по минимальному приоритету (без удаления)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    enqueue(queue, 100, 400);
    
    print_queue_state(queue, "До просмотра");
    
    QueueElement* elem = peek_by_min_priority(queue, 60);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(70, elem->priority);
    TEST_ASSERT_EQUAL_INT(300, elem->data);
    print_success("Найден элемент с приор>=60: Приор=70, Данные=300");
    
    TEST_ASSERT_EQUAL_INT(4, queue->size);
    print_success("Размер очереди не изменился (4)");
    
    print_queue_state(queue, "После просмотра");
    
    destroy_queue(queue);
}

void test_dequeue_front(void) {
    print_test_header("Извлечение первого элемента (с удалением)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    
    print_queue_state(queue, "До извлечения");
    
    QueueElement* elem = dequeue_front(queue);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(30, elem->priority);
    TEST_ASSERT_EQUAL_INT(200, elem->data);
    print_success("Извлечён элемент: Приор=30, Данные=200");
    free(elem);
    
    TEST_ASSERT_EQUAL_INT(2, queue->size);
    print_success("Размер очереди уменьшился до 2");
    
    print_queue_state(queue, "После извлечения");
    
    destroy_queue(queue);
}

void test_dequeue_by_priority(void) {
    print_test_header("Извлечение по приоритету (с удалением)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 50, 300);
    enqueue(queue, 70, 400);
    
    print_queue_state(queue, "До извлечения");
    
    QueueElement* elem = dequeue_by_priority(queue, 50);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(50, elem->priority);
    TEST_ASSERT_EQUAL_INT(100, elem->data);
    print_success("Извлечён элемент с приор=50: Данные=100");
    free(elem);
    
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    print_success("Размер очереди уменьшился до 3");
    
    print_queue_state(queue, "После извлечения");
    
    destroy_queue(queue);
}

void test_dequeue_by_min_priority(void) {
    print_test_header("Извлечение по минимальному приоритету (с удалением)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    enqueue(queue, 100, 400);
    
    print_queue_state(queue, "До извлечения");
    
    QueueElement* elem = dequeue_by_min_priority(queue, 60);
    TEST_ASSERT_NOT_NULL(elem);
    TEST_ASSERT_EQUAL_INT(70, elem->priority);
    TEST_ASSERT_EQUAL_INT(300, elem->data);
    print_success("Извлечён элемент с приор>=60: Приор=70, Данные=300");
    free(elem);
    
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    print_success("Размер очереди уменьшился до 3");
    
    print_queue_state(queue, "После извлечения");
    
    destroy_queue(queue);
}

void test_dequeue_empty_queue(void) {
    print_test_header("Извлечение из пустой очереди");
    
    PriorityQueue* queue = create_queue();
    
    TEST_ASSERT_TRUE(is_empty(queue));
    print_success("Очередь пуста");
    
    QueueElement* elem = dequeue_front(queue);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_front вернул NULL");
    
    elem = dequeue_by_priority(queue, 50);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_by_priority вернул NULL");
    
    elem = dequeue_by_min_priority(queue, 50);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_by_min_priority вернул NULL");
    
    destroy_queue(queue);
}

void test_priority_not_found(void) {
    print_test_header("Поиск несуществующего приоритета");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    
    print_queue_state(queue, "Текущая очередь");
    
    QueueElement* elem = peek_by_priority(queue, 100);
    TEST_ASSERT_NULL(elem);
    print_success("peek_by_priority(100) вернул NULL");
    
    elem = dequeue_by_priority(queue, 100);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_by_priority(100) вернул NULL");
    
    elem = peek_by_min_priority(queue, 100);
    TEST_ASSERT_NULL(elem);
    print_success("peek_by_min_priority(100) вернул NULL");
    
    destroy_queue(queue);
}

void test_is_empty_and_get_size(void) {
    print_test_header("Проверка is_empty и get_size");
    
    PriorityQueue* queue = create_queue();
    
    TEST_ASSERT_TRUE(is_empty(queue));
    TEST_ASSERT_EQUAL_INT(0, get_size(queue));
    print_success("Новая очередь: is_empty=true, size=0");
    
    enqueue(queue, 50, 100);
    TEST_ASSERT_FALSE(is_empty(queue));
    TEST_ASSERT_EQUAL_INT(1, get_size(queue));
    print_success("После добавления: is_empty=false, size=1");
    
    enqueue(queue, 30, 200);
    TEST_ASSERT_EQUAL_INT(2, get_size(queue));
    print_success("После ещё одного добавления: size=2");
    
    QueueElement* elem = dequeue_front(queue);
    free(elem);
    TEST_ASSERT_EQUAL_INT(1, get_size(queue));
    print_success("После извлечения: size=1");
    
    elem = dequeue_front(queue);
    free(elem);
    TEST_ASSERT_TRUE(is_empty(queue));
    TEST_ASSERT_EQUAL_INT(0, get_size(queue));
    print_success("После извлечения всех: is_empty=true, size=0");
    
    destroy_queue(queue);
}

void test_mixed_operations(void) {
    print_test_header("Смешанные операции (просмотр + извлечение)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    enqueue(queue, 50, 400);
    
    print_queue_state(queue, "Начальное состояние");
    
    QueueElement* elem = peek_front(queue);
    TEST_ASSERT_EQUAL_INT(30, elem->priority);
    print_success("peek_front: Приор=30 (элемент НЕ удалён)");
    TEST_ASSERT_EQUAL_INT(4, queue->size);
    
    elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(30, elem->priority);
    print_success("dequeue_front: Приор=30 (элемент УДАЛЁН)");
    free(elem);
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    
    elem = peek_by_priority(queue, 50);
    TEST_ASSERT_EQUAL_INT(100, elem->data);
    print_success("peek_by_priority(50): Данные=100 (НЕ удалён)");
    TEST_ASSERT_EQUAL_INT(3, queue->size);
    
    elem = dequeue_by_priority(queue, 50);
    TEST_ASSERT_EQUAL_INT(100, elem->data);
    print_success("dequeue_by_priority(50): Данные=100 (УДАЛЁН)");
    free(elem);
    TEST_ASSERT_EQUAL_INT(2, queue->size);
    
    print_queue_state(queue, "Конечное состояние");
    
    destroy_queue(queue);
}

void test_edge_cases(void) {
    print_test_header("Крайние случаи (приоритеты 0 и 255)");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 0, 100);
    QueueElement* elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(0, elem->priority);
    TEST_ASSERT_EQUAL_INT(100, elem->data);
    print_success("Приоритет 0 (наивысший): данные=100");
    free(elem);
    
    enqueue(queue, 255, 200);
    elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(255, elem->priority);
    TEST_ASSERT_EQUAL_INT(200, elem->data);
    print_success("Приоритет 255 (наинизший): данные=200");
    free(elem);
    
    enqueue(queue, 255, 1);
    enqueue(queue, 0, 2);
    enqueue(queue, 128, 3);
    
    elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(0, elem->priority);
    print_success("Наивысший приоритет 0 извлечён первым");
    free(elem);
    
    elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(128, elem->priority);
    print_success("Затем приоритет 128");
    free(elem);
    
    elem = dequeue_front(queue);
    TEST_ASSERT_EQUAL_INT(255, elem->priority);
    print_success("Затем приоритет 255 (наинизший)");
    free(elem);
    
    destroy_queue(queue);
}

void test_null_pointer_handling(void) {
    print_test_header("Обработка NULL указателей");
    
    bool result = enqueue(NULL, 50, 100);
    TEST_ASSERT_FALSE(result);
    print_success("enqueue(NULL) вернул false");
    
    QueueElement* elem = dequeue_front(NULL);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_front(NULL) вернул NULL");
    
    elem = dequeue_by_priority(NULL, 50);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_by_priority(NULL) вернул NULL");
    
    elem = dequeue_by_min_priority(NULL, 50);
    TEST_ASSERT_NULL(elem);
    print_success("dequeue_by_min_priority(NULL) вернул NULL");
    
    elem = peek_front(NULL);
    TEST_ASSERT_NULL(elem);
    print_success("peek_front(NULL) вернул NULL");
    
    elem = peek_by_priority(NULL, 50);
    TEST_ASSERT_NULL(elem);
    print_success("peek_by_priority(NULL) вернул NULL");
    
    elem = peek_by_min_priority(NULL, 50);
    TEST_ASSERT_NULL(elem);
    print_success("peek_by_min_priority(NULL) вернул NULL");
    
    TEST_ASSERT_TRUE(is_empty(NULL));
    print_success("is_empty(NULL) вернул true");
    
    TEST_ASSERT_EQUAL_INT(0, get_size(NULL));
    print_success("get_size(NULL) вернул 0");
    
    destroy_queue(NULL);
    print_success("destroy_queue(NULL) не вызвал ошибку");
}

void test_stress_test(void) {
    print_test_header("Нагрузочное тестирование (1000 элементов)");
    
    PriorityQueue* queue = create_queue();
    
    printf("  📊 Добавление 1000 элементов...\n");
    for (int i = 0; i < 1000; i++) {
        uint8_t priority = i % 256;
        int data = i;
        bool result = enqueue(queue, priority, data);
        TEST_ASSERT_TRUE(result);
    }
    
    TEST_ASSERT_EQUAL_INT(1000, queue->size);
    print_success("Добавлено 1000 элементов");
    
    printf("  📊 Извлечение всех элементов...\n");
    int count = 0;
    int last_priority = -1;
    while (!is_empty(queue)) {
        QueueElement* elem = dequeue_front(queue);
        if (elem) {
            // Проверяем, что приоритеты не убывают
            if (last_priority != -1) {
                TEST_ASSERT_TRUE(elem->priority >= last_priority);
            }
            last_priority = elem->priority;
            free(elem);
            count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT(1000, count);
    print_success("Извлечено 1000 элементов");
    TEST_ASSERT_TRUE(is_empty(queue));
    print_success("Очередь пуста");
    
    destroy_queue(queue);
}

void test_peek_dequeue_consistency(void) {
    print_test_header("Консистентность peek и dequeue");
    
    PriorityQueue* queue = create_queue();
    
    enqueue(queue, 50, 100);
    enqueue(queue, 30, 200);
    enqueue(queue, 70, 300);
    
    print_queue_state(queue, "Начальное состояние");
    
    QueueElement* peek_elem = peek_front(queue);
    QueueElement* dequeue_elem = dequeue_front(queue);
    
    TEST_ASSERT_EQUAL_INT(peek_elem->priority, dequeue_elem->priority);
    TEST_ASSERT_EQUAL_INT(peek_elem->data, dequeue_elem->data);
    print_success("peek и dequeue показывают одинаковые данные");
    print_info("peek: Приор=30, Данные=200 (НЕ удалён)");
    print_info("dequeue: Приор=30, Данные=200 (УДАЛЁН)");
    
    free(dequeue_elem);
    
    TEST_ASSERT_EQUAL_INT(2, queue->size);
    print_success("После dequeue размер = 2");
    
    peek_elem = peek_front(queue);
    TEST_ASSERT_EQUAL_INT(50, peek_elem->priority);
    print_success("Теперь peek показывает Приор=50");
    
    destroy_queue(queue);
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║         ТЕСТИРОВАНИЕ ОЧЕРЕДИ С ПРИОРИТЕТАМИ          ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\nПриоритет: 0 = наивысший, 255 = наинизший\n");
    printf("Максимальный размер очереди: %d\n", MAX_QUEUE_SIZE);
    
    UNITY_BEGIN();
    
    // Базовые тесты
    RUN_TEST(test_create_and_destroy_queue);
    RUN_TEST(test_enqueue_single_element);
    RUN_TEST(test_enqueue_multiple_elements);
    RUN_TEST(test_enqueue_priority_ordering);
    RUN_TEST(test_enqueue_queue_full);
    
    // Тесты просмотра (peek)
    RUN_TEST(test_peek_front);
    RUN_TEST(test_peek_by_priority);
    RUN_TEST(test_peek_by_min_priority);
    
    // Тесты извлечения (dequeue)
    RUN_TEST(test_dequeue_front);
    RUN_TEST(test_dequeue_by_priority);
    RUN_TEST(test_dequeue_by_min_priority);
    RUN_TEST(test_dequeue_empty_queue);
    RUN_TEST(test_priority_not_found);
    
    // Вспомогательные функции
    RUN_TEST(test_is_empty_and_get_size);
    
    // Смешанные операции
    RUN_TEST(test_mixed_operations);
    
    // Крайние случаи
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_null_pointer_handling);
    
    // Нагрузочное тестирование
    RUN_TEST(test_stress_test);
    
    // Консистентность
    RUN_TEST(test_peek_dequeue_consistency);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║              ТЕСТИРОВАНИЕ ЗАВЕРШЕНО                  ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    return UNITY_END();
}