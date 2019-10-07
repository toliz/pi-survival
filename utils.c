#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "utils.h"


/* Converts messages to strings
 *
 * Parameters:
 *  - m : the message
 *  - string: the string to be written
 * 
 * returns: TRUE upon success, FALSE otherwise
 */
bool message_to_string(const message m, char* string) {
    return sprintf(string, "%lu_%lu_%llu_%s", m.sAEM, m.rAEM, m.timestamp, m.text) > 0;
}


/* Converts strings to messages
 * 
 * Parameters:
 *  - _string : the string containing the message
 *  - m: the message to be written
 * 
 * returns: TRUE upon success, FALSE otherwise
 */
bool string_to_message(const char* _string, message* m) {
    char buffer[512], *ptr, *string = buffer;
    
    strcpy(buffer, _string);

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
    }

    if (m->sAEM == 0 || m->rAEM == 0 || m->timestamp == 0)
        return FALSE;
    else
        return TRUE;
}


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
 *  - pi : the AEM of the  pi that sent this message or myPI for message generation
 * 
 * returns the idx of the written message int the records or -1 for duplicate message
 */
int write_message(message m, uint32 pi) {
    int idx;
    static int curr_idx = 0;

    // Check for duplicates
    for (uint32 idx = 0; idx < N_MESSAGES; idx++) {
        if (records[idx].message.sAEM == m.sAEM &&
            records[idx].message.rAEM == m.rAEM &&
            records[idx].message.timestamp == m.timestamp &&
            strcmp(records[idx].message.text, m.text) == 0) {
                return -1;
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
    idx = curr_idx;
    curr_idx = (curr_idx + 1) % N_MESSAGES;

    return idx;
}


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
 *  - m: pointer to the message to be written
 *
 *  returns: the idx of the read message int the records or -1 for duplicate message
 */
int read_message(uint32 pi, message *m) {
    static int curr_idx = 0;

    // Scan the records ONCE for the given AEM
    while (TRUE) {
        if (curr_idx == N_MESSAGES) {
            curr_idx = 0;
            return -1;
        }

        if (records[curr_idx].recipients[map[pi]] == FALSE &&
            records[curr_idx].message.sAEM != 0) {
                m = &records[curr_idx].message;
                return curr_idx++;
            }

        curr_idx++;
    }
}
