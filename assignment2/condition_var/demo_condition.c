#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include "general.h"
#include "printer.h"
#include "signaller.h"

// return TRUE if it computed the average OK
// average is written into pAvgOut
static bool getAverage(double *pData, int size, double *pAvgOut)
{
    if (size == 0) {
        return false;
    }

    double sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += (*pData + i);
    }

    // Store value into output parameter
    *pAvgOut = sum / size;

    return true;
}

int main(int argc, char** args)
{
    // Bad Practice - Fork Bomb

    // for (int i = 0; i < 20; i++)
    // {
    //     printf(" %d ", i);
    //     fork();
    // }    

    double myData[] = {1.0, 2.0};
    double myAverage = 0;
    if (getAverage(myData, 2, &myAverage)) 
    {
        printf("Average is %f:\n", myAverage);
    } 
    else 
    {
        printf("Could not compute average.\n");
    }

    // Startup
    Printer_init();
    Signaller_init();
     
    // Wait until ready to shutdown
    ShutdownManager_waitForShutdown();

    // Cleanup
    Printer_shutdown();
    Signaller_waitForShutdown();

    printf("\n\nDONE\n");
    return 0;
}