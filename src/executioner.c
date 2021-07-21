#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "executioner.h"
#include "process_cb.h"
#include "queue_manager.h"
#include "kernel_sim.h"
#include "sem.h"

// Static Variables

// Count of current number of processes including init process
static unsigned process_count = 1;
// Init process ID
static uint32_t init_pid;
// Init process
static pcb* init_process;
// Current running process
static pcb* current_process;
// Map of PID to processes
static pcb* pcb_array[PCB_MAX_PID];

// Static Functions

// Attempts termination
// return: 2 when init is only process
// return: 1 otherwise
static int termination();

// checks if the input pid belongs to a running process
static bool pid_not_in_use(uint32_t pid);

// switches current process to the new process
static void new_current_running_process(pcb* new_pcb);

void executioner_init(){
    assert(PCB_MIN_PID == 0);
    memset(pcb_array, '\0', sizeof(pcb_array));
    init_process = pcb_init(PRIO_INIT, STATE_RUNNING);
    pcb_set_location(init_process, PCB_INIT_LOC);
    init_pid = pcb_get_pid(init_process);
    pcb_array[pcb_get_pid(init_process)] = init_process;
    current_process = init_process;
    printf("Created Init Process:\n\t");
    pcb_print_all_info(current_process);
}

bool executioner_create(uint32_t prio){
    if(prio > 2 || prio < 0){
        printf("Invalid priority (%d) aborting create command\n", prio);
        return KERNEL_SIM_FAILURE;
    }
    pcb *p_pcb = pcb_init(prio, STATE_READY);
    pcb_array[pcb_get_pid(p_pcb)] = p_pcb;
    queue_manager_add_ready(p_pcb);
    printf("Created New Process: \n\t");
    pcb_print_all_info(p_pcb);
    if(current_process == init_process){
        new_current_running_process(queue_manager_get_next_ready());
    }
    ++process_count;
    return KERNEL_SIM_SUCCESS;
}

bool executioner_fork(void){
    if(current_process == init_process){
        printf("Failure Cannot fork the init process\n");
        return KERNEL_SIM_FAILURE;
    }
    pcb* p_pcb = pcb_clone(current_process);
    pcb_array[pcb_get_pid(p_pcb)] = p_pcb;
    queue_manager_add_ready(p_pcb);
    printf("Forked New Process: \n\t");
    pcb_print_all_info(p_pcb);
    ++process_count;
    return KERNEL_SIM_SUCCESS;
}

int executioner_kill(uint32_t pid){ 
    if(pid_not_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(pid == init_pid){
        return termination();
    }
    pcb* p_pcb = pcb_array[pid];
    if(p_pcb == current_process){
        return executioner_exit();
    }
    if(queue_manager_remove(p_pcb) && semaphore_remove_blocked_pcb(p_pcb)){
        // TODO
        printf("Could not kill the process\n");
        return KERNEL_SIM_FAILURE;    
    }
    printf("Killing Process: \n\t");
    pcb_array[pcb_get_pid(p_pcb)] = NULL;
    pcb_print_all_info(p_pcb);
    pcb_free(p_pcb);
    --process_count;
    return KERNEL_SIM_SUCCESS;
}

int executioner_exit(void){ 
    if(current_process == init_process){
        return termination();
    }
    // Free current process
    pcb* p_prev = current_process;
    // Get the next process to execute
    new_current_running_process(queue_manager_get_next_ready());

    // Free the previous one
    pcb_array[pcb_get_pid(p_prev)] = NULL;
    pcb_free(p_prev);
    
    printf("Exited Previous Process\n");
    --process_count;
    return KERNEL_SIM_SUCCESS;
}

bool executioner_quantum(void){
    if(current_process == init_process && queue_manager_any_ready_non_empty()){
        fprintf(stderr, "Invalid State reached init is current process with process in ready queue\n");
        new_current_running_process(queue_manager_get_next_ready());
        return KERNEL_SIM_SUCCESS;
    }
    new_current_running_process(queue_manager_get_next_ready_exempt(current_process));
    return KERNEL_SIM_SUCCESS;
}

bool executioner_send(uint32_t pid, char* msg){ 
    if(pid_not_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(current_process == init_process || pid == init_pid){
        // TODO
        printf("Init process sending messages not implemented\n");
        return KERNEL_SIM_FAILURE;
    } else {
        pcb* reciever = pcb_array[pid];
        if(queue_manager_check_block_recieve(reciever)){
            // Fetch reciever and set message fields
            queue_manager_remove(reciever); 
            pcb_set_message(reciever, msg);
            pcb_set_received_message(reciever);
            pcb_set_message_pid(reciever, pcb_get_pid(current_process));
            // Add reciever to ready queue
            queue_manager_add_ready(reciever);
        }
        // Add current process to blocked on send queue, waiting for reply
        pcb_set_message(current_process, msg);
        pcb_set_message_pid(current_process, pid);
        printf("Added Current Process to blocked send queue waiting for reply\n");
        queue_manager_add_block_send(current_process);
        // Get next ready process
        new_current_running_process(queue_manager_get_next_ready());
        return KERNEL_SIM_SUCCESS;
    }
}

bool executioner_receive(void){ 
    if(current_process == init_process){
        // TODO
        printf("init receive not implemented\n");
        return KERNEL_SIM_FAILURE;
    }
    pcb* sender = queue_manager_check_block_send(pcb_get_pid(current_process));
    // TODO CANNOT RECEiVE MESSAGES MULTIPLE TIMES
    if(sender){
        printf("Received Message From: %#04x, Message:%s\n", pcb_get_pid(sender), pcb_get_message(sender));
    } else {
        printf("Adding process to blocked receive queue\n");
        queue_manager_add_block_recieve(current_process);
        new_current_running_process(queue_manager_get_next_ready());
    }
    return KERNEL_SIM_SUCCESS;
}

bool executioner_reply(uint32_t pid, char* msg){ 
    if(pid_not_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(pid == 0 || current_process == init_process){
        // TODO
        printf("Init not supported for IPC\n");
        return KERNEL_SIM_FAILURE;
    }
    pcb* p_sender = pcb_array[pid];
    if(queue_manager_check_blocked_on_send(p_sender)){
        queue_manager_remove(p_sender);
        pcb_set_message(p_sender, msg);
        pcb_set_received_message(p_sender);
        pcb_set_message_pid(p_sender, pcb_get_pid(current_process));
        queue_manager_add_ready(p_sender);
        printf("Added sender to ready queue\n\t");
        pcb_print_all_info(p_sender);
        return KERNEL_SIM_SUCCESS;
    } else {
        printf("The pid%#x is not on the blocked send queue\n", pid);
        return KERNEL_SIM_FAILURE;
    }

}

bool executioner_semaphore_new(uint32_t sem_id, uint32_t init_value){ 
    return semaphore_new(sem_id, init_value);
}

bool executioner_semaphore_p(uint32_t sem_id){ 
    if(semaphore_p(sem_id, current_process)){
        return KERNEL_SIM_FAILURE;
    }
    // if the current process was blocked need to exempt it
    if(pcb_get_state(current_process) == STATE_BLOCKED){
        if(current_process == init_process){
            // Init process cannot be blocked
            pcb_set_state(init_process, STATE_RUNNING);
        } else {
            // Atempt to get a new process from ready queues
            new_current_running_process(queue_manager_get_next_ready());
        }
    }
    return KERNEL_SIM_SUCCESS;
}

bool executioner_semaphore_v(uint32_t sem_id){ 
    if(semaphore_v(sem_id)){
        return KERNEL_SIM_FAILURE;
    }
    if(current_process == init_process){
        // Check if a process was readied up
        new_current_running_process(queue_manager_get_next_ready());
    }
    return KERNEL_SIM_SUCCESS;
}

bool executioner_procinfo(uint32_t pid){ 
    if(pid_not_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    pcb_print_all_info(pcb_array[pid]);
    return KERNEL_SIM_SUCCESS;
}

bool executioner_totalinfo(void){ 
    printf("Processes In Use: %u\n", process_count);
    printf("Running Process: \n\t");
    pcb_print_all_info(current_process);
    if(current_process != init_process){
        printf("Init Process: \n\t");
        pcb_print_all_info(init_process);
    }
    queue_manager_print_info();
    semaphore_print_all_info();
    return KERNEL_SIM_SUCCESS;
}

void executioner_module_shutdown(){
    pcb_free(init_process);
    // TODO
}

static int termination(){
    printf("Attempted Termination\n");
    if(process_count == 1){
        printf("Termination Proceding\n");
        return KERNEL_SIM_TERMINATION;
    }
    printf("Init process is not only process, not terminating\n");
    return KERNEL_SIM_FAILURE;
}

static bool pid_not_in_use(uint32_t pid){
    if(pcb_array[pid] == NULL){
        return true;
    }
    return false;
}

static void new_current_running_process(pcb* new_pcb){
    // Info / Actions on previous running process
    if(current_process == init_process && new_pcb == NULL){
        return;
    }
    if(current_process != NULL){
        printf("Previous Current Process: \n\t");
        pcb_print_all_info(current_process);
        if(current_process == init_process){
            pcb_set_state(init_process, STATE_READY);
        }
    }

    // Setting the new current running process
    if(new_pcb == NULL){
        current_process = init_process;
        pcb_set_state(current_process, STATE_RUNNING);
    } else {
        current_process = new_pcb;
    }
    printf("New Current Process: \n\t");
    pcb_print_all_info(current_process);

    // TODO INIT IPC
    if(current_process != init_process && pcb_get_received_message(current_process)){
        // message recieved
        printf("Recieved Message From: %#04x, Message:%s\n", pcb_get_message_pid(current_process), pcb_get_message(current_process));
        pcb_clear_received_message(current_process);
        pcb_clear_message(current_process);
    }
}
