#include "list.h"

#ifndef _USER_DISPLAY_H_A2
#define _USER_DISPLAY_H_A2
// Initialize the user display 
//  creates a new thread that monitors the input port
void user_display_init(void);

// Kills the thread and cleans up
void user_display_destroy(void);

// Gets the next tx list item
//  msg: pointer to message retrieved from list
//  returns 0 on success and 1 on failure
bool user_display_rxList_add(char* msg);

#endif