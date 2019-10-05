#include "types.h"
#include "utils.h"
#include "logger.h"
#include "network.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


/* PLEASE MODIFY THE LIST ACCORDING TO THE AVAILABLE STUDENTS */
const uint32 myPI = 8880;
const uint32 list[] = {8261, 8844, 8861, 8941, 8997};

const int N_PI = sizeof(list) / sizeof(uint32), N_MESSAGES = 2000;

uint32 map[10000];  // Map table for AEMs
record *records;    // Structure to hold data

pthread_mutex_t lock;   // Used to secure operations on the global variables


void* receive_thread_func();
void* send_thread_func(void *sleep_time);
void* generate(int min_sleep_time, int max_sleep_time);

int main() {
    /* Initialization */
    srand(time(NULL));

    records = (record *) malloc(N_MESSAGES * sizeof(record));
    for (int i = 0; i < N_MESSAGES; i++)
        records[i].recipients = (bool *) malloc(N_PI * sizeof(bool));

    for (int i = 0; i < N_PI; i++)
        map[list[i]] = i;

    init_logger("log.txt");

    /* Create threads for sending and receiving messages */
    int send_sleep_time = 60;
    int generate_min_sleep_time = 60, generate_max_sleep_time = 300;

    pthread_t send_thread, receive_thread;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("Mutex initilization has failed!\nQuitting...\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&receive_thread, NULL, receive_thread_func, NULL) != 0 ||
        pthread_create(&send_thread, NULL, send_thread_func, &send_sleep_time) != 0) {
            perror("Thread initialization has failed!\nQuitting...\n");
            exit(EXIT_FAILURE);
        }
    
    /* Generate random messages */
    generate(generate_min_sleep_time, generate_max_sleep_time);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    return 0;
}


void* generate(int min_sleep_time, int max_sleep_time) {
    message m;
    int sleep_time;
    char buffer[512];
    
    for (int id = 12; TRUE; id++) {        
        /* Generate message */
        m = (message) {myPI, list[rand()%N_PI], time(NULL), ""};
        sprintf(m.text, "Message from toliz with id %d", id);
        
        pthread_mutex_lock(&lock);
        write_message(m, myPI);
        pthread_mutex_unlock(&lock);
        
        message_to_string(m, buffer);
        log(INFO, "Generated message: "ANSI_BOLD"%s\n"ANSI_COLOR_RESET, buffer);

        sleep_time = min_sleep_time + rand() % (max_sleep_time - min_sleep_time + 1);
        sleep(sleep_time);
    }
}


void* send_thread_func(void *sleep_time) {
    int idx, status;
    message m;
    uint32 AEM;
    char buffer[512];

    while(TRUE) {

        /* Send unsent messages */
        for (int pi = 0; pi < N_PI; pi++) {
            AEM = list[idx];
            
            pthread_mutex_lock(&lock);
            while((idx = read_message(AEM, &m)) != -1) {
                message_to_string(records[idx].message, buffer);

                if ((status = send_message(buffer, pi)) == 1)
                    records[idx].recipients[idx] = TRUE;
                else if (status == -1)
                    break;
            }
            pthread_mutex_unlock(&lock);
        }

        sleep(*((int *) sleep_time));
    }
}


void* receive_thread_func() {
    uint32 pi;
    message m;
    char msg[279];

    if (intialize_listener() == 0)
        return 0;
    
    while(TRUE) {
        /* Receive new messages */
        pi = receive_message(msg, 279);
        if (string_to_message(msg, &m)) {
            pthread_mutex_lock(&lock);
            write_message(m, pi);
            pthread_mutex_unlock(&lock);
        }
    }
}
