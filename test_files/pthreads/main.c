#include <stdio.h>
#include <stdlib.h>
#include "reciever.h"

int main(int argc, char** args) 
{
    printf("Starting...\n");

    // Start my modules
    // pthread_mutex_t sharedMutex = ...;
    // Reciever_init("An incomin message...", &sharedMutex);

    Reciever_init("An incomin message...");

    // Wait for user input
    printf("Enter something to kill the recieve thread...\n");
    char x;
    scanf("%c", &x);

    // Shutdown my modules
    Reciever_shutdown();

    printf("Done!!!\n");

    return 0;
}