#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
    return 0;
}

bool semaphore_any_blocked(){
    return 0;
}

bool semaphore_remove_blocked_pcb(pcb* p_pcb){
    return 1;
}

void semaphore_print_all_info(){
    printf("Semaphores\n");
    printf("\t TODO\n");
}

