#include <semaphore.h>

#include "../inc/client.h"

sem_t mutex;

// char *ip = "127.0.0.1";
char *ip = "192.168.0.53";

int sock;
struct sockaddr_in addr;
int connection_established = 1;

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
        printf("Server: %s\n", buffer);

        if (strstr(buffer, "Close Connection")) {
            connection_established = 0;
            close(sock);
        }
    }
}

void sent_message(char *message) {
    char buffer[1024];

    bzero(buffer, 1024);
    strcpy(buffer, message);
    printf("Client: %s\n", buffer);

    send(sock, buffer, strlen(buffer), 0);
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

    strcat(message, "Sensor|");
    sprintf(num, "%.1f", temp);
    strcat(message, num);
    strcat(message, "|");
    sprintf(num, "%.1f", hum);
    strcat(message, num);

    sent_message(message);
}

void sent_update_sensor(Module module) {
    char message[1024];
    char *num;

    bzero(message, 1024);

    strcat(message, "Update|");
    strcat(message, module.tag);
    strcat(message, "|");
    asprintf(&num, "%d", module.value);
    strcat(message, num);

    sent_message(message);
}
