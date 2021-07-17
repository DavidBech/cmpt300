#include <stdbool.h>

#include "kernel_sim.h"
#include "list.h"
#include "process_cb.h"

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#define SEMAPHORE_MIN_ID 0
#define SEMAPHORE_MAX_ID 4

typedef struct semaphore_s semaphore;
struct semaphore_s {
    uint32_t id;
    int value;
    List* blocked;
};
// Create new semaphore
bool semaphore_init(uint32_t id, uint32_t value);

// increment sem
bool semaphore_v(uint32_t id, pcb* pCaller);

// decrament sem
bool semaphore_p(uint32_t id, pcb* pCaller);

// checks if this pPcb is blocked on any semaphore
bool semaphore_block_on_sem(pcb* pPcb);
#endif
