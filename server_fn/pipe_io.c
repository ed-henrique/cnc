#include <stdio.h>
#include <string.h>
#include "error_handling.h"

#define BUFFER_SIZE 4096

void command_output(char* command, char* output) {
    printf("%s\n", command);

    FILE* pipe = popen(command, "r");

    if (pipe == NULL) error_output("Unable to Open Pipe");

    int new_size = fread(output, sizeof(char), BUFFER_SIZE, pipe);

    if (ferror(pipe) != 0) error_output("Error reading pipe");
    else output[new_size++] = '\0';

    pclose(pipe);

    printf("\033[34m[+]Ouput:\n\033[33m%s\n", output);

    if (strcmp(output, "") == 0) {
        printf("\033[31m[+]Something Went Wrong.\n");
        bzero(output, BUFFER_SIZE);
        strcpy(output, "Output was either blank or command doesn't exist. Please, retry!");
    }

}