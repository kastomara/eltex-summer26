#include <math.h>

double power_func(double a, double b) {
    return pow(a, b);
}

char get_symbol(void) {
    return '^';
}

char* get_name(void) {
    return "Степень";
}

double (*get_func(void))(double, double) {
    return power_func;
}