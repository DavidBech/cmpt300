#include <stdbool.h>
#include <stdint.h>

#include "semaphore.h"
#include "process_cb.h"

bool semaphore_init(uint32_t id, uint32_t value){
    return 1;
}

bool semaphore_v(uint32_t id, pcb* pCaller){
    return 1;
}


bool semaphore_p(uint32_t id, pcb* pCaller){
    return 1;
}


bool semaphore_block_on_sem(pcb* pPcb){
    return false;
}

