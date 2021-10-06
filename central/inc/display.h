#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <string.h>
#include <ncurses.h>

void display();
void clear_commands();
void clear_sensors();
void reset_i_sensors();
void reset_info();
void wait_command();
void write_in_sensors(char *message, int value);
void write_in_sensors_float(char *message, float value);
void write_in_info(char *number, char *message);
void write_in_menu(int number, char *message);
void write_in_logs(char *message);
void close_curses();

#endif
