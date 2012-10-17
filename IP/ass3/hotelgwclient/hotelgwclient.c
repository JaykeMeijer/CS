#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

/* Create a socket and connect to it, to the given address. */
void create_connect_socket(char *address, int *sockfd) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if(getaddrinfo(address, "1337", &hints, &result) != 0) {
        perror("Failed to get addressinfo");
        exit(1);
    }

    // Try all returned addresses.
    for(rp = result; rp != NULL; rp = rp->ai_next) {
        // Create a socket.
        *sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sockfd < 0) {
            perror("Error creating socket");
            exit(1);
        }

        // Connect to the server.
        if(connect(*sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }

    if(rp == NULL) {
        perror("Failed to connect");
        exit(1);
    }

    freeaddrinfo(result);
}

/* Print the usage of the program and exit. */
void print_usage() {
    printf("Available options:\n\thotelclient <address> list\n"\
           "\thotelclient <address> book <type> <Guest name>\n"\
           "\thotelclient <address> guests\n");
    exit(1);
}

/*
 * Write an entire message to the socket. This function was given in the
 * lecture slides. It will not exit until the entire message is writen to the
 * pipe. This ensure the entire message will be sent, which is not guaranteed by
 * just using write().
 */
ssize_t writen(int fd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten < 0 ) {
            if (errno == EINTR)
                nwritten = 0; /* and call write() again */
            else
                return -1; /* error */
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

int main(int argc, char *argv[]) {
    int sockfd, nread = -1, i, ptr = 0, total = 0;
    char buffer[1024] = "";
    char input[1024] = "";

    if(argc < 2)
        print_usage();

    // Create a string of all parameters that are passed to the server.
    for(i = 2; i < argc; i++) {
        if(i > 2)
            strcat(input, " ");
        strcat(input, argv[i]);
    }
    input[strlen(input)] = '\0';

    create_connect_socket(argv[1], &sockfd);

    // Send the request to the server.
    if(writen(sockfd, &input, strlen(input) * sizeof(char)) < 0) {
        perror("Failed to write\n");
        exit(1);
    }

    // Read the response of the server. Do this untill the socket gets closed,
    // to be sure to receive everything.
    while(nread != 0) {
        nread = read(sockfd, buffer, sizeof(buffer));
        if(nread < 0) {
            perror("Error reading from socket");
            exit(1);
        }

        total += nread;

        // Print the answer
        for(i = 0; i < nread; i++)
            printf("%c", buffer[i]);
    }

    // Close the socket and exit the program.
    close(sockfd);
    return 0;
}
