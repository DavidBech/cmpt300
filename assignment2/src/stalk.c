#include <pthread.h>

#include "stalk.h"

static pthread_cond_t shutdown_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t shutdown_mutex = PTHREAD_MUTEX_INITIALIZER;

void stalk_waitForShutdown(){
    // Wait for signal on shutdown_cond
    pthread_mutex_lock(&shutdown_mutex);
    {
        pthread_cond_wait(&shutdown_cond, &shutdown_mutex);
    }
    pthread_mutex_unlock(&shutdown_mutex);
}

void stalk_initiateShutdown(){
    // Send signal to shutdown_cond
    pthread_mutex_lock(&shutdown_mutex);
    {
        pthread_cond_signal(&shutdown_cond);
    }
    pthread_mutex_unlock(&shutdown_mutex);
}