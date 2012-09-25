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

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, res, optval, counter = 0;
    struct sockaddr_in addr, addrc;
    socklen_t addrlen;
    
    signal(SIGCHLD, SIG_IGN);

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
    addr.sin_family = AF_INET;
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

        if(fork() == 0) {
            /* Write the counter to the connected client. */
            writen(newsockfd, &counter, sizeof(counter));

            /* Close this socket and go again. */
            close(newsockfd);
            exit(0);
        }
        else {
            /* Increment the counter, close the socket from this process and
             * prepare to go again. */
            counter++;
            close(newsockfd);
        }
    }

    return 0;
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

