#include <stdbool.h>

#include "kernel_sim.h"
#include "list.h"
#include "process_cb.h"

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#define SEMAPHORE_NUM 5
extern const char* semaphore_queue_names[];

typedef struct semaphore_s semaphore;
struct semaphore_s {
    List* blocked;
    uint32_t value;
    uint8_t id;
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

// removes a pcb blocked on any semaphore
//  return: 1 if the pcb was not blocked on any semaphore
//  return: 0 if the pcb was removed
bool semaphore_remove_blocked_pcb(pcb* pPcb);

// Prints all semaphore info
void semaphore_print_all_info(void);

int semaphore_list_hash(List* loc);

void semaphore_shutdown();

#endif
