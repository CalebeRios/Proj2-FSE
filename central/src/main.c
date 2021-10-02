#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "../inc/server.h"

pthread_t pServerConnect;

void handle_sigint(int sig) {
    pthread_cancel(pServerConnect);
    pthread_join(pServerConnect, NULL);
    destroy_server();
    exit(0);
}

void* init_server_connect(void* p) {
    int server = create_server(5566);
    listen_client(server);
}

int main() {
    signal(SIGINT, handle_sigint);

    pthread_create(&pServerConnect, NULL, init_server_connect, NULL);

    for(;;) {
        sleep(1);
    }

    return 0;
}
