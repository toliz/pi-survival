#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#include <stdint.h>

extern record *records;
extern uint32 map[10000]; // Map table for AEMs
extern const uint32 myPI;
extern const int N_PI, N_MESSAGES;

uint32 n_digits(uint64);
int message_to_string(const message, char* buf);
bool string_to_message(const char*, message *m);

void write_message(message m, uint32 pi);
record* read_message(uint32 pi);

#endif