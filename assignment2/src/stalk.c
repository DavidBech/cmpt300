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

void stalk_initialize(char **argv){
    user_reader_init();
    udp_tx_init(argv[2], argv[3]);
    udp_rx_init(argv[1]);
    user_display_init();
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
    // TODO change the order?
    udp_rx_destroy();
    udp_tx_destroy();
    user_display_destroy();
    user_reader_destroy();
}