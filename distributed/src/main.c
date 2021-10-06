#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#include "../inc/wiring-pi.h"
#include "../inc/jsonParser.h"
#include "../inc/global.h"
#include "../inc/dht22.h"
#include "../inc/client.h"

pthread_t pDistributed, pSensors, pClient;
Configuration terreo, firstFloor;

void handle_sigint(int sig) {
    pthread_cancel(pDistributed);
    pthread_cancel(pSensors);
    pthread_cancel(pClient);

    destroy_client();

    stop_pwm_wiringPi(terreo.outputs, terreo.outputSize);
    stop_pwm_wiringPi(firstFloor.outputs, firstFloor.outputSize);

    exit(0);
}

void* init_client(void* p) {
    listen_message();
}

void* init_distributed(void* p) {
    char *json = read_file("/home/calebemendes/2.proj/distributed/doc/configuracao_andar_1.json");
    firstFloor = parse_json(json);

    json = read_file("/home/calebemendes/2.proj/distributed/doc/configuracao_andar_terreo.json");
    terreo = parse_json(json);

    init_outputs(terreo.outputs, terreo.outputSize);
    init_inputs(terreo.inputs, terreo.inputSize, "terreo");

    init_outputs(firstFloor.outputs, firstFloor.outputSize);
    init_inputs(firstFloor.inputs, firstFloor.inputSize, "first_floor");

    sent_configuration(terreo);
    sent_configuration(firstFloor);
}

void* read_sensors(void* p) {
    while(1) {
        read_dht22_dat();
        sent_temp_hum(get_hum(), get_temp());
        sleep(1);
    }
}

int main() {
    signal(SIGINT, handle_sigint);

    init_pwm_wiringPi();

    Client client = create_client(5566);
    connect_server();

    pthread_create(&pSensors, NULL, read_sensors, NULL);
    pthread_create(&pClient, NULL, init_client, NULL);
    pthread_create(&pDistributed, NULL, init_distributed, NULL);

    for(;;) {
        sleep(1);
    }

    return 0;
}
