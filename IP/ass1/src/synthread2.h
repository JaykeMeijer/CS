#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

sem_t sem1, sem2;

void display(char*);
void thread1();
void *thread2();
int main();
