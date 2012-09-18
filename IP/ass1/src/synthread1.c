#include "synthread1.h"

void display(char *str) {
    char *tmp;
    for (tmp=str; *tmp; tmp++) {
        write(1, tmp, 1);
        usleep(100);
    }
}

/* The code to be executed by the original thread. */
void thread1() {
    int i;
    for(i=0;i<10;i++) {
        pthread_mutex_lock(&mutex);
        display("Hello world\n");
        pthread_mutex_unlock(&mutex);
    }
}

/* The code to be executed by the new thread. */
void *thread2() {
    int i;
    for(i=0;i<10;i++) {
        pthread_mutex_lock(&mutex);
        display("Bonjour monde\n");
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_mutexattr_t mutex_attr;

    /* Create the attributes needed for the thread and mutex. */
    pthread_attr_init(&attr);
    pthread_mutexattr_init(&mutex_attr);

    /* Create the mutex, than create the thread and let it execute its code. */
    pthread_mutex_init(&mutex, &mutex_attr);
    pthread_create(&thread_id, &attr, thread2, NULL);

    thread1();

    /* Wait for the other thread to be done. */
    pthread_join(thread_id, NULL);

    /* Destroy all created objects. */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    return 0;
}
