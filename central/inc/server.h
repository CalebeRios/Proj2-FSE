#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
    int server_sock;
    struct sockaddr_in client_addr;
} Server;

void destroy_server();
int create_server(int port);
void listen_client(int server_sock);
void sent_message(int sock, char *message);
void sent_command(int pin);

#endif
