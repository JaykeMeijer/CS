#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    int sockfd, nread;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int recv;

    if(argc == 1) {
        printf("Usage: ./client [address of server]\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if(getaddrinfo(argv[1], "1337", &hints, &result) != 0) {
        perror("Failed to get addressinfo");
        exit(1);
    }

    /* Try all returned addresses. */
    for(rp = result; rp != NULL; rp = rp->ai_next) {
        /* Create a socket. */
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sockfd < 0) {
            perror("Error creating socket");
            exit(1);
        }

        /* Connect to the server. */
        if(connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }

    if(rp == NULL) {
        perror("Failed to connect");
        exit(1);
    }

    freeaddrinfo(result);

    /* Read the response of the server. */
    nread = read(sockfd, &recv, 4);
    if(nread < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    printf("I recieved: %i\n", recv);

    /* Close the socket and exit the program. */
    close(sockfd);
    return 0;
}
