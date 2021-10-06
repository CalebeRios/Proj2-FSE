#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "../inc/wiring-pi.h"
#include "../inc/client.h"

Module* output_modules;
Module* input_modules_t;
Module* input_modules_1f;

int output_size = 0;
int input_size_t, input_size_1f;

sem_t mutex_output, mutex_input;

void handle_sensors(int pin, Module* modules, int size);
void in(int pin, Module* modules, int size);
void out(int pin, Module* modules, int size);

void handle_pres_t(void) { handle_sensors(25, input_modules_t, input_size_t); }
void handle_pres(void) { handle_sensors(1, input_modules_1f, input_size_1f); }

void handle_smoke_t(void) { handle_sensors(4, input_modules_t, input_size_t); }
void handle_smoke(void) { handle_sensors(5, input_modules_1f, input_size_1f); }

void handle_window_t01(void) { handle_sensors(13, input_modules_t, input_size_t); }
void handle_window_t02(void) { handle_sensors(14, input_modules_t, input_size_t); }

void handle_window_01(void) { handle_sensors(21, input_modules_1f, input_size_1f); }
void handle_window_02(void) { handle_sensors(22, input_modules_1f, input_size_1f); }

void handle_door(void) { handle_sensors(12, input_modules_t, input_size_t); }

void handle_in(void) { in(23, input_modules_t, input_size_t); }
void handle_out(void) { out(24, input_modules_t, input_size_t); }

void init_pwm_wiringPi() {
    wiringPiSetup();
    sem_init(&mutex_output, 0, 1);
    sem_init(&mutex_input, 0, 1);
}

void init_outputs(Module* modules, int size) {
    sem_wait(&mutex_output);

    output_modules = modules;
    output_size += size;

    for(int i = 0; i < size; i++) {
        Module* module = &modules[i];
        
        pinMode(module->wiringPi, OUTPUT);
        softPwmCreate(module->wiringPi, 0, 1);

        module->value = digitalRead(module->wiringPi);
        printf("%s: %d\n", module->tag, module->value);
    }

    sem_post(&mutex_output);
}

void init_inputs(Module* modules, int size, char* floor) {
    sem_wait(&mutex_input);

    if (strcmp(floor, "terreo") == 0) {
        input_modules_t = modules;
        input_size_t = size;
    } else {
        input_modules_1f = modules;
        input_size_1f = size;
    }

    for(int i = 0; i < size; i++) {
        Module* module = &modules[i];

        pinMode(module->wiringPi, INPUT);

        module->value = digitalRead(module->wiringPi);

        if (strcmp(module->type, "presenca") == 0) {
            if (strcmp(floor, "terreo") == 0) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_pres_t);
            } else {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_pres);
            }
        } else if (strcmp(module->type, "fumaca") == 0) {
            if (strcmp(floor, "terreo") == 0) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_smoke_t);
            } else {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_smoke);
            }
        } else if (strcmp(module->type, "janela") == 0) {
            if (strstr(module->tag, "T01")) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_t01);
            } else if (strstr(module->tag, "T02")) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_t02);
            } else if (strstr(module->tag, "101")) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_01);
            } else if (strstr(module->tag, "102")) {
                wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_02);
            }
        } else if (strcmp(module->type, "porta") == 0) {
            wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_door);
        } else {
            if (strstr(module->tag, "Entrando")) {
                wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_in);
            } else {
                wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_out);
            }
        }

        printf("%s (%s): %d\n", module->tag, module->type, module->value);
    }

    sem_post(&mutex_input);
}

void stop_pwm_wiringPi(Module* modules, int size) {
    for(int i = 0; i < size; i++) {
        printf("Stop: %s | with %d\n", modules[i].tag, size);
        softPwmStop(modules[i].wiringPi);
    }
    sem_destroy(&mutex_output);
    sem_destroy(&mutex_input);
}

void turn_pin(int pin, int on) {
    softPwmWrite(pin, on);
}

void handle_sensors(int pin, Module* modules, int size) {
    Module* module;

    for (int i = 0; i < size; i++) {
        if (modules[i].wiringPi == pin) {
            module = &modules[i];
            break;
        }
    }

    module->value = !module->value;

    sent_update_sensor_in(pin, module->value);

    printf("Value (%s): %d\n", module->tag, module->value);
}

void in(int pin, Module* modules, int size) {
    Module* module;

    for (int i = 0; i < size; i++) {
        if (modules[i].wiringPi == pin) {
            module = &modules[i];
            break;
        }
    }

    module->value += 1;

    sent_update_sensor_in(pin, module->value);
    usleep(50);
    sent_update_sensor_people(1);

    printf("Value (%s): %d\n", module->tag, module->value);
}

void out(int pin, Module* modules, int size) {
    Module* module;

    for (int i = 0; i < size; i++) {
        if (modules[i].wiringPi == pin) {
            module = &modules[i];
            break;
        }
    }

    module->value += 1;

    sent_update_sensor_in(pin, module->value);
    usleep(50);
    sent_update_sensor_people(-1);

    printf("Value (%s): %d\n", module->tag, module->value);
}
