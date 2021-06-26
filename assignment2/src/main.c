#include <stdio.h>
#include <stdlib.h>

#include "stalk.h"

int main(int argc, char** argv){
    // Ensure there is the correct amount of arguments
    if(argc != 4){
        fprintf(stderr, "Invalid Number of Input Arguments \n");
        exit(EXIT_FAILURE);
    }
 
    // Initialize Modules
    stalk_initialize(argv);
        
    // Wait for shutdown from either user_reader or udp_rx module
    // TODO user_reader should not send the shutdown signal
    //      udp_tx should as it ensures that the shutdown message will
    //      be sent to the other stalk instance
    stalk_waitForShutdown();

    // Clean up all threads
    stalk_Shutdown();

    //printf("Finished Main\n");
    return(0);
}
