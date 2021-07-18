#include <stdlib.h>
#include <stdio.h>

#include "queue_manager.h"
#include "kernel_sim.h"

static List* pReady_high = NULL;
static List* pReady_norm = NULL;
static List* pReady_low = NULL;
static List* pBlocked_send = NULL;
static List* pBlocked_receive = NULL;

static bool comapre_pcb_pointer(void* item0, void* item1);

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

bool queue_manager_add_ready(pcb* p_pcb){
    int prio = pcb_get_priority(p_pcb);
    if(pcb_get_state(p_pcb) != STATE_READY){
        return KERNEL_SIM_FAILURE;
    }
    switch(prio){
        case(PRIO_HIGH): 
            List_prepend(pReady_high, p_pcb);
            pcb_set_location(p_pcb, pReady_high);
            break;
        case(PRIO_NORMAL):
            List_prepend(pReady_norm, p_pcb);
            pcb_set_location(p_pcb, pReady_norm);
            break;
        case(PRIO_LOW):
            List_prepend(pReady_low, p_pcb);
            pcb_set_location(p_pcb, pReady_low);
            break;
        default:
            return KERNEL_SIM_FAILURE;
    }
    return KERNEL_SIM_SUCCESS;
}

void queue_manager_add_block_send(pcb* p_pcb){
    List_prepend(pBlocked_send, p_pcb);
    pcb_set_location(p_pcb, pBlocked_send);
}

void queue_manager_add_block_recieve(pcb* p_pcb){
    List_prepend(pBlocked_receive, p_pcb);
    pcb_set_location(p_pcb, pBlocked_receive);
}

bool queue_manager_remove(pcb* p_pcb){
    List* pLoc_pcb = pcb_get_location(p_pcb);
    if(pLoc_pcb == NULL){
        return KERNEL_SIM_FAILURE;
    }
    List_first(pLoc_pcb);
    if(List_search(pLoc_pcb, comapre_pcb_pointer, p_pcb)){
        // found
        List_remove(pLoc_pcb);
        return  KERNEL_SIM_SUCCESS;
    } else {
        // not found
        return KERNEL_SIM_FAILURE;
    }
    return KERNEL_SIM_SUCCESS;
}

pcb* queue_manager_get_next_ready_exempt(pcb* exempted_process){
    return NULL;
}

pcb* queue_manager_get_next_ready(void){
    return NULL;
}

void queue_manager_print_info(){

}

static bool comapre_pcb_pointer(void* item0, void* item1){
    return item0 == item1;
}
