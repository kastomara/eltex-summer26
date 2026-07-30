double subtract_func(double a, double b) {
    return a - b;
}

char get_symbol(void) {
    return '-';
}

char* get_name(void) {
    return "Вычитание";
}

double (*get_func(void))(double, double) {
    return subtract_func;
}