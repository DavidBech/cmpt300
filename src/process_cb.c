#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "process_cb.h"
#include "queue_manager.h"
#include "sem.h"

// Stack for keeping track of available pids 
// TODO TEST OUT OF PIDS
static struct pid_stack_s{
    int head_index;
    int next_index[PCB_MAX_PID + 1];
}pid_stack;

// gets the next valid pid
static int get_next_pid();

// returns the input pid to the pool of available pids
static void free_pid(int pid_to_free);

static int get_next_pid(){
    int next = pid_stack.head_index;
    pid_stack.head_index = pid_stack.next_index[pid_stack.head_index];
    return next;
}

static void free_pid(int pid_to_free){
    pid_stack.next_index[pid_to_free] = pid_stack.head_index;    
    pid_stack.head_index = pid_to_free;
}

void pcb_module_init(){
    assert(PCB_MAX_PID <= 0xEFFFFFFF);
    for(int i=PCB_MIN_PID; i<=PCB_MAX_PID; ++i){
        pid_stack.next_index[i] = i+1;
    }
    pid_stack.next_index[PCB_MAX_PID] = PCB_OUT_OF_PID;
    pid_stack.head_index = 0;
}

pcb* pcb_init(uint32_t prio, uint32_t state){
    uint32_t pid = get_next_pid();
    if(pid == PCB_OUT_OF_PID){
        printf("Out of PIDs\n");
        return NULL;
    }

    pcb* p_pcb = malloc(sizeof(pcb));
    if(p_pcb == NULL){
        printf("Out of Memory\n");
        return NULL;
    }
    p_pcb->location = NULL;
    p_pcb->field.pid = pid;
    p_pcb->field.prio = prio;
    p_pcb->field.state = state;
    p_pcb->message[0] = '\0';
    memset(&p_pcb->message_info, '\0', sizeof(p_pcb->message_info));
    return p_pcb;
}

pcb* pcb_clone(pcb* p_pcb_origional){
    uint32_t pid = get_next_pid();
    if(pid == PCB_OUT_OF_PID){
        printf("Out of PIDs\n");
        return NULL;
    }
    pcb* p_pcb = malloc(sizeof(pcb));
    if(p_pcb == NULL){
        printf("Out of Memory\n");
        return NULL;
    }
    p_pcb->location = NULL;
    p_pcb->field = p_pcb_origional->field;
    p_pcb->field.pid = get_next_pid();
    p_pcb->message[0] = '\0';
    memset(&p_pcb->message_info, '\0', sizeof(p_pcb->message_info));
    return p_pcb;
}

void pcb_free(void* p_pcb){
    if(p_pcb == NULL){
        return;
    }
    free_pid(((pcb*)p_pcb)->field.pid);
    free(p_pcb);
    p_pcb = NULL;
}

uint32_t pcb_get_pid(pcb* pPcb){
    return pPcb->field.pid;
}

uint32_t pcb_get_priority(pcb* pPcb){
    return pPcb->field.prio;
}

uint32_t pcb_get_state(pcb* pPcb){
    return pPcb->field.state;
}

void pcb_set_state(pcb* pPcb, uint32_t state){
    if(pPcb->field.prio == PRIO_INIT){
        return;
    }
    pPcb->field.state = state;
    if(state == STATE_RUNNING){
        pcb_set_location(pPcb, NULL);
    }
}

char* pcb_get_message(pcb* pPcb){
    return (pPcb->message);
}

void pcb_clear_message(pcb* pPcb){
    pPcb->message[0] = '\0';
}

void pcb_set_message(pcb* pPcb, char* msg){
    if(pPcb->message == msg){
        return;
    }
    strcpy(pPcb->message, msg);
    //TODO ERROR HANDLING
}

List* pcb_get_location(pcb* pPcb){
    return pPcb->location;
}

void pcb_set_location(pcb* pPcb, List* location){
    if(pPcb->location == PCB_INIT_LOC){
        return;
    }
    pPcb->location = location;
}

uint32_t pcb_get_message_pid(pcb* pPcb){
    return pPcb->message_info.pid;
}

void pcb_set_message_pid(pcb* pPcb, uint32_t pid){
    pPcb->message_info.pid = pid;
}

void pcb_set_received_message(pcb* pPcb){
    pPcb->message_info.received = 1;
}

void pcb_clear_received_message(pcb* pPcb){
    pPcb->message_info.received = 0;
}

uint32_t pcb_get_received_message(pcb* pPcb){
    return pPcb->message_info.received;
}

void pcb_set_sent_message(pcb* pPcb){
    pPcb->message_info.sent = 1;
}

void pcb_clear_sent_message(pcb* pPcb){
    pPcb->message_info.sent = 0;
}

uint32_t pcb_get_sent_message(pcb* pPcb){
    return pPcb->message_info.sent;
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
    printf("PID: %u, Prio: %s, State: %s, Queue: %s,", 
        pPcb->field.pid, 
        pPcb->field.prio == PRIO_HIGH ? "high" : pPcb->field.prio == PRIO_NORMAL ? "norm" : pPcb->field.prio == PRIO_LOW ? "low" : "init", 
        pPcb->field.state == STATE_RUNNING ? "running" : pPcb->field.state == STATE_READY ? "ready" : "blocked",
        list_location
        );
    
    char* msg = pcb_get_message(pPcb);
    if(pcb_get_received_message(pPcb)){
        printf(" Message (From %#04x): %s\n", pcb_get_message_pid(pPcb), msg);
    } else if (msg[0] != '\0' && !pcb_get_sent_message(pPcb)){
        printf(" Message (To %#04x): %s\n", pcb_get_message_pid(pPcb), msg);
    } else {
        printf(" Message: No Message\n");
    }
}
