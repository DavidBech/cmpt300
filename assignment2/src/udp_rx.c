#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include <time.h>
#include <errno.h>

#include "udp_rx.h"
#include "list.h"
#include "stalk.h"
#include "user_display.h"

// Loop that thread runs till cancled
static void* udp_recieve_loop(void* arg);

// Message Buffer
static char* messageRx = NULL;

// pthread Variables
static pthread_t udp_rx_pid;

// Udp variables
static int rx_socket_desc;

void udp_rx_init(char* rx_port){
    // UDP Setup Connection
    char* endptr;
    int port = strtol(rx_port, &endptr, 10);
    //TODO valid port values?
    if(port == 0){
        fprintf(stderr, "Error invalid RX port : %s\n", rx_port);
        exit(EXIT_FAILURE);
    }
    
    // Setup Address TODO remote address support
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(port);              // Port Watching

    // Create the socket
    rx_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if(rx_socket_desc == -1){
        fprintf(stderr, "Error creating rx socket descriptor\n");
        exit(EXIT_FAILURE);
    }
    // Bind socket to port specified
    if(bind(rx_socket_desc, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) == -1){
        fprintf(stderr, "Error binding rx socket\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&udp_rx_pid, NULL, udp_recieve_loop, NULL);
}

void udp_rx_destroy(){
    // Stops the thread
    pthread_cancel(udp_rx_pid);

    if(messageRx){
        free(messageRx);
    }
    // Close the UPD socket
    close(rx_socket_desc);

    // Waits until thread finishes before continuing 
    pthread_join(udp_rx_pid, NULL);
}

static void* udp_recieve_loop(void* arg){
    while(1){
        messageRx = malloc(MAX_MESSAGE_SIZE);
        // blocking call to receive message
        int bytesRx = recv(rx_socket_desc, messageRx, MAX_MESSAGE_SIZE, 0);

        if(bytesRx < 1){
            free(messageRx);
            messageRx = NULL;
            continue;
        }
        else 
        {
            if(strcmp(messageRx, TERMINATION_STRING) == 0){
                stalk_initiateShutdown();
                return NULL;
            }
            else {
                user_display_rxList_add(messageRx);
            }
        }
    }
    return NULL;
}