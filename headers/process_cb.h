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
#define PCB_MAX_PID 0xFF

#define PCB_FAIL 1 
#define PCB_PASS 0

// Process Control Block State
enum PCB_STATE{
    RUNNING = 0,
    READY = 1,
    BLOCKED = 2,
    INIT_STATE = 3
};

// Process Control Block Priority
enum PCB_PRIO{
    HIGH = 0,
    NORMAL = 1, 
    LOW = 2,
    INIT_PRIO = 3
};

// Process Control Block
typedef struct pcb_s pcb;
struct pcb_s {
    struct fields_t{
        uint32_t pid      :  8;
        uint32_t prio     :  2; 
        uint32_t state    :  2;
        uint32_t reserved : 20;
    } field;
    char message[PCB_ICP_MESSAGE_SIZE];
    List* location;
};

void pcb_set_location(pcb* pPcb, List* location);
List* pcb_get_location(pcb* pPcb);

// Creates a new pcb struct with the provided information
//  returns 0 on success 1 on failure
bool pcb_init(pcb* pPcb, uint32_t pid, uint32_t prio, uint32_t state);

// Creates a new pcb struct by cloning the origional provided
//  returns 0 on success 1 on failure
bool pcb_clone(pcb* pPcb_new, pcb* pPcb_origional, uint32_t pid_new);

// Frees a pcb struct
//  returns 0 on success 1 on failure
bool pcb_free(pcb* pPcb);

// Returns the pid of the provided pcb through the inout provided
//  returns 0 on success 1 on failure
uint32_t pcb_get_pid(pcb* pPcb);

// Returns the state of the provided pcb through the inout provided
//  returns 0 on success 1 on failure
uint32_t pcb_get_state(pcb* pPcb);

// Returns the state of the provided pcb through the inout provided
//  returns 0 on success 1 on failure
void pcb_set_state(pcb* pPcb, uint32_t state);

// Returns the pid of the provided pcb through the inout provided
//  returns 0 on success 1 on failure
uint32_t pcb_get_priority(pcb* pPcb);

// Returns the pid of the provided pcb through the inout provided
//  returns 0 on success 1 on failure
char* pcb_get_message(pcb* pPcb);

// Clears the current messasge in the pcb provided
//  returns 0 on success 1 on failure
void pcb_clear_message(pcb* pPcb);

// Sets the current messasge in the pcb provided
//  returns 0 on success 1 on failure
void pcb_set_message(pcb* pPcb, char* msg);

// Sets all info from this module into the string argument
//  returns 0 on success 1 on failure
char* pcb_get_all_info(pcb* pPcb);

#endif
