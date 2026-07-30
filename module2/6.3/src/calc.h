#ifndef CALC_H
#define CALC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>

typedef double (*action)(double, double);

typedef struct {
    char symb;
    char* name;
    action fun;
    void* handle;
} actionData;

typedef struct {
    double num1;
    double num2;
    char operat;
    int valid;
} actionExp;

int loadLibraries(const char* libDir);
void unloadLibraries();
int getActionCount();
char getActionSymbol(int index);
char* getActionName(int index);
action getActionFunc(int index);

action findAct(char symb);
char* findName(char symb);

actionExp parseExp(const char* input);

void menu();

#endif