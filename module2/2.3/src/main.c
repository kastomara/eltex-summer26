#include "calc.h"

int main() {
    char input[256];
    int choice;
    double num1, num2, result;
    int error;
    char* endptr;
    
    while (1) {
        menu();
        printf("Введите команду или выражение: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) continue;

        actionExp expr = parseExp(input);
        
        if (expr.valid == -1) {
            printf("До свидания!\n");
            break;
        }
        
        if (expr.valid) {
            result = execAct(expr.operat, expr.num1, expr.num2, &error);
            if (error) {
                printf("Ошибка: деление на ноль!\n");
            } else {
                printf("Результат: %.2lf %c %.2lf = %.2lf\n", 
                       expr.num1, expr.operat, expr.num2, result);
            }
            continue;
        }

        choice = strtol(input, &endptr, 10);
        
        if (*endptr != '\0' || endptr == input) {
            printf("Ошибка: неверный формат! Введите число или выражение.\n");
            continue;
        }
        
        if (choice == 0) {
            printf("До свидания!\n");
            break;
        }
        
        if (choice < 1 || choice > 6) {
            printf("Ошибка: неверный выбор! Введите число от 0 до 6.\n");
            continue;
        }

        printf("Введите первое число: ");
        if (scanf("%lf", &num1) != 1) {
            printf("Ошибка ввода!\n");
            while (getchar() != '\n');
            continue;
        }
        
        printf("Введите второе число: ");
        if (scanf("%lf", &num2) != 1) {
            printf("Ошибка ввода!\n");
            while (getchar() != '\n');
            continue;
        }
        
        while (getchar() != '\n');
        
        char ops[] = {'+', '-', '*', '/', 'M', '^'};
        char op_char = ops[choice - 1];
        
        result = execAct(op_char, num1, num2, &error);
        
        if (error) {
            printf("Ошибка: деление на ноль!\n");
        } else {
            printf("Результат: %.2lf %c %.2lf = %.2lf\n", 
                   num1, op_char, num2, result);
        }
    }
    
    return 0;
}