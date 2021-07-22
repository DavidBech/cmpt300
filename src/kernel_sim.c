#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_sim.h"
#include "executioner.h"
#include "process_cb.h"
#include "queue_manager.h"
#include "sem.h"

static char message_buffer[PCB_ICP_MESSAGE_SIZE];

// Get int for arguments
//  return value 0 to 9
static int get_input_small_int();

// Get int for larger args
static int get_input_int();

// Get msg input for commands
static void get_input_message();

void kernel_sim_init(void){
    pcb_module_init();
    queue_manager_init();
    semaphore_init();
    executioner_init();
}

void kernel_sim_interpreter_loop(void){
    int status;
    char command;
    while(1){
        command = fgetc(stdin);
        if(command == ' ' || command == '\n' || command == '\0' || command == '\t'){
            // TODO is there other blank space?
            continue;
        }
        // Convert lower case to upper case
        if(command >= 'a' && command <= 'z'){
            command -= 'a';
            command += 'A';
        }
        switch(command){
            case('C'):
                printf("===== C: Create Command =====\n");
                printf("Enter Priority (0=high, 1=norm, 2=low): ");
                int prio = get_input_small_int();
                if(executioner_create(prio)){
                    printf("===== Create Command Completed with error =====\n\n");
                } else {
                    printf("===== Create Command Completed =====\n\n");
                }
                break;
            case('F'):
                printf("===== F: Fork Command =====\n");
                if(executioner_fork()){
                    printf("===== Fork Command Completed with error =====\n\n");
                } else {
                    printf("===== Fork Command Completed =====\n\n");
                }
                break;
            case('K'):
                printf("===== K: Kill Command =====\n");
                printf("Enter the process ID to kill: ");
                int kill_pid = get_input_int();
                status = executioner_kill(kill_pid);
                if(status == KERNEL_SIM_FAILURE){
                    printf("===== Kill Command Completed with error =====\n\n");
                } else if(status == KERNEL_SIM_TERMINATION){
                    return;
                } else {
                    printf("===== Kill Command Completed =====\n\n");
                }
                break;
            case('E'):
                printf("===== E: Exit Command =====\n");
                status = executioner_exit();
                if(status == KERNEL_SIM_FAILURE){
                    printf("===== Exit Command Completed with error\n");
                } else if(status == KERNEL_SIM_TERMINATION){
                    return;
                } else {
                    printf("===== Exit Command Completed =====\n\n");
                }
                break;
            case('Q'):
                printf("===== Q: Quantum Command =====\n");
                if(executioner_quantum()){
                    printf("===== Quantum Command Completed with error\n");
                } else {
                    printf("===== Quantum Command Completed =====\n\n");
                }
                break;
            case('S'):
                printf("===== S: Send Command =====\n");
                printf("Enter the process ID to send message to: ");
                int process_send = get_input_int();
                printf("Enter the message to send (max 40 chars): ");
                get_input_message();
                if(executioner_send(process_send, message_buffer)){
                    printf("===== Send Command Completed with error =====\n\n");
                } else {
                    printf("===== Send Command Completed =====\n\n");
                }
                break;
            case('R'):
                printf("===== R: Recieve Command =====\n");
                if(executioner_receive()){
                    printf("===== Recieve Command Completed with error =====\n\n");
                } else {
                    printf("===== Recieve Command Completed =====\n\n");
                }
                break;
            case('Y'):
                printf("===== Y: Reply Command =====\n");
                printf("Enter the process ID to reply to: ");
                int process_reply = get_input_int();
                printf("Enter the message to send (max 40 chars): ");
                get_input_message();
                if(executioner_reply(process_reply, message_buffer)){
                    printf("===== Reply Command Completed with error =====\n\n");
                } else {
                    printf("===== Reply Command Completed =====\n\n");
                }
                break;
            case('N'):
                printf("===== N: New Semaphore Command =====\n");
                printf("Enter the semaphore ID to create (0 to 4): ");
                int semaphore_new_id = get_input_small_int();
                printf("Enter the semaphore's initial input value ( >= 0): ");
                unsigned init_value = get_input_int();
                if(executioner_semaphore_new(semaphore_new_id, init_value)){
                    printf("===== Semaphore New Command Completed with error =====\n\n");
                } else {
                    printf("===== Semaphore New Command Completed =====\n\n");
                }
                break;
            case('P'):
                printf("===== P: P Semaphore Command =====\n");
                printf("Enter the semaphore ID to p: ");
                int semaphore_id_p = get_input_small_int();
                if(executioner_semaphore_p(semaphore_id_p)){
                    printf("===== Semaphore P Command Completed with error =====\n\n");
                } else {
                    printf("===== Semaphore P Command Completed =====\n\n");
                }
                break;
            case('V'):
                printf("===== V: V Semaphore Command =====\n");
                printf("Enter the semaphore ID to v: ");
                int semaphore_id_v = get_input_small_int();
                if(executioner_semaphore_v(semaphore_id_v)){
                    printf("===== Semaphore V Command Completed with error =====\n\n");
                } else {
                    printf("===== Semaphore V Command Completed =====\n\n");
                }
                break;
            case('I'):
                printf("===== I: Procinfo Command\n");
                printf("Enter process ID to display info of: ");
                int info_pid = get_input_int();
                if(executioner_procinfo(info_pid)){
                    printf("===== Procinfo Command Completed with error =====\n\n");
                } else {
                    printf("===== Procinfo Command Completed =====\n\n");
                }
                break;
            case('T'):
                printf("===== T: Totalinfo Command =====\n");
                if(executioner_totalinfo()){
                    printf("===== Totalinfo Command Completed with error =====\n\n");
                } else {
                    printf("===== Totalinfo Command Completed =====\n\n");
                }
                break;
            case('!'):
                // TODO -- remove
                printf("!: Temparary Termination\n");
                exit(0);
            default:
                printf("===== %c: Inavlid Command =====\n\n", command);
                break;
        }
    }
}

void kernel_sim_shutdown(void){
    queue_manager_shutdown();
    semaphore_shutdown();
    executioner_module_shutdown();
}

static int get_input_small_int(){
    //TODO -- error handling
    char character;
    int value = -1;
    do {
        character = fgetc(stdin);
        if(character >= (int)'0' && character <= (int)'9'){
            value = character - (int)'0';
        }
    } while(value == -1);
    return value;
}

static int get_input_int(){
    int value = -1;
    // Remove non number chars from stdin
    char c = '\0';
    while(!(c >= (int)'0' && c <= (int)'9')){
        c = fgetc(stdin);
    } 
    // put number back on buffer
    ungetc(c, stdin);
    // Read number
    while(value == -1){
        char in_string[10];
        char *ptr;
        fgets(in_string, 10, stdin);
        value = strtol(in_string, &ptr, 10);
    }
    return value;
}

static void get_input_message(){
    char c;
    for(int i=0; i<PCB_ICP_MESSAGE_SIZE; ++i){
        if(i == PCB_ICP_MESSAGE_SIZE-1){
            message_buffer[i] = '\0';
            break;
        }
        c = fgetc(stdin);
        if(c == '\n' || c == '\0'){
            message_buffer[i] = '\0';
            break;
        }
        message_buffer[i] = c;
    }
    return;
    //TODO -- error handling
}
