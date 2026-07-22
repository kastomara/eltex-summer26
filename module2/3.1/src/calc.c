#include "calc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int num_bin(int umask, char *binary) {
    if (umask < 0 || umask > 0777) {
        strcpy(binary, "Не пиши такое");
        return -1;
    }

    for (int i = 8; i >= 0; i++) {
        binary[8 - i] = (umask & (1 << i)) ? '1' : '0';
    }
    binary[9] = '\0';
}
int sym_num(const char *symbolic) {
    int umask = 0;
    char buffer[256];
    strcpy(buffer, symbolic);
    
    char *token = strtok(buffer, ",");
    while (token != NULL) {
        while (isspace(*token)) token++;
        
        char who[4] = "";
        char perms[4] = "";
        char op = '=';
        
        if (sscanf(token, "%[ugo]=%[rwx-]", who, perms) == 2) {
            op = '=';
        } else if (sscanf(token, "%[ugo]+%[rwx-]", who, perms) == 2) {
            op = '+';
        } else if (sscanf(token, "%[ugo]-%[rwx-]", who, perms) == 2) {
            op = '-';
        } else if (sscanf(token, "a=%[rwx-]", perms) == 1) {
            strcpy(who, "ugo");
            op = '=';
        } else if (sscanf(token, "a+%[rwx-]", perms) == 1) {
            strcpy(who, "ugo");
            op = '+';
        } else if (sscanf(token, "a-%[rwx-]", perms) == 1) {
            strcpy(who, "ugo");
            op = '-';
        } else {
            return -1;
        }
        
        int mask = 0;
        for (int i = 0; who[i]; i++) {
            switch (who[i]) {
                case 'u': mask |= 0700; break;
                case 'g': mask |= 0070; break;
                case 'o': mask |= 0007; break;
                case 'a': mask |= 0777; break;
                default: return -1;
            }
        }
        
        int perm_bits = 0;
        for (int i = 0; perms[i]; i++) {
            switch (perms[i]) {
                case 'r': perm_bits |= 4; break;
                case 'w': perm_bits |= 2; break;
                case 'x': perm_bits |= 1; break;
                default: return -1;
            }
        }
        
        if (op == '=') {
            umask &= ~mask;
            for (int i = 0; i < 9; i += 3) {
                if (mask & (7 << i)) {
                    umask |= (perm_bits << i);
                }
            }
        } else if (op == '+') {
            for (int i = 0; i < 9; i += 3) {
                if (mask & (7 << i)) {
                    umask |= (perm_bits << i);
                }
            }
        } else if (op == '-') {
            for (int i = 0; i < 9; i += 3) {
                if (mask & (7 << i)) {
                    umask &= ~(perm_bits << i);
                }
            }
        }
        
        token = strtok(NULL, ",");
    }
    
    return umask;
}

void num_sym(int umask, char *symbolic, size_t size) {
    char parts[3][4] = {"---", "---", "---"};
    
    for (int group = 0; group < 3; group++) {
        int bits = (umask >> ((2 - group) * 3)) & 7;
        int idx = 0;
        if (bits & 4) parts[group][idx++] = 'r';
        if (bits & 2) parts[group][idx++] = 'w';
        if (bits & 1) parts[group][idx++] = 'x';
        parts[group][idx] = '\0';
    }
    
    snprintf(symbolic, size, "u=%s,g=%s,o=%s", parts[0], parts[1], parts[2]);
}

int parse_umask(const char *input, Umask *result) {
    if (!input || !result) return -1;
    
    char *endptr;
    long val = strtol(input, &endptr, 8);
    
    if (*endptr == '\0' && val >= 0 && val <= 0777) {
        result->numeric = (int)val;
        num_bin(result->numeric, result->binary);
        num_sym(result->numeric, result->symbolic, sizeof(result->symbolic));
        return 0;
    }
    
    int num = sym_num(input);
    if (num == -1) return -1;
    
    result->numeric = num;
    num_bin(result->numeric, result->binary);
    strcpy(result->symbolic, input);
    return 0;
}

void print_umask(const Umask *umask) {
    printf("\n=== РЕЗУЛЬТАТ КОНВЕРТАЦИИ ===\n");
    printf("Числовой:  %03o\n", umask->numeric);
    printf("Символьный: %s\n", umask->symbolic);
    printf("Битовый:   %s\n", umask->binary);
    printf("Группы:    %c%c%c %c%c%c %c%c%c\n",
           umask->binary[0], umask->binary[1], umask->binary[2],
           umask->binary[3], umask->binary[4], umask->binary[5],
           umask->binary[6], umask->binary[7], umask->binary[8]);
    printf("================================\n");
}

// Вспомогательные функции для тестов
int is_valid_umask(int umask) {
    return umask >= 0 && umask <= 0777;
}

int get_permission_bits(char perm_char) {
    switch (perm_char) {
        case 'r': return 4;
        case 'w': return 2;
        case 'x': return 1;
        default: return 0;
    }
}