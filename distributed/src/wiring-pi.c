// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <semaphore.h>
// #include <wiringPi.h>
// #include <softPwm.h>

// #include "../inc/wiring-pi.h"

// Module* output_modules;
// Module* input_modules_t;
// Module* input_modules_1f;

// int output_size = 0;
// int input_size_t, input_size_1f;

// sem_t mutex_output, mutex_input;

// void handle_sensors(int pin, Module* modules, int size);
// void in(int pin, Module* modules, int size);
// void out(int pin, Module* modules, int size);

// void handle_pres_t(void) { handle_sensors(26, input_modules_t, input_size_t); }
// void handle_pres(void) { handle_sensors(18, input_modules_1f, input_size_1f); }

// void handle_smoke_t(void) { handle_sensors(23, input_modules_t, input_size_t); }
// void handle_smoke(void) { handle_sensors(24, input_modules_1f, input_size_1f); }

// void handle_window_t01(void) { handle_sensors(9, input_modules_t, input_size_t); }
// void handle_window_t02(void) { handle_sensors(11, input_modules_t, input_size_t); }

// void handle_window_01(void) { handle_sensors(5, input_modules_1f, input_size_1f); }
// void handle_window_02(void) { handle_sensors(6, input_modules_1f, input_size_1f); }

// void handle_door(void) { handle_sensors(10, input_modules_t, input_size_t); }

// void handle_in(void) { in(13, input_modules_t, input_size_t); }
// void handle_out(void) { out(19, input_modules_t, input_size_t); }

// void init_pwm_wiringPi() {
//     wiringPiSetup();
//     sem_init(&mutex_output, 0, 1);
//     sem_init(&mutex_input, 0, 1);
// }

// void init_outputs(Module* modules, int size) {
//     printf("OUTPUTS\n");

//     sem_wait(&mutex_output);

//     output_modules = modules;
//     output_size += size;

//     for(int i = 0; i < size; i++) {
//         Module* module = &modules[i];
        
//         pinMode(module->wiringPi, OUTPUT);
//         softPwmCreate(module->wiringPi, 0, 100);

//         module->value = digitalRead(module->wiringPi);
//         printf("%s: %d\n", module->tag, module->value);
//     }

//     sem_post(&mutex_output);
// }

// void init_inputs(Module* modules, int size, char* floor) {
//     printf("INPUTS\n");

//     sem_wait(&mutex_input);

//     if (strcmp(floor, "terreo") == 0) {
//         input_modules_t = modules;
//         input_size_t = size;
//     } else {
//         input_modules_1f = modules;
//         input_size_1f = size;
//     }

//     for(int i = 0; i < size; i++) {
//         Module* module = &modules[i];

//         pinMode(module->wiringPi, INPUT);

//         if (strcmp(module->type, "presenca") == 0) {
//             if (strcmp(floor, "terreo") == 0) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_pres_t);
//             } else {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_pres);
//             }
//         } else if (strcmp(module->type, "fumaca") == 0) {
//             if (strcmp(floor, "terreo") == 0) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_smoke_t);
//             } else {
//                 wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_smoke);
//             }
//             wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_smoke);
//         } else if (strcmp(module->type, "janela") == 0) {
//             if (strstr(module->tag, "T01")) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_t01);
//             } else if (strstr(module->tag, "T02")) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_t02);
//             } else if (strstr(module->tag, "101")) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_01);
//             } else if (strstr(module->tag, "102")) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_window_02);
//             }
//         } else if (strcmp(module->type, "porta") == 0) {
//             wiringPiISR(module->wiringPi, INT_EDGE_BOTH, &handle_door);
//         } else {
//             if (strstr(module->tag, "Entrando")) {
//                 wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_in);
//             } else {
//                 wiringPiISR(module->wiringPi, INT_EDGE_RISING, &handle_out);
//             }
//         }

//         module->value = digitalRead(module->wiringPi);
//         printf("%s (%s): %d\n", module->tag, module->type, module->value);
//     }

//     sem_post(&mutex_input);
// }

// void stop_pwm_wiringPi(Module* modules, int size) {
//     for(int i = 0; i < size; i++) {
//         printf("Stop: %s | with %d\n", modules[i].tag, size);
//         softPwmStop(modules[i].wiringPi);
//     }
//     sem_destroy(&mutex_output);
//     sem_destroy(&mutex_input);
// }

// void turn_pin(int pin, int on) {
//     softPwmWrite(pin, on);
// }

// void handle_sensors(int pin, Module* modules, int size) {
//     Module* module;
//     printf("Window: %d\n", pin);

//     for (int i = 0; i < size; i++) {
//         if (modules[i].gpio == pin) {
//             module = &modules[i];
//             break;
//         }
//     }

//     module->value = !module->value;

//     printf("Window Value (%s): %d\n", module->tag, module->value);
// }

// void in(int pin, Module* modules, int size) {
//     Module* module;
//     printf("Window: %d\n", pin);

//     for (int i = 0; i < size; i++) {
//         if (modules[i].gpio == pin) {
//             module = &modules[i];
//             break;
//         }
//     }

//     module->value += module->value;

//     printf("Window Value (%s): %d\n", module->tag, module->value);
// }

// void out(int pin, Module* modules, int size) {
//     Module* module;
//     printf("Window: %d\n", pin);

//     for (int i = 0; i < size; i++) {
//         if (modules[i].gpio == pin) {
//             module = &modules[i];
//             break;
//         }
//     }

//     module->value -= module->value;

//     printf("Window Value (%s): %d\n", module->tag, module->value);
// }
