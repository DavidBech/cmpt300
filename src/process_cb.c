#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "process_cb.h"

void pcb_init(pcb* pPcb, uint32_t pid, uint32_t prio, uint32_t state){
    pPcb->field.pid = pid;
    pPcb->field.prio = prio;
    pPcb->field.state = state;
    pPcb->message[0] = '\0';
    pPcb->location = NULL;
}

bool pcb_clone(pcb* pPcb_new, pcb* pPcb_origional, uint32_t new_pid){
    // TODO
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

void pcb_print_all_info(pcb* pPcb){
    // TODO
    printf("info\n");
}
