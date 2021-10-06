#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <ncurses.h>

#include "../inc/server.h"
#include "../inc/display.h"

pthread_t pServerConnect;

WINDOW *create_newwin(int height, int width, int starty, int startx);

void handle_sigint(int sig) {
    pthread_cancel(pServerConnect);
    pthread_join(pServerConnect, NULL);
    destroy_server();
    endwin();
    exit(0);
}

void* init_server_connect(void* p) {
    int server = create_server(5566);
    listen_client(server);
}

int main() {
    signal(SIGINT, handle_sigint);

    display();

    pthread_create(&pServerConnect, NULL, init_server_connect, NULL);

    wait_command();

    return 0;
}
