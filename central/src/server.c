#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../inc/global.h"

pthread_t pServerListen[5], pParser;

char *ip = "192.168.0.53";
// char *ip = "127.0.0.1";
int n;
int client_socks[5];
int clients = 0;
int server_sock;

void listen_message(int sock);
void sent_message(int sock, char *message);
void* parser_message(void *p);

void destroy_server() {
    for(int i = 0; i < clients; i++) {
        sent_message(client_socks[i], "Close Connection");
        pthread_cancel(pServerListen[i]);
        pthread_join(pServerListen[i], NULL);
        close(client_socks[i]);
    }
    close(server_sock);
}

void* init_server_listen(void* p) {
    int sock = *((int *) p);
    listen_message(sock);
}

int create_server(int port) {
    struct sockaddr_in server_addr;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        perror("[-] Socket error");
        exit(1);
    }
    printf("[+] TCP server socket created\n");

    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-] Bind error");
        exit(1);
    }
    printf("[+] Bind to port number: %d\n", port);

    listen(server_sock, 5);
    printf("Listening...\n");

    return server_sock;
}

void listen_client(int server_sock) {
    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);

        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        printf("[+] Client connected\n");
        
        client_socks[clients] = client_sock;

        pthread_create(&pServerListen[clients], NULL, init_server_listen, (void *) &client_sock);

        clients++;
    }
}

void listen_message(int sock) {
    char buffer[1024];
    int connection_established = 1;

    while(connection_established) {
        bzero(buffer, 1024);
        int m = recv(sock, buffer, sizeof(buffer), 0);
        // printf("Client with sock %d (%d): %s\n", sock, m, buffer);

        if (strstr(buffer, "Close Connection")) connection_established = 0;

        char message[1024];

        strcpy(message, buffer);

	    pthread_create(&pParser, NULL, parser_message, (void *)message);
    }

    close(sock);
}

void sent_message(int sock, char *message) {
    char buffer[1024];

    bzero(buffer, 1024);
    strcpy(buffer, message);
    printf("Server: %s\n", buffer);

    send(sock, buffer, strlen(buffer), 0);
}

int get_index(char *message) {
    char *e;
    int index;

    e = strchr(message, '|');
    return (int)(e - message);
}

void* parser_message(void *p) {
    char *message = (char *) p;

    if (strstr(message, "Update")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");
            // printf("Token %d: %s\n", i, mat[i]);

            i++;
        }

        printf("Tag: %s\n", mat[1]);
        printf("Value: %d\n", atoi(mat[2]));
    } else if(strstr(message, "Sensor")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");
            // printf("Token %d: %s\n", i, mat[i]);

            i++;
        }

        printf("Temp: %.1f%%\n", atof(mat[1]));
        printf("Hum: %.1f%%\n", atof(mat[2]));
    } else if (strstr(message, "Config")) {
        char mat[100][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|\n");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|\n");
            // printf("Token %d: %s\n", i, mat[i]);

            i++;
        }

        i = 1;

        Configuration *config;

        if (strcmp(mat[i], "Térreo") == 0) {
            config = &terreo;
        } else {
            config = &firstFloor;
        }

        config->name = mat[i];

        i++;
        i++;

        config->outputSize = atoi(mat[i]);
        config->outputs = malloc(config->outputSize * sizeof(Module));
    

        int counter = 0;

        for(int j = 0; j < config->outputSize; j++) {
            Module mod;

            i++;
            mod.tag = mat[i];
            i++;
            mod.wiringPi = atoi(mat[i]);
            i++;
            mod.value = atoi(mat[i]);

            config->outputs[counter] = mod;
            counter++;
        }
        
        i++;
        i++;

        config->inputSize = atoi(mat[i]);
        config->inputs = malloc(config->inputSize * sizeof(Module));

        counter = 0;

        for(int j = 0; j < config->inputSize; j++) {
            Module mod;

            i++;
            mod.tag = mat[i];
            i++;
            mod.wiringPi = atoi(mat[i]);
            i++;
            mod.value = atoi(mat[i]);

            config->inputs[counter] = mod;
            counter++;
        }

        printf("%s: O-%d I-%d\n", config->name, config->outputSize, config->inputSize);
        printf("%s: T-%s W-%d V-%d\n", config->name, config->outputs[0].tag, config->outputs[0].wiringPi, config->outputs[0].value);
        printf("%s: T-%s W-%d V-%d\n", config->name, config->inputs[0].tag, config->inputs[0].wiringPi, config->inputs[0].value);
    }
}
