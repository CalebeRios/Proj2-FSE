#include <semaphore.h>

#include "../inc/server.h"
#include "../inc/display.h"
#include "../inc/global.h"

sem_t mutex_output, mutex_input;

void config_mutex() {
    sem_init(&mutex_output, 0, 1);
    sem_init(&mutex_input, 0, 1);
}

void update_all_outputs(Configuration *config) {
    sem_wait(&mutex_output);

    for(int i = 0; i < config->outputSize; i++) {
        char message[1024];
        bzero(message, 1024);

        if (config->outputs[i].value == 0) {
            strcat(message, "Ligar ");
        } else {
            strcat(message, "Desligar ");
        }

        strcat(message, config->outputs[i].tag);
        write_in_menu(config->outputs[i].wiringPi, message);
    }

    sem_post(&mutex_output);
}

void update_all_inputs(Configuration *config) {
    sem_wait(&mutex_input);

    for(int i = 0; i < config->inputSize; i++) {
        write_in_sensors(config->inputs[i].tag, config->inputs[i].value);
    }

    sem_post(&mutex_input);
}

void update_temp_umi() {
    write_in_sensors_float("Temperatura", temp);
    write_in_sensors_float("Umidade", umi);
}

void update_people() {
    write_in_sensors("Quantidade de Pessoas", qtd_people);
}
