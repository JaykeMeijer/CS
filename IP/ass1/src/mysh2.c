#include "mysh2.h"

int main(int argc, char *argv[]) {
    char input[128] = "";
    printf("Welcome to the microshell 2\n");
    
    while(1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        handle_input(input);
    }
}

/* Handle the input given to the shell by executing it as command. */
void handle_input(char input[]) {
    pid_t pid;
    char clean_input[128];
    char *input_array[64];
    char *temp;
    int i = 0;
    
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
        
        /* Parse the command to a list of arguments. */
        temp = strtok(input, " ");
        while(temp != NULL) {
            input_array[i] = temp;
            temp = strtok(NULL, " ");
            i++;
        }
        
        /* Terminate the list with a NULL pointer. */
        input_array[i] = NULL;
        
        /* Create a childprocess and execute the command. */
        pid = fork();
        if(pid == 0) {
            execvp(input_array[0], input_array);
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
