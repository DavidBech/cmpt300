#ifndef _STALK_H_A2
#define _STALK_H_A2

// Larger messages would have to be handled thourgh
// potentially multiple packets
#define MAX_MESSAGE_SIZE 1024

// Initialize threads
void stalk_initialize(char **argv);

// Caller stalls until stalk_initiateShutdown is called by a different thread
void stalk_waitForShutdown();

// Wake up all processes that are wating for shutdown
void stalk_initiateShutdown();

// Shutdown all processes
void stalk_Shutdown();

#endif