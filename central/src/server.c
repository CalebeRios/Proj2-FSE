#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

pthread_t pServerListen[5];

char *ip = "192.168.0.53";
// char *ip = "127.0.0.1";
int n;
int client_socks[5];
int clients = 0;
int server_sock;

void listen_message(int sock);
void sent_message(int sock, char *message);

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
        recv(sock, buffer, sizeof(buffer), 0);
        printf("Client with sock %d: %s\n", sock, buffer);

        if (strstr(buffer, "Close Connection")) {
            connection_established = 0;
            close(sock);
        } else {
            sent_message(sock, "Hello, this is server");
        }
    }
}

void sent_message(int sock, char *message) {
    char buffer[1024];

    bzero(buffer, 1024);
    strcpy(buffer, message);
    printf("Server: %s\n", buffer);

    send(sock, buffer, strlen(buffer), 0);
}
