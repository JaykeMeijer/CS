#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>

int writen(int, const void*, size_t);
int readn(int, void*, size_t);
void writethread();
void *receivethread();
int sockfd;

int main(int argc, char *argv[]) {
    pthread_t thread_id;
    pthread_attr_t attr;\

    if(argc == 1) {
        /* Server */
        int listensockfd, optval;
        struct sockaddr_in addr, addrc;
        socklen_t addrlen;

        /* Create the listening socket. */
        listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(listensockfd < 0) {
            perror("Error creating socket");
            exit(1);
        }

        /* Allow the reusing of the socket, so the server can be restarted
         * immediatly. */
        optval = 1;
        if(setsockopt(listensockfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                sizeof(optval)) < 0) {
            perror("Setting sockopt fails");
            exit(1);
        }

        /* Prepare the servers address administration. */
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1337);
        addr.sin_addr.s_addr = INADDR_ANY;

        /* Bind the socket to allow it to listen */
        if(bind(listensockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("Error binding socket");
            exit(1);
        }

        /* Listen for a new connection and handle it. */
        if(listen(listensockfd, 5) < 0) {
            perror("Error while listening");
            exit(1);
        }

        addrlen = sizeof(addrc);

        /* Accept the new connection. */
        sockfd = accept(listensockfd, (struct sockaddr *) &addrc, &addrlen);
        if(sockfd < 0) {
            perror("Failed to accept connection");
            exit(1);
        }
        /* Close the listening socket, since the connection is established. */
        close(listensockfd);
    }
    else {
        /* Client */
        struct addrinfo hints;
        struct addrinfo *result, *rp;

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
    }

    /* Create the attributes needed for the threads and initiate the thread. */
    pthread_attr_init(&attr);
    pthread_create(&thread_id, &attr, receivethread, NULL);

    writethread();

    /* Close the socket. */
    close(sockfd);

    /* Wait for the other thread to join. */
    pthread_join(thread_id, NULL);

    /* Destroy all created objects. */
    pthread_attr_destroy(&attr);

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

ssize_t readn(int fd, void *vptr, size_t n) {
    int nread;

    nread = read(sockfd, vptr, n);
    if(nread < 0) {
        return -1; /* error */
    }
    return nread;
}

void writethread() {
    char input[256] = "";

    while(1) {
        if(!fgets(input, sizeof(input), stdin)) {
            perror("Failed to read input");
            exit(1);
        }

        if(writen(sockfd, &input, strlen(input) * sizeof(char)) < 0) {
            perror("Failed to write\n");
            exit(1);
        }
    }
}

void *receivethread() {
    int bread;
    char buffer[256]= "";

    while(1) {
        bread = readn(sockfd, &buffer, sizeof(buffer));
        if(bread < 0) {
            perror("Failed to read from other");
            exit(1);
        }
        else if(bread == 0) {
            perror("Other closed socket");
            exit(1);
        }
        buffer[bread - 1] = '\0';
        printf("-> %s\n", buffer);
    }
}
