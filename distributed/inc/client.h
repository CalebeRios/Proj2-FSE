#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
    int sock;
    struct sockaddr_in addr;
} Client;

void destroy_client();
Client create_client(int port);
void connect_server();
void listen_message();
void sent_message(char *message);

#endif
