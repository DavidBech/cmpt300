#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#ifndef __PROCESS_CB_H
#define __PROCESS_CB_H

// Process Control Block
//  -- This file defines an api for creation/deletion/modification to pcb structure

// Max IPC message to fit in PCB, this include \0 terminator
#define PCB_ICP_MESSAGE_SIZE 40
// Minimum PID value
#define PCB_MIN_PID 0
// Maximum PID value
#define PCB_MAX_PID 0xFFFF

#define PCB_FAIL 1 
#define PCB_PASS 0

#define PCB_INIT_LOC (List*)0xFFFFFFFF

// Process Control Block State
enum PCB_STATE{
    STATE_RUNNING = 0,
    STATE_READY = 1,
    STATE_BLOCKED = 2
};

// Process Control Block Priority
enum PCB_PRIO{
    PRIO_HIGH = 0,
    PRIO_NORMAL = 1, 
    PRIO_LOW = 2,
    PRIO_INIT = 3
};

// Process Control Block
typedef struct pcb_s pcb;
struct pcb_s {
    struct fields_t{
        // Process ID
        uint32_t pid      : 16;
        // Process Priority
        uint32_t prio     :  2; 
        // Process State
        uint32_t state    :  2;
        // Reserved
        uint32_t in_use   :  1;
        uint32_t reserved : 11;
    } field;
    // Process IPC Message
    char message[PCB_ICP_MESSAGE_SIZE];
    // Process Location 
    List* location;
};

// Sets the current location
void pcb_set_location(pcb* pPcb, List* location);

// Returns the current location
List* pcb_get_location(pcb* pPcb);

// Initializes the pcb struct with the provided information
void pcb_init(pcb* pPcb, uint32_t pid, uint32_t prio, uint32_t state);

// Creates a new pcb struct by cloning the origional provided
//  returns 0 on success 1 on failure
bool pcb_clone(pcb* pPcb_new, pcb* pPcb_origional, uint32_t pid_new);

// Frees a pcb struct
void pcb_free(pcb* pPcb);

// Returns the pid of the provided pcb
uint32_t pcb_get_pid(pcb* pPcb);

// Returns the state of the provided pcb
uint32_t pcb_get_state(pcb* pPcb);

// Sets the state of the provided pcb
void pcb_set_state(pcb* pPcb, uint32_t state);

// Returns the priority of the provided pcb
uint32_t pcb_get_priority(pcb* pPcb);

// Returns the message stored in the provided pcb
char* pcb_get_message(pcb* pPcb);

// Clears the current messasge in the pcb provided
void pcb_clear_message(pcb* pPcb);

// Sets the current messasge in the pcb provided
void pcb_set_message(pcb* pPcb, char* msg);

// prints all info for the provided pcb
void pcb_print_all_info(pcb* pPcb);

#endif
