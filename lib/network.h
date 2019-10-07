#ifndef NETWORK_H
#define NETWORK_H

#define PORT 2288   // default port for raspberry pis communication

int server_fd;      // server file descriptor
                    // used so listener is initialized once

int intialize_listener();
int send_message(const char* message, uint32 pi);
int receive_message(char* buffer, int nbytes);

#endif