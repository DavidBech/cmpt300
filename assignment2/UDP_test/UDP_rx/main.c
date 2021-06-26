#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define INVALID_INPUT 1
#define MSG_MAX_LEN 1024
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
    sin.sin_port = htons(PORT);              // Port Watching

    // Create the socket
    int socketDesc = socket(PF_INET, SOCK_DGRAM, 0);
    if(socketDesc == -1){
        perror(NULL);
        fprintf(stderr, "fail socket call\n");
        exit(1);
    }
    // Bind socket to port specified
    if(bind(socketDesc, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) == -1){
        perror(NULL);
        fprintf(stderr, "fail bind call\n");
        exit(1);
    }

    char messageRx[MSG_MAX_LEN];
    while(1){
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        printf("Waiting for message\n");
        int bytesRx = recv(socketDesc, messageRx, MSG_MAX_LEN, 0);
        int term_null = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN -1;
        messageRx[term_null] = '\0';
        
        printf("Message recieved %d Bytes: |%s|\n", bytesRx, messageRx);
    }
    // sendto(socket, &msg, len, 0, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));

    close(socketDesc);
    return(0);
}
