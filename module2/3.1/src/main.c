#include <stdio.h>
#include <string.h>
#include "calc.h"

int main() {
    char input[256];
    Umask umask;
    
    printf("====================================\n");
    printf("   КОНВЕРТЕР UMASK В БИТОВЫЙ ВИД\n");
    printf("====================================\n");
    printf("Поддерживаемые форматы:\n");
    printf("  - Числовой: 0-0777 (например, 755)\n");
    printf("  - Символьный: u=rwx,g=rx,o=r\n");
    printf("  - Спецсимвол 'a' для всех (a=rwx)\n");
    printf("  - Операции: = (уст), + (доб), - (уд)\n");
    printf("====================================\n\n");
    
    while (1) {
        printf("Введите umask (или 'exit' для выхода): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "exit") == 0 || strcmp(input, "q") == 0) {
            printf("Выход из программы.\n");
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        if (parse_umask(input, &umask) == 0) {
            print_umask(&umask);
        } else {
            printf("Ошибка: неверный формат umask!\n");
            printf("Примеры: 755, u=rwx,g=rx,o=r, a=rwx\n\n");
        }
    }
    
    return 0;
}