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
    
    udp_rx_init(argv[1]);
    udp_tx_init(argv[2], argv[3]);
    user_display_init();
    user_reader_init();
    
    char user_input[MAX_USER_INPUT];
    printf("Input To End Program:\n");
    scanf("%s", user_input);


    udp_rx_destroy();
    udp_tx_destroy();
    user_display_destroy();
    user_reader_destroy();
  
    printf("Finished Main\n");

    return(0);
}
