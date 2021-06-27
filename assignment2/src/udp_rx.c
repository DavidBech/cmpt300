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

static char* messageRx = NULL;

static pthread_t udp_rx_pid;

// Socket recieving UDP packets on
static int rx_socket_desc;
// Port reciving UDP packets on
static int port;

#ifdef DEBUG
    // Variables for debugging purposes, unused otherwise
    // Log file to print log messages to
    static char* udp_rx_log_file_name = "./log/udp_rx_log.log";
    static FILE* udp_rx_log;
    // Start time fetched in init function
    static struct tm udp_rx_start_time;
    // gets the current time relative to the start time
    static struct tm udp_rx_current_time(){
        time_t time_temp;
        time(&time_temp);
        struct tm time_now = *localtime(&time_temp);
        // only minutes and seconds are used for printout
        time_now.tm_min -= udp_rx_start_time.tm_min;
        time_now.tm_sec -= udp_rx_start_time.tm_sec;
        return time_now;
    }
    #define UDP_RX_LOG(_message) STALK_LOG(udp_rx_log, _message, udp_rx_current_time())
#else
    #define UDP_RX_LOG(_message) ;
#endif

void udp_rx_init(char* rx_port){
    // TODO - add error handling, 
    //      the port setup could fail, etc
    //      port number could be not in valid range
    #ifdef DEBUG
        // fetch start time, bust be done before thread as not threadsafe
        udp_rx_start_time = get_start_time();
        // Open log file
        udp_rx_log = fopen(udp_rx_log_file_name, "w");
        if(udp_rx_log == NULL){
            fprintf(stderr, "Invalid File name %s, %i", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    #endif

    // UDP Setup Connection
    char* endptr;
    port = strtol(rx_port, &endptr, 10);
    if(port == 0){
        fprintf(stderr, "Error in RX port number: %s\n", rx_port);
    }
    //printf("Receiving Data on port: %d\n", port);
       // Setup Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(port);              // Port Watching
    // TODO INVALID PORT?
    // TODO get host name?

    // Create the socket
    rx_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if(rx_socket_desc == -1){
        UDP_RX_LOG("ERROR: invalid socket descriptor in udp_rx\n");
        //TODO
    }
    // Bind socket to port specified
    // TODO BIND CAN FAIL?
    bind(rx_socket_desc, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));

    pthread_create(&udp_rx_pid, NULL, udp_recieve_loop, NULL);
}

void udp_rx_destroy(){
    // Stops the thread
    pthread_cancel(udp_rx_pid);
    
    #ifdef DEBUG
        // close logging file
        fclose(udp_rx_log);
    #endif
    // Close the UPD socket
    close(rx_socket_desc);

    // Waits until thread finishes before continuing 
    pthread_join(udp_rx_pid, NULL);
    //printf("Fished UPD Receiver\n");
}

static void* udp_recieve_loop(void* arg){
    //printf("Started UDP Transmitor on UDP_RX\n");
    UDP_RX_LOG("Started UDP RX Loop\n");
    while(1){
        messageRx = malloc(MAX_MESSAGE_SIZE);
        UDP_RX_LOG("Waiting for message\n");
        // blocking call to receive message
        int bytesRx = recv(rx_socket_desc, messageRx, MAX_MESSAGE_SIZE, 0);
        //printf("Recieved Message: %s", messageRx);
        if(bytesRx == 0){
            UDP_RX_LOG("ERROR: Recieved 0 bytes\n");
            // TODO 
        }
        else if(bytesRx == -1){
            UDP_RX_LOG("ERROR: Recieved Invalid Message\n");
            // TODO: error recieving message
        } 
        else 
        {
            UDP_RX_LOG("Message Recieved\n");
            int term_null = (bytesRx < MAX_MESSAGE_SIZE) ? bytesRx : MAX_MESSAGE_SIZE -1;
            messageRx[term_null] = '\0';
            if(strcmp(messageRx, "!\n\0") == 0){
                UDP_RX_LOG("Recieved Termination Message\n");
                stalk_initiateShutdown();
                return NULL;
            }
            else if(user_display_rxList_add(messageRx)){
                UDP_RX_LOG("ERROR: Adding Message to list\n");
                // TODO: error
            }
            UDP_RX_LOG("Message Added to List\n");
        }
    }
    return NULL;
}