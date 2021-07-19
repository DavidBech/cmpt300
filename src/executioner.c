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
static bool termination();

// removes the pcb from its queue to free it
static bool remove_from_queue(pcb* p_pcb);

// checks if the input pid belongs to a running process
static bool pid_in_use(uint32_t pid);

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
    pcb_init(init_process, init_pid, STATE_RUNNING, PRIO_INIT);
    pcb_set_location(init_process, PCB_INIT_LOC);
    current_process = init_process;
    printf("Created Init Process\n");
}

bool executioner_create(uint32_t prio){
    if(prio > 2 || prio < 0){
        printf("Invalid priority %d aborting create command\n", prio);
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
    printf("Created New Process: ");
    pcb_print_all_info(p_pcb);
    if(current_process == init_process){
        current_process = queue_manager_get_next_ready();
        pcb_set_state(current_process, STATE_RUNNING);
        pcb_set_state(init_process, STATE_READY);
        printf("Previous Process: ");
        pcb_print_all_info(init_process);
        printf("Current Process: ");
        print_current_proc_info();
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
    pcb_set_state(new_pcb, STATE_READY);
    queue_manager_add_ready(new_pcb);
    printf("Created New Process: ");
    pcb_print_all_info(new_pcb);
    ++process_count;
    return KERNEL_SIM_FAILURE;
}

bool executioner_kill(uint32_t pid){ 
    if(pid_in_use(pid)){
        printf("The input pid:%#04x does not map to a current created process\n", pid);
        return KERNEL_SIM_FAILURE;
    }
    if(pid == init_pid){
        if(termination()){
            return KERNEL_SIM_FAILURE;
        }
        assert(false);
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

bool executioner_exit(void){ 
    if(current_process == init_process){
       if(termination()){
           return KERNEL_SIM_FAILURE;
       }
       assert(false);
    }
    // Free current process
    free_pid(pcb_get_pid(current_process));
    pcb_free(current_process);
    printf("Exited Current Process\n");

    // Get the next process to execute
    pcb* temp = queue_manager_get_next_ready();
    if(temp == NULL){
        current_process = init_process;
    } else {
        current_process = temp;
    }
    pcb_set_state(current_process, STATE_RUNNING);
    printf("Current Process: ");
    print_current_proc_info();
    --process_count;
    return KERNEL_SIM_SUCCESS;
}

bool executioner_quantum(void){ 
    printf("Previous Current Process: ");
    print_current_proc_info();
    current_process = queue_manager_get_next_ready_exempt(current_process);
    printf("New Current Process: ");
    print_current_proc_info();
    return KERNEL_SIM_SUCCESS;
}

bool executioner_send(uint32_t pid, char* msg){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_receive(void){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_reply(uint32_t pid, char* msg){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_semaphore_new(uint32_t sem_id, uint32_t init_value){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_semaphore_p(uint32_t sem_id){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_semaphore_v(uint32_t sem_id){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
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
    printf("%u total processes currently in use\n", process_count);
    printf("Running Process: ");
    print_current_proc_info();
    if(current_process != init_process){
        printf("Init Process: ");
        pcb_print_all_info(init_process);
    }
    queue_manager_print_info();
    semaphore_print_all_info();
    return KERNEL_SIM_SUCCESS;
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

static bool termination(){
    printf("Attempted Termination\n");
    if(current_process != init_process){
        printf("Termination Canceled Init is Not Current Process\n");
        return 1;
    }
    if(semaphore_any_blocked()){
        printf("Termination Canceled Process Blocked on Semaphore\n");
        return 1;
    } 
    if(queue_manager_any_non_empty()){
        printf("Termination Canceled Process on Ready or Blocked Queues\n");
        return 1;
    }
    printf("Termination Proceding\n");
    assert(process_count == 1);
    exit(EXIT_SUCCESS);// TODO CLEANUP? // TERMINATION FROM HERE?
    return 0;
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
