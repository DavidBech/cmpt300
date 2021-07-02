#include <stdio.h>
#include <string.h>
#include "general.h"
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include "reciever.h"

#define DYNAMIC_LEN 128
#define INVALID_INPUT 1
#define MSG_MAX_LEN 1024
#define PORT 42069

static pthread_mutex_t dynamicMsgMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t threadPID;
static char* rxMessage;
static char* dynamicMessage;

void* recieveThread(void* unused)
{
    // char* msg = msAsVoid;
    // for (int i = 0; i < strlen(msg); i++)
    // {
    //     printf("%c", msg[i]);
    //     // To help print things out on the screen character by character
    //     fflush(stdout);
    //     sleep_msec(200);
    // }
    // return NULL;


    // Dynamically allocate a message
    pthread_mutex_lock(&dynamicMsgMutex);
    // strncpy is used to prevent the buffer from overflows as opposed to strcpy
    {
        strcpy(dynamicMessage, "Dynamic!");
    }
    pthread_mutex_unlock(&dynamicMsgMutex);

    // UDP Setup Connection
    // Setup Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(PORT);              // Port Watching

    // Create the socket
    int socketDesc = socket(PF_INET, SOCK_DGRAM, 0);
    // Bind socket to port specified
    bind(socketDesc, (struct sockaddr *) &sin, sizeof(sin));

    while(1){
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];
        printf("Waiting for message...\n");
        int bytesRx = recvfrom(socketDesc, messageRx, MSG_MAX_LEN, 0, (struct sockaddr *) &sinRemote, &sin_len);
        int term_null = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
        messageRx[term_null] = '\0';

        pthread_mutex_lock(&dynamicMsgMutex);
        // strncpy is used to prevent the buffer from overflows as opposed to strcpy
        {
            printf("Message first: %s and then %s recieved %d Bytes: %s\n", 
            dynamicMessage, rxMessage, bytesRx, messageRx);
        }
        pthread_mutex_unlock(&dynamicMsgMutex);
        // printf("Sending a message back to it's original sender...\n");

        // bzero(messageRx, MSG_MAX_LEN);
        // fgets(messageRx, MSG_MAX_LEN - 1, stdin);
        // sendto(socketDesc, &messageRx, MSG_MAX_LEN, 0, (struct sockaddr *) &sinRemote, sizeof(sinRemote));
    }

    // NOTE: Never executes because thread is canceled

    // printf("Done rx thread!\n");
    // free(dynamicMessage);

    return NULL;
}

void Reciever_init(char* inMessage)
{
    pthread_mutex_lock(&dynamicMsgMutex);
    // strncpy is used to prevent the buffer from overflows as opposed to strcpy
    {
        dynamicMessage = malloc(DYNAMIC_LEN);
    }
    pthread_mutex_unlock(&dynamicMsgMutex);

    rxMessage = inMessage;
    pthread_create(
        &threadPID,     // PID (by pointer)
        NULL,           // Attributes
        recieveThread,
        NULL);
}

void Reciever_changeDynamicMessage(char* newDynamic)
{
    pthread_mutex_lock(&dynamicMsgMutex);
    // strncpy is used to prevent the buffer from overflows as opposed to strcpy
    {
        strncpy(dynamicMessage, newDynamic, DYNAMIC_LEN);
    }
    pthread_mutex_unlock(&dynamicMsgMutex);
}

void Reciever_shutdown(void)
{
    // Cancel the thread
    pthread_cancel(threadPID);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

    // Clean up the memory
    pthread_mutex_lock(&dynamicMsgMutex);
    // strncpy is used to prevent the buffer from overflows as opposed to strcpy
    {
        free(dynamicMessage);
    }
    pthread_mutex_unlock(&dynamicMsgMutex);
}