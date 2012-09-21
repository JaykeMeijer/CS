#include "mysh1.h"

int main(int argc, char *argv[]) {
    char input[128] = "";
    printf("Welcome to the microshell\n");
    
    while(1) {
        printf("> ");
        if(!fgets(input, sizeof(input), stdin)) {
            perror("Failed to read input");
            exit(1);
        }
        handle_input(input);
    }
}

/* Handle the input given to the shell by executing it as command. */
void handle_input(char input[]) {
    pid_t pid;
    char clean_input[128];
    
    if (!strcmp(input, "exit\n"))
        /* Command to exit the shell was given. */
        exit(0);
    else if(strlen(input) <= 1) {
        /* If an empty line or only [enter] was given, do not try to execute. */
        printf("\n");
        return;
    }
    else {
        input[strlen(input) - 1] = '\0';
        
        /* Create a child process and execute the command. */
        pid = fork();
        if(pid == 0) {
            execlp(input, input, (char*) 0);
            perror("Failed to start program");
            exit(1);
        }
        else if(pid < 0) {
            perror("Failed to fork");
            exit(1);
        }
        else
            /* Parent process wait untill the child is done. */
            wait(0);
    }
}
