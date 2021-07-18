#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "executioner.h"
#include "process_cb.h"
#include "queue_manager.h"
#include "kernel_sim.h"
#include "semaphore.h"

// Static Variables
static pcb* current_process;
static struct pid_stack_s{
    int head_index;
    int next_index[PCB_MAX_PID];
}pid_stack;
static const uint32_t init_pid = PCB_MIN_PID;
static pcb* init_process;
static pcb pcb_array[PCB_MAX_PID];

// TODO TEST OUT OF PIDs
// Static Functions
static int get_next_pid();
static void free_pid(int pid_to_free);
static void print_current_proc_info();
static bool termination();

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
    return KERNEL_SIM_SUCCESS;
}

bool executioner_fork(void){
    // TODO
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_kill(uint32_t pid){ 
    // TODO
    if(pid == init_pid){
        // TODO 
        fprintf(stderr, "Not Implemented\n");
        return KERNEL_SIM_FAILURE;
    }
    queue_manager_remove(&pcb_array[pid]);
    free_pid(pid);
    return KERNEL_SIM_SUCCESS;
}

bool executioner_exit(void){ 
    if(current_process == init_process){
       if(termination()){
           return KERNEL_SIM_FAILURE;
       }
    }
    // Free current process
    free_pid(pcb_get_pid(current_process));
    pcb_free(current_process);
    printf("Exited Process\n");

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
    return KERNEL_SIM_SUCCESS;
}

bool executioner_quantum(void){ 
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
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
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_totalinfo(void){ 
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
        printf("Termination Canceled Process Blocked on Semaphore");
        return 1;
    } 
    if(queue_manager_any_non_empty()){
        printf("Termination Canceled Process on Ready or Blocked Queues");
        return 1;
    }
    printf("Termination Proceding\n");
    exit(EXIT_SUCCESS);
    return 0;
}
