#include <stdlib.h>
#include <stdio.h>

#include "queue_manager.h"
#include "process_cb.h"
#include "kernel_sim.h"

static List* pReady_high = NULL;
static List* pReady_norm = NULL;
static List* pReady_low = NULL;
static List* pBlocked_send = NULL;
static List* pBlocked_receive = NULL;

static bool comapre_pcb_pointer(void* item0, void* item1);
static bool print_pcb(void* item0, void* item1);

const char* queue_manager_list_names[] = {"Ready (high)", 
                                          "Ready (norm)", 
                                          "Ready  (low)", 
                                          "Blocked (send)", 
                                          "Blocked  (Rec)"};

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
        fprintf(stderr, "At least one list is null in queue manager\n");
        exit(EXIT_FAILURE);
    }
    printf("Created Queues\n");
}

bool queue_manager_add_ready(pcb* p_pcb){
    int prio = pcb_get_priority(p_pcb);
    pcb_set_state(p_pcb, STATE_READY);
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
    pcb_set_state(p_pcb, STATE_BLOCKED);
    pcb_set_location(p_pcb, pBlocked_send);
    List_prepend(pBlocked_send, p_pcb);
}

void queue_manager_add_block_recieve(pcb* p_pcb){
    pcb_set_state(p_pcb, STATE_BLOCKED);
    pcb_set_location(p_pcb, pBlocked_receive);
    List_prepend(pBlocked_receive, p_pcb);
}

bool queue_manager_any_non_empty(){
    if(List_count(pReady_high) 
            || List_count(pReady_norm)
            || List_count(pReady_low)
            || List_count(pBlocked_send)
            || List_count(pBlocked_receive) 
        ){
        return 1;
    }
    return 0;
}

bool queue_manager_remove(pcb* p_pcb){
    List* pLoc_pcb = pcb_get_location(p_pcb);
    if(queue_manager_list_hash(pLoc_pcb) == INVALID_HASH){
        return KERNEL_SIM_FAILURE;
    }
    List_first(pLoc_pcb);
    if(List_search(pLoc_pcb, comapre_pcb_pointer, p_pcb)){
        // found
        List_remove(pLoc_pcb);
        return  KERNEL_SIM_SUCCESS;
    } else {
        // not found TODO should never get here
        return KERNEL_SIM_FAILURE;
    }
}

pcb* queue_manager_get_next_ready_exempt(pcb* exempted_process){
    queue_manager_add_ready(exempted_process);
    pcb* cur = queue_manager_get_next_ready();
    pcb_set_state(cur, STATE_RUNNING);
    return cur;
}

pcb* queue_manager_get_next_ready(void){
    if(List_count(pReady_high)){
        return List_trim(pReady_high);
    } else if(List_count(pReady_norm)){
        return List_trim(pReady_norm);
    } else if(List_count(pReady_low)){
        return List_trim(pReady_low);
    }
    // all ready queues empty
    return NULL;
}

void queue_manager_print_info(){
    printf("Queues: \n");
    if(List_count(pReady_high)){
        printf("Ready (high):\n");
        List_first(pReady_high);
        List_search(pReady_high, print_pcb, NULL);
    } else {
        printf("Ready (high): empty\n");
    }

    if(List_count(pReady_norm)){
        printf("Ready (norm):\n");
        List_first(pReady_norm);
        List_search(pReady_norm, print_pcb, NULL);
    } else {
        printf("Ready (norm): empty\n");
    }
    
    if(List_count(pReady_low)){
        printf("Ready (low):\n");
        List_first(pReady_low);
        List_search(pReady_low, print_pcb, NULL);
    } else {
        printf("Ready (low): empty\n");
    }

    if(List_count(pBlocked_send)){
        printf("Blocked (send):\n");
        List_first(pBlocked_send);
        List_search(pBlocked_send, print_pcb, NULL);
    } else {
        printf("Blocked (send): empty\n");
    }

    if(List_count(pBlocked_receive)){
        printf("Blocked (Rec):\n");
        List_first(pBlocked_receive);
        List_search(pBlocked_receive, print_pcb, NULL);
    } else {
        printf("Blocked (Rec): empty\n");
    }

}

int queue_manager_list_hash(List* pList){
    if(pList == pReady_high){
        return R_HIGH_HASH;
    } else if(pList == pReady_norm){
        return R_NORM_HASH;
    } else if (pList == pReady_low){
        return R_LOW_HASH;
    } else if (pList == pBlocked_send){
        return B_SEND_HASH;
    } else if (pList == pBlocked_receive){
        return B_RECEIVE_HASH;
    } else {
        return INVALID_HASH;
    }
}

static bool comapre_pcb_pointer(void* item0, void* item1){
    return item0 == item1;
}

static bool print_pcb(void* item0, void* item1){
    printf("\t");
    pcb_print_all_info(item0);
    return 0;
}
