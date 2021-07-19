# Assignment 3 Branch
Makefile Targets:
"make" or "make all" to build the default binary file
"make run" for defult run 
"make valgrind" to run with valgrind
"make clean" to clean the directory

Process printout:
    PID: [Process ID], Prio: [low, norm, high, init], State: [running, ready, blocked], Queue: [Ready (low, norm, high), Blocked (send, recieve), TODO SEMAPHORE], Message:[TODO];

RR Arbiter Priority:
    - A process may continually win arbitration when they are the only process of that priority even if there are other processes of a lower priority

How the Init process is handled:
    - pid is always 0
    - info messages will state that the init is located on the "Init" queue regardless of its state
    - the total info printout will display the init process separately when it isn't the current process
    - attempting to fork the init process will result in error
    - killing the init process will cause termination when it is the only process
    - exiting the init process will cause termination when it is the only process
    - the init process will only recieve arbitration if every other process is blocked
    - TODO OTHER INIT INTERACTIONS
