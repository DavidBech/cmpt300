#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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

void executioner_init(){
    assert(PCB_MIN_PID == 0);
    assert(PCB_MAX_PID <= 0xEFFFFFFF);
    for(int i=PCB_MIN_PID; i<=PCB_MAX_PID; ++i){
        pid_stack.next_index[i] = i+1;
    }
    pid_stack.next_index[PCB_MAX_PID] = -1;
    pid_stack.head_index = 0;
    get_next_pid();
    init_process = &pcb_array[init_pid];
    pcb_init(init_process, init_pid, INIT_STATE, INIT_PRIO);
    current_process = init_process;
}

bool executioner_create(uint32_t prio){
    uint32_t pid = get_next_pid();
    if(pid <= PCB_MIN_PID || pid > PCB_MAX_PID){
        fprintf(stderr, "Invalid Pid: %d\n", pid);
        return KERNEL_SIM_FAILURE;
    } 
    pcb *p_pcb = &pcb_array[pid];
    pcb_init(p_pcb, pid, prio, READY);
    queue_manager_add_ready(p_pcb);
    printf("Process pid:%d, prio:%d \n", pid, prio);
    if(current_process == init_process){
        current_process = queue_manager_get_next_ready();
        printf("Current Process: %s\n", pcb_get_all_info(current_process));
    }
    return KERNEL_SIM_SUCCESS;
}

bool executioner_fork(void){
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
}

bool executioner_kill(uint32_t pid){ 
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
    if(pcb_get_pid(current_process) == init_pid){
        // TODO 
        fprintf(stderr, "Not Implemented\n");
        return KERNEL_SIM_FAILURE;
    }
    free_pid(pcb_get_pid(current_process));
    pcb* temp = queue_manager_get_next_ready();
    pcb_free(current_process);
    if(temp == NULL){
        current_process = init_process;
        fprintf(stderr, "Not Implemented\n");
    } else {
        current_process = temp;
    }
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
    fprintf(stderr, "Not Implemented\n");
    return KERNEL_SIM_FAILURE;
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
