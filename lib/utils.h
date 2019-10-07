#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#include <stdint.h>

extern record *records;
extern uint32 map[10000]; // Map table for AEMs
extern const uint32 myPI;
extern const int N_PI, N_MESSAGES;

bool message_to_string(const message m, char* string);
bool string_to_message(const char* string, message *m);

int write_message(message m, uint32 pi);
int read_message(uint32 pi, message *m);

#endif