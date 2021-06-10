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

    char messageTx[MSG_MAX_LEN];
    printf("Waiting for input to send\n");
    scanf("%s", messageTx);
    sendto(socketDesc, &messageTx, strlen(messageTx), 0, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));

    close(socketDesc);
    return(0);
}

