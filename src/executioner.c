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
static pcb* current_process;
static struct pid_stack_s{
    int head_index;
    int next_index[PCB_MAX_PID];
}pid_stack;
static const uint32_t init_pid = PCB_MIN_PID;
static pcb* init_process;
static pcb pcb_array[PCB_MAX_PID];
static unsigned process_count = 1;

// TODO TEST OUT OF PIDs
// Static Functions

// gets the next valid pid
static int get_next_pid();

// returns the input pid to the pool of available pids
static void free_pid(int pid_to_free);

// prints out the current processes info
static void print_current_proc_info();

// checks condidtions for termination and does so if they all pass
static int termination();

// removes the pcb from its queue to free it
static bool remove_from_queue(pcb* p_pcb);

// checks if the input pid belongs to a running process
static bool pid_in_use(uint32_t pid);

// switches current process to the new process
static void new_current_running_process(pcb* new_pcb);

void executioner_init(){
    assert(PCB_MIN_PID == 0);
    assert(PCB_MAX_PID <= 0xEFFFFFFF);
    for(int i=PCB_MIN_PID; i<=PCB_MAX_PID; ++i){
        pid_stack.next_index[i] = i+1;
    }
    pid_stack.next_index[PCB_MAX_PID] = -1;
    pid_stack.head_index = 0;
    memset(pcb_array, 0, sizeof(pcb_array));
    get_next_pid(); //remove init pid
    init_process = &pcb_array[init_pid];
    pcb_init(init_process, init_pid, PRIO_INIT, STATE_RUNNING);
    pcb_set_location(init_process, PCB_INIT_LOC);
    current_process = init_process;
    printf("Created Init Process:\n\t");
    print_current_proc_info();
}

bool executioner_create(uint32_t prio){
    if(prio > 2 || prio < 0){
        printf("Invalid priority (%d) aborting create command\n", prio);
        return KERNEL_SIM_FAILURE;
    }
    uint32_t pid = get_next_pid();
    if(pid <= PCB_MIN_PID || pid > PCB_MAX_PID){
        // min pid reserved for init_process
        printf("Invalid pid %d aborting create command\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    pcb *p_pcb = &pcb_array[pid];
    pcb_init(p_pcb, pid, prio, STATE_READY);
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
    uint32_t new_pid = get_next_pid();
    pcb* new_pcb = &pcb_array[new_pid];
    pcb_clone(new_pcb, current_process, new_pid);
    queue_manager_add_ready(new_pcb);
    printf("Forked New Process: ");
    pcb_print_all_info(new_pcb);
    ++process_count;
    return KERNEL_SIM_FAILURE;
}

int executioner_kill(uint32_t pid){ 
    if(pid_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(pid == init_pid){
        return termination();
    }
    pcb* pcb = &pcb_array[pid];
    if(pcb == current_process){
        return executioner_exit();
    }
    if(remove_from_queue(pcb)){
        printf("Could not kill the process\n");
        return KERNEL_SIM_FAILURE;
    }

    printf("Terminating Process: ");
    pcb_print_all_info(pcb);
    free_pid(pid);
    pcb_free(pcb);
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
    free_pid(pcb_get_pid(p_prev));
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
    if(pid_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(current_process == init_process || pid == init_pid){
        printf("Init process sending messages not implemented\n");
        return KERNEL_SIM_FAILURE;
    } else {
        pcb* reciever = &pcb_array[pid];
        if(queue_manager_check_block_recieve(reciever)){
            // Fetch reciever and set message fields
            queue_manager_remove(reciever); 
            pcb_set_message(reciever, msg);
            pcb_set_reciever(reciever);
            pcb_set_message_source(reciever, pcb_get_pid(current_process));
            // Add reciever to ready queue
            queue_manager_add_ready(reciever);
        }
        // Add current process to blocked on send queue, waiting for reply
        pcb_set_message(current_process, msg);
        pcb_set_message_source(current_process, pid);
        printf("Added Current Process to blocked send queue waiting for reply\n");
        queue_manager_add_block_send(current_process);
        // Get next ready process
        new_current_running_process(queue_manager_get_next_ready());
        return KERNEL_SIM_SUCCESS;
    }
}

bool executioner_receive(void){ 
    if(current_process == init_process){
        printf("init receive not implemented\n");
        return KERNEL_SIM_FAILURE;
    }
    pcb* sender = queue_manager_check_block_send(pcb_get_pid(current_process));
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
    if(pid_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(pid == 0 || current_process == init_process){
        printf("Init not supported for IPC\n");
        return KERNEL_SIM_FAILURE;
    }
    pcb* p_sender = &pcb_array[pid];
    if(queue_manager_check_blocked_on_send(p_sender)){
        queue_manager_remove(p_sender);
        pcb_set_message(p_sender, msg);
        pcb_set_reciever(p_sender);
        pcb_set_message_source(p_sender, pcb_get_pid(current_process));
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
    if(pid_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    pcb_print_all_info(&pcb_array[pid]);
    return KERNEL_SIM_SUCCESS;
}

bool executioner_totalinfo(void){ 
    printf("Processes In Use: %u\n", process_count);
    printf("Running Process: \n\t");
    print_current_proc_info();
    if(current_process != init_process){
        printf("Init Process: \n\t");
        pcb_print_all_info(init_process);
    }
    queue_manager_print_info();
    semaphore_print_all_info();
    return KERNEL_SIM_SUCCESS;
}

void executioner_module_shutdown(){
    // TODO
}

static int get_next_pid(){
    int next = pid_stack.head_index;
    pid_stack.head_index = pid_stack.next_index[pid_stack.head_index];
    return next;
}

static void free_pid(int pid_to_free){
    pid_stack.next_index[pid_to_free] = pid_stack.head_index;    
    pid_stack.head_index = pid_to_free;
}

static void print_current_proc_info(){
    pcb_print_all_info(current_process);
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

static bool remove_from_queue(pcb* pPcb){
    if(queue_manager_remove(pPcb)){
        if(semaphore_remove_blocked_pcb(pPcb)){
            return KERNEL_SIM_FAILURE;
        }
    }
    return KERNEL_SIM_SUCCESS;
}

static bool pid_in_use(uint32_t pid){
    pcb* pcb = &pcb_array[pid];
    if(pcb->field.in_use == 0){
        return KERNEL_SIM_FAILURE;
    }
    return KERNEL_SIM_SUCCESS;
}

static void new_current_running_process(pcb* new_pcb){
    // Info / Actions on previous running process
    if(current_process == init_process && new_pcb == NULL){
        return;
    }
    if(current_process != NULL){
        printf("Previous Current Process: \n\t");
        print_current_proc_info();
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
    print_current_proc_info();

    // TODO INIT IPC
    if(current_process != init_process && pcb_get_reciever(current_process)){
        // message recieved
        printf("Recieved Message From: %#04x, Message:%s\n", pcb_get_message_source(current_process), pcb_get_message(current_process));
        pcb_clear_reciever(current_process);
        pcb_clear_message(current_process);
    }
}
