#ifndef __KERNAL_SIM_H_
#define __KERNAL_SIM_H_

#define KERNEL_SIM_MAX_NUM_NODES 1000
#define KERNEL_SIM_MAX_NUM_HEADS 100
#define KERNEL_SIM_FAILURE 1
#define KERNEL_SIM_SUCCESS 0
#define KERNEL_SIM_TERMINATION 2

// Initialize the simulation environment
void kernel_sim_init(void);

// Loop until simulation is terminated
//  Fetch command -> Fetch Args -> Execute command -> Display Status -> repeat
void kernel_sim_interpreter_loop(void);

// Cleanup environment
void kernel_sim_shutdown(void);

#endif
