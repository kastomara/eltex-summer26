#include "chmod.h"

void print_permissions(const FilePermissions *perms) {
    printf("Буквенное представление: %s\n", perms->symbolic);
    printf("Цифровое представление:  %s\n", perms->octal);
    printf("Битовое представление:   %o (0x%03X)\n", 
           perms->mode & 07777, perms->mode & 07777);
}

int main() {
    FilePermissions perms;
    mode_t current_mode = 0;
    char input[256];
    int choice;
    int format_type;
    
    printf("Программа для расчета маски прав доступа к файлу\n");
    printf("===============================================\n\n");
    
    while (1) {
        printf("\nВыберите действие:\n");
        printf("1. Ввести права доступа (буквенное или цифровое обозначение)\n");
        printf("2. Получить информацию о файле\n");
        printf("3. Изменить права доступа\n");
        printf("4. Выйти из программы\n");
        printf("Ваш выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода\n");
            break;
        }
        getchar();
        
        switch (choice) {
            case 1: {
                printf("Введите права доступа:\n");
                printf("  - буквенный формат (например, rwxr-xr-x или -rwxr-xr-x)\n");
                printf("  - цифровой формат (например, 755 или 0755)\n");
                printf("> ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                format_type = parse_permissions(input, &current_mode);
                format_permissions(current_mode, &perms);
                
                printf("\nРезультат (%s):\n", 
                       format_type ? "цифровой формат" : "буквенный формат");
                print_permissions(&perms);
                break;
            }
            
            case 2: {
                printf("Введите имя файла: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                
                if (get_file_permissions(input, &perms) == 0) {
                    printf("\nПрава доступа к файлу '%s':\n", input);
                    print_permissions(&perms);
                    current_mode = perms.mode;

                    char cmd[512];
                    snprintf(cmd, sizeof(cmd), "ls -l %s 2>/dev/null", input);
                    printf("\nСравнение с ls -l:\n");
                    system(cmd);
                }
                break;
            }
            
            case 3: {
                if (current_mode == 0) {
                    printf("Сначала введите или получите права доступа (пункт 1 или 2)\n");
                    break;
                }
                
                printf("Текущие права доступа:\n");
                format_permissions(current_mode, &perms);
                print_permissions(&perms);
                
                printf("\nВведите команду модификации (например, u+x, go-rwx, a=rw): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                
                modify_permissions(&current_mode, input);
                format_permissions(current_mode, &perms);
                printf("\nНовые права доступа:\n");
                print_permissions(&perms);
                printf("\n(Изменения не применены к файлу)\n");
                break;
            }
            
            case 4:
                printf("До свидания!\n");
                return 0;
                
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    }
    
    return 0;
}