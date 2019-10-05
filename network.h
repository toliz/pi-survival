#ifndef NETWORK_H
#define NETWORK_H

#define PORT 2288   // default port for raspberry pis communication

int server_fd;      // server file descriptor
                    // used so listener is initialized once

uint32 intialize_listener();
uint32 send_message(const char* message, uint32 pi);
uint32 receive_message(char* buffer, uint32 nbytes);

#endif