#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include <time.h>

#include "udp_rx.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* upd_recieve_loop(void* arg);

static pthread_t upd_rx_pid;

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
    printf("Receiving Data through UDP on port %d\n", port);
       // Setup Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(port);              // Port Watching

    // Create the socket
    rx_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    // Bind socket to port specified
    bind(rx_socket_desc, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));

    pthread_create(&upd_rx_pid, NULL, upd_recieve_loop, NULL);
}

void udp_rx_destroy(){
    // Stops the thread
    pthread_cancel(upd_rx_pid);
    
    #ifdef DEBUG
        // close logging file
        fclose(udp_rx_log);
    #endif
    // Close the UPD socket
    close(rx_socket_desc);

    // Waits until thread finishes before continuing 
    pthread_join(upd_rx_pid, NULL);
    printf("Fished UPD Receiver\n");
}

static void* upd_recieve_loop(void* arg){
    printf("Started UPD Receiver\n");
    UDP_RX_LOG("Started UDP RX Loop\n");
    // TODO - coninually read upd socket adding recieved messages to rx_list
    //  currently waits for messages and prints them itself should put them on list
    while(1){
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MAX_MESSAGE_SIZE];
        printf("Waiting for message\n");
        // blocking call to receive message
        int bytesRx = recvfrom(rx_socket_desc, messageRx, MAX_MESSAGE_SIZE, 0, (struct sockaddr *) &sinRemote, &sin_len);
        
        // prints out message
        if(bytesRx == -1){
            fprintf(stderr, "ERROR RECIEVING MESSAGE\n");
            exit(EXIT_FAILURE);
        } else {
            int term_null = (bytesRx < MAX_MESSAGE_SIZE) ? bytesRx : MAX_MESSAGE_SIZE -1;
            messageRx[term_null] = '\0';
            printf("Message recieved %d Bytes: |%s|\n", bytesRx, messageRx);
        }
        
    }
    printf("should never happen\n");
    return NULL;
}