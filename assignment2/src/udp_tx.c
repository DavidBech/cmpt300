#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "user_reader.h"
#include "udp_tx.h"
#include "list.h"
#include "stalk.h"

#ifdef DEBUG
    // Variables for debugging purposes, unused otherwise
    // Log file to print log messages to
    static char* udp_tx_log_file_name = "./log/udp_tx_log.log";
    static FILE* udp_tx_log;
    // Start time fetched in init function
    static struct tm udp_tx_start_time;
    // gets the current time relative to the start time
    static struct tm udp_tx_current_time(){
        time_t time_temp;
        time(&time_temp);
        struct tm time_now = *localtime(&time_temp);
        // only minutes and seconds are used for printout
        time_now.tm_min -= udp_tx_start_time.tm_min;
        time_now.tm_sec -= udp_tx_start_time.tm_sec;
        return time_now;
    }
    #define UDP_TX_LOG(_message) STALK_LOG(udp_tx_log, _message, udp_tx_current_time())
#else
    #define UDP_TX_LOG(_message) ;
#endif

// Loop that thread runs till cancled
static void* upd_transmit_loop(void* arg);

// pthread variables
static pthread_t upd_tx_pid;

// UDP variables
static int tx_socket_desc;
static struct sockaddr_in sin;

void udp_tx_init(char* tx_machine, char* tx_port){
    #ifdef DEBUG
        // fetch start time, bust be done before thread as not threadsafe
        udp_tx_start_time = get_start_time();
        // Open log file
        udp_tx_log = fopen(udp_tx_log_file_name, "w");
        if(udp_tx_log == NULL){
            fprintf(stderr, "Invalid File name %s, %i", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    #endif
    
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
            fprintf(stderr, "Error: IP V 6 \n");
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

    #ifdef DEBUG
        // close logging file
        fclose(udp_tx_log);
    #endif

    close(tx_socket_desc);

    // Waits until thread finishes before continuing 
    pthread_join(upd_tx_pid, NULL);
}

static void* upd_transmit_loop(void* arg){
    UDP_TX_LOG("Started UDP TX Loop\n");
    char* msg = NULL;
    int bytesTx;
    while(1){
        // Blocking call to get next message
        // Cancel is disabled when a message is retrieved
        user_reader_txList_getNext(&msg);
        UDP_TX_LOG("Retrieved Message from tx list\n");
        bytesTx = sendto(tx_socket_desc, msg, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &sin, sizeof(struct sockaddr_in));
        if(bytesTx < 1){
            UDP_TX_LOG("Error in sendto call\n");
            // TODO
            fprintf(stderr, "Error sendto call TODO\n");
            exit(EXIT_FAILURE);
            // Attempt to send message again should not terminate here
        } else {
            UDP_TX_LOG("Message Sent without error\n");  
            if (strcmp(msg, TERMINATION_STRING) == 0){
                UDP_TX_LOG("Recieved Termination message - initiating shutdown\n");
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
