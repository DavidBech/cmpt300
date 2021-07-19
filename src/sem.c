#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sem.h"
#include "process_cb.h"
#include "kernel_sim.h"

static semaphore sem_array[SEMAPHORE_NUM];

void semaphore_init_startup(){
    memset(&sem_array, 0, sizeof(sem_array));
    printf("Initialized semaphores");
}

bool semaphore_init(uint32_t id, uint32_t value){
    if(id < 0 || id >=  SEMAPHORE_NUM){
        printf("Invalid sem ID: %d\n", id);
        return KERNEL_SIM_FAILURE;
    }
    if(value < 0){
        printf("Invalid inital value: %d\n", value);
        return KERNEL_SIM_FAILURE;
    }
    semaphore* sem = &sem_array[id];
    if(sem->init){
        printf("This semaphore has already been initialized\n");
        return KERNEL_SIM_FAILURE;
    }
    sem->id = id;
    sem->value = value;
    sem->blocked = List_create();
    sem->init = true;
    if(sem->blocked == NULL){
        printf("Failure Out of Queues\n");
        exit(EXIT_FAILURE);
    }
    return KERNEL_SIM_SUCCESS;
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
    printf("Semaphores:\n");
    printf("\t TODO\n");
}

