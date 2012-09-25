#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

int writen(int, const void*, size_t);
void child_function(int*);

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, res, optval, p[2];
    struct sockaddr_in addr, addrc;
    socklen_t addrlen;

    /* Create pipe. */
    if(pipe(p) < 0) {
        perror("Failed to create pipe.");
        exit(1);
    }

    if(fork() == 0) {
        close(p[1]);
        child_function(p);
        exit(0);
    }
    else {
        close(p[0]);

        /* Create the listening socket. */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0) {
            perror("Error creating socket");
            exit(1);
        }

        /* Allow the reusing of the socket, so the server can be restarted
         * immediatly. */
        optval = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                sizeof(optval)) < 0) {
            perror("Setting sockopt fails");
            exit(1);
        }

        /* Prepare the servers address administration. */
        addr.sin_family = AF_UNIX;
        addr.sin_port = htons(1337);
        addr.sin_addr.s_addr = INADDR_ANY;

        /* Bind the socket to allow it to listen */
        if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("Error binding socket");
            exit(1);
        }

        /* Repeatedly listen for a new connection and handle it. */
        while(1) {
            res = listen(sockfd, 5);
            if(res < 0) {
                perror("Error while listening");
                exit(1);
            }

            addrlen = sizeof(addrc);

            /* Accept the new connection. */
            newsockfd = accept(sockfd, (struct sockaddr *) &addrc, &addrlen);
            if(newsockfd < 0) {
                perror("Failed to accept connection");
                exit(1);
            }

            printf("Created socket fd: %i\n", newsockfd);

            /* Sent socket fd to child */
            if(write(p[1], &newsockfd, sizeof(newsockfd)) < 0) {
                perror("Failed to read socket fd from pipe");
                exit(1);
            }
        }
    }

    return 0;
}

/* The function performed by the child process. */
void child_function(int p[2]) {
    int newsockfd;
    int counter = 0;

    printf("Child process operational\n");

    while(1) {
        printf("Waiting for write on pipe\n");

        /* Receive the socket fd from the parent process. */
        if(read(p[0], &newsockfd, sizeof(newsockfd)) < 0) {
            perror("Failed to read socket fd from pipe");
            exit(1);
        }

        printf("Received fd: %i\n", newsockfd);

        /* Write the counter to the connected client. */
        if(writen(newsockfd, &counter, sizeof(counter)) < 0) {
            perror("Failed to write to socket");
            exit(1);
        }
        counter++;

        /* Close this socket and go again. */
        close(newsockfd);
    }
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
        if (nwritten = write(fd, ptr, nleft) <= 0 ) {
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

