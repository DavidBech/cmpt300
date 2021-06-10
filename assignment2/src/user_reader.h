#ifndef _USER_READER_H_A2
#define _USER_READER_H_A2
#define MAX_USER_INPUT 100

// Initialize the udp transmition port 
//  creates a new thread that monitors the input port
void user_reader_init(List* tx_list);

// Kills the thread and cleans up
void user_reader_destroy(void);

#endif