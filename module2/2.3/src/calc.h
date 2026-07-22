#ifndef CALC_H
#define CALC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef double (*action)(double, double);

typedef struct {
    char symb;
    char* name;
    action fun;
} actionData;

typedef struct {
    double num1;
    double num2;
    char operat;
    int valid;
} actionExp;

double add(double a, double b);
double substract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double max(double a, double b);
double power(double a, double b);

void menu();

action findAct(char symb);
char* findName(char symb);
double execAct(char symb, double a, double b, int* error);

actionExp parseExp(const char* input);

#endif