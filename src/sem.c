#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sem.h"
#include "process_cb.h"
#include "kernel_sim.h"
#include "queue_manager.h"

static semaphore sem_array[SEMAPHORE_NUM];

// returns true on invalid semaphore id for initialized semaphores
static bool invalid_init_id(uint32_t id);

// Blocks the process on the semaphore
static void block_process(pcb* p_pcb, semaphore* p_sem);

void semaphore_init(){
    memset(&sem_array, 0, sizeof(sem_array));
    printf("Initialized semaphores");
}

bool semaphore_new(uint32_t id, uint32_t value){
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

bool semaphore_v(uint32_t id){
    if(invalid_init_id(id)){
        printf("Invalid semaphore id: %d\n", id);
        return KERNEL_SIM_FAILURE;
    }
    semaphore* p_sem = &sem_array[id];
    ++p_sem->value;
    if(p_sem->value == 1 && List_count(p_sem->blocked)){
        // Wake Blocked process on sem
        pcb* p_waked = List_trim(p_sem->blocked);
        pcb_set_state(p_waked, STATE_READY);
        queue_manager_add_ready(p_waked);
        printf("Process readied: ");
        pcb_print_all_info(p_waked);
        --p_sem->value; // P operation for waked process
    } else {
        // No blocked process
        printf("New Semaphore value: %d\n", p_sem->value);
    }
    return KERNEL_SIM_SUCCESS;
}

bool semaphore_p(uint32_t id, pcb* pCaller){
    if(invalid_init_id(id)){
        printf("Invalid semaphore id: %d\n", id);
        return KERNEL_SIM_FAILURE;
    }
    semaphore* p_sem = &sem_array[id];
    if(p_sem->value == 0){
        printf("Semiphore is zero, blocking running process\n");
        block_process(pCaller, p_sem);
    } else {
        --p_sem->value;
    }
    return KERNEL_SIM_SUCCESS;
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

static bool invalid_init_id(uint32_t id){
    return id < 0 || id >= SEMAPHORE_NUM || !sem_array[id].init;    
}

static void block_process(pcb* p_pcb, semaphore* p_sem){
    List* pBlocked_list = p_sem->blocked;
    pcb_set_state(p_pcb, STATE_BLOCKED);
    pcb_set_location(p_pcb, pBlocked_list);
    List_prepend(pBlocked_list, p_pcb);
}
