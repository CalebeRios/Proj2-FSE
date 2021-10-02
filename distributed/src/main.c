#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

// #include "../inc/wiring-pi.h"
// #include "../inc/jsonParser.h"
// #include "../inc/global.h"
// #include "../inc/dht22.h"
#include "../inc/client.h"

pthread_t pTerreo, pFirstFloor, pSensors, pClient;
// Configuration terreo, firstFloor;

void handle_sigint(int sig) {

    // pthread_cancel(pTerreo);
    // pthread_cancel(pFirstFloor);
    // pthread_cancel(pSensors);
    // pthread_join(pTerreo, NULL);
    // pthread_join(pFirstFloor, NULL);
    // pthread_join(pSensors, NULL);

    pthread_cancel(pClient);
    pthread_join(pClient, NULL);
    destroy_client();

    // stop_pwm_wiringPi(terreo.outputs, terreo.outputSize);
    // stop_pwm_wiringPi(firstFloor.outputs, firstFloor.outputSize);

    exit(0);
}

void* init_client(void* p) {
    Client client = create_client(5566);
    connect_server();
    sent_message("Hello, this is a client");
    listen_message();
}

// void* init_terreo(void* p) {
//     char *json = read_file("/home/calebemendes/2.proj/distributed/doc/configuracao_andar_terreo.json");
//     terreo = parse_json(json);

//     init_outputs(terreo.outputs, terreo.outputSize);
//     init_inputs(terreo.inputs, terreo.inputSize, "terreo");
// }

// void* init_first_floor(void* p) {
//     char *json = read_file("/home/calebemendes/2.proj/distributed/doc/configuracao_andar_1.json");
//     firstFloor = parse_json(json);

//     init_outputs(firstFloor.outputs, firstFloor.outputSize);
//     init_inputs(firstFloor.inputs, firstFloor.inputSize, "first_floor");
// }

// void* read_sensors(void* p) {
//     while(1) {
//         read_dht22_dat();
//         sleep(1);
//     }
// }

int main() {
    signal(SIGINT, handle_sigint);

    // init_pwm_wiringPi();

    // pthread_create(&pSensors, NULL, read_sensors, NULL);
    // pthread_create(&pTerreo, NULL, init_terreo, NULL);
    // pthread_create(&pFirstFloor, NULL, init_first_floor, NULL);
    pthread_create(&pClient, NULL, init_client, NULL);

    for(;;) {
        sleep(1);
    }

    return 0;
}
