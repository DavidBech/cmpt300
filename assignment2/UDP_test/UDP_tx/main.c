#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_INPUT 1
#define MSG_MAX_LEN 2048
#define PORT 42069

int main(){
    //htonl -> host to network long
    //htons -> host to network short
    //ntohl -> network to host long
    //ntohs -> network to host short

    // UDP Setup Connection
    // Setup Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Address
    sin.sin_port = htons(PORT);           // Port Watching for response

    // Create the socket
    int socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
    // Bind socket to port specified
    bind(socketDesc, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));

    // struct sockaddr_in sinRemote;
    // unsigned int sin_len = sizeof(sinRemote);

    while(1){
        char messageTx[MSG_MAX_LEN];
        //unsigned int len = sizeof(messageTx);
        printf("Waiting for input to send\n");
        bzero(messageTx, MSG_MAX_LEN);
        fgets(messageTx, MSG_MAX_LEN - 1, stdin);
        int sendByte = sendto(socketDesc, &messageTx, strlen(messageTx), 0, (struct sockaddr *)&sin, sizeof(sin));

        if (sendByte < 0)
        {
            printf("There was an error in sending the message.\n");
        }
        else
        {
            printf("Success!!! %s\n", messageTx);
        }

        printf("\nNow waiting for the reply to my message: \n");

        // bzero(messageTx, MSG_MAX_LEN);
        // int recievedBytes = recvfrom(socketDesc, messageTx, MSG_MAX_LEN, 0, (struct sockaddr*)&sin, &len);
        // int term_null = (recievedBytes < MSG_MAX_LEN) ? recievedBytes : MSG_MAX_LEN - 1;
        // messageTx[term_null] = '\0';

        // printf("The message is recieved as: %s\n", messageTx);
    }
    close(socketDesc);
    return(0);
}

