#include "chmod.h"

void parse_symbolic(const char *input, mode_t *mode) {
    const char *str = input;

    if (str[0] == '-' || str[0] == 'd') {
        str++;
    }

    if (strlen(str) != 9) {
        return;
    }

    if (str[0] == 'r') *mode |= S_IRUSR; else *mode &= ~S_IRUSR;
    if (str[1] == 'w') *mode |= S_IWUSR; else *mode &= ~S_IWUSR;
    if (str[2] == 'x') *mode |= S_IXUSR; else *mode &= ~S_IXUSR;

    if (str[3] == 'r') *mode |= S_IRGRP; else *mode &= ~S_IRGRP;
    if (str[4] == 'w') *mode |= S_IWGRP; else *mode &= ~S_IWGRP;
    if (str[5] == 'x') *mode |= S_IXGRP; else *mode &= ~S_IXGRP;

    if (str[6] == 'r') *mode |= S_IROTH; else *mode &= ~S_IROTH;
    if (str[7] == 'w') *mode |= S_IWOTH; else *mode &= ~S_IWOTH;
    if (str[8] == 'x') *mode |= S_IXOTH; else *mode &= ~S_IXOTH;
}

void parse_octal(const char *input, mode_t *mode) {
    char *endptr;
    long value = strtol(input, &endptr, 8);
    
    if (*endptr != '\0') {
        return;
    }
    
    if (value < 0 || value > 07777) {
        return;
    }
    
    *mode = (mode_t)value;
}

int parse_permissions(const char *input, mode_t *mode) {
    int is_octal = 1;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] < '0' || input[i] > '7') {
            is_octal = 0;
            break;
        }
    }
    
    if (is_octal && strlen(input) >= 3 && strlen(input) <= 4) {
        parse_octal(input, mode);
        return 1;
    } else {
        parse_symbolic(input, mode);
        return 0;
    }
}

void format_permissions(mode_t mode, FilePermissions *perms) {
    perms->mode = mode;

    char sym[11];
    sym[0] = S_ISDIR(mode) ? 'd' : '-';
    sym[1] = (mode & S_IRUSR) ? 'r' : '-';
    sym[2] = (mode & S_IWUSR) ? 'w' : '-';
    
    if (mode & S_ISUID) {
        sym[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        sym[3] = (mode & S_IXUSR) ? 'x' : '-';
    }
    
    sym[4] = (mode & S_IRGRP) ? 'r' : '-';
    sym[5] = (mode & S_IWGRP) ? 'w' : '-';
    
    if (mode & S_ISGID) {
        sym[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        sym[6] = (mode & S_IXGRP) ? 'x' : '-';
    }
    
    sym[7] = (mode & S_IROTH) ? 'r' : '-';
    sym[8] = (mode & S_IWOTH) ? 'w' : '-';

    if (mode & S_ISVTX) {
        sym[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        sym[9] = (mode & S_IXOTH) ? 'x' : '-';
    }
    
    sym[10] = '\0';
    strcpy(perms->symbolic, sym);

    snprintf(perms->octal, sizeof(perms->octal), "%04o", mode & 07777);
}

int get_file_permissions(const char *filename, FilePermissions *perms) {
    struct stat st;
    
    if (stat(filename, &st) != 0) {
        return -1;
    }
    
    format_permissions(st.st_mode, perms);
    return 0;
}

void modify_permissions(mode_t *mode, const char *command) {
    if (strlen(command) < 2) {
        return;
    }

    char who[4] = {0};
    char op[2] = {0};
    char perms[4] = {0};

    int i = 0;
    while (command[i] && command[i] != '+' && command[i] != '-' && command[i] != '=') {
        if (i < 3) who[i] = command[i];
        i++;
    }
    
    if (command[i] == '+' || command[i] == '-' || command[i] == '=') {
        op[0] = command[i];
        i++;
    } else {
        return;
    }
    
    int j = 0;
    while (command[i] && j < 3) {
        perms[j] = command[i];
        i++;
        j++;
    }

    int apply_user = 0, apply_group = 0, apply_other = 0;
    if (strchr(who, 'u') || strchr(who, 'a')) apply_user = 1;
    if (strchr(who, 'g') || strchr(who, 'a')) apply_group = 1;
    if (strchr(who, 'o') || strchr(who, 'a')) apply_other = 1;

    if (strlen(who) == 0) {
        apply_user = apply_group = apply_other = 1;
    }
 
    mode_t r_bit = 0, w_bit = 0, x_bit = 0;
    if (strchr(perms, 'r')) {
        r_bit = S_IRUSR | S_IRGRP | S_IROTH;
    }
    if (strchr(perms, 'w')) {
        w_bit = S_IWUSR | S_IWGRP | S_IWOTH;
    }
    if (strchr(perms, 'x')) {
        x_bit = S_IXUSR | S_IXGRP | S_IXOTH;
    }
    
    mode_t bits = 0;
    if (apply_user) bits |= r_bit & (S_IRUSR | S_IWUSR | S_IXUSR);
    if (apply_user) bits |= w_bit & (S_IRUSR | S_IWUSR | S_IXUSR);
    if (apply_user) bits |= x_bit & (S_IRUSR | S_IWUSR | S_IXUSR);
    
    if (apply_group) bits |= r_bit & (S_IRGRP | S_IWGRP | S_IXGRP);
    if (apply_group) bits |= w_bit & (S_IRGRP | S_IWGRP | S_IXGRP);
    if (apply_group) bits |= x_bit & (S_IRGRP | S_IWGRP | S_IXGRP);
    
    if (apply_other) bits |= r_bit & (S_IROTH | S_IWOTH | S_IXOTH);
    if (apply_other) bits |= w_bit & (S_IROTH | S_IWOTH | S_IXOTH);
    if (apply_other) bits |= x_bit & (S_IROTH | S_IWOTH | S_IXOTH);

    switch (op[0]) {
        case '+':
            *mode |= bits;
            break;
        case '-':
            *mode &= ~bits;
            break;
        case '=':
            if (apply_user) {
                *mode &= ~(S_IRUSR | S_IWUSR | S_IXUSR);
                *mode |= bits & (S_IRUSR | S_IWUSR | S_IXUSR);
            }
            if (apply_group) {
                *mode &= ~(S_IRGRP | S_IWGRP | S_IXGRP);
                *mode |= bits & (S_IRGRP | S_IWGRP | S_IXGRP);
            }
            if (apply_other) {
                *mode &= ~(S_IROTH | S_IWOTH | S_IXOTH);
                *mode |= bits & (S_IROTH | S_IWOTH | S_IXOTH);
            }
            break;
        default:
            break;
    }
}