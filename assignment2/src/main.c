#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

#include "list.h"
#include "user_display.h"
#include "user_reader.h"
#include "udp_rx.h"
#include "udp_tx.h"

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr, "Invalid Number of Input Arguments \n");
        exit(1);
    }
    List* tx_list = List_create();
    List* rx_list = List_create();

    udp_rx_init(argv[1], rx_list);
    udp_tx_init(argv[2], argv[3], tx_list);
    user_display_init(rx_list);
    user_reader_init(tx_list);
    // TODO - set main in a wait loop before killing all processes
    
    user_reader_destroy();
    udp_rx_destroy();
    udp_tx_destroy();
    user_display_destroy();
    
  
    printf("Finished Main\n");

    return(0);
}
