#ifndef _RECIEVER_H
#define _RECIEVER_H

// Start background recieve thread
// Using shared mutexes
// void Reciever_init(char* rxMessage, pthread_mutex_t sharedMutexWithOtheThread);
void Reciever_init(char* rxMessage);

// It is now thread safe
void Reciever_changeDynamicMessage(char* rxMessage);

// sTop bacground recieve thread and clean up
void Reciever_shutdown(void);

#endif