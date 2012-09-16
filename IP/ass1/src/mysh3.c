#include "mysh3.h"

int main(int argc, char *argv[]) {
    char input[128] = "";
    printf("Welcome to the microshell 3\n");

    while(1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        handle_double_input(input);
    }
}

/* Handle the input given to the shell by executing it as command. */
void handle_input(char input[]) {
    pid_t pid;
    char clean_input[128];
    char *input_array[64];
    char *temp;
    char buffer[128];
    int i = 0;

    /* Parse the command to a list of arguments. */
    temp = strtok(input, " ");
    while(temp != NULL) {
        input_array[i] = temp;
        temp = strtok(NULL, " ");
        i++;
    }

    /* Terminate the list with a NULL pointer. */
    input_array[i] = NULL;

    execvp(input_array[0], input_array);
    perror("Failed to start program");
    exit(1);
}

/*
 * Handle the input for two programs by spliting the commands, piping the output
 * of the first to the input of the second.
 */
void handle_double_input(char input[]) {
    char *temp, *commands[2];
    int i, pipe[2];
    pid_t pid1, pid2;

    if(check_input(input)) {
        input[strlen(input) - 1] = '\0';

        /* split the commands. */
        temp = strtok(input, "|");
        while(temp != NULL) {
            commands[i] = temp;
            temp = strtok(NULL, "|");
            i++;
            if(i > 2) {
                perror("Too many piped commands given");
                exit(1);
            }
        }

        if(i == 1) { // No pipe in the command, execute normally
            printf("Only one command given:\n\t%s", commands[0]);
            /* Create a childprocess and execute the command. */
            pid = fork();
            if(pid == 0) {
                handle_input(commands[0]);
            }
            else if(pid < 0) {
                perror("Failed to fork");
                exit(1);
            }
            else
                /* Parent process wait untill the child is done. */
                wait(0);
        }
        else { // A Pipe in the input, execute both commands
            printf("Two commands given:\n\t%s\n\t%s", commands[0], commands[1]);

            /* Create the pipe. */
            if(pipe(pipe) < 0) {
                perror("Failed to create pipe.");
                exit(1);
            }

            /* Create the first childprocess and execute the first command. */
            pid1 = fork();
            if(pid1 == 0) {
                close(p[0]); // close read
                dup2(pipe, STDOUT_FILENO); // duplicate pipe and output
                handle_input(commands[0]);
            }
            else if(pid1 < 0) {
                perror("Failed to fork");
                exit(1);
            }
            else {
                /* Parent process create second process. */
                pid2 = fork();

                if(pid2 == 0) {
                    close(p[1]); // close write
                    dup2(pipe, STDIN_FILENO); // duplicate pipe and input
                    handle_input(commands[1]);
                }
                else if(pid2 < 0) {
                    perror("Failed to fork");
                    exit(1);
                }

                /* Parent process wait untill the children are done. */
                wait(0);
            }
        }
    }
}

/* Check if the input is a (series of) command(s) */
int check_input(char[] input) {
    if (!strcmp(input, "exit\n"))
        /* Command to exit the shell was given. */
        exit(0);
    else if(strlen(input) <= 1) {
        /* If an empty line or only [enter] was given, do not try to execute. */
        printf("\n");
        return 0;
    }
    else
        return 1;
}