// parser.c
#include "calc.h"

static char* trimStr(char* str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || 
            c == '^' || c == 'M');
}

actionExp parseExp(const char* input) {
    actionExp expr = {0, 0, ' ', 0};
    char buffer[256];
    char* str;
    char* endptr;
    
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    str = trimStr(buffer);
    
    if (strlen(str) == 0) return expr;

    if (strcmp(str, "q") == 0 || strcmp(str, "Q") == 0 || 
        strcmp(str, "exit") == 0 || strcmp(str, "quit") == 0) {
        expr.valid = -1;
        return expr;
    }

    int op_pos = -1;
    char op = ' ';
    
    for (int i = 1; str[i] != '\0'; i++) {
        if (isOperator(str[i])) {
            if (str[i] == '-') {
                if (i > 0 && str[i-1] == ' ') {
                    op_pos = i;
                    op = '-';
                    break;
                }
                else if (i > 0 && (isdigit(str[i-1]) || str[i-1] == ')' || 
                                   str[i-1] == 'M' || str[i-1] == '^')) {
                    op_pos = i;
                    op = '-';
                    break;
                }
            } else {
                op_pos = i;
                op = str[i];
                break;
            }
        }
    }
    
    if (op_pos != -1) {
        char left[128] = {0};
        char right[128] = {0};
        int left_len = 0, right_len = 0;
        
        for (int i = 0; i < op_pos; i++) {
            if (i < op_pos) {
                left[left_len++] = str[i];
            }
        }
        left[left_len] = '\0';
        
        for (int i = op_pos + 1; str[i] != '\0'; i++) {
            right[right_len++] = str[i];
        }
        right[right_len] = '\0';
        
        char* left_trim = trimStr(left);
        char* right_trim = trimStr(right);
        
        if (strlen(left_trim) == 0 || strlen(right_trim) == 0) {
            return expr;
        }
        
        expr.num1 = strtod(left_trim, &endptr);
        if (*endptr != '\0') return expr;
        
        expr.num2 = strtod(right_trim, &endptr);
        if (*endptr != '\0') return expr;
        
        expr.operat = op;
        expr.valid = 1;
        return expr;
    }
    
    char* tokens[3];
    int token_count = 0;
    char* saveptr;
    char* p = strtok_r(str, " ", &saveptr);
    
    while (p != NULL && token_count < 3) {
        tokens[token_count++] = p;
        p = strtok_r(NULL, " ", &saveptr);
    }
    
    if (token_count == 3) {
        expr.num1 = strtod(tokens[0], &endptr);
        if (*endptr != '\0') return expr;
        
        if (!isOperator(tokens[1][0])) return expr;
        expr.operat = tokens[1][0];
        
        expr.num2 = strtod(tokens[2], &endptr);
        if (*endptr != '\0') return expr;
        
        expr.valid = 1;
        return expr;
    }
    
    return expr;
}