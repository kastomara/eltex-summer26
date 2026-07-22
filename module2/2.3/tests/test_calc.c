#define UNITY_INCLUDE_DOUBLE
#include "unity.h"
#include "calc.h"
#include <math.h>
#include <float.h>

void setUp(void) {}
void tearDown(void) {}

void test_findAct_all_operations(void) {
    action func;
    
    func = findAct('+');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, func(2.0, 3.0));
    
    func = findAct('-');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, func(2.0, 3.0));
    
    func = findAct('*');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(6.0, func(2.0, 3.0));
    
    func = findAct('/');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(0.6666666666666666, func(2.0, 3.0));
    
    func = findAct('M');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, func(2.0, 3.0));
    
    func = findAct('^');
    TEST_ASSERT_NOT_NULL(func);
    TEST_ASSERT_EQUAL_DOUBLE(8.0, func(2.0, 3.0));
}

void test_findAct_case_sensitivity(void) {
    action func = findAct('m');
    TEST_ASSERT_NULL(func);
}

void test_findName_all_operations(void) {
    TEST_ASSERT_EQUAL_STRING("Сложение", findName('+'));
    TEST_ASSERT_EQUAL_STRING("Вычитание", findName('-'));
    TEST_ASSERT_EQUAL_STRING("Умножение", findName('*'));
    TEST_ASSERT_EQUAL_STRING("Деление", findName('/'));
    TEST_ASSERT_EQUAL_STRING("Максимум", findName('M'));
    TEST_ASSERT_EQUAL_STRING("Степень", findName('^'));
}

void test_findName_case_sensitivity(void) {
    TEST_ASSERT_EQUAL_STRING("Неизвестная операция", findName('m'));
}

void test_execAct_subtract(void) {
    int error = 0;
    double result = execAct('-', 5.0, 3.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, result);
    TEST_ASSERT_EQUAL_INT(0, error);
}

void test_execAct_multiply(void) {
    int error = 0;
    double result = execAct('*', 5.0, 3.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(15.0, result);
    TEST_ASSERT_EQUAL_INT(0, error);
}

void test_execAct_divide_normal(void) {
    int error = 0;
    double result = execAct('/', 6.0, 3.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, result);
    TEST_ASSERT_EQUAL_INT(0, error);
}

void test_execAct_divide_by_zero(void) {
    int error = 0;
    double result = execAct('/', 5.0, 0.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result);
    TEST_ASSERT_EQUAL_INT(1, error);
}

void test_execAct_unknown_operation(void) {
    int error = 0;
    double result = execAct('@', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result);
    TEST_ASSERT_EQUAL_INT(1, error);
}

void test_execAct_null_function(void) {
    int error = 0;
    double result = execAct('x', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result);
    TEST_ASSERT_EQUAL_INT(1, error);
}

/* ===== ТЕСТЫ ДЛЯ parseExp ===== */
void test_parseExp_multiple_spaces(void) {
    actionExp expr = parseExp("2    +    3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_parseExp_trailing_spaces(void) {
    actionExp expr = parseExp("  2 + 3  ");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_parseExp_division_symbol(void) {
    actionExp expr = parseExp("10 / 2");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('/', expr.operat);
}

void test_parseExp_mixed_case_M(void) {
    actionExp expr = parseExp("5 m 3");
    TEST_ASSERT_EQUAL_INT(0, expr.valid);
}

void test_parseExp_scientific_notation(void) {
    actionExp expr = parseExp("1e3 + 2e2");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(1000.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(200.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_parseExp_large_numbers(void) {
    actionExp expr = parseExp("1e308 + 1e308");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_TRUE(isinf(expr.num1 + expr.num2));
}

void test_parseExp_zero_division(void) {
    actionExp expr = parseExp("10 / 0");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('/', expr.operat);
}

void test_parseExp_only_numbers(void) {
    actionExp expr = parseExp("5 3");
    TEST_ASSERT_EQUAL_INT(0, expr.valid);
}

void test_parseExp_only_operator(void) {
    actionExp expr = parseExp("+");
    TEST_ASSERT_EQUAL_INT(0, expr.valid);
}

void test_parseExp_empty_expression(void) {
    actionExp expr = parseExp("   ");
    TEST_ASSERT_EQUAL_INT(0, expr.valid);
}

void test_parseExp_newline_characters(void) {
    actionExp expr = parseExp("2\n+\n3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_parseExp_tab_characters(void) {
    actionExp expr = parseExp("2\t+\t3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_power_zero_exponent(void) {
    TEST_ASSERT_EQUAL_DOUBLE(1.0, power(5.0, 0.0));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, power(0.0, 0.0));
}

void test_power_negative_base_even_exponent(void) {
    TEST_ASSERT_EQUAL_DOUBLE(9.0, power(-3.0, 2.0));
}

void test_power_negative_base_odd_exponent(void) {
    TEST_ASSERT_EQUAL_DOUBLE(-27.0, power(-3.0, 3.0));
}

void test_power_fractional_exponent(void) {
    TEST_ASSERT_EQUAL_DOUBLE(2.0, power(4.0, 0.5));
}

void test_add_precision(void) {
    double result = add(0.1, 0.2);
    TEST_ASSERT_FLOAT_WITHIN(1e-10, 0.3, result);
}

void test_multiply_precision(void) {
    double result = multiply(0.1, 0.2);
    TEST_ASSERT_FLOAT_WITHIN(1e-10, 0.02, result);
}

void test_max_equal_numbers(void) {
    TEST_ASSERT_EQUAL_DOUBLE(5.0, max(5.0, 5.0));
}

void test_max_negative_numbers(void) {
    TEST_ASSERT_EQUAL_DOUBLE(-3.0, max(-5.0, -3.0));
}

void test_max_mixed_signs(void) {
    TEST_ASSERT_EQUAL_DOUBLE(3.0, max(-5.0, 3.0));
}

void test_divide_precision(void) {
    double result = divide(1.0, 3.0);
    TEST_ASSERT_FLOAT_WITHIN(1e-10, 0.3333333333333333, result);
}

void test_error_handling_invalid_input(void) {
    int error = 0;
    double result = execAct('+', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL_INT(0, error);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, result);
    
    result = execAct('@', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL_INT(1, error);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result);
}

void test_error_handling_null_error_pointer(void) {
    TEST_IGNORE();
}

void test_parseExp_without_operator(void) {
    actionExp expr = parseExp("42");
    TEST_ASSERT_EQUAL_INT(0, expr.valid);
}

void test_parseExp_with_extra_tokens(void) {
    actionExp expr = parseExp("2 + 3 extra");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('+', expr.operat);
}

void test_parseExp_negative_first_number(void) {
    actionExp expr = parseExp("-5 * 3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(-5.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('*', expr.operat);
}

void test_parseExp_negative_second_number(void) {
    actionExp expr = parseExp("5 * -3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(-3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('*', expr.operat);
}

void test_parseExp_both_negative_numbers(void) {
    actionExp expr = parseExp("-5 * -3");
    TEST_ASSERT_EQUAL_INT(1, expr.valid);
    TEST_ASSERT_EQUAL_DOUBLE(-5.0, expr.num1);
    TEST_ASSERT_EQUAL_DOUBLE(-3.0, expr.num2);
    TEST_ASSERT_EQUAL_CHAR('*', expr.operat);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_findAct_all_operations);
    RUN_TEST(test_findAct_case_sensitivity);
    
    RUN_TEST(test_findName_all_operations);
    RUN_TEST(test_findName_case_sensitivity);
    
    RUN_TEST(test_execAct_subtract);
    RUN_TEST(test_execAct_multiply);
    RUN_TEST(test_execAct_divide_normal);
    RUN_TEST(test_execAct_divide_by_zero);
    RUN_TEST(test_execAct_unknown_operation);
    RUN_TEST(test_execAct_null_function);
    
    RUN_TEST(test_parseExp_multiple_spaces);
    RUN_TEST(test_parseExp_trailing_spaces);
    RUN_TEST(test_parseExp_division_symbol);
    RUN_TEST(test_parseExp_mixed_case_M);
    RUN_TEST(test_parseExp_scientific_notation);
    RUN_TEST(test_parseExp_large_numbers);
    RUN_TEST(test_parseExp_zero_division);
    RUN_TEST(test_parseExp_only_numbers);
    RUN_TEST(test_parseExp_only_operator);
    RUN_TEST(test_parseExp_empty_expression);
    RUN_TEST(test_parseExp_newline_characters);
    RUN_TEST(test_parseExp_tab_characters);
    
    RUN_TEST(test_power_zero_exponent);
    RUN_TEST(test_power_negative_base_even_exponent);
    RUN_TEST(test_power_negative_base_odd_exponent);
    RUN_TEST(test_power_fractional_exponent);
    RUN_TEST(test_add_precision);
    RUN_TEST(test_multiply_precision);
    RUN_TEST(test_max_equal_numbers);
    RUN_TEST(test_max_negative_numbers);
    RUN_TEST(test_max_mixed_signs);
    RUN_TEST(test_divide_precision);
    
    RUN_TEST(test_error_handling_invalid_input);
    RUN_TEST(test_error_handling_null_error_pointer);
    
    RUN_TEST(test_parseExp_without_operator);
    RUN_TEST(test_parseExp_with_extra_tokens);
    RUN_TEST(test_parseExp_negative_first_number);
    RUN_TEST(test_parseExp_negative_second_number);
    RUN_TEST(test_parseExp_both_negative_numbers);
    
    return UNITY_END();
}