#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if(argc != 3){
        printf("Usage: %s <seed> <arr_size>\n", argv[0]);
        return EXIT_FAILURE;
    } 
    pid_t child_pid;
    child_pid = fork();
    switch (child_pid)
    {
    case -1: // fork failed
        perror("Fork error");
        exit(EXIT_FAILURE);
    case 0: 
        //get args 
        printf("Child process start at: %d PID\n", getpid());
        char *const exec_args[] = {"sequential_min_max", argv[1], argv[2], NULL};
        execv(exec_args[0],exec_args);
        // if execl returns, it must have failed
        perror("execl failed");
        _exit(EXIT_FAILURE);
    default:
        //parent process
        int status;
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) { // WIFEXITED - checking status
            printf("Child process %d exited with status %d\n", child_pid, WEXITSTATUS(status));
        } else {
            printf("Child process %d exited with err((\n", child_pid);
        }
        printf("Parent process end at: %d PID\n", getpid());
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}