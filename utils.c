#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "utils.h"


uint32 n_digits(uint64 n){
/* Counts the number of digits of an integer
 *
 * Parameters:
 *  - n : the integer
 * 
 * returns: the number of digits of n
 */
    uint32 ndigits = 1;

    while (n >= 10) {
        n = (n - n%10) / 10;
        ndigits++;
    }

    return ndigits;
}

int message_to_string(const message m, char* buf) {
/* Converts messages to strings
 *
 * Parameters:
 *  - m : the message
 * 
 * returns: a string representing the message
 */
    // char* string = (char*) malloc((n_digits(m.sAEM) + 
    //                                n_digits(m.rAEM) + 
    //                                n_digits(m.timestamp) + 
    //                                strlen(m.text) + 1) * sizeof(char));

    return sprintf(buf, "%lu_%lu_%llu_%s", m.sAEM, m.rAEM, m.timestamp, m.text);
}

bool string_to_message(const char* _string, message* m) {
/* Converts strings to messages
 * 
 * Parameters:
 *  - _string : the string containing the message
 * 
 * returns: a pointer to the message variable or a NULL pointer if the _string
 *          hasn't a proper format
 */
    char *ptr, buffer[512], *string = buffer;
    
    strcpy(buffer, _string);
    string[strlen(_string)] = '\0';

    if ((ptr = strstr(string, "_")) == NULL) return FALSE;
    *ptr = '\0';
    m->sAEM = atoi(string);
    string = ++ptr;

    if ((ptr = strstr(string, "_")) == NULL) return FALSE;
    *ptr = '\0';
    m->rAEM = atoi(string);
    string = ++ptr;

    if ((ptr = strstr(string, "_")) == NULL) return FALSE;
    *ptr = '\0';
    m->timestamp = atoi(string);
    string = ++ptr;

    if (strlen(string) > 256) {
        strncpy(m->text, string, 255);
        m->text[255] = '\0';
    } else {
        strcpy(m->text, string);
        m->text[strlen(string)] = '\0';
    }

    if (m->sAEM == 0 || m->rAEM == 0 || m->timestamp == 0)
        return FALSE;
    else
        return TRUE;
}

void write_message(message m, uint32 pi) {
/* Updates records with a new message m.
 *
 * First it checks for duplicate messages already in the records, and if not
 * present it writes the message m to the records, assigning as recipient only
 * the device that has sent the message.
 * 
 * If records is full it overwrites the last received message.
 *
 * Parameters:
 *  - m : the message to be inserted
 */
    static uint32 curr_idx = 0;

    // Check for duplicates
    for (uint32 idx = 0; idx < N_MESSAGES; idx++) {
        if (records[idx].message.sAEM == m.sAEM &&
            records[idx].message.rAEM == m.rAEM &&
            records[idx].message.timestamp == m.timestamp &&
            strcmp(records[idx].message.text, m.text) == 0) {
                return;
            }
    }

    // (Over)write message and refresh recipients records
    records[curr_idx].message = m;
    for (int i = 0; i < N_PI; i++)
        records[curr_idx].recipients[i] = FALSE;

    // Creator and sender of the message have already seen it
    if (pi != myPI)
        records[curr_idx].recipients[map[pi]] = TRUE;
    if (m.sAEM != myPI)
        records[curr_idx].recipients[map[m.sAEM]] = TRUE;

    // Message for me are not retransmitted
    if (m.rAEM == myPI) {
        for (int i = 0; i < N_PI; i++)
            records[curr_idx].recipients[i] = TRUE;
    }
   
    // Move current index
    curr_idx = (curr_idx + 1) % N_MESSAGES;
}

record* read_message(uint32 pi) {
/* This function yields the messages that need to be sent to pi with a specific
 * AEM.
 *
 * Yielding is custom and should be used as while (c = read() !=  NULL) {...}
 *
 * This function scans the records and searches for messages that has not
 * already been marked as sent to pi with the mentioned AEM.
 *
 * Parameters:
 *  - pi: the AEM of the pi
 *
 *  returns: a pointer to the record to be sent or NULL if there is no message
 *           to send.
 */
    static uint32 curr_idx = 0;

    // Scan the records ONCE for the given AEM
    while (TRUE) {
        if (curr_idx == N_MESSAGES) {
            curr_idx = 0;
            return NULL;
        }

        if (records[curr_idx].recipients[map[pi]] == FALSE &&
            records[curr_idx].message.sAEM != 0) {
                return &records[curr_idx++];
            }

        curr_idx++;
    }
}