#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>

struct sembuf up = {0, 1, 0};
struct sembuf down = {0, -1, 0};
int sem1, sem2;

void display(char*);
void thread1();
void *thread2();
int main();
