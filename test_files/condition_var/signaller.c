#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include "general.h"
#include "signaller.h"
#include "printer.h"

pthread_t threadSignaller;
static const char* MESSAGE = "Hello World in Sync!";

void* signallerThread(void* unused)
{
    for (const char* msg = MESSAGE; *msg != '\0'; msg++)
    {
        sleep_msec(100);
        // Signal other thread
        Printer_signalNextChar();
    }
    return NULL;
}


void Signaller_init()
{
    pthread_create(&threadSignaller, NULL, signallerThread, NULL);  
}

void Signaller_waitForShutdown()
{
    pthread_join(threadSignaller, NULL);
}
