#include <stdlib.h>

#include "kernel_sim.h"

int main(void) {
    kernel_sim_init();
    kernel_sim_interpreter_loop();
    kernel_sim_shutdown();
    return(EXIT_SUCCESS);
}
