#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct {
    char* type;
    char* tag;
    int wiringPi;
    int gpio;
    int value;
} Module;

typedef struct {
    char* ip;
    int port;
    char* name;
    Module* outputs;
    Module* inputs;
    int outputSize;
    int inputSize;
} Configuration;

#endif
