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

// Loop that thread runs till cancled
static void* upd_transmit_loop(void* arg);

static pthread_t upd_tx_pid;

// Socket that would send the packets recieved from a user's input
static int tx_socket_desc;

static int port;

static struct sockaddr_in sin;

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
    
    // Get the port to transmit to
    char* tempPtr;
    port = strtol(tx_port, &tempPtr, 10);
    if (port == 0)
    {
        fprintf(stderr, "Error in TX port number: %s", tx_port);
        exit(EXIT_FAILURE);
    }
    
    // Address -- TODO  use tx_machine to send to remote machines
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(port);

    // Creating the socket
    tx_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if (tx_socket_desc == -1)
    {
        fprintf(stderr, "Error creating tx socekt\n");
        exit(EXIT_FAILURE);
    }

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
    while(1){
        if(user_reader_txList_getNext(&msg))
        {
            UDP_TX_LOG("ERROR: Cannot retrieve the next message from the list.\n");
            // TODO: ERROR HANDLING
        }
        UDP_TX_LOG(msg);
        int bytesTx = sendto(tx_socket_desc, msg, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &sin, sizeof(struct sockaddr_in));
        if(bytesTx == 0){
            UDP_TX_LOG("ERROR: Sent 0 bytes \n");
            // TODO 
        }
        else if(bytesTx == -1){
            UDP_TX_LOG("ERROR: Sent Invalid Message\n");
            // TODO: error recieving message
        }
        else
        {
            UDP_TX_LOG("Message Sent Successfully\n");  
            if (strcmp(msg, "!\n\0") == 0)
            {
                UDP_TX_LOG("Received Termination Request; now preparing for termination\n");
                stalk_initiateShutdown();
                return NULL;
            }
        }
        free(msg);
    }
    return NULL;
}
