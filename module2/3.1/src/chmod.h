#ifndef CHMOD_H
#define CHMOD_H

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    mode_t mode;
    char symbolic[11];
    char octal[5];
} FilePermissions;

void parse_symbolic(const char *input, mode_t *mode);
void parse_octal(const char *input, mode_t *mode);
void format_permissions(mode_t mode, FilePermissions *perms);
void print_permissions(const FilePermissions *perms);
int get_file_permissions(const char *filename, FilePermissions *perms);
void modify_permissions(mode_t *mode, const char *command);
int parse_permissions(const char *input, mode_t *mode);

#endif