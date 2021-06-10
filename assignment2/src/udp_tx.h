#include "list.h"

#ifndef _UDP_TX_H_A2
#define _UDP_TX_H_A2
// Initialize the udp transmition port 
//  creates a new thread that monitors the input port
void udp_tx_init( char* tx_machine, char* tx_port, List* tx_list);

// Kills the thread and cleans up
void udp_tx_destroy(void);

#endif