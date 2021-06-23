#include "list.h"

#ifndef _USER_DISPLAY_H_A2
#define _USER_DISPLAY_H_A2
// Initialize the user display 
//  creates a new thread that monitors the input port
void user_display_init(void);

// Kills the thread and cleans up
void user_display_destroy(void);

// Adds an item to the rxList 
//  msg: pointer to message to add to list
//  returns 0 on success and 1 on failure
bool user_display_rxList_add(char* msg);

// Allocates memory for new message to add to rxList
//  msg: output pointer to new allocated memory
//  returns 0 on success and 1 onfailure
bool user_display_allocate_message(char* msg);

#endif