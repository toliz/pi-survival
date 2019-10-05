#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <netinet/in.h> 

#include "types.h"
#include "logger.h"
#include "network.h"


int intialize_listener() {
/* Initializes the listener.
 *
 * returns 1 upon successful initialization and 0 with the proper message otherwise
 */
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log(ERROR, "Listener socket initialization failed.\n");
        exit(EXIT_FAILURE);
    }
           
    // Forcefully attaching socket to PORT
    struct sockaddr_in server_addr = {.sin_family = AF_INET,
                                      .sin_addr.s_addr = htonl(INADDR_ANY),
                                      .sin_port = htons(PORT)};
    socklen_t addrlen = sizeof(server_addr);
    if (bind(server_fd, (struct sockaddr *) &server_addr, addrlen) < 0) {
        log(ERROR, "server_fd bind failed\n");
        close(server_fd);
        return 0;
    }

    // Server is ready to listen
    if (listen(server_fd, 2000) < 0) {
        log(ERROR, "Listen failed\n");
        close(server_fd);
        return 0;
    }

    log(INFO, "Listener initialized successfully\n");
    return 1;
}


int send_message(const char *msg, uint32 pi) {
/* Sends a message to a designated raspbery pi
 *
 * First it converts the pi AEM into an adress and then sends the message.
 * 
 * Parameters:
 *  - msg : the message to be sent
 *  - pi : the AEM of the recipient pi
 * 
 * returns -1 upon connection failure (indicating recipient pi is absent)
 *          0 upon any other failure
 *          1 upon transmission success
 */
    char address[13];
    int yy = pi%100, xx = (pi-yy)/100;
    sprintf(address, "10.0.%d.%d", xx, yy);

    // Create socket
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log(ERROR, "Tranmission socket creation failed\n");
        return 0;
    }
          
    // Convert IPv4 and IPv6 addresses from text to binary form
    struct sockaddr_in server_addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};
    if(inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        log(ERROR, "Invalid address / Address not supported: %s\n", address);
        close(sock);
        return 0;
    }

    // Connect
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        log(ERROR, "Connection to %s failed\n", address);
        close(sock);
        return -1;
    }

    if (send(sock , msg , strlen(msg) , MSG_CONFIRM) < 0) {
        log(ERROR, "Transmission of message %s failed\n", msg);
        close(sock);
        return 0;
    } else {
        log(INFO, "Sent message:     "ANSI_BOLD" %s "ANSI_COLOR_RESET"to %d\n", msg, pi);
        close(sock);
        return 1;
    }
}


int receive_message(char* buffer, int nbytes) {
/* Receives a message from any pi in the network
 *
 * The first part of the script waits a messages to be accepted from any adress in the
 * network. When a message is received it converts sender's ip into a 4-digit AEM and
 * then copies the message to the buffer.
 * 
 * Parameters:
 *  - buffer : a buffer with preallocated memory to store the message
 *  - nbytes : the number of bytes to be copied into the buffer
 * 
 * returns 0 upon any error
 *         1 upon successful read of the message
 */
    int sock;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    if ((sock = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen)) < 0) {
        log(ERROR, "Acceptance of client failed\n");
        close(sock);
        return 0;
    }

    // Get sender's AEM
    uint32 xx, yy, pi;
    char ip[INET_ADDRSTRLEN];
    inet_ntop (AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    sscanf(ip, "10.0.%d.%d", &xx, &yy);
    pi = 100*xx + yy;
    
    // Read data
    int len;
    if ((len = recv(sock , buffer, nbytes, 0)) < 0){
        log(ERROR, "Reception of message failed\n");
        close(sock);
        return 0;
    } else {
        buffer[len] = '\0';
        log(INFO, "Received message: "ANSI_BOLD" %s "ANSI_COLOR_RESET"from %d\n", buffer, pi);
        close(sock);
        return pi;
    }
}
