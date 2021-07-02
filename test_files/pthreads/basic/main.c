#include <stdio.h>
#include <string.h>
#include "general.h"
#include <pthread.h>

void* slowPrintThread(void* msAsVoid)
{
    char* msg = msAsVoid;
    for (int i = 0; i < strlen(msg); i++)
    {
        printf("%c", msg[i]);
        // To help print things out on the screen character by character
        fflush(stdout);
        sleep_msec(200);
    }
    return NULL;
}

int main(int argc, char** args) 
{
    printf("***DONE PROGRAM****\n");

    pthread_t threadPID;

    pthread_create(
        &threadPID,     // PID (by pointer)
        NULL,           // Attributes
        slowPrintThread,// function
        "This is the new argument!\n");          // Arguments

    pthread_t threadPID2;
    pthread_create(
        &threadPID2,     // PID (by pointer)
        NULL,           // Attributes
        slowPrintThread,// function
        "************************** OOPS");          // Arguments

    pthread_join(threadPID, NULL);
    pthread_join(threadPID2, NULL);

    return 0;
}