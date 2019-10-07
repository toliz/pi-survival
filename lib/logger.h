#ifndef LOGGER_H
#define LOGGER_H

#include "types.h"

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define ANSI_BOLD   "\e[1m"
#define ANSI_NORMAL "\e[0m"

enum {ERROR = -1, WARNING = 0, INFO = 1};

char *logfile;

void init_logger(const char* _logfile);
void log(int status, char *message, ...);

#endif