#include <stdio.h>
#include <stdlib.h>
#include <string.h>  


#include "list.h"
#include "user_display.h"
#include "user_reader.h"
#include "udp_rx.h"
#include "udp_tx.h"

int main(int argc, char** argv){
    char *endptr;
    long rx_port;
    long tx_port;
    char *tx_machine;

    rx_port = strtol(argv[1], &endptr, 10);
    if(endptr == argv[1]){
        fprintf(stderr, "Invalid Rx Port input: %s", argv[1]);
        exit(1);
    }
    tx_machine = argv[2];
    tx_port = strtol(argv[3], &endptr, 10);
    if(endptr == argv[3]){
        fprintf(stderr, "Invalid Tx Port input: %s", argv[3]);
        exit(1);
    }

    printf("rx_port: %ld \n", rx_port);
    printf("tx_machine: %s \n", tx_machine);
    printf("tx_port: %ld \n", tx_port);

    return(0);
}

