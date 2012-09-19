#include "synthread2.h"

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

/* The code to be executed by the original thread. */
void thread1() {
    int i;

    /* This thread is allowed to start without waiting. */
    display("ab");
    sem_post(&sem1);

    for(i=0;i<9;i++) {
        sem_wait(&sem2);
        display("ab");
        sem_post(&sem1);
    }
}

/* The code to be executed by the new thread. */
void *thread2() {
    int i;

    for(i=0;i<10;i++) {
        sem_wait(&sem1);
        display("cd\n");
        sem_post(&sem2);
    }
}

int main() {
    pthread_t thread_id;
    pthread_attr_t attr;

    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    /* Create the attributes needed for the thread. */
    pthread_attr_init(&attr);

    /* Create the thread and let it execute its code. */
    pthread_create(&thread_id, &attr, thread2, NULL);

    thread1();

    /* Wait for the other thread to be done. */
    pthread_join(thread_id, NULL);

    /* Destroy all created objects. */
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    pthread_attr_destroy(&attr);
    return 0;
}
