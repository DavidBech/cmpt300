#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h> 

#include "user_reader.h"
#include "udp_tx.h"
#include "list.h"
#include "stalk.h"

// Loop that thread runs till cancled
static void* upd_transmit_loop(void* arg);

static pthread_t upd_tx_pid;
// TODO - add socket variables

void udp_tx_init(char* tx_machine, char* tx_port){
    // TODO - Create UPD socket and bind it to the port

    // Launch UDP tx thread
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


static void* upd_transmit_loop(void* arg){
    printf("Started UPD Transmitor\n");
    // TODO - coninually wait for tx_list to have messages to send to other s-talk instance
    char* msg = NULL;
    while(1){
        pthread_testcancel();
        if(user_reader_txList_getNext(msg)){
            // TODO ERROR HANDLING with recieved message
        }
        // TODO -- if msg == termination
        //  Send termination to other stalk instance
        //  call stalk_initiateShutdown
        
        // TODO -- else
        //  Send regulare message to other stalk instance
    }
    return NULL;
}