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
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf[15];

    if(argc == 1) {
        printf("Usage: ./client [address of server]\n");
        exit(1);
    }

    /* Set up the servers address. */
    server = gethostbyname(argv[1]);
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1337);
    bcopy((char*) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, \
        server->h_length);

    /* Create a socket. */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    /* Connect to the server. */
    if(connect(sockfd, (struct sockaddr *) &serv_addr, 
            sizeof(struct sockaddr_in)) < 0) {
        perror("Cannot connect");
        exit(1);
    }

    /* Read the response of the server. */
    nread = read(sockfd, buf, 14);
    if(nread < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    printf("I recieved: %i\n", atoi(buf));

    /* Close the socket and exit the program. */
    close(sockfd);
    return 0;
}
