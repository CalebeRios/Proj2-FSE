#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#include "../inc/client.h"
#include "../inc/wiring-pi.h"

sem_t mutex;
pthread_t pParser;
char *ip = "192.168.0.53";

int sock;
struct sockaddr_in addr;
int connection_established = 1;

void* parser_message(void *p);

void destroy_client() {
    sent_message("Close Connection");
    connection_established = 0;
    close(sock);
}

Client create_client(int port) {
    Client client = { 0 };

    sem_init(&mutex, 0, 1);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("[-] Socket error");
        exit(1);
    }
    printf("[+] TCP server socket created\n");

    memset(&addr, '\0', sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    return client;
}

void connect_server() {
    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("[+] Connected to the server\n");
}

void listen_message() {
    char buffer[1024];

    while(connection_established) {
        bzero(buffer, 1024);
        recv(sock, buffer, sizeof(buffer), 0);

        if (strstr(buffer, "Close Connection")) connection_established = 0;

        char *message = malloc(1024 * sizeof(char));

        strcpy(message, buffer);

	    pthread_create(&pParser, NULL, parser_message, (void *)message);
    }

    printf("[-] Connection closed");

    close(sock);
}

void sent_message(char *message) {
    sem_wait(&mutex);
    char buffer[1024];

    bzero(buffer, 1024);
    strcpy(buffer, message);
    printf("Client: %s\n", buffer);

    send(sock, buffer, strlen(buffer), 0);

    sem_post(&mutex);

    sleep(1);
}

void sent_configuration(Configuration config) {
    char message[1024];
    char *num;

    bzero(message, 1024);

    strcat(message, "Config|");
    strcat(message, config.name);
    strcat(message, "\nOutputs |");
    asprintf(&num, "%d", config.outputSize);
    strcat(message, num);

    for(int i  = 0; i < config.outputSize; i++) {
        strcat(message, "\n");
        strcat(message, config.outputs[i].tag);
        strcat(message, "|");
        asprintf(&num, "%d", config.outputs[i].wiringPi);
        strcat(message, num);
        strcat(message, "|");
        asprintf(&num, "%d", config.outputs[i].value);
        strcat(message, num);
    }

    strcat(message, "\nInputs|");
    asprintf(&num, "%d", config.inputSize);
    strcat(message, num);

    for(int i  = 0; i < config.inputSize; i++) {
        strcat(message, "\n");
        strcat(message, config.inputs[i].tag);
        strcat(message, "|");
        asprintf(&num, "%d", config.inputs[i].wiringPi);
        strcat(message, num);
        strcat(message, "|");
        asprintf(&num, "%d", config.inputs[i].value);
        strcat(message, num);
    }

    sent_message(message);
}

void sent_temp_hum(float hum, float temp) {
    char message[1024];
    char num[10];

    bzero(message, 1024);

    strcat(message, "TempUm |");
    sprintf(num, "%.1f", temp);
    strcat(message, num);
    strcat(message, "|");
    sprintf(num, "%.1f", hum);
    strcat(message, num);

    sent_message(message);
}

void sent_update_sensor_people(int value) {
    char message[1024];
    char *num;

    bzero(message, 1024);

    strcat(message, "UpdatePeople |");
    asprintf(&num, "%d", value);
    strcat(message, num);

    sent_message(message);
}

void sent_update_sensor_in(int pin, int value) {
    char message[1024];
    char *num;

    bzero(message, 1024);

    strcat(message, "UpdateSensor |");
    asprintf(&num, "%d", pin);
    strcat(message, num);
    strcat(message, "|");
    asprintf(&num, "%d", value);
    strcat(message, num);

    sent_message(message);
}

void sent_update_sensor_out(int pin, int value) {
    char message[1024];
    char *num;

    bzero(message, 1024);

    strcat(message, "UpdateCommand |");
    asprintf(&num, "%d", pin);
    strcat(message, num);
    strcat(message, "|");
    asprintf(&num, "%d", value);
    strcat(message, num);

    sent_message(message);
}

void* parser_message(void *p) {
    char *message = (char *) p;

    if (strstr(message, "Command")) {
        char mat[10][50];
        char *line;
        int i = 0;

        line = strtok(strdup(message), "|");

        while (line) {
            strcpy(mat[i], line);
            line = strtok(NULL, "|");

            i++;
        }

        printf("%s: %d %d\n", message, atoi(mat[1]), !atoi(mat[2]));

        turn_pin(atoi(mat[1]), !atoi(mat[2]));

        sent_update_sensor_out(atoi(mat[1]), !atoi(mat[2]));
    }
}
