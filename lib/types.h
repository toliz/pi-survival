#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t uint32;
typedef uint64_t uint64;
typedef enum {FALSE = 0, TRUE = 1} bool;

typedef struct message {
    uint32 sAEM;
    uint32 rAEM;
    uint64 timestamp;
    char text[256];
} message;

typedef struct record {
    message message;
    bool *recipients;
} record;

#endif