#ifndef _GLOBAL_H
#define _GLOBAL_H

typedef struct {
    char* tag;
    int wiringPi;
    int value;
} Module;

typedef struct {
    char* name;
    Module* outputs;
    Module* inputs;
    int outputSize;
    int inputSize;
} Configuration;

Configuration terreo, firstFloor;
float temp, umi;
static int qtd_people = 0;

#endif
