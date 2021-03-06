#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define NR_CHILD 3

struct sembuf up = {0, 1, 0};
struct sembuf down = {0, -1, 0};

ssize_t writen(int, const void*, size_t);
void child_function();
void exit_neatly();

int children[NR_CHILD];
int counter, *shared_counter, sockfd, sem, semaccept;

int main(int argc, char *argv[]) {
    int optval, i, parent, new_pid;
    char input[32];
    unsigned short semargs[1] = {1};
    struct sockaddr_in addr;

    sem = semget(IPC_PRIVATE, 1, 0600);
    semctl(sem, 0, SETALL, semargs);
    semaccept = semget(IPC_PRIVATE, 1, 0600);
    semctl(semaccept, 0, SETALL, semargs);

    /* Create listening socket. */
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

    /* Listen on sockfd for new connections. */
    if(listen(sockfd, 5) < 0) {
        perror("Error while listening");
        exit(1);
    }

    /* Create and attach the counter in the shared memory. */
    counter = shmget(IPC_PRIVATE, sizeof(int), 0600);
    shared_counter = (int *) shmat(counter, 0, 0);

    for(i = 0; i < NR_CHILD; i++) {
        new_pid = fork();
        if(new_pid == 0) {
            parent = 0;
            child_function();
        }
        else if(new_pid < 0) {
            perror("Fork failed");
            exit(1);
        }
        else {
            parent = 1;
            children[i] = new_pid;
            printf("Child %i created\n", i);
        }
    }

    if(parent) {
        signal(SIGINT, exit_neatly);

        /* Wait for a kill command. */
        while(1) {
            if(!fgets(input, sizeof(input), stdin)) {
                perror("Failed to read input");
                exit(1);
            }
            if (!strcmp(input, "exit\n"))
                break;
        }

        exit_neatly();
    }

    return 0;
}

/* The function performed by the child process. */
void child_function() {
    int newsockfd, res, temp;
    socklen_t addrlen;
    struct sockaddr_in addrc;

    addrlen = sizeof(addrc);

    /* Repeatedly listen for a new connection and handle it. */
    while(1) {
        /* Accept the new connection. */
        semop(semaccept, &down, 1);
        newsockfd = accept(sockfd, (struct sockaddr *) &addrc, &addrlen);
        semop(semaccept, &up, 1);
        if(newsockfd < 0) {
            perror("Failed to accept connection");
            exit(1);
        }

        semop(sem, &down, 1);
        temp = htonl((*shared_counter)++);
        semop(sem, &up, 1);

        if(writen(newsockfd, &temp, sizeof(int)) < 0) {
            perror("Failed to write to socket");
            exit(1);
        }

        close(newsockfd);
    }

    shmdt(shared_counter);
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
        if (nwritten <= 0 ) {
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

/*
 * Exit neatly by first signalling the children to stop, then wait untill the
 * children have indeed stopped. After that, close the listening socket and
 * delete the shared memory.
 */
void exit_neatly() {
    int i;

    printf("byebye\n");

    /* Kill all the children. */
    for(i = 0; i < NR_CHILD; i++)
        kill(children[i], SIGTERM);

    /* Wait for the child processes to stop. */
    for(i = 0; i < NR_CHILD; i++)
        waitpid(children[i], NULL, 0);

    /* Close the socket. */
    close(sockfd);    

    /* Delete the shared counter. */
    shmdt(shared_counter);
    shmctl(counter, IPC_RMID, NULL);

    /* Delete the semaphores. */
    semctl(sem, 0, IPC_RMID);
    semctl(semaccept, 0, IPC_RMID);

    exit(0);
}

