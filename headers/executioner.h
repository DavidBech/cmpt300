#include <stdbool.h>
#include <stdint.h>

#ifndef __EXECUTIONER_H_
#define __EXECUTIONER_H_

void executioner_init();

bool executioner_create(uint32_t prio);
bool executioner_fork(void);
int executioner_kill(uint32_t pid);
int executioner_exit(void);
bool executioner_quantum(void);
bool executioner_send(uint32_t pid, char* msg);
bool executioner_receive(void);
bool executioner_reply(uint32_t pid, char* msg);
bool executioner_semaphore_new(uint32_t sem_id, uint32_t init_value);
bool executioner_semaphore_p(uint32_t sem_id);
bool executioner_semaphore_v(uint32_t sem_id);
bool executioner_procinfo(uint32_t pid);
bool executioner_totalinfo(void);

void executioner_module_shutdown(void);
#endif