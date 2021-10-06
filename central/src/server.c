#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include "../inc/global.h"
#include "../inc/display.h"
#include "../inc/sensors.h"
#include "../inc/csv.h"

sem_t mutex;
pthread_t pServerListen[5], pParser[10];

char *ip = "192.168.0.53";
int n;
int client_sock;
int clients = 0;
int i_parser = 0;
int server_sock;

void listen_message(int sock);
void sent_message(int sock, char *message);
void* parser_message(void *p);

void destroy_server() {
    for(int i = 0; i < clients; i++) {
        pthread_cancel(pServerListen[i]);
        pthread_join(pServerListen[i], NULL);
    }
    sent_message(client_sock, "Close Connection");
    close(client_sock);
    close(server_sock);
}

void* init_server_listen(void* p) {
    int sock = *((int *) p);
    listen_message(sock);
}

int create_server(int port) {
    struct sockaddr_in server_addr;

    config_mutex();
    sem_init(&mutex, 0, 1);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        write_in_logs("[-] Socket error");
        exit(1);
    }
    write_in_logs("[+] TCP server socket created\n");

    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        printf("[-] Bind error");
        close_curses();
        exit(1);
    }

    char buf[30];

    snprintf(buf, 30, "[+] Bind to port number: %d", port);
    write_in_logs(buf);

    listen(server_sock, 5);
    write_in_logs("Listening...");

    return server_sock;
}

void listen_client(int server_sock) {
    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);

        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        write_in_logs("[+] Client connected\n");

        pthread_create(&pServerListen[clients], NULL, init_server_listen, (void *) &client_sock);

        clients++;
    }
}

void listen_message(int sock) {
    char buffer[1024];
    int connection_established = 1;

    while(connection_established) {
        bzero(buffer, 1024);
        recv(sock, buffer, sizeof(buffer), 0);

        if (strstr(buffer, "Close Connection")) connection_established = 0;

        char *message = malloc(1024 * sizeof(char));

        strcpy(message, buffer);

        if (i_parser == 9) {
            i_parser = 0;
        }

	    pthread_create(&pParser[i_parser], NULL, parser_message, (void *)message);
        i_parser++;
    }

    write_in_logs("[-] Connection closed");

    close(sock);
}

void sent_message(int sock, char *message) {
    char buffer[1024];

    bzero(buffer, 1024);
    strcpy(buffer, message);

    send(sock, buffer, strlen(buffer), 0);
}

void log(char *message, int value) {
    char log[1024];
    char *num;
    bzero(log, 1024);

    strcat(log, message);
    strcat(log, " ");
    asprintf(&num, "%d", value);
    strcat(log, num);

    write_in_logs(log);
}

void log_float(char *message, int value) {
    char log[1024];
    char *num;
    bzero(log, 1024);

    strcat(log, message);
    strcat(log, " ");
    asprintf(&num, "%d", value);
    strcat(log, num);

    write_in_logs(log);
}

void* parser_message(void *p) {
    sem_wait(&mutex);
    char *message = (char *) p;

    if (strstr(message, "UpdatePeople")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");

            i++;
        }

        int a = atoi(mat[1]);

        if (a > 0 || (a < 0 && qtd_people > 0) ) {
            qtd_people += a;
        }

        log("Quantidade de pessoas no predio", qtd_people);

        clear_sensors();

        update_all_inputs(&terreo);
        update_all_inputs(&firstFloor);
        update_temp_umi();
        update_people();
    } else if (strstr(message, "UpdateSensor")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");

            i++;
        }

        for (int i = 0; i < terreo.inputSize; i++) {
            if (terreo.inputs[i].wiringPi == atoi(mat[1])) {
                terreo.inputs[i].value = atoi(mat[2]);

                log(terreo.inputs[i].tag, terreo.inputs[i].value);

                write_csv(terreo.inputs[i].tag, terreo.inputs[i].wiringPi, terreo.inputs[i].value);
                break;
            }
        }

        for (int i = 0; i < firstFloor.inputSize; i++) {
            if (firstFloor.inputs[i].wiringPi == atoi(mat[1])) {
                firstFloor.inputs[i].value = atoi(mat[2]);

                log(firstFloor.inputs[i].tag, firstFloor.inputs[i].value);

                write_csv(firstFloor.inputs[i].tag, firstFloor.inputs[i].wiringPi, firstFloor.inputs[i].value);
                break;
            }
        }

        clear_sensors();

        update_all_inputs(&terreo);
        update_all_inputs(&firstFloor);
        update_temp_umi();
        update_people();
    } else if (strstr(message, "UpdateCommand")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");

            i++;
        }

        for (int i = 0; i < terreo.outputSize; i++) {
            if (terreo.outputs[i].wiringPi == atoi(mat[1])) {
                terreo.outputs[i].value = atoi(mat[2]);

                log(terreo.outputs[i].tag, terreo.outputs[i].value);

                write_csv(terreo.outputs[i].tag, terreo.outputs[i].wiringPi, terreo.outputs[i].value);
                break;
            }
        }

        for (int i = 0; i < firstFloor.outputSize; i++) {
            if (firstFloor.outputs[i].wiringPi == atoi(mat[1])) {
                firstFloor.outputs[i].value = atoi(mat[2]);

                log(firstFloor.outputs[i].tag, firstFloor.outputs[i].value);

                write_csv(firstFloor.outputs[i].tag, firstFloor.outputs[i].wiringPi, firstFloor.outputs[i].value);
                break;
            }
        }

        clear_commands();

        update_all_outputs(&terreo);
        update_all_outputs(&firstFloor);
    } else if(strstr(message, "TempUm")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");

            i++;
        }

        temp = atof(mat[1]);
        umi = atof(mat[2]);

        log_float("Temperatura", temp);
        log_float("Umidade", umi);

        clear_sensors();

        update_all_inputs(&terreo);
        update_all_inputs(&firstFloor);
        update_temp_umi();
        update_people();
    } else if (strstr(message, "Config")) {
        char mat[100][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|\n");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|\n");

            i++;
        }

        i = 1;

        Configuration *config;

        if (strcmp(mat[i], "Térreo") == 0) {
            config = &terreo;
        } else {
            config = &firstFloor;
        }

        config->name = malloc(strlen(mat[i])+1);
        strcpy(config->name, mat[i]);

        i++;
        i++;

        config->outputSize = atoi(mat[i]);
        config->outputs = malloc(config->outputSize * sizeof(Module));

        for(int j = 0; j < config->outputSize; j++) {
            Module mod;

            i++;
            mod.tag = malloc(strlen(mat[i])+1);
            strcpy(mod.tag, mat[i]);
            i++;
            mod.wiringPi = atoi(mat[i]);
            i++;
            mod.value = atoi(mat[i]);

            config->outputs[j] = mod;
        }

        update_all_outputs(config);
        
        i++;
        i++;

        config->inputSize = atoi(mat[i]);
        config->inputs = malloc(config->inputSize * sizeof(Module));

        for(int j = 0; j < config->inputSize; j++) {
            Module mod;

            i++;
            mod.tag = malloc(strlen(mat[i])+1);
            strcpy(mod.tag, mat[i]);
            i++;
            mod.wiringPi = atoi(mat[i]);
            i++;
            mod.value = atoi(mat[i]);

            config->inputs[j] = mod;
        }

        update_all_inputs(config);
    }

    sem_post(&mutex);
}

void log_command(Module module) {
    char message[1024];
    bzero(message, 1024);
    
    if (!module.value) {
        strcat(message, "Você está tentando ligar o ");
    } else {
        strcat(message, "Você está tentando desligar o ");
    }

    strcat(message, module.tag);

    write_in_logs(message);
}

char *mount_command(Module module) {
    char command[1024];
    char *num;

    bzero(command, 1024);

    strcat(command, "Command|");
    asprintf(&num, "%d", module.wiringPi);
    strcat(command, num);
    strcat(command, "|");
    asprintf(&num, "%d", module.value);
    strcat(command, num);

    log_command(module);
    sent_message(client_sock, command);
    sleep(1);
}

void sent_command(int pin) {
    for (int i = 0; i < terreo.outputSize; i++) {
        if (terreo.outputs[i].wiringPi == pin) {
            mount_command(terreo.outputs[i]);
            return;
        }
    }

    for (int i = 0; i < firstFloor.outputSize; i++) {
        if (firstFloor.outputs[i].wiringPi == pin) {
            mount_command(firstFloor.outputs[i]);
            return;
        }
    }
}
