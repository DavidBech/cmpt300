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

static semaphore* sem_array[SEMAPHORE_NUM];

// Blocks the process on the semaphore
static void block_process(pcb* p_pcb, semaphore* p_sem);

// prints the list
static void print_list(int id);

// find pcb
static bool find_pcb(void* item0, void* item1);

// prints summary of semaphore
static void print_semaphore_summary(int id);

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
    semaphore* p_sem = sem_array[id];
    if(p_sem != NULL){
        printf("This semaphore has already been initialized\n");
        return KERNEL_SIM_FAILURE;
    }
    p_sem = malloc(sizeof(semaphore));
    p_sem->id = id;
    p_sem->value = value;
    p_sem->blocked = List_create();
    if(p_sem->blocked == NULL){
        printf("Failure Out of Queues\n");
        free(p_sem);
        return KERNEL_SIM_FAILURE;
    }
    sem_array[id] = p_sem;
    print_semaphore_summary(id);
    return KERNEL_SIM_SUCCESS;
}

bool semaphore_v(uint32_t id){
    if(id < 0 || id >= SEMAPHORE_NUM){
        printf("Invalid semaphore id: %d\n", id);
        return KERNEL_SIM_FAILURE;
    }
    semaphore* p_sem = sem_array[id];
    if(p_sem == NULL){
        printf("Semaphore%d is unititialized\n", id);
        return KERNEL_SIM_FAILURE;
    }

    if(p_sem->value == 0 && List_count(p_sem->blocked)){
        // Wake Blocked process on sem
        pcb* p_waked = List_trim(p_sem->blocked);
        if(pcb_get_pid(p_waked) == 0){
            // if waked is init process
            printf("Waked Init Process\n");

        } else {
            queue_manager_add_ready(p_waked);
            printf("Process readied: \n\t");
            pcb_print_all_info(p_waked);
        }
    } else {
        // No blocked process
        if(p_sem->value == 0xffffffff){
            printf("Semaphore cannot be incremented it is at max value\n");
            print_semaphore_summary(id);
            return KERNEL_SIM_FAILURE;
        }
        ++(p_sem->value);
    }
    print_semaphore_summary(id);
    return KERNEL_SIM_SUCCESS;
}

bool semaphore_p(uint32_t id, pcb* pCaller){
    if(id < 0 || id >= SEMAPHORE_NUM){
        printf("Invalid semaphore id: %d\n", id);
        return KERNEL_SIM_FAILURE;
    }
    semaphore* p_sem = sem_array[id];
    if(p_sem == NULL){
        printf("Semaphore%d is unititialized\n", id);
        return KERNEL_SIM_FAILURE;
    }
    if(p_sem->value == 0){
        block_process(pCaller, p_sem);
        print_semaphore_summary(id);
        printf("Blocking running process\n");
    } else {
        --p_sem->value;
        print_semaphore_summary(id);
        printf("Running process still running\n");
    }
    return KERNEL_SIM_SUCCESS;
}

bool semaphore_remove_blocked_pcb(pcb* p_pcb){
    List* p_pcb_list = pcb_get_location(p_pcb);
    if(p_pcb_list == PCB_INIT_LOC){
        return KERNEL_SIM_FAILURE;
    }
    int list_hash = semaphore_list_hash(p_pcb_list);
    if(list_hash == -1){
        return KERNEL_SIM_FAILURE;
    }
    List_first(p_pcb_list);
    List_search(p_pcb_list, find_pcb, p_pcb);
    List_remove(p_pcb_list);
    return KERNEL_SIM_SUCCESS;
}

void semaphore_print_all_info(){
    printf("Semaphores:\n");
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        printf("\t");
        print_semaphore_summary(i);
        print_list(i);
    }
}

int semaphore_list_hash(List* loc){
    if(loc == NULL){
        return -1;
    }
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        if(sem_array[i] == NULL){
            continue;
        }
        if(loc == sem_array[i]->blocked){
            return i;
        }
    }
    return -1;
}

void semaphore_shutdown(){
    for(int i=0; i<SEMAPHORE_NUM; ++i){
        semaphore* p_sem = sem_array[i];
        if(p_sem == NULL){
            continue;
        }
        List_free(sem_array[i]->blocked, pcb_free);
        free(p_sem);
    }
}

static void block_process(pcb* p_pcb, semaphore* p_sem){
    List* pBlocked_list = p_sem->blocked;
    pcb_set_state(p_pcb, STATE_BLOCKED);
    pcb_set_location(p_pcb, pBlocked_list);
    List_prepend(pBlocked_list, p_pcb);
}

static void print_list(int id){
    if(id < 0 || id >= SEMAPHORE_NUM){
        return;
    }
    semaphore * p_sem = sem_array[id];
    if(p_sem == NULL){
        return;
    }
    List* pList = p_sem->blocked;
    if(!List_count(pList)){
        return;
    }
    pcb* item;
    item = List_last(pList);
    do {
        printf("\t\t");
        pcb_print_all_info(item);
        item = List_prev(pList);
    } while(item != NULL);
}

static bool find_pcb(void* item0, void* item1){
    return item0 == item1;
}

static void print_semaphore_summary(int id){
    if(id < 0 || id >= SEMAPHORE_NUM){
        return;
    }
    semaphore* sem = sem_array[id];
    if(sem == NULL){
        printf("Seamphore%d: Uninitialized\n", id);
    } else {
       printf("Semaphore%d: value:%d Blocked Count:%d\n", sem->id, sem->value, List_count(sem->blocked));
    }
}
