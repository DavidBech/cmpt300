#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "user_reader.h"
#include "list.h"
#include "stalk.h"

static void* user_reader_loop(void* arg);

static pthread_t user_reader_pid;

static void* user_reader_loop(void* arg){
    // TODO - read user input puting messages onto a List
    printf("Started User Reader\n");
    while(1){
        char user_input[MAX_MESSAGE_SIZE];
        printf("Input '!' To End Program:\n");
        scanf("%s", user_input);
        if(user_input[0] == '!' && user_input[1] == '\0'){
            break;
        }
    }
    return NULL;
}

void user_reader_init(List* tx_list){
    pthread_create(&user_reader_pid, NULL, user_reader_loop, NULL);
}

void user_reader_destroy(){
    // Stops the thread
    //pthread_cancel(user_reader_pid);
    // The reader is killed when the input is '!' doesn't need to be canceled

    // Waits until thread finishes before continuing 
    pthread_join(user_reader_pid, NULL);
    printf("Finished User Reader\n");
}