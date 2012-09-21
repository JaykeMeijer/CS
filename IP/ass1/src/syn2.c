#include "syn2.h"

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

    /* Create the semaphores. */
    sem1 = semget(IPC_PRIVATE, 1, 0600);
    sem2 = semget(IPC_PRIVATE, 1, 0600);

    if(fork()) {
        for(i = 0; i < 10; i++) {
            semop(sem1, &down, 1);
            display("cd\n");
            semop(sem2, &up, 1);
        }
        wait(NULL);
    }
    else {
        display("ab");
        semop(sem1, &up, 1);

        for(i = 0; i < 10; i++) {
            semop(sem2, &down, 1);
            display("ab");
            semop(sem1, &up, 1);
        }
    }

    semctl(sem1, 0, IPC_RMID);
    semctl(sem2, 0, IPC_RMID);
    return 0;
}
