#include <stdio.h>
#include <pthread.h>

#include "user_display.h"

static void* user_display_loop(void* arg);

static pthread_t user_display_pid;

static void* user_display_loop(void* arg){
    printf("Started User Display\n");
    // TODO - wait for rx_list to have messages to display to screen
    while(1){
        pthread_testcancel();
    }
    return NULL;
}

void user_display_init(){
    // TODO - Create UPD socket and bind it to the port
    pthread_create(&user_display_pid, NULL, user_display_loop, NULL);
}

void user_display_destroy(){
    // Stops the thread
    pthread_cancel(user_display_pid);

    // Waits until thread finishes before continuing 
    pthread_join(user_display_pid, NULL);
    printf("Finished User Display\n");
}