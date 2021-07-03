#include <stdio.h>
#include <pthread.h>
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

void user_display_init(){
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
    pthread_cancel(user_display_pid);

    // Waits until thread finishes before continuing 
    pthread_join(user_display_pid, NULL);

    if(message){
        free_message(message);
    }
    List_free(rx_list, free_message);
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
        List_prepend(rx_list, (void*)msg);
        msg = NULL;
        // new item on list
        pthread_cond_signal(&rx_list_empty);
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
    while(1){
        user_display_rxList_getNext(&message);
        fputs(message, stdout);
        fflush(stdout);
        free_message(message);
        message = NULL;
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}
