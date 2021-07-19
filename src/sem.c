#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sem.h"
#include "process_cb.h"
#include "kernel_sim.h"
#include "queue_manager.h"

const char* semaphore_queue_names[] = {"Sem0","Sem1","Sem2","Sem3","Sem4"};

static semaphore sem_array[SEMAPHORE_NUM];

// returns true on invalid semaphore id for initialized semaphores
static bool invalid_init_id(uint32_t id);

// Blocks the process on the semaphore
static void block_process(pcb* p_pcb, semaphore* p_sem);

// prints the pcb used in search list method
static bool print_pcb(void* item0, void* item1);

void semaphore_init(){
    memset(&sem_array, 0, sizeof(sem_array));
    printf("Initialized semaphores\n");
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
    if(p_sem->value == 0 && List_count(p_sem->blocked)){
        // Wake Blocked process on sem
        pcb* p_waked = List_trim(p_sem->blocked);
        if(pcb_get_pid(p_waked) == 0){
            // if waked is init process
            --(p_sem->init_count);
        } else {
            queue_manager_add_ready(p_waked);
            printf("Process readied: ");
            pcb_print_all_info(p_waked);
        }
    } else {
        // No blocked process
        ++(p_sem->value);
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
        printf("Semaphore is 0, blocking running process\n");
        block_process(pCaller, p_sem);
    } else {
        --p_sem->value;
        printf("Semaphore is %d, running process still running\n", p_sem->value);
    }
    return KERNEL_SIM_SUCCESS;
}

bool semaphore_block_on_sem(pcb* pPcb){
    return 0;
}

bool semaphore_any_blocked(){
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        if(invalid_init_id(i)){
            continue;
        }
        if(List_count(sem_array[i].blocked)){
            return 1; // there is a blocked process
        }
    }
    return 0;
}

bool semaphore_remove_blocked_pcb(pcb* p_pcb){
    return 1;
}

void semaphore_print_all_info(){
    printf("Semaphores:\n");
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        if(invalid_init_id(i)){
            printf("Semaphore%d: Uninitialized\n", i);
            continue;
        }
        semaphore* p_sem = &sem_array[i];
        printf("Semaphore%d: value:%d Blocked:", i, p_sem->value);
        if(List_count(p_sem->blocked)){
            printf("\n");
            List_first(p_sem->blocked);
            List_search(p_sem->blocked, print_pcb, NULL);
        } else {
            printf(" None\n");
        }
    }
}

int semaphore_list_hash(List* loc){
    if(loc == NULL){
        return -1;
    }
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        if(loc == sem_array[i].blocked){
            return i;
        }
    }
    return -1;
}

static bool invalid_init_id(uint32_t id){
    return id < 0 || id >= SEMAPHORE_NUM || !sem_array[id].init;    
}

static void block_process(pcb* p_pcb, semaphore* p_sem){
    List* pBlocked_list = p_sem->blocked;
    pcb_set_state(p_pcb, STATE_BLOCKED);
    if(pcb_get_pid(p_pcb) != 0){
        pcb_set_location(p_pcb, pBlocked_list);
    } else {
        ++p_sem->init_count;
    }
    List_prepend(pBlocked_list, p_pcb);
}

static bool print_pcb(void* item0, void* item1){
    printf("\t");
    pcb_print_all_info(item0);
    return 0;
}
