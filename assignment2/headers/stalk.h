#ifndef _STALK_H_A2
#define _STALK_H_A2

// Larger messages handled thourgh multiple packets
#define MAX_MESSAGE_SIZE 1024
#define STALK_MAX_NUM_NODES LIST_MAX_NUM_NODES
#define TERMINATION_STRING "!\n\0"

// Initialize threads
void stalk_initialize(char **argv);

// Caller stalls until stalk_initiateShutdown is called by a different thread
void stalk_waitForShutdown();

// Wake up all processes that are wating for shutdown
void stalk_initiateShutdown();

// Shutdown all processes
void stalk_Shutdown();

#endif