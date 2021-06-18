#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "user_display.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* user_display_loop(void* arg);

// Gets the next meessage in the rx list
//  effectivly bypassing other messages on the list
//  msg: pointer to message to append to list
//  returns 0 on success and 1 on failure
static bool user_display_rxList_getNext(char* msg);

static pthread_t user_display_pid;

List* rx_list = NULL;

#ifdef DEBUG
    // Variables for debugging purposes, unused otherwise
    // Log file to print log messages to
    static char* display_log_file_name = "./log/display_log.log";
    static FILE* display_log;
    // Start time fetched in init function
    static struct tm display_start_time;
    // gets the current time relative to the start time
    static struct tm display_current_time(){
        time_t time_temp;
        time(&time_temp);
        struct tm time_now = *localtime(&time_temp);
        // only minutes and seconds are used for printout
        time_now.tm_min -= display_start_time.tm_min;
        time_now.tm_sec -= display_start_time.tm_sec;
        return time_now;
    }
    #define DISPLAY_LOG(_message) STALK_LOG(display_log, _message, display_current_time())
#else
    #define DISPLAY_LOG(_message) ;
#endif

void user_display_init(){
    // Open log file if debugging is active
    #ifdef DEBUG
        // fetch start time, bust be done before thread as not threadsafe
        display_start_time = get_start_time();
        // Open log file
        display_log = fopen(display_log_file_name, "w");
        if(display_log == NULL){
            fprintf(stderr, "Invalid File name %s, %i", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    #endif

    // create the rx list
    rx_list = List_create();

    // TODO - Create UPD socket and bind it to the port
    pthread_create(&user_display_pid, NULL, user_display_loop, NULL);
}

void user_display_destroy(){
    // Stops the thread
    pthread_cancel(user_display_pid);

    // TODO FREE LIST
    // TODO FREE MEMEORY
    #ifdef DEBUG
        // close logging file
        fclose(display_log);
    #endif

    // Waits until thread finishes before continuing 
    pthread_join(user_display_pid, NULL);
    printf("Finished User Display\n");
}

bool user_display_rxList_add(char* msg){
    //TODO
    return true;
}

static bool user_display_rxList_getNext(char* msg){
    //TODO
    return true;
}

static void* user_display_loop(void* arg){
    printf("Started User Display\n");
    DISPLAY_LOG("Started Display Loop\n");
    char* message = NULL;
    // TODO - wait for rx_list to have messages to display to screen
    while(1){
        pthread_testcancel();
        if(user_display_rxList_getNext(message)){
            // TODO ERROR HANDLING
        }
        // TODO DISPLAY MESSAGE
        // TODO FREE MESSAGE
    }
    return NULL;
}
