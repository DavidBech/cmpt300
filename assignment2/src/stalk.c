#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "stalk.h"
#include "user_reader.h"
#include "user_display.h"
#include "udp_rx.h"
#include "udp_tx.h"

static pthread_cond_t shutdown_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t shutdown_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef DEBUG
    // Get the start time of stalk process used in logging messages
    static struct tm start_time;
    // Should be called before the process is made this is not thread safe
    struct tm get_start_time(){
        return start_time;
    }
#endif

void stalk_initialize(char **argv){
    #ifdef DEBUG
        // Setup the start time of stalk
        time_t time_temp;
        time(&time_temp);
        start_time = *localtime(&time_temp);
    #endif
    udp_rx_init(argv[1]);
    udp_tx_init(argv[2], argv[3]);
    user_display_init();
    user_reader_init();
}

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
        // difference between broadcast and signal is that
        // broadcast will ensure that all waiting processes are released
        pthread_cond_broadcast(&shutdown_cond);
    }
    pthread_mutex_unlock(&shutdown_mutex);
}

void stalk_Shutdown(){
    user_reader_destroy();
    udp_rx_destroy();
    udp_tx_destroy();
    user_display_destroy();
}