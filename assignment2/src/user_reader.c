#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

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
static List* tx_list;

void user_reader_init(){
    tx_list = List_create();
    if(tx_list == NULL){
        // TODO failure
    }

    pthread_create(&user_reader_pid, NULL, user_reader_loop, NULL);
}

void user_reader_destroy(){
    // Stops the thread
    pthread_cancel(user_reader_pid);
    
    // free allocated memory
    //List_free(tx_list, TODO FREE FUNCTION );
    //FREE BUFFER?

    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);
    printf("Finished User Reader\n");
}


bool user_reader_txList_getNext(char* msg){
    // TODO
    return true;
}

static bool txList_addLast(char* msg){
    // TODO
    return true;
}

static bool txList_addFirst(char* msg){
    // TODO 
    return true;
}

static void* user_reader_loop(void* arg){
    // TODO - read user input puting messages onto a List
    printf("Started User Reader\n");
    while(1){
        // TODO ALLOCATE MEMORY?
        char user_input[MAX_MESSAGE_SIZE];
        printf("Input '!' To End Program:\n");
        // TODO, don't use scanf change to fgets
        scanf("%s", user_input);

        // Case for termination -- TODO doesn't work for '! ' as input
        if(user_input[0] == '!' && user_input[1] == '\0'){
            // TODO -- send shutdown to other stalk process
            // TODO -- move into udp tx to ensure the message is sent
            txList_addLast(user_input);
            stalk_initiateShutdown();
        } else {
            // append message to list
            // TODO message too large, potentially send multiple packets?
            if(txList_addFirst(user_input)){
                // TODO ERROR HANDLING
            }
            // TODO ALLOCATE MEMORY?
        }
        
    }
    return NULL;
}