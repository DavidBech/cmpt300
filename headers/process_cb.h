#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#ifndef __PROCESS_CB_H
#define __PROCESS_CB_H

// Process Control Block
//  -- This file defines an api for creation/deletion/modification to pcb structure

// Max IPC message to fit in PCB, this include \0 terminator
#define PCB_ICP_MESSAGE_SIZE 41
// Minimum PID value
#define PCB_MIN_PID 0
// Maximum PID value
#define PCB_MAX_PID 0xFFFF

#define PCB_FAIL 1 
#define PCB_PASS 0

#define PCB_INIT_LOC (List*)0xFFFFFFFFFFFFFFFF

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
    // Process Location 
    List* location;
    struct fields_t{
        // Process ID
        uint32_t pid      : 28;
        // Process Priority
        uint32_t prio     :  2; 
        // Process State
        uint32_t state    :  2;
    } field;
    struct message_info_t{
        uint32_t pid      : 28;
        uint32_t received :  1;
        // TODO RECIEVED
        uint32_t reserved :  3;
    } message_info;
    // Process IPC Message
    char message[PCB_ICP_MESSAGE_SIZE];
};

void pcb_module_init();

// Constructor for pcb struct
pcb* pcb_init(uint32_t prio, uint32_t state);

// Copy Constructor for pcb struct
pcb* pcb_clone(pcb* pPcb_origional);

// Frees a pcb struct
void pcb_free(void* pPcb);

// Returns the pid of the provided pcb
uint32_t pcb_get_pid(pcb* pPcb);

// Returns the priority of the provided pcb
uint32_t pcb_get_priority(pcb* pPcb);

// Returns the state of the provided pcb
uint32_t pcb_get_state(pcb* pPcb);

// Sets the state of the provided pcb
void pcb_set_state(pcb* pPcb, uint32_t state);

// Sets the current location
void pcb_set_location(pcb* pPcb, List* location);

// Returns the current location
List* pcb_get_location(pcb* pPcb);

// Returns the message stored in the provided pcb
char* pcb_get_message(pcb* pPcb);

// Clears the current messasge in the pcb provided
void pcb_clear_message(pcb* pPcb);

// Sets the current messasge in the pcb provided
void pcb_set_message(pcb* pPcb, char* msg);

// Sets bit indicating message received
void pcb_set_received_message(pcb* pPcb);

// Clears received bit
void pcb_clear_received_message(pcb* pPcb);

// Gets received bit
uint32_t pcb_get_received_message(pcb* pPcb);

// Gets the pid of the source of the current message
uint32_t pcb_get_message_pid(pcb* pPcb);

// Gets the pid of the source of the current message
void pcb_set_message_pid(pcb* pPcb, uint32_t pid);

// prints all info for the provided pcb
void pcb_print_all_info(pcb* pPcb);

#endif
