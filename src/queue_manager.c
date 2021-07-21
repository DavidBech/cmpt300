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
static bool compare_message_pid(void* item0, void* item1);
static void print_list(List* pList);

const char* queue_manager_list_names[] = {"Ready (high)", 
                                          "Ready (norm)", 
                                          "Ready  (low)", 
                                          "Blocked (send)", 
                                          "Blocked (rec)"};

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
    switch(prio){
        case(PRIO_HIGH): 
            pcb_set_state(p_pcb, STATE_READY);
            List_prepend(pReady_high, p_pcb);
            pcb_set_location(p_pcb, pReady_high);
            break;
        case(PRIO_NORMAL):
            pcb_set_state(p_pcb, STATE_READY);
            List_prepend(pReady_norm, p_pcb);
            pcb_set_location(p_pcb, pReady_norm);
            break;
        case(PRIO_LOW):
            pcb_set_state(p_pcb, STATE_READY);
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

bool queue_manager_any_ready_non_empty(){
    if(List_count(pReady_high) 
            || List_count(pReady_norm)
            || List_count(pReady_low)
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
    return cur;
}

pcb* queue_manager_get_next_ready(void){
    pcb* p_pcb = NULL;
    if(List_count(pReady_high)){
        p_pcb = List_trim(pReady_high);
        pcb_set_state(p_pcb, STATE_RUNNING);    
    } else if(List_count(pReady_norm)){
        p_pcb = List_trim(pReady_norm);
        pcb_set_state(p_pcb, STATE_RUNNING);    
    } else if(List_count(pReady_low)){
        p_pcb = List_trim(pReady_low);
        pcb_set_state(p_pcb, STATE_RUNNING);    
    }
    // all ready queues empty
    return p_pcb;
}

bool queue_manager_check_block_recieve(pcb* p_pcb){
    return p_pcb->location == pBlocked_receive;
}

pcb* queue_manager_check_block_send(uint32_t pid){
    List_first(pBlocked_send);
    return (pcb*)List_search(pBlocked_send, compare_message_pid, &pid);
}

bool queue_manager_check_blocked_on_send(pcb* p_pcb){
    return pcb_get_location(p_pcb) == pBlocked_send;
}

void queue_manager_print_info(){
    printf("Queues: \n");
    printf("\tReady (high): ");
    print_list(pReady_high);
    
    printf("\tReady (norm): ");
    print_list(pReady_norm);
    
    printf("\tReady (low): ");
    print_list(pReady_low);
    
    printf("\tBlocked (send): ");
    print_list(pBlocked_send);

    printf("\tBlocked (rec): ");
    print_list(pBlocked_receive);
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

void queue_manager_shutdown(){
    List_free(pReady_high, pcb_free);
    List_free(pReady_norm, pcb_free);
    List_free(pReady_low, pcb_free);
    List_free(pBlocked_send, pcb_free);
    List_free(pBlocked_receive, pcb_free);
}

static bool comapre_pcb_pointer(void* item0, void* item1){
    return item0 == item1;
}

static bool compare_message_pid(void* list_item, void* pid){
    pcb* p_pcb = (pcb*)list_item;
    if(!pcb_get_sent_message(p_pcb)){
        return p_pcb->message_info.pid == *((uint32_t*)pid);    
    }
    return 0;
}

static void print_list(List* pList){
    if(!List_count(pList)){
        printf("Empty\n");
        return;
    } else {
        printf("\n");
    }
    pcb* item;
    item = List_last(pList);
    do {
        printf("\t\t");
        pcb_print_all_info(item);
        item = List_prev(pList);

    } while(item != NULL);
}
