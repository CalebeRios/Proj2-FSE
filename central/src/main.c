#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "../inc/server.h"
#include "../inc/display.h"
#include "../inc/csv.h"

pthread_t pServerConnect, pWaitCommand;

void handle_sigint(int sig) {
    pthread_cancel(pServerConnect);
    pthread_cancel(pWaitCommand);
    destroy_server();
    endwin();
    close_csv();
    exit(0);
}

void* init_server_connect(void* p) {
    int server = create_server(5566);
    listen_client(server);
}

void* command(void* p) {
    wait_command();
}

int main() {
    signal(SIGINT, handle_sigint);

    display();

    init_csv();

    pthread_create(&pServerConnect, NULL, init_server_connect, NULL);
    pthread_create(&pWaitCommand, NULL, command, NULL);

    for(;;) {
        sleep(1);
    }

    return 0;
}
