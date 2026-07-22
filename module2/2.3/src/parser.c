#include "calc.h"

static char* trimStr(char* str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static int isActChar(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || 
            c == '^' || c == '%' || c == 'M' || c == 'm');
}

actionExp parseExp(const char* input) {
    actionExp expr = {0, 0, ' ', 0};
    char buffer[256];
    char* str;
    char* endptr;
    double num1, num2;
    char op = ' ';
    int found_op = 0;

    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    str = trimStr(buffer);
    
    if (strlen(str) == 0) {
        return expr;
    }

    if (strcmp(str, "q") == 0 || strcmp(str, "Q") == 0 || 
        strcmp(str, "exit") == 0 || strcmp(str, "quit") == 0) {
        expr.valid = -1;
        return expr;
    }

    char temp[256];
    strcpy(temp, str);

    for (int i = 0; temp[i]; i++) {
        if (isActChar(temp[i])) {
            op = temp[i];
            found_op = 1;
            temp[i] = ' ';
        }
    }
    
    if (!found_op) {
        char* tokens[3];
        int token_count = 0;
        char* saveptr;
        char* p = strtok_r(str, " ", &saveptr);
        
        while (p != NULL && token_count < 3) {
            tokens[token_count++] = p;
            p = strtok_r(NULL, " ", &saveptr);
        }
        
        if (token_count == 3) {
            num1 = strtod(tokens[0], &endptr);
            if (*endptr != '\0') return expr;
            
            op = tokens[1][0];
            if (!isActChar(op)) return expr;
            
            num2 = strtod(tokens[2], &endptr);
            if (*endptr != '\0') return expr;
        } else {
            return expr;
        }
    } else {
        char* tokens[2];
        int token_count = 0;
        char* saveptr;
        char* p = strtok_r(temp, " ", &saveptr);
        
        while (p != NULL && token_count < 2) {
            tokens[token_count++] = p;
            p = strtok_r(NULL, " ", &saveptr);
        }
        
        if (token_count == 2) {
            num1 = strtod(tokens[0], &endptr);
            if (*endptr != '\0') return expr;
            
            num2 = strtod(tokens[1], &endptr);
            if (*endptr != '\0') return expr;
        } else {
            return expr;
        }
    }
    
    expr.num1 = num1;
    expr.num2 = num2;
    expr.operat = op;
    expr.valid = 1;
    
    return expr;
}