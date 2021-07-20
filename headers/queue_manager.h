#include <stdbool.h>

#include "process_cb.h"

#ifndef __QUEUE_MANAGER_H
#define __QUEUE_MANAGER_H

#define R_HIGH_HASH 0
#define R_NORM_HASH 1
#define R_LOW_HASH 2
#define B_SEND_HASH 3
#define B_RECEIVE_HASH 4
#define INVALID_HASH -1

extern const char* queue_manager_list_names[];

// Creates manditory queues
void queue_manager_init(void);

// Adds the provided pcb to its appropriate ready queue
//  return: 1 if the provided pcb has an invalid priority
//  return: 0 otherwise
bool queue_manager_add_ready(pcb* p_pcb);

// Adds the provided pcb to the queue for processes blocked on send command
void queue_manager_add_block_send(pcb* p_pcb);

// Adds the provided pcb to the queue for processes blocked on recieve command
void queue_manager_add_block_recieve(pcb* p_pcb);

// Returns wether the process is currently on the block for recieve queue
// return: 1 if the the process is blocked on the recieve queue
// return: 0 otherwise
bool queue_manager_check_block_recieve(pcb* p_pcb);

// Returns if the pid is currently being sent a message on the send queue
// return: the pcb sending the message to the pid
// return: NULL if not being sent a message
pcb* queue_manager_check_block_send(uint32_t pid);

// Returns weather the input pcb is on the blockd_send queue
// return: 1 if the pcb is on the queue
// return: 0 otherwise
bool queue_manager_check_blocked_on_send(pcb* p_pcb);

// Checks if any of the queues are non-empty
//  return: 1 if a queue is non-empty
//  return: 0 otherwise
bool queue_manager_any_non_empty();

// Checks if any of the ready queues are non-empty
//  return: 1 if a queue is non-empty
//  return: 0 otherwise
bool queue_manager_any_ready_non_empty();

// Removes the pcb from the queue its currently in
//  return: 1 if the pcb is not currently in a queue handled by queue_manager
//  return: 0 otherwise
bool queue_manager_remove(pcb* p_pcb);

// Adds the exempted_process to its ready queue returning the next
//  process to execute setting its state to running
//  return: the next process who's turn it is to be running which 
//              could be the same as the exempted_process
pcb* queue_manager_get_next_ready_exempt(pcb* exempted_process);

// Gets the next process who's turn it to be running next setting its state to running
//  note, this process is removed from its ready queue
//  return: the next process who's turn it is to be running
//  return: NULL if no such process exists
pcb* queue_manager_get_next_ready(void);

// Prints all info about all queues
void queue_manager_print_info();

// Returns has for the list
int queue_manager_list_hash();

// Cleans up the queues
void queue_manager_shutdown();
#endif