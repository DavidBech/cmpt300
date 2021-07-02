#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "user_display.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till canceled
static void* user_display_loop(void* arg);

// Gets the next message in the rx list
//  msg: pointer to message to output should be NULL on input
//  returns 0 on success and 1 on failure
static void user_display_rxList_getNext(char** msg);

// Function used to free allocated memory
static void free_message(void* msg);

// Process Id, used for join and cancel
static pthread_t user_display_pid;

// Condition and Mutex Variables for access to rx list
static pthread_cond_t rx_list_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t rx_list_full = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t rx_list_mutex = PTHREAD_MUTEX_INITIALIZER;

// List for incoming messages
static List* rx_list = NULL;
// Pointer for messages retrieved from list
static char* message = NULL;

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
    if(rx_list == NULL){
        fprintf(stderr, "Error: rx_list is NULL\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&user_display_pid, NULL, user_display_loop, NULL);
}

void user_display_destroy(){
    // Stops the thread
    // TODO -- ensure this memory frees are correct

    pthread_cancel(user_display_pid);

    #ifdef DEBUG
        // close logging file
        fclose(display_log);
    #endif

    if(message){
        free_message(message);
    }
    List_free(rx_list, free_message);

    // Waits until thread finishes before continuing 
    pthread_join(user_display_pid, NULL);
}

void user_display_rxList_add(char* msg){
    // Lock mutex for access to rx_list
    pthread_mutex_lock(&rx_list_mutex);
    {  
        if(List_count(rx_list) == STALK_MAX_NUM_NODES/2){
            pthread_cond_wait(&rx_list_full, &rx_list_mutex);
        }
        // Append Message to list
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        bool status = List_prepend(rx_list, (void*)msg);
        if(status == LIST_SUCCESS){
            msg = NULL;
            // new item on list
            pthread_cond_signal(&rx_list_empty);
        } else {  
            // TODO Failed
        }
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    pthread_mutex_unlock(&rx_list_mutex);
}

static void user_display_rxList_getNext(char** msg){
    // Attempt to access List lock access or wait for access
    pthread_mutex_lock(&rx_list_mutex);
    {  
        // Check if list is empty
        if(List_count(rx_list) == 0){ 
            // Wait for Item on list 
            pthread_cond_wait(&rx_list_empty, &rx_list_mutex);
        } 
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        *msg = (char*)List_trim(rx_list);
        pthread_cond_signal(&rx_list_full);
    }
    pthread_mutex_unlock(&rx_list_mutex);
}

static void free_message(void* msg){
    free(msg);
}

static void* user_display_loop(void* arg)
{
    DISPLAY_LOG("Started Display Loop\n");
    while(1){
        user_display_rxList_getNext(&message);
        if(fputs(message, stdout) == EOF){
            DISPLAY_LOG("ERROR: fputs returned error\n");
            // TODO fputs Error
        }
        fflush(stdout);
        free_message(message);
        message = NULL;
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}
