#include "../inc/client.h"

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
