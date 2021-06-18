#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 
#include <stdlib.h>
#include <time.h>

#include "user_reader.h"
#include "udp_tx.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* upd_transmit_loop(void* arg);

static pthread_t upd_tx_pid;
// TODO - add socket variables

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
    // TODO - Create UPD socket and bind it to the port

    // Launch UDP tx thread
    pthread_create(&upd_tx_pid, NULL, upd_transmit_loop, NULL);
}

void udp_tx_destroy(){
    // Stops the thread
    pthread_cancel(upd_tx_pid);

    #ifdef DEBUG
        // close logging file
        fclose(udp_tx_log);
    #endif

    // TODO - Close the UPD socket

    // Waits until thread finishes before continuing 
    pthread_join(upd_tx_pid, NULL);
    printf("Fished UPD Transmitor\n");
}


static void* upd_transmit_loop(void* arg){
    printf("Started UPD Transmitor\n");
    UDP_TX_LOG("Started UDP TX Loop\n");
    // TODO - coninually wait for tx_list to have messages to send to other s-talk instance
    char* msg = NULL;
    while(1){
        pthread_testcancel();
        if(user_reader_txList_getNext(msg)){
            // TODO ERROR HANDLING with recieved message
        }
        // TODO -- if msg == termination
        //  Send termination to other stalk instance
        //  call stalk_initiateShutdown
        
        // TODO -- else
        //  Send regulare message to other stalk instance
    }
    return NULL;
}