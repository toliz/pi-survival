#include "logger.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void init_logger(const char* _logfile) {
    FILE* fp;
    logfile = (char *) malloc(strlen(_logfile) * sizeof(char));

    strcpy(logfile, _logfile);
    
    if ((fp = fopen(logfile, "w")) == NULL) {
        printf("Logger initialization failed\nQuitting...\n");
        exit(EXIT_FAILURE);
    } else {
        fclose(fp);
    }
}

void log(int status, char *msg, ...) {
	FILE* fp;
	if ((fp = fopen(logfile, "a")) == NULL)
        return;

    /* Get time in human readable format */
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    /* Log */
    fprintf(fp, "[%s] ", buffer);
    switch (status) {
        case ERROR:
            printf(ANSI_COLOR_RED ANSI_BOLD"[ERROR]\t");
            fprintf(fp, "[ERROR]\t");
            break;
        case WARNING:
            printf(ANSI_BOLD ANSI_BOLD"[WARNING]\t");
            fprintf(fp, "[WARNING]\t");
            break;
        case INFO:
            printf("[INFO]\t");
            fprintf(fp, "[INFO]\t");
            break;
    }
    va_list args;
	va_start(args, msg);
    vprintf(msg, args);
    printf(ANSI_COLOR_RESET);
    va_end(args);

    va_start(args, msg);
    vfprintf(fp, msg, args);
    va_end(args);

    /* Exit */
    fclose(fp);
}
