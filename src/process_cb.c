#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "process_cb.h"
#include "queue_manager.h"
#include "sem.h"

void pcb_init(pcb* pPcb, uint32_t pid, uint32_t prio, uint32_t state){
    pPcb->field.pid = pid;
    pPcb->field.prio = prio;
    pPcb->field.state = state;
    pPcb->field.in_use = 1;
    pPcb->message[0] = '\0';
    pPcb->location = NULL;
}

bool pcb_clone(pcb* pPcb_new, pcb* pPcb_origional, uint32_t new_pid){
    pPcb_new->field = pPcb_origional->field;
    pPcb_new->field.pid = new_pid;
    return PCB_PASS;
}

void pcb_free(pcb* pPcb){
    memset(pPcb, 0, sizeof(pcb));
}

uint32_t pcb_get_pid(pcb* pPcb){
    return pPcb->field.pid;
}

uint32_t pcb_get_state(pcb* pPcb){
    return pPcb->field.state;
}

void pcb_set_state(pcb* pPcb, uint32_t state){
    pPcb->field.state = state;
    if(state == STATE_RUNNING){
        pcb_set_location(pPcb, NULL);
    }
}

uint32_t pcb_get_priority(pcb* pPcb){
    return pPcb->field.prio;
}

char* pcb_get_message(pcb* pPcb){
    return (pPcb->message);
}

void pcb_clear_message(pcb* pPcb){
    pPcb->message[0] = '\0';
}

void pcb_set_message(pcb* pPcb, char* msg){
    strcpy(pPcb->message, msg);
    //TODO ERROR HANDLING
}

void pcb_set_location(pcb* pPcb, List* location){
    if(pPcb->location == PCB_INIT_LOC){
        return;
    }
    pPcb->location = location;
}

List* pcb_get_location(pcb* pPcb){
    return pPcb->location;
}

void pcb_print_all_info(pcb* pPcb){
    const char* list_location;
    if(pPcb->location == PCB_INIT_LOC){
        list_location = "Init";
    } else {
        int loc = queue_manager_list_hash(pPcb->location);
        if(loc != -1){
            list_location = queue_manager_list_names[loc];
        } else {
            loc = semaphore_list_hash(pPcb->location);
            if(loc != -1){
                list_location = semaphore_queue_names[loc];
            } else {
                list_location = "None";
            }
        }
    }
    printf("PID: %#04x, Prio: %s, State: %s, Queue: %s, Message:%s;\n", 
        pPcb->field.pid, 
        pPcb->field.prio == PRIO_HIGH ? "high" : pPcb->field.prio == PRIO_NORMAL ? "norm" : pPcb->field.prio == PRIO_LOW ? " low" : "init", 
        pPcb->field.state == STATE_RUNNING ? "running" : pPcb->field.state == STATE_READY ? "ready" : "blocked",
        list_location,
        pPcb->message);
}
