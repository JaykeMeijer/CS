#include "talk.h"

int main(int argc, char *argv[]) {
    if(argc == 1)
        connect_as_server();
    else
        connect_as_client(argv[1]);

    /* Create the attributes needed for the threads and initiate the thread. */
    pthread_attr_init(&attr);
    pthread_create(&thread_id, &attr, receivethread, NULL);

    /* Set the write thread id, so it can be used to signal to. */
    write_id = pthread_self();
    signal(SIGCHLD, exit_write);

    /* Start the writing method. */
    writethread();

    /*
     * If this thread returns, signal to the receiving thread that it should
     * stop.
     */
    pthread_kill(thread_id, SIGTERM);

    quit_program();

    return 0;
}

/*
 * Set up as server, listening to an incoming connection.
 */
void connect_as_server() {
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

/*
 * Set up as a client, directly connecting to the given address.
 */
void connect_as_client(char* address) {
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

/*
 * Receive the input a user gives on STDIN and write it to the socket so the
 * other program can read it.
 *
 * If "!exit" is typed the execution of the program is stopped.
 */
void writethread() {
    char input[256] = "";

    while(1) {
        if(!fgets(input, sizeof(input), stdin)) {
            perror("Failed to read input");
            exit(1);
        }

        /* If "!exit" is typed, exit this function and ultimatly the program. */
        if (!strcmp(input, "!exit\n")) {
            printf("Received command to exit\n");
            break;
        }

        /* Write to the socket. */
        if(writen(sockfd, &input, strlen(input) * sizeof(char)) < 0) {
            perror("Failed to write\n");
            exit(1);
        }
    }
}

/*
 * Receive data that is writen to the socket by the other end. If an empty
 * message is received, the other closed the connection and we should stop.
 */
void *receivethread() {
    int bread;
    char buffer[256]= "";

    signal(SIGTERM, exit_receive);

    while(1) {
        bread = read(sockfd, buffer, sizeof(buffer));
        if(bread < 0) {
            perror("Failed to read from other");
            exit(1);
        }
        else if(bread == 0) {
            printf("Other has closed the connection\n");
            pthread_kill(write_id, SIGCHLD);
            break;
        }
        buffer[bread - 1] = '\0';
        printf("-> %s\n", buffer);
    }
}

/*
 * Signal handler for the receiving thread, when the 'parent' thread signals it
 * to stop execution.
 */
void exit_receive(int sig)
{
    pthread_exit(NULL);
}

/*
 * Signal handler for the writing thread, when the 'child' thread signals that
 * it has stopped executing.
 */
void exit_write(int sig) {
    quit_program();
}

/*
 * Properly shutdown the program, by closing the socket and waiting for the
 * other thread to finish.
 */
void quit_program() {
    printf("Closing program\n");
    /* Close the socket. */
    close(sockfd);

    /* Wait for the other thread to join. */
    pthread_join(thread_id, NULL);

    /* Destroy all created objects. */
    pthread_attr_destroy(&attr);

    exit(0);
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
