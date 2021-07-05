# CMPT 300 Assignment 2 Branch

This branch contains the code for assignment 2

CLI Syntax:
s-talk [incoming port] [outgoing machine] [outgoing port]

unresolved Known Issues
 - Valgrind reports still reachable data when program ends from other console, but not from present console

Other Notes
 - the constant TERMINATION_STRING is defined to have a newline character after it ie. "!\n\0"
 - The Reader thread modifies strings equal to the termination string from "!\n\0" to "! \n\0" when
    the previous string did not have a new line ie. If the previous string filled up buffer and happend 
    to have an '!' as the final character the string will be modified by adding a space
