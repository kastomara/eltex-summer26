double divide_func(double a, double b) {
    if (b == 0) {
        return 0;
    }
    return a / b;
}

char get_symbol(void) {
    return '/';
}

char* get_name(void) {
    return "Деление";
}

double (*get_func(void))(double, double) {
    return divide_func;
}