#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../general_fn/error_handling.h"

void command_output_with_fork(char* command, char* output, int BUFFER_SIZE) {
    int pipe_flag[2];
    pid_t pid;

    if (pipe(pipe_flag)==-1) error_output("Could Not Pipe");

    if ((pid = fork()) == -1) error_output("Could Not Fork");

    if (pid == 0) {
        close(pipe_flag[0]);
        dup2(pipe_flag[1], STDOUT_FILENO);
        close(pipe_flag[1]);
        
        execl("/bin/bash", "sh", "-c", command,(char *)0);
        error_output("Could Not Execute");
    }

    wait(NULL);

    close(pipe_flag[1]);
    char tmp[BUFFER_SIZE];
    int nbytes = read(pipe_flag[0], tmp, BUFFER_SIZE);
    if (nbytes == -1) //error_output("Could Not Read");
    close(pipe_flag[0]);

    if (nbytes != -1) sprintf(output, "%.*s", nbytes, tmp);

    if (output[0] == '\0') {
        printf("\033[31m[+]Something Went Wrong When Executing The Command.\n");
        bzero(output, BUFFER_SIZE);
        strcpy(output, "Output was either blank or command doesn't exist. Please, retry!");
    }
}