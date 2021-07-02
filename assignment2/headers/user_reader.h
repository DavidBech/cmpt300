#include "list.h"

#ifndef _USER_READER_H_A2
#define _USER_READER_H_A2

// Initialize the udp transmition port 
//  creates a new thread that monitors the input port
void user_reader_init(void);

// Kills the thread and cleans up
void user_reader_destroy(void);

// Gets the next tx list item
//  msg: pointer to message retrieved from list
//  returns 0 on success and 1 on failure
void user_reader_txList_getNext(char** msg);

#endif