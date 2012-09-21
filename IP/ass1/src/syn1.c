#include "syn1.h"

void display(char *str) {
    char *tmp;
    for (tmp=str; *tmp; tmp++) {
        if(!write(1, tmp, 1)) {
            perror("Failed to write to stdout");
            exit(1);
        }
        usleep(100);
    }
}

int main() {
    int i;

    /* Create the semaphore and increase to one to allow 1 process in. */
    sem = semget(IPC_PRIVATE, 1, 0600);
    semop(sem, &up, 1);

    if(fork()) {
        for(i = 0; i < 10; i++) {
            semop(sem, &down, 1);
            display("Hello world\n");
            semop(sem, &up, 1);
        }
        wait(NULL);
    }
    else {
        for(i = 0; i < 10; i++) {
            semop(sem, &down, 1);
            display("Bonjour monde\n");
            semop(sem, &up, 1);
        }
    }

    semctl(sem, 0, IPC_RMID);

    return 0;
}
