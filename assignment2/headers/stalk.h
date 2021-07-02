#ifndef _STALK_H_A2
#define _STALK_H_A2

#ifdef DEBUG
    #include <time.h>
    struct tm get_start_time();
    #define STALK_LOG(_fd, _message, _time) fprintf(_fd, "%im:%is: %s, Line:%i, %s", _time.tm_min, _time.tm_sec, __FILE__, __LINE__, _message); fflush(_fd);
#else
    #define STALK_LOG(_fd, _message) ;
#endif

// Larger messages would have to be handled thourgh
// potentially multiple packets
#define MAX_MESSAGE_SIZE 1024
#define STALK_MAX_NUM_NODES LIST_MAX_NUM_NODES

// Initialize threads
void stalk_initialize(char **argv);

// Caller stalls until stalk_initiateShutdown is called by a different thread
void stalk_waitForShutdown();

// Wake up all processes that are wating for shutdown
void stalk_initiateShutdown();

// Shutdown all processes
void stalk_Shutdown();

#endif