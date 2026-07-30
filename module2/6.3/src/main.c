#include "calc.h"

int main(int argc, char* argv[]) {
    char input[256];
    int choice;
    double num1, num2, result;
    char* endptr;
    const char* libDir = "./lib";
    
    if (argc > 1) {
        libDir = argv[1];
    }
    
    printf("\n========================================\n");
    printf("   КАЛЬКУЛЯТОР С ДИНАМИЧЕСКИМИ БИБЛИОТЕКАМИ\n");
    printf("========================================\n");
    
    if (!loadLibraries(libDir)) {
        printf("Ошибка загрузки библиотек!\n");
        return 1;
    }
    
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
            action act = findAct(expr.operat);
            if (!act) {
                printf("Ошибка: операция '%c' не поддерживается!\n", expr.operat);
                continue;
            }
            
            if ((expr.operat == '/' || expr.operat == '%') && expr.num2 == 0) {
                printf("Ошибка: деление на ноль!\n");
                continue;
            }
            
            result = act(expr.num1, expr.num2);
            printf("Результат: %.2lf %c %.2lf = %.2lf\n", 
                   expr.num1, expr.operat, expr.num2, result);
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
        
        int actCount = getActionCount();
        if (choice < 1 || choice > actCount) {
            printf("Ошибка: неверный выбор! Введите число от 0 до %d.\n", actCount);
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
        
        char op_char = getActionSymbol(choice - 1);
        action act = getActionFunc(choice - 1);
        
        if ((op_char == '/' || op_char == '%') && num2 == 0) {
            printf("Ошибка: деление на ноль!\n");
            continue;
        }
        
        result = act(num1, num2);
        printf("Результат: %.2lf %c %.2lf = %.2lf\n", 
               num1, op_char, num2, result);
    }
    
    unloadLibraries();
    return 0;
}