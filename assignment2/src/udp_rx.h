#ifndef _UDP_RX_H_A2
#define _UDP_RX_H_A2
// Initialize the udp receiving port 
//  creates a new thread that monitors the input port
void udp_rx_init(char* rx_port, List* rx_list);

// Kills the thread and cleans up 
void udp_rx_destroy(void);

#endif