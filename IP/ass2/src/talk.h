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

/* Function declarations */
void connect_as_server();
void connect_as_client(char*);
void writethread();
void *receivethread();
void exit_receive(int);
void exit_write(int);
void quit_program();
int writen(int, const void*, size_t);

/* Global variables */
int sockfd;
pthread_t write_id;
pthread_t thread_id;
pthread_attr_t attr;
