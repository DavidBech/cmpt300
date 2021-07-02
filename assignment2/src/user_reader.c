#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "user_reader.h"
#include "list.h"
#include "stalk.h"

#ifdef DEBUG
    // Variables for debugging purposes, unused otherwise
    // Log file to print log messages to
    static char* reader_log_file_name = "./log/reader_log.log";
    static FILE* reader_log;
    // Start time fetched in init function
    static struct tm reader_start_time;
    // gets the current time relative to the start time
    static struct tm reader_current_time(){
        time_t time_temp;
        time(&time_temp);
        struct tm time_now = *localtime(&time_temp);
        // only minutes and seconds are used for printout
        time_now.tm_min -= reader_start_time.tm_min;
        time_now.tm_sec -= reader_start_time.tm_sec;
        return time_now;
    }
    #define READER_LOG(_message) STALK_LOG(reader_log, _message, reader_current_time())
#else
    #define READER_LOG(_message) ;
#endif

// Loop that thread runs till cancled
static void* user_reader_loop(void* arg);

// Adds a message to tx_list
static void txList_addmessage(char* msg);

// Frees the pointer input
static void free_message(void* pointer);

// List and input pointers, Need to be freed
static List* tx_list = NULL;
static char* user_input = NULL;

// Thread Variables
static pthread_t user_reader_pid;
static pthread_cond_t tx_list_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t tx_list_full = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t tx_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void user_reader_init(){
    // Open log file if debugging is active
    #ifdef DEBUG
        // fetch start time, bust be done before thread as not threadsafe
        reader_start_time = get_start_time();
        // Open log file
        reader_log = fopen(reader_log_file_name, "w");
        if(reader_log == NULL){
            fprintf(stderr, "Invalid File name %s, %i", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    #endif

    tx_list = List_create();

    if(tx_list == NULL){
        fprintf(stderr, "Error Creating tx list\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&user_reader_pid, NULL, user_reader_loop, NULL);
}

void user_reader_destroy(){
    // Stops the thread
    pthread_cancel(user_reader_pid);
    
    #ifdef DEBUG
        // close logging file
        fclose(reader_log);
    #endif

    if(user_input){
        free_message(user_input);
    }
    List_free(tx_list, free_message);

    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);
}

void user_reader_txList_getNext(char** msg){
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_count(tx_list) == 0){
            pthread_cond_wait(&tx_list_empty, &tx_list_mutex);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        *msg = (char*)List_trim(tx_list);
        pthread_cond_signal(&tx_list_full);
    }
    pthread_mutex_unlock(&tx_list_mutex);
}

static void txList_addmessage(char* msg){
    int status = 0;
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_count(tx_list) == STALK_MAX_NUM_NODES/2){
            // Wait for available list node
            pthread_cond_wait(&tx_list_full, &tx_list_mutex);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        status = List_prepend(tx_list, (void*)msg);
        if(status == LIST_SUCCESS){
            msg = NULL;
            // New Item on List
            pthread_cond_signal(&tx_list_empty);
        } else {
            // TODO FAILED
        }
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);   
    }
    pthread_mutex_unlock(&tx_list_mutex);
}

static void free_message(void* msg){
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    free(msg);
    msg = NULL;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

static void* user_reader_loop(void* arg){
    READER_LOG("Started Reader Loop\n");
    char* readerReturn = NULL;
    while(1){
        user_input = malloc(MAX_MESSAGE_SIZE);
        memset(user_input, '\0', MAX_MESSAGE_SIZE);
        // Get user input
        readerReturn = fgets(user_input, MAX_MESSAGE_SIZE, stdin);
        
        // ensure user input is valid
        if(readerReturn == NULL){
            // free the buffer
            READER_LOG("Error fgets failed\n");
            free_message(user_input);
            continue;
        }

        // Add message to list
        if(strcmp(user_input, TERMINATION_STRING) == 0){
            // append messge to list,
            txList_addmessage(user_input);
            // stop reading new messages
            stalk_waitForShutdown();
            return NULL;
        } else {
            // append message to list
            txList_addmessage(user_input);
        }
    }
    return NULL;
}