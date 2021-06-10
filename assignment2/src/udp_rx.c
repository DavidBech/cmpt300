#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 

#include "udp_rx.h"
#include "list.h"

static void* upd_recieve_loop(void* arg);

static pthread_t upd_rx_pid;
// TODO - add socket variables

static void* upd_recieve_loop(void* arg){
    printf("Started UPD Receiver\n");
    // TODO - coninually read upd socket adding recieved messages to rx_list
    while(1){
        pthread_testcancel();
    }
    printf("should never happen\n");
    return NULL;
}

void udp_rx_init(char* rx_port, List* rx_list){
    // TODO - Create UPD socket and bind it to the port
    pthread_create(&upd_rx_pid, NULL, upd_recieve_loop, NULL);
}

void udp_rx_destroy(){
    // Stops the thread
    pthread_cancel(upd_rx_pid);

    // TODO - Close the UPD socket

    // Waits until thread finishes before continuing 
    pthread_join(upd_rx_pid, NULL);
    printf("Fished UPD Receiver\n");
}