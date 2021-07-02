#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>

#include "user_reader.h"
#include "udp_tx.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* upd_transmit_loop(void* arg);

// pthread variables
static pthread_t upd_tx_pid;

// UDP variables
static int tx_socket_desc;
static struct sockaddr_in sin;

void udp_tx_init(char* tx_machine, char* tx_port){
    // set sin to all zeros
    memset(&sin, 0, sizeof(sin));
    struct addrinfo *res;

    // Get address from machine and port
    if(getaddrinfo(tx_machine, tx_port, NULL, &res)){
        // TODO -- Hints, currently NULL
        fprintf(stderr, "Error getaddrinfo failed\n");
        exit(EXIT_FAILURE);
    }

    for(struct addrinfo *p=res; p !=NULL; p=p->ai_next){
        if(p->ai_family == AF_INET){
            sin = *(struct sockaddr_in *)p->ai_addr;
        } else {
            fprintf(stderr, "Error: IP V6 \n");
            exit(EXIT_FAILURE);
        }
    }

    // Creating the socket
    tx_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if (tx_socket_desc == -1) {
        fprintf(stderr, "Error creating tx socekt\n");
        exit(EXIT_FAILURE);
    }
    // Free the results to ensure no memory leak
    freeaddrinfo(res);

    // Create the pthread
    pthread_create(&upd_tx_pid, NULL, upd_transmit_loop, NULL);
}

void udp_tx_destroy(){
    // Stops the thread
    pthread_cancel(upd_tx_pid);

    close(tx_socket_desc);

    // Waits until thread finishes before continuing 
    pthread_join(upd_tx_pid, NULL);
}

static void* upd_transmit_loop(void* arg){
    char* msg = NULL;
    int bytesTx;
    while(1){
        // Blocking call to get next message
        // Cancel is disabled when a message is retrieved
        user_reader_txList_getNext(&msg);
        bytesTx = sendto(tx_socket_desc, msg, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &sin, sizeof(struct sockaddr_in));
        if(bytesTx < 1){
            // TODO
            fprintf(stderr, "Error sendto call\n");
            exit(EXIT_FAILURE);
            // Attempt to send message again should not terminate here
        } else {
            if (strcmp(msg, TERMINATION_STRING) == 0){
                stalk_initiateShutdown();
                return NULL;
            }
        }
        free(msg);
        // once the retrieved message is freed enable canceling again
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}
