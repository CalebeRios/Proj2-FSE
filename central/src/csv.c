#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

FILE *fp;

void init_csv() {
    fp = fopen("log.csv", "w+");
    fprintf(fp, "Timestamp, TAG, WiringPi, Value\n");
}

void write_csv(char *tag, int wiringPi, int value) {
    int hours, minutes, seconds, day, month, year;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;
    minutes = local->tm_min;
    seconds = local->tm_sec;

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    fprintf(fp,"%02d/%02d/%d %02d:%02d:%02d,", day, month, year, hours, minutes, seconds);
    fprintf(fp,"%s,%d,%d\n", tag, wiringPi, value);
}

void close_csv() {
    fclose(fp);
}
