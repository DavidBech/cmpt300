#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "user_reader.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* user_reader_loop(void* arg);

// Adds a message to the end of tx list
//  msg: pointer to message to append to list
//  returns 0 on success and 1 on failure
static bool txList_addLast(char* msg);

// Adds a message to first location in tx list
//  effectivly bypassing other messages on the list
//  msg: pointer to message to append to list
//  returns 0 on success and 1 on failure
static bool txList_addFirst(char* msg);

static pthread_t user_reader_pid;
static List* tx_list = NULL;
// TODO need to handle memory allocation leeks
static char* user_input = NULL;

pthread_cond_t tx_list_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t tx_list_mutex = PTHREAD_MUTEX_INITIALIZER;

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
        // TODO failure
        READER_LOG("ERROR: You have now exceeded the total messages that you can have on the list at a time.\n");
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

    // free allocated memory
    //List_free(tx_list, TODO FREE FUNCTION );
    //FREE BUFFER?
    free(user_input);

    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);
    printf("Finished User Reader\n");
}

bool user_reader_txList_getNext(char** msg){
    // TODO
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_count(tx_list) == 0)
        {
            READER_LOG("Waiting for an item to be available on the list\n");
            pthread_cond_wait(&tx_list_cond, &tx_list_mutex);
        }
        READER_LOG("Retrieving the message from the list for printing to the screen\n");
        *msg = (char*)List_trim(tx_list);
    }
    pthread_mutex_unlock(&tx_list_mutex);
    
    if (*msg == NULL)
    {
        READER_LOG("ERROR: The item retrieved from the list is NULL\n");
        return true;
    }
    return false;
}

static bool txList_addLast(char* msg){
    bool status = 0;
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_append(tx_list, (void*)msg) == LIST_FAIL)
        {
            // TODO: ERROR HANDLING
            status = 1;
        }
        else
        {
            // Signalling that the exclamation mark has now been appended onto the list
            pthread_cond_signal(&tx_list_cond);
        }
    }
    pthread_mutex_unlock(&tx_list_mutex);
    return status;
}

static bool txList_addFirst(char* msg){
    bool status = 0;
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_prepend(tx_list, (void*)msg) == LIST_FAIL)
        {
            // TODO: ERROR HANDLING
            status = 1;
        }
        else
        {
            // Signalling that the exclamation mark has now been appended onto the list
            pthread_cond_signal(&tx_list_cond);
        }
    }
    pthread_mutex_unlock(&tx_list_mutex);
    return status;
}

static void* user_reader_loop(void* arg)
{
    READER_LOG("Started Reader Loop\n");
    // TODO - read user input puting messages onto a List
    printf("Started User Reader\n");
    // used to ensure fgets didn't fail
    char* readerReturn = NULL;
    while(1){
        // TODO ALLOCATE MEMORY here?
        user_input = malloc(MAX_MESSAGE_SIZE);
        // TODO ALLOCATE MEMORY?
       
        printf("Input '!' and then hit the Enter Key To End Program:\n");
        // Get user input
        // TODO message too large, potentially send multiple packets?
        readerReturn = fgets(user_input, MAX_MESSAGE_SIZE - 1, stdin);
        
        // TODO readerReturn == NULL ERROR HANDLING?
        if(readerReturn == NULL){
            // TODO ERROR
            READER_LOG("ERROR: No characters have been read. Please, input again\n");
        }

        char output[] = "The input sent successfully is: ";
        strcat(output, user_input);
        READER_LOG(output);

        if(strcmp(user_input, "!\n\0") == 0)
        {
            // TODO -- send shutdown to other stalk process
            // TODO -- move into udp tx to ensure the message is sent
            txList_addLast(user_input);
            // I changed here to stalk_waitforshutdown() as opposed to stalk_initiateShutdown()
            stalk_waitForShutdown();
        } 
        else 
        {
            // append message to list
            if(txList_addFirst(user_input)){
                // TODO ERROR HANDLING
                READER_LOG("ERROR: The message has not been added to the list. Please, try again.\n");
            }
        }
        // TODO THIS FREE SHOULDN'T BE HERE
        // free(user_input);
        printf("Here is now an output\n");
    }
    return NULL;
}