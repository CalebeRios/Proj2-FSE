#ifndef WIRING_PI_H_
#define WIRING_PI_H_

#include "global.h"

void init_pwm_wiringPi();
void init_outputs(Module* modules, int size);
void init_inputs(Module* modules, int size, char* floor);
void stop_pwm_wiringPi(Module* modules, int size);
void turn_pin(int pin, int on);

#endif
