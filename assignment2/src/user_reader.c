#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "user_reader.h"
#include "list.h"
#include "stalk.h"

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
    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);

    if(user_input){
        free(user_input);
    }
    List_free(tx_list, free_message);
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
    pthread_mutex_lock(&tx_list_mutex);
    {
        if (List_count(tx_list) == STALK_MAX_NUM_NODES/2){
            // Wait for available list node
            printf("wating list_full\n");
            pthread_cond_wait(&tx_list_full, &tx_list_mutex);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        List_prepend(tx_list, (void*)msg);
        msg = NULL;
        pthread_cond_signal(&tx_list_empty);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);   
    }
    pthread_mutex_unlock(&tx_list_mutex);
}

static void free_message(void* msg){
    free(msg);
}

static void* user_reader_loop(void* arg){
    char* readerReturn = NULL;
    bool last_had_linefeed = true;
    while(1){
        user_input = malloc(MAX_MESSAGE_SIZE);
        memset(user_input, '\0', MAX_MESSAGE_SIZE);
        // Get user input
        readerReturn = fgets(user_input, MAX_MESSAGE_SIZE, stdin);
        
        // ensure user input is valid
        if(readerReturn == NULL){
            stalk_waitForShutdown();
            return NULL;
        }
        
        // Add message to list
        if(strcmp(user_input, TERMINATION_STRING) == 0){
            if(last_had_linefeed){
                // append messge to list,
                txList_addmessage(user_input);
                // stop reading new messages
                stalk_waitForShutdown();
                return NULL;
            } else {
                user_input[0] = '!';
                user_input[1] = ' ';
                user_input[2] = '\n';
                user_input[3] = '\0';
            }
        }

        // append message to list
        txList_addmessage(user_input);
        last_had_linefeed = strchr(user_input, '\n') != NULL;
    }
    return NULL;
}
