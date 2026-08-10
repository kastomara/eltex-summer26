double max_func(double a, double b) {
    return (a > b) ? a : b;
}

char get_symbol(void) {
    return 'M';
}

char* get_name(void) {
    return "Максимум";
}

double (*get_func(void))(double, double) {
    return max_func;
}