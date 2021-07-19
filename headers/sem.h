#include <stdbool.h>

#include "kernel_sim.h"
#include "list.h"
#include "process_cb.h"

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#define SEMAPHORE_NUM 5

typedef struct semaphore_s semaphore;
struct semaphore_s {
    int value;
    List* blocked;
    uint8_t id;
    bool init;
};

// Initalize the semaphore module
void semaphore_init();

// Create new semaphore
//  return: 1 failure
//  return: 0 success
bool semaphore_new(uint32_t id, uint32_t value);

// increment semaphore potentially adding a waked process to ready queue
//  return: 1 failure
//  return: 0 success
bool semaphore_v(uint32_t id);

// decrament sem pCaller may be blocked after this call 
//  return: 1 failure
//  return: 0 success
bool semaphore_p(uint32_t id, pcb* pCaller);

// checks if this pPcb is blocked on any semaphore
//  return: 1 if the provided pcb is blocked on any initialized semaphore
//  return: 0 otherwise
bool semaphore_block_on_sem(pcb* pPcb);

// removes a pcb blocked on any semaphore
//  return: 1 if the pcb was not blocked on any semaphore
//  return: 0 if the pcb was removed
bool semaphore_remove_blocked_pcb(pcb* pPcb);

// checks if any semaphore has a blocked pcb on it
//  return: 1 if there is a blocked process
//  return: 0 if no processes are blocked
bool semaphore_any_blocked();

// Prints all semaphore info
void semaphore_print_all_info(void);

#endif
