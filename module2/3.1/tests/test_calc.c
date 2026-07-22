#include "unity.h"
#include "calc.h"
#include <string.h>

// Настройка перед каждым тестом
void setUp(void) {
    // Инициализация перед каждым тестом
}

// Очистка после каждого теста
void tearDown(void) {
    // Очистка после каждого теста
}

// === ТЕСТЫ ДЛЯ numeric_to_binary ===
void test_numeric_to_binary_valid(void) {
    char binary[10];
    
    TEST_ASSERT_EQUAL_INT(0, numeric_to_binary(0755, binary));
    TEST_ASSERT_EQUAL_STRING("111101101", binary);
    
    TEST_ASSERT_EQUAL_INT(0, numeric_to_binary(0644, binary));
    TEST_ASSERT_EQUAL_STRING("110100100", binary);
    
    TEST_ASSERT_EQUAL_INT(0, numeric_to_binary(0000, binary));
    TEST_ASSERT_EQUAL_STRING("000000000", binary);
    
    TEST_ASSERT_EQUAL_INT(0, numeric_to_binary(0777, binary));
    TEST_ASSERT_EQUAL_STRING("111111111", binary);
}

void test_numeric_to_binary_invalid(void) {
    char binary[10];
    
    TEST_ASSERT_EQUAL_INT(-1, numeric_to_binary(-1, binary));
    TEST_ASSERT_EQUAL_STRING("Ошибка", binary);
    
    TEST_ASSERT_EQUAL_INT(-1, numeric_to_binary(1000, binary));
    TEST_ASSERT_EQUAL_STRING("Ошибка", binary);
}

// === ТЕСТЫ ДЛЯ symbolic_to_numeric ===
void test_symbolic_to_numeric_basic(void) {
    TEST_ASSERT_EQUAL_INT(0755, symbolic_to_numeric("u=rwx,g=rx,o=rx"));
    TEST_ASSERT_EQUAL_INT(0644, symbolic_to_numeric("u=rw,g=r,o=r"));
    TEST_ASSERT_EQUAL_INT(0000, symbolic_to_numeric("u=,g=,o="));
    TEST_ASSERT_EQUAL_INT(0777, symbolic_to_numeric("u=rwx,g=rwx,o=rwx"));
}

void test_symbolic_to_numeric_with_a(void) {
    TEST_ASSERT_EQUAL_INT(0755, symbolic_to_numeric("a=rx,u=w"));
    TEST_ASSERT_EQUAL_INT(0777, symbolic_to_numeric("a=rwx"));
    TEST_ASSERT_EQUAL_INT(0000, symbolic_to_numeric("a="));
}

void test_symbolic_to_numeric_operations(void) {
    // Начинаем с базовых прав
    int base = symbolic_to_numeric("u=rwx,g=rx,o=r");
    TEST_ASSERT_EQUAL_INT(0754, base);
    
    // Тестируем сложение
    TEST_ASSERT_EQUAL_INT(0755, symbolic_to_numeric("u=rwx,g=rx,o=r,o+x"));
    TEST_ASSERT_EQUAL_INT(0777, symbolic_to_numeric("u=rwx,g=rx,o=r,g+w,o+wx"));
    
    // Тестируем вычитание
    TEST_ASSERT_EQUAL_INT(0700, symbolic_to_numeric("u=rwx,g=rx,o=r,g-rx,o-r"));
    TEST_ASSERT_EQUAL_INT(0000, symbolic_to_numeric("u=rwx,g=rx,o=r,u-rwx,g-rx,o-r"));
}

void test_symbolic_to_numeric_invalid(void) {
    TEST_ASSERT_EQUAL_INT(-1, symbolic_to_numeric("invalid"));
    TEST_ASSERT_EQUAL_INT(-1, symbolic_to_numeric("u=rwx,g=rx,o=invalid"));
    TEST_ASSERT_EQUAL_INT(-1, symbolic_to_numeric("x=rwx"));
    TEST_ASSERT_EQUAL_INT(-1, symbolic_to_numeric("u=rwx;g=rx"));
}

// === ТЕСТЫ ДЛЯ numeric_to_symbolic ===
void test_numeric_to_symbolic(void) {
    char symbolic[256];
    
    numeric_to_symbolic(0755, symbolic, sizeof(symbolic));
    TEST_ASSERT_EQUAL_STRING("u=rwx,g=rx,o=rx", symbolic);
    
    numeric_to_symbolic(0644, symbolic, sizeof(symbolic));
    TEST_ASSERT_EQUAL_STRING("u=rw,g=r,o=r", symbolic);
    
    numeric_to_symbolic(0000, symbolic, sizeof(symbolic));
    TEST_ASSERT_EQUAL_STRING("u=,g=,o=", symbolic);
    
    numeric_to_symbolic(0777, symbolic, sizeof(symbolic));
    TEST_ASSERT_EQUAL_STRING("u=rwx,g=rwx,o=rwx", symbolic);
}

// === ТЕСТЫ ДЛЯ parse_umask ===
void test_parse_numeric_umask(void) {
    Umask result;
    
    TEST_ASSERT_EQUAL_INT(0, parse_umask("755", &result));
    TEST_ASSERT_EQUAL_INT(0755, result.numeric);
    TEST_ASSERT_EQUAL_STRING("111101101", result.binary);
    TEST_ASSERT_EQUAL_STRING("u=rwx,g=rx,o=rx", result.symbolic);
    
    TEST_ASSERT_EQUAL_INT(0, parse_umask("644", &result));
    TEST_ASSERT_EQUAL_INT(0644, result.numeric);
    TEST_ASSERT_EQUAL_STRING("110100100", result.binary);
}

void test_parse_symbolic_umask(void) {
    Umask result;
    
    TEST_ASSERT_EQUAL_INT(0, parse_umask("u=rwx,g=rx,o=r", &result));
    TEST_ASSERT_EQUAL_INT(0754, result.numeric);
    TEST_ASSERT_EQUAL_STRING("u=rwx,g=rx,o=r", result.symbolic);
    
    TEST_ASSERT_EQUAL_INT(0, parse_umask("a=rwx", &result));
    TEST_ASSERT_EQUAL_INT(0777, result.numeric);
}

void test_parse_invalid_umask(void) {
    Umask result;
    
    TEST_ASSERT_EQUAL_INT(-1, parse_umask("invalid", &result));
    TEST_ASSERT_EQUAL_INT(-1, parse_umask("", &result));
    TEST_ASSERT_EQUAL_INT(-1, parse_umask(NULL, &result));
    TEST_ASSERT_EQUAL_INT(-1, parse_umask("999", &result)); // > 777
}

// === ТЕСТЫ ДЛЯ ВСПОМОГАТЕЛЬНЫХ ФУНКЦИЙ ===
void test_is_valid_umask(void) {
    TEST_ASSERT_TRUE(is_valid_umask(0000));
    TEST_ASSERT_TRUE(is_valid_umask(0755));
    TEST_ASSERT_TRUE(is_valid_umask(0777));
    TEST_ASSERT_FALSE(is_valid_umask(-1));
    TEST_ASSERT_FALSE(is_valid_umask(1000));
}

void test_get_permission_bits(void) {
    TEST_ASSERT_EQUAL_INT(4, get_permission_bits('r'));
    TEST_ASSERT_EQUAL_INT(2, get_permission_bits('w'));
    TEST_ASSERT_EQUAL_INT(1, get_permission_bits('x'));
    TEST_ASSERT_EQUAL_INT(0, get_permission_bits('z'));
}

// === ТЕСТЫ КРАЙНИХ СЛУЧАЕВ ===
void test_edge_cases(void) {
    Umask result;
    
    // Минимальное значение
    TEST_ASSERT_EQUAL_INT(0, parse_umask("0", &result));
    TEST_ASSERT_EQUAL_INT(0000, result.numeric);
    TEST_ASSERT_EQUAL_STRING("000000000", result.binary);
    
    // Максимальное значение
    TEST_ASSERT_EQUAL_INT(0, parse_umask("777", &result));
    TEST_ASSERT_EQUAL_INT(0777, result.numeric);
    TEST_ASSERT_EQUAL_STRING("111111111", result.binary);
    
    // Сложная комбинация
    TEST_ASSERT_EQUAL_INT(0, parse_umask("u=rwx,g=,o=x", &result));
    TEST_ASSERT_EQUAL_INT(0701, result.numeric);
}

// === ЗАПУСК ВСЕХ ТЕСТОВ ===
int main(void) {
    UNITY_BEGIN();
    
    // Тесты для numeric_to_binary
    RUN_TEST(test_numeric_to_binary_valid);
    RUN_TEST(test_numeric_to_binary_invalid);
    
    // Тесты для symbolic_to_numeric
    RUN_TEST(test_symbolic_to_numeric_basic);
    RUN_TEST(test_symbolic_to_numeric_with_a);
    RUN_TEST(test_symbolic_to_numeric_operations);
    RUN_TEST(test_symbolic_to_numeric_invalid);
    
    // Тесты для numeric_to_symbolic
    RUN_TEST(test_numeric_to_symbolic);
    
    // Тесты для parse_umask
    RUN_TEST(test_parse_numeric_umask);
    RUN_TEST(test_parse_symbolic_umask);
    RUN_TEST(test_parse_invalid_umask);
    
    // Тесты для вспомогательных функций
    RUN_TEST(test_is_valid_umask);
    RUN_TEST(test_get_permission_bits);
    
    // Тесты крайних случаев
    RUN_TEST(test_edge_cases);
    
    return UNITY_END();
}