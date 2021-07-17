#ifndef __KERNAL_SIM_H_
#define __KERNAL_SIM_H_

#define KERNEL_SIM_MAX_NUM_NODES 1000 // TODO pick value
#define KERNEL_SIM_MAX_NUM_HEADS 100  // TODO pick value
#define KERNEL_SIM_FAILURE 1
#define KERNEL_SIM_SUCCESS 0

// Initialize the simulation environment
void kernel_sim_init(void);

#endif
