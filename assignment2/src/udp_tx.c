#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 

#include "udp_tx.h"

static void* upd_transmit_loop(void* arg);

static pthread_t upd_tx_pid;
// TODO - add socket variables

static void* upd_transmit_loop(void* arg){
    printf("Started UPD Transmitor\n");
    // TODO - coninually wait for tx_list to have messages to send to other s-talk instance
    while(1){
        printf(" \b");
    }
    return NULL;
}

void udp_tx_init(char* tx_machine, char* tx_port){
    // TODO - Create UPD socket and bind it to the port
    pthread_create(&upd_tx_pid, NULL, upd_transmit_loop, NULL);
}

void udp_tx_destroy(){
    // Stops the thread
    pthread_cancel(upd_tx_pid);

    // TODO - Close the UPD socket

    // Waits until thread finishes before continuing 
    pthread_join(upd_tx_pid, NULL);
    printf("Fished UPD Transmitor\n");
}