#include "list.h"

#ifndef _USER_DISPLAY_H_A2
#define _USER_DISPLAY_H_A2
// Initialize the user display 
//  creates a new thread that monitors the input port
void user_display_init(List* rx_list);

// Kills the thread and cleans up
void user_display_destroy(void);

#endif