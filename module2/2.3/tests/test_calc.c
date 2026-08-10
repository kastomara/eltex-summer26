// test_calc.c
#include "unity.h"
#include "calc.h"
#include <math.h>

#define EPSILON 0.0001

void setUp (void) {}
void tearDown (void) {}
void assertDoubleEqual(double expected, double actual, double epsilon) {
    if (fabs(expected - actual) > epsilon) {
        TEST_FAIL_MESSAGE("Double values are not equal within epsilon");
    }
}

// ==================== ТЕСТЫ ДЛЯ АРИФМЕТИЧЕСКИХ ФУНКЦИЙ ====================

void test_add_positive_numbers(void) {
    assertDoubleEqual(5.0, add(2.0, 3.0), EPSILON);
}

void test_add_negative_numbers(void) {
    assertDoubleEqual(-5.0, add(-2.0, -3.0), EPSILON);
}

void test_add_mixed_numbers(void) {
    assertDoubleEqual(1.0, add(-2.0, 3.0), EPSILON);
}

void test_add_zero(void) {
    assertDoubleEqual(5.0, add(5.0, 0.0), EPSILON);
}

void test_substract_positive(void) {
    assertDoubleEqual(2.0, substract(5.0, 3.0), EPSILON);
}

void test_substract_negative(void) {
    assertDoubleEqual(-1.0, substract(-3.0, -2.0), EPSILON);
}

void test_substract_mixed(void) {
    assertDoubleEqual(-5.0, substract(-2.0, 3.0), EPSILON);
}

void test_multiply_positive(void) {
    assertDoubleEqual(15.0, multiply(3.0, 5.0), EPSILON);
}

void test_multiply_negative(void) {
    assertDoubleEqual(6.0, multiply(-2.0, -3.0), EPSILON);
}

void test_multiply_mixed(void) {
    assertDoubleEqual(-6.0, multiply(-2.0, 3.0), EPSILON);
}

void test_multiply_by_zero(void) {
    assertDoubleEqual(0.0, multiply(5.0, 0.0), EPSILON);
}

void test_divide_positive(void) {
    assertDoubleEqual(2.0, divide(6.0, 3.0), EPSILON);
}

void test_divide_negative(void) {
    assertDoubleEqual(2.0, divide(-6.0, -3.0), EPSILON);
}

void test_divide_mixed(void) {
    assertDoubleEqual(-2.0, divide(6.0, -3.0), EPSILON);
}

void test_divide_by_zero(void) {
    assertDoubleEqual(0.0, divide(5.0, 0.0), EPSILON);
}

void test_max_first_greater(void) {
    assertDoubleEqual(5.0, max(5.0, 3.0), EPSILON);
}

void test_max_second_greater(void) {
    assertDoubleEqual(5.0, max(3.0, 5.0), EPSILON);
}

void test_max_equal(void) {
    assertDoubleEqual(3.0, max(3.0, 3.0), EPSILON);
}

void test_max_negative(void) {
    assertDoubleEqual(-2.0, max(-5.0, -2.0), EPSILON);
}

void test_power_positive(void) {
    assertDoubleEqual(8.0, power(2.0, 3.0), EPSILON);
}

void test_power_zero_exponent(void) {
    assertDoubleEqual(1.0, power(5.0, 0.0), EPSILON);
}

void test_power_negative_base(void) {
    assertDoubleEqual(-8.0, power(-2.0, 3.0), EPSILON);
}

void test_power_fractional(void) {
    assertDoubleEqual(9.0, power(3.0, 2.0), EPSILON);
}

// ==================== findAct ====================

void test_findAct_addition(void) {
    action act = findAct('+');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(5.0, act(2.0, 3.0), EPSILON);
}

void test_findAct_subtraction(void) {
    action act = findAct('-');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(2.0, act(5.0, 3.0), EPSILON);
}

void test_findAct_multiplication(void) {
    action act = findAct('*');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(15.0, act(3.0, 5.0), EPSILON);
}

void test_findAct_division(void) {
    action act = findAct('/');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(2.0, act(6.0, 3.0), EPSILON);
}

void test_findAct_max(void) {
    action act = findAct('M');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(5.0, act(5.0, 3.0), EPSILON);
}

void test_findAct_power(void) {
    action act = findAct('^');
    TEST_ASSERT_NOT_NULL(act);
    assertDoubleEqual(8.0, act(2.0, 3.0), EPSILON);
}

void test_findAct_invalid(void) {
    action act = findAct('?');
    TEST_ASSERT_NULL(act);
}

// ==================== findName ====================

void test_findName_addition(void) {
    TEST_ASSERT_EQUAL_STRING("Сложение", findName('+'));
}

void test_findName_subtraction(void) {
    TEST_ASSERT_EQUAL_STRING("Вычитание", findName('-'));
}

void test_findName_multiplication(void) {
    TEST_ASSERT_EQUAL_STRING("Умножение", findName('*'));
}

void test_findName_division(void) {
    TEST_ASSERT_EQUAL_STRING("Деление", findName('/'));
}

void test_findName_max(void) {
    TEST_ASSERT_EQUAL_STRING("Максимум", findName('M'));
}

void test_findName_power(void) {
    TEST_ASSERT_EQUAL_STRING("Степень", findName('^'));
}

void test_findName_invalid(void) {
    TEST_ASSERT_EQUAL_STRING("Неизвестная операция", findName('?'));
}

// ==================== execAct ====================

void test_execAct_addition(void) {
    int error = 0;
    double result = execAct('+', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(5.0, result, EPSILON);
}

void test_execAct_subtraction(void) {
    int error = 0;
    double result = execAct('-', 5.0, 3.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(2.0, result, EPSILON);
}

void test_execAct_multiplication(void) {
    int error = 0;
    double result = execAct('*', 3.0, 5.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(15.0, result, EPSILON);
}

void test_execAct_division(void) {
    int error = 0;
    double result = execAct('/', 6.0, 3.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(2.0, result, EPSILON);
}

void test_execAct_division_by_zero(void) {
    int error = 0;
    double result = execAct('/', 5.0, 0.0, &error);
    TEST_ASSERT_EQUAL(1, error);
    assertDoubleEqual(0.0, result, EPSILON);
}

void test_execAct_max(void) {
    int error = 0;
    double result = execAct('M', 5.0, 3.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(5.0, result, EPSILON);
}

void test_execAct_power(void) {
    int error = 0;
    double result = execAct('^', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL(0, error);
    assertDoubleEqual(8.0, result, EPSILON);
}

void test_execAct_invalid_operator(void) {
    int error = 0;
    double result = execAct('?', 2.0, 3.0, &error);
    TEST_ASSERT_EQUAL(1, error);
    assertDoubleEqual(0.0, result, EPSILON);
}

// ==================== parseExp ====================

void test_parseExp_simple_addition(void) {
    actionExp expr = parseExp("2 + 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(2.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

void test_parseExp_simple_subtraction(void) {
    actionExp expr = parseExp("5 - 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(5.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('-', expr.operat);
}

void test_parseExp_simple_multiplication(void) {
    actionExp expr = parseExp("3 * 5");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(3.0, expr.num1, EPSILON);
    assertDoubleEqual(5.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('*', expr.operat);
}

void test_parseExp_simple_division(void) {
    actionExp expr = parseExp("6 / 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(6.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('/', expr.operat);
}

void test_parseExp_simple_max(void) {
    actionExp expr = parseExp("5 M 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(5.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('M', expr.operat);
}

void test_parseExp_simple_power(void) {
    actionExp expr = parseExp("2 ^ 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(2.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('^', expr.operat);
}

void test_parseExp_without_spaces(void) {
    actionExp expr = parseExp("2+3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(2.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

void test_parseExp_with_negative_first(void) {
    actionExp expr = parseExp("-5 + 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(-5.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

void test_parseExp_with_negative_second(void) {
    actionExp expr = parseExp("5 + -3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(5.0, expr.num1, EPSILON);
    assertDoubleEqual(-3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

void test_parseExp_with_both_negative(void) {
    actionExp expr = parseExp("-5 - -3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(-5.0, expr.num1, EPSILON);
    assertDoubleEqual(-3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('-', expr.operat);
}

void test_parseExp_with_negative_and_subtraction(void) {
    actionExp expr = parseExp("-5 - 3");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(-5.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('-', expr.operat);
}

void test_parseExp_with_extra_spaces(void) {
    actionExp expr = parseExp("  2   +   3  ");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(2.0, expr.num1, EPSILON);
    assertDoubleEqual(3.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

void test_parseExp_exit_command(void) {
    actionExp expr = parseExp("q");
    TEST_ASSERT_EQUAL(-1, expr.valid);
}

void test_parseExp_exit_command_Q(void) {
    actionExp expr = parseExp("Q");
    TEST_ASSERT_EQUAL(-1, expr.valid);
}

void test_parseExp_exit_command_exit(void) {
    actionExp expr = parseExp("exit");
    TEST_ASSERT_EQUAL(-1, expr.valid);
}

void test_parseExp_exit_command_quit(void) {
    actionExp expr = parseExp("quit");
    TEST_ASSERT_EQUAL(-1, expr.valid);
}

void test_parseExp_invalid_expression(void) {
    actionExp expr = parseExp("abc");
    TEST_ASSERT_EQUAL(0, expr.valid);
}

void test_parseExp_empty_string(void) {
    actionExp expr = parseExp("");
    TEST_ASSERT_EQUAL(0, expr.valid);
}

void test_parseExp_missing_operator(void) {
    actionExp expr = parseExp("2 3");
    TEST_ASSERT_EQUAL(0, expr.valid);
}

void test_parseExp_division_with_negative(void) {
    actionExp expr = parseExp("10 / -2");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(10.0, expr.num1, EPSILON);
    assertDoubleEqual(-2.0, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('/', expr.operat);
}

void test_parseExp_fractional_numbers(void) {
    actionExp expr = parseExp("2.5 + 3.7");
    TEST_ASSERT_EQUAL(1, expr.valid);
    assertDoubleEqual(2.5, expr.num1, EPSILON);
    assertDoubleEqual(3.7, expr.num2, EPSILON);
    TEST_ASSERT_EQUAL('+', expr.operat);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    RUN_TEST(test_add_mixed_numbers);
    RUN_TEST(test_add_zero);
    RUN_TEST(test_substract_positive);
    RUN_TEST(test_substract_negative);
    RUN_TEST(test_substract_mixed);
    RUN_TEST(test_multiply_positive);
    RUN_TEST(test_multiply_negative);
    RUN_TEST(test_multiply_mixed);
    RUN_TEST(test_multiply_by_zero);
    RUN_TEST(test_divide_positive);
    RUN_TEST(test_divide_negative);
    RUN_TEST(test_divide_mixed);
    RUN_TEST(test_divide_by_zero);
    RUN_TEST(test_max_first_greater);
    RUN_TEST(test_max_second_greater);
    RUN_TEST(test_max_equal);
    RUN_TEST(test_max_negative);
    RUN_TEST(test_power_positive);
    RUN_TEST(test_power_zero_exponent);
    RUN_TEST(test_power_negative_base);
    RUN_TEST(test_power_fractional);

    RUN_TEST(test_findAct_addition);
    RUN_TEST(test_findAct_subtraction);
    RUN_TEST(test_findAct_multiplication);
    RUN_TEST(test_findAct_division);
    RUN_TEST(test_findAct_max);
    RUN_TEST(test_findAct_power);
    RUN_TEST(test_findAct_invalid);

    RUN_TEST(test_findName_addition);
    RUN_TEST(test_findName_subtraction);
    RUN_TEST(test_findName_multiplication);
    RUN_TEST(test_findName_division);
    RUN_TEST(test_findName_max);
    RUN_TEST(test_findName_power);
    RUN_TEST(test_findName_invalid);

    RUN_TEST(test_execAct_addition);
    RUN_TEST(test_execAct_subtraction);
    RUN_TEST(test_execAct_multiplication);
    RUN_TEST(test_execAct_division);
    RUN_TEST(test_execAct_division_by_zero);
    RUN_TEST(test_execAct_max);
    RUN_TEST(test_execAct_power);
    RUN_TEST(test_execAct_invalid_operator);

    RUN_TEST(test_parseExp_simple_addition);
    RUN_TEST(test_parseExp_simple_subtraction);
    RUN_TEST(test_parseExp_simple_multiplication);
    RUN_TEST(test_parseExp_simple_division);
    RUN_TEST(test_parseExp_simple_max);
    RUN_TEST(test_parseExp_simple_power);
    RUN_TEST(test_parseExp_without_spaces);
    RUN_TEST(test_parseExp_with_negative_first);
    RUN_TEST(test_parseExp_with_negative_second);
    RUN_TEST(test_parseExp_with_both_negative);
    RUN_TEST(test_parseExp_with_negative_and_subtraction);
    RUN_TEST(test_parseExp_with_extra_spaces);
    RUN_TEST(test_parseExp_exit_command);
    RUN_TEST(test_parseExp_exit_command_Q);
    RUN_TEST(test_parseExp_exit_command_exit);
    RUN_TEST(test_parseExp_exit_command_quit);
    RUN_TEST(test_parseExp_invalid_expression);
    RUN_TEST(test_parseExp_empty_string);
    RUN_TEST(test_parseExp_missing_operator);
    RUN_TEST(test_parseExp_division_with_negative);
    RUN_TEST(test_parseExp_fractional_numbers);
    
    return UNITY_END();
}