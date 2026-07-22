#define UNITY_INCLUDE_DOUBLE
#include "unity.h"
#include "calc.h"
#include <math.h>

void setUp (void) {}
void tearDown (void) {}

void test_add_positive_numbers(void)
{
    double result = add(5.0, 3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 8.0f, (float)result);
}

void test_add_negative_numbers(void)
{
    double result = add(-5.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, -8.0f, (float)result);
}

void test_add_positive_and_negative(void)
{
    double result = add(5.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 2.0f, (float)result);
}

void test_add_zero(void)
{
    double result = add(5.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, (float)result);
}

void test_add_decimal_numbers(void)
{
    double result = add(2.5, 3.7);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 6.2f, (float)result);
}

void test_subtract_positive_numbers(void)
{
    double result = subtract(8.0, 3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, (float)result);
}

void test_subtract_negative_numbers(void)
{
    double result = subtract(-8.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, -5.0f, (float)result);
}

void test_subtract_positive_and_negative(void)
{
    double result = subtract(8.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 11.0f, (float)result);
}

void test_subtract_zero(void)
{
    double result = subtract(5.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, (float)result);
}

void test_subtract_decimal_numbers(void)
{
    double result = subtract(6.7, 2.3);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 4.4f, (float)result);
}

void test_multiply_positive_numbers(void)
{
    double result = multiply(4.0, 3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 12.0f, (float)result);
}

void test_multiply_negative_numbers(void)
{
    double result = multiply(-4.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 12.0f, (float)result);
}

void test_multiply_positive_and_negative(void)
{
    double result = multiply(4.0, -3.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, -12.0f, (float)result);
}

void test_multiply_zero(void)
{
    double result = multiply(5.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, (float)result);
}

void test_multiply_decimal_numbers(void)
{
    double result = multiply(2.5, 1.5);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.75f, (float)result);
}

void test_divide_positive_numbers(void)
{
    double result = divide(10.0, 2.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, (float)result);
}

void test_divide_negative_numbers(void)
{
    double result = divide(-10.0, -2.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, (float)result);
}

void test_divide_positive_and_negative(void)
{
    double result = divide(10.0, -2.0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, -5.0f, (float)result);
}

void test_divide_decimal_numbers(void)
{
    double result = divide(7.5, 2.5);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.0f, (float)result);
}

void test_divide_by_zero_infinity(void)
{
    double result = divide(5.0, 0.0);
    TEST_ASSERT_TRUE(isinf(result) || result > 1e300);
}

void test_large_numbers(void)
{
    double result = add(1e10, 1e10);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 2e10f, (float)result);
}

void test_small_numbers(void)
{
    double result = add(1e-10, 1e-10);
    TEST_ASSERT_FLOAT_WITHIN(1e-11f, 2e-10f, (float)result);
}

void test_precision_maintenance(void)
{
    double result = multiply(0.1, 0.1);
    TEST_ASSERT_FLOAT_WITHIN(0.0000001f, 0.01f, (float)result);
}

void test_menu_does_not_crash(void)
{
    menu();
    TEST_ASSERT_TRUE(1);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    RUN_TEST(test_add_positive_and_negative);
    RUN_TEST(test_add_zero);
    RUN_TEST(test_add_decimal_numbers);

    RUN_TEST(test_subtract_positive_numbers);
    RUN_TEST(test_subtract_negative_numbers);
    RUN_TEST(test_subtract_positive_and_negative);
    RUN_TEST(test_subtract_zero);
    RUN_TEST(test_subtract_decimal_numbers);

    RUN_TEST(test_multiply_positive_numbers);
    RUN_TEST(test_multiply_negative_numbers);
    RUN_TEST(test_multiply_positive_and_negative);
    RUN_TEST(test_multiply_zero);
    RUN_TEST(test_multiply_decimal_numbers);

    RUN_TEST(test_divide_positive_numbers);
    RUN_TEST(test_divide_negative_numbers);
    RUN_TEST(test_divide_positive_and_negative);
    RUN_TEST(test_divide_decimal_numbers);
    RUN_TEST(test_divide_by_zero_infinity);

    RUN_TEST(test_large_numbers);
    RUN_TEST(test_small_numbers);
    RUN_TEST(test_precision_maintenance);

    RUN_TEST(test_menu_does_not_crash);
    
    return UNITY_END();
}