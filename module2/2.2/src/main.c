#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

int main() {
    int choice;
    double num1, num2, result;
    
    while (1) {
        menu();
        printf("Выберите действие (0 для выхода): ");
        scanf("%d", &choice);

        if (choice == 0) {
            printf("До свидания!\n");
            break;
        }
        if (choice < 1 || choice > 4) {
            printf("Ошибка: неверный выбор!\n");
            printf("\n");
            continue;
        }
        printf("Введите первое число: ");
        scanf("%lf", &num1);
        printf("Введите второе число: ");
        scanf("%lf", &num2);

switch(choice) {
            case 1:
                result = add(num1, num2);
                printf("Результат: %.2lf + %.2lf = %.2lf\n", num1, num2, result);
                break;
            case 2:
                result = subtract(num1, num2);
                printf("Результат: %.2lf - %.2lf = %.2lf\n", num1, num2, result);
                break;
            case 3:
                result = multiply(num1, num2);
                printf("Результат: %.2lf * %.2lf = %.2lf\n", num1, num2, result);
                break;
            case 4:
                if (num2 == 0) {
                    printf("Не делай так: деление на ноль!\n");
                } else {
                    result = divide(num1, num2);
                    printf("Результат: %.2lf / %.2lf = %.2lf\n", num1, num2, result);
                }
                break;
        }
        
        printf("\n");
    }
    
    return 0;
}