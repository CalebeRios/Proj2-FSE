#include "../inc/display.h"
#include "../inc/server.h"

WINDOW *sensors_win, *info_win, *logs_win;
int i_sensors = 3, i_info = 4, i_logs = 3;
int startx, starty, width, height;

WINDOW *create_newwin(int height, int width, int starty, int startx, char *message);

void display() {
    initscr();

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set( false );

    height = LINES / 2;
    width = COLS / 2;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;

    refresh();

    sensors_win = create_newwin(height, width, 0, 0, "Informações dos sensores");
    info_win = create_newwin(height, width, 0, (COLS / 2), "Comandos");
    logs_win = create_newwin(height, COLS, LINES / 2, 0, "Logs");

    mvwprintw(info_win, 3, 1, "ctrl+c - Sair");
}

void clear_commands() {
    wclear(info_win);

    i_info = 4;

    mvwprintw(info_win, 1, 1, "Comandos");
    mvwprintw(info_win, 2, 1, "--------------------------");

    mvwprintw(info_win, 3, 1, "ctrl+c - Sair");

    wborder(info_win, '|', '|', '-', '-', '+', '+', '+', '+');

    wrefresh(info_win);
}

void clear_sensors() {
    wclear(sensors_win);

    i_sensors = 3;

    mvwprintw(sensors_win, 1, 1, "Informações dos sensores");
    mvwprintw(sensors_win, 2, 1, "--------------------------");

    wborder(sensors_win, '|', '|', '-', '-', '+', '+', '+', '+');

    wrefresh(sensors_win);
}

void reset_i_sensors() {
}

void reset_info() {
    i_info = 3;
}

void wait_command() {
    char command[10];
    
    while(1) {
        getstr(command);

        if (strstr(command, "7") || strstr(command, "0")
                || strstr(command, "2") || strstr(command, "11")
                || strstr(command, "27") || strstr(command, "3")
                || strstr(command, "6") || strstr(command, "10")
                || strstr(command, "26")) {
            sent_command(atoi(command));
        } else {
            write_in_logs("Comando inválido, tente outro.");
        }
    }
}

void write_in_sensors(char *message, int value) {
    mvwprintw(sensors_win, i_sensors, 1, "%s = %d", message, value);
    wrefresh(sensors_win);
    i_sensors++;
}

void write_in_sensors_float(char *message, float value) {
    mvwprintw(sensors_win, i_sensors, 1, "%s = %.1f", message, value);
    wrefresh(sensors_win);
    i_sensors++;
}

void write_in_info(char *number, char *message) {
    mvwprintw(info_win, i_info, 1, "%s - %s", number, message);
    wrefresh(info_win);
    i_info++;
}

void write_in_menu(int number, char *message) {
    mvwprintw(info_win, i_info, 1, "%d - %s", number, message);
    wrefresh(info_win);
    i_info++;
}

void write_in_logs(char *message) {
    if (i_logs == (LINES / 2) - 1) {
        i_logs = 3;
    }

    if (i_logs != (LINES / 2) - 2) mvwprintw(logs_win, i_logs + 1, 1, "                                                                           ");

    mvwprintw(logs_win, i_logs, 1, "                                                                           ");
    mvwprintw(logs_win, i_logs, 1, message);
    wrefresh(logs_win);
    i_logs++;
}

WINDOW *create_newwin(int height, int width, int starty, int startx, char *message) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);

    wclear(local_win);
    mvwprintw(local_win, 1, 1, message);
    mvwprintw(local_win, 2, 1, "--------------------------");

    wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');

    wrefresh(local_win);

    return local_win;
}

void close_curses() {
    endwin();
}
