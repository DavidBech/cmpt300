#include <stdlib.h>
#include <stdio.h>

#include "queue_manager.h"
#include "kernel_sim.h"

static List* pReady_high = NULL;
static List* pReady_norm = NULL;
static List* pReady_low = NULL;
static List* pBlocked_send = NULL;
static List* pBlocked_receive = NULL;

void queue_manager_init(void){
    pReady_high = List_create();
    pReady_norm = List_create();
    pReady_low = List_create();
    pBlocked_send = List_create();
    pBlocked_receive = List_create();
    if(pReady_high == NULL
        || pReady_norm == NULL
        || pReady_low == NULL
        || pBlocked_send == NULL
        || pBlocked_receive == NULL){
        fprintf(stderr, "At least one list is null\n");
        exit(EXIT_FAILURE);
    }
    printf("Created Queues\n");
}

bool queue_manager_add_ready(pcb* p_pid){
    return KERNEL_SIM_FAILURE;
}

bool queue_manager_add_block_send(pcb* p_pid){
    return KERNEL_SIM_FAILURE;
}

bool queue_manager_add_block_recieve(pcb* p_pid){
    return KERNEL_SIM_FAILURE;
}

bool queue_manager_remove(pcb* p_pid){
    return KERNEL_SIM_FAILURE;
}

pcb* queue_manager_get_next_ready_exempt(pcb* exempted_process){
    return NULL;
}

pcb* queue_manager_get_next_ready(void){
    return NULL;
}

void queue_manager_print_info(){

}

