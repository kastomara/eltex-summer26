#ifndef CALC_H
#define CALC_H
#include <stddef.h>

typedef struct {
    int numeric;
    char binary[10];
    char symbolic[256];
} Umask;

int num_bin (int umask, char *binary);
void num_sym (int umask, char *symbolic, size_t size);
int sym_num (const char* symbolic);
int parse_umask(const char *input, Umask *result);
void printUmask(const Umask *umask);
int is_valid_umask(int umask);
int get_permission_bits(char perm_char);

#endif