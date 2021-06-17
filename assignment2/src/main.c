#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

#include "stalk.h"
#include "list.h"
#include "user_display.h"
#include "user_reader.h"
#include "udp_rx.h"
#include "udp_tx.h"

int main(int argc, char** argv){
    // Ensure there is the correct amount of arguments
    if(argc != 4){
        fprintf(stderr, "Invalid Number of Input Arguments \n");
        exit(EXIT_FAILURE);
    }
 
    // Initialize Modules
    udp_rx_init(argv[1]);
    udp_tx_init(argv[2], argv[3]);
    user_display_init();
    user_reader_init();
    
    // Wait for shutdown from either user_reader or udp_rx module
    stalk_waitForShutdown();

    // Clean up all threads
    user_reader_destroy();
    udp_rx_destroy();
    udp_tx_destroy();
    user_display_destroy();

    printf("Finished Main\n");
    return(0);
}
