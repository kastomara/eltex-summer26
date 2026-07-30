double add_func(double a, double b) {
    return a + b;
}

char get_symbol(void) {
    return '+';
}

char* get_name(void) {
    return "Сложение";
}

double (*get_func(void))(double, double) {
    return add_func;
}