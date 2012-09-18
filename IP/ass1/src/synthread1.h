#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

pthread_mutex_t mutex;

void display(char*);
void thread1();
void *thread2();
int main();
