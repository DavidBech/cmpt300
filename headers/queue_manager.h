#include <stdbool.h>

#include "process_cb.h"

#ifndef __QUEUE_MANAGER_H
#define __QUEUE_MANAGER_H

void queue_manager_init(void);

bool queue_manager_add_ready(pcb* p_pid);

bool queue_manager_add_block_send(pcb* p_pid);

bool queue_manager_add_block_recieve(pcb* p_pid);

bool queue_manager_remove(pcb* p_pid);

pcb* queue_manager_get_next_ready_exempt(pcb* exempted_process);

pcb* queue_manager_get_next_ready(void);

void queue_manager_print_info();

#endif