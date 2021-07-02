// #include "printer.h"
// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <netdb.h>
// #include <unistd.h>

// static pthread_t threadPrinter;
// static const char* MESSAGE = "Hello World in Sync!";

// // Synchronization
// static pthread_cond_t s_syncOkToPrintCondVar = PTHREAD_COND_INITIALIZER;
// static pthread_mutex_t s_syncOkToPrintMutex = PTHREAD_MUTEX_INITIALIZER;

// static int* s_pAllocated = NULL;
// static List* s_pMyList = NULL;

// struct sPassToThread {
//     int height;
//     double weight;
//     List* pList;
// };

// #define MAX_SIZE 10

// void* printerThread(void* arg)
// {
//     char buff1[MAX_SIZE];
//     char* buff2 = malloc(MAX_SIZE);
//     free(buff2);

//     struct sPassToThread *pArgData;
//     int count = 0;
//     for (const char* msg = MESSAGE; *msg != '\0'; msg++)
//     {
//         // Wait until signalled

//         // pthread_mutex_lock(&s_syncOkToPrintMutex); 
//         // {
//         //     pthread_cond_wait(&s_syncOkToPrintCondVar, &s_syncOkToPrintMutex);
//         // }
//         // pthread_mutex_unlock(&s_syncOkToPrintMutex); 

//         // Read from Keyboard
//         myReadFromKeyboard(s_pAllocated); // Assuming it's a system call that calls gets()
//         List_append(myList, s_pAllocated);

//         s_pAllocated = malloc(100 + count);
//         ++count;

//         // Assume that whoever recieves from myList cleans up.


//         // printf("%c", *msg);
//         // fflush(stdout);
//     }

//     free(s_pAllocated);
//     return NULL;
// }

// void Printer_signalNextChar()
// {
//     // Signal other thread
//      pthread_mutex_lock(&s_syncOkToPrintMutex); 
//     {
//         pthread_cond_signal(&s_syncOkToPrintCondVar);
//     }
//     pthread_mutex_unlock(&s_syncOkToPrintMutex); 
// }

// void Printer_init(List* inputList)
// {
//     s_pMyList = inputList;
//     pthread_create(&threadPrinter, NULL, printerThread, NULL);  
// }

// void Printer_shutdown()
// {
//     // Note: The ptherad_cancel function immediately terminates the current state of a thread
//     pthread_cancel(threadPrinter);
//     pthread_join(threadPrinter, NULL);

//     // Cleanup
//     if (s_pAllocated)
//     {
//         free(s_pAllocated);
//         s_pAllocated = NULL;
//     }
// }