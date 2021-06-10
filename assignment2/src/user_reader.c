#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "user_reader.h"

static void* user_reader_loop(void* arg);

static pthread_t user_reader_pid;

static void* user_reader_loop(void* arg){
    // TODO - read user input 
    printf("Started User Reader\n");
    while(1){
        printf(" \b");
    }
    return NULL;
}

void user_reader_init(){
    pthread_create(&user_reader_pid, NULL, user_reader_loop, NULL);
}

void user_reader_destroy(){
    // Stops the thread
    pthread_cancel(user_reader_pid);

    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);
    printf("Finished User Display\n");
}