#include "calc.h"

double add(double a, double b) {
    return a + b;
}

double substract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) return 0;
    return a / b;
}

double max(double a, double b) {
    return (a > b) ? a : b;
}

double power(double a, double b) {
    return pow(a, b);
}

actionData actions[] = {
    {'+', "Сложение", add},
    {'-', "Вычитание", substract},
    {'*', "Умножение", multiply},
    {'/', "Деление", divide},
    {'M', "Максимум", max},
    {'^', "Степень", power},
    {'%', "Целочисленное деление", divide}

};
const int ACT_COUNT = sizeof(actions) / sizeof(actions[0]);
action findAct(char symb) {
    for (size_t i = 0; i < ACT_COUNT; i++) {
        if (actions[i].symb == symb) {
            return actions[i].fun;
        }
    }
    return NULL;
}

char* findName(char symb) {
    for (size_t i = 0; i < ACT_COUNT; i++) {
        if (actions[i].symb == symb) {
            return actions[i].name;
        }
    }
    return "Неизвестная операция";
}

double execAct(char symb, double a, double b, int* error) {
    *error = 0;

    if ((symb == '/' || symb == '%') && b == 0) {
        *error = 1;
        return 0;
    }
    
    action act = findAct(symb);
    if (act == NULL) {
        *error = 1;
        return 0;
    }
    
    return act(a, b);
}

void menu() {
    printf("\n===== КАЛЬКУЛЯТОР =====\n");
    for (size_t i = 0; i < ACT_COUNT; i++) {
        printf("%zu. %s (%c)\n", i + 1, actions[i].name, actions[i].symb);
    }
    printf("0. Выход\n");
    printf("=======================\n");
    printf("Или введите выражение: 2 + 3\n");
    printf("========================\n");
}
