#include <stdbool.h>
#include <stdint.h>

#ifndef __EXECUTIONER_H_
#define __EXECUTIONER_H_

// Initiate invironment
void executioner_init();

// create command
//  input: priority of new process
//  return: 0 on success
//          1 on failure 
bool executioner_create(uint32_t prio);

// fork command
//  return: 0 on success
//          1 on failure 
bool executioner_fork(void);

// kill command
//  input: process ID to kill
//  return: 0 on success 
//          1 on failure
//          2 if the simulation should terminate
int executioner_kill(uint32_t pid);

// exit command
//  return: 0 on success
//          1 on failure
//          2 if the simulation should terminate
int executioner_exit(void);

// quantum comand
//  return: 0 on success
//          1 on failure
bool executioner_quantum(void);

// send command
//  input: Process ID of receiver
//         Message to send to receiver
//  return: 0 on success
//          1: on failure
bool executioner_send(uint32_t pid, char* msg);

// receive command
//  return: 0 on success
//          1: on failure
bool executioner_receive(void);

// reply command 
//  input: Process ID of sender to reply to
//         message to reply to sender
//  return: 0 on success
//          1: on failure
bool executioner_reply(uint32_t pid, char* msg);

// new semaphore command
//  return: 0 on success
//          1: on failure
bool executioner_semaphore_new(uint32_t sem_id, uint32_t init_value);

// semaphore p command
//  return: 0 on success
//          1: on failure
bool executioner_semaphore_p(uint32_t sem_id);

// semaphore v command
//  return: 0 on success
//          1: on failure
bool executioner_semaphore_v(uint32_t sem_id);

// procinfo command
//  return: 0 on success
//          1: on failure
bool executioner_procinfo(uint32_t pid);

// totalinfo command
//  return: 0 on success
//          1: on failure
bool executioner_totalinfo(void);

// Frees any memory allocated by this module
void executioner_module_shutdown(void);
#endif