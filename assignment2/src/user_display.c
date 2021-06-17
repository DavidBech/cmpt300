#include <stdio.h>
#include <pthread.h>

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

void user_display_init(){
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
