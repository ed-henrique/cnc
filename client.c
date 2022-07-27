#include <zlib.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include "client_fn/io_related.h"
#include "client_fn/log_related.h"
#include "client_fn/socket_related.h"
#include "client_fn/options_related.h"
#include "general_fn/error_handling.h"
#include "general_fn/compress_related.h"

#define BUFFER_SIZE 16384

int PORT = 4444; // Set as default

int COMPRESS = 0;
int LOG_SAVE = 0;
int SEND_OR_RECEIVE = 0; // 0 for send and 1 for receive

char* LOG_NAME = NULL;
char* HOST = "127.0.0.1"; // Set as default

int main(int argc, char* argv[]) {
    test_if_options_passed_client(argc, argv, &LOG_NAME, &PORT, &HOST, &COMPRESS, &LOG_SAVE);
    print_config(PORT, HOST, COMPRESS, LOG_SAVE, LOG_NAME);

    int socket = connect_socket_client(PORT, HOST);
    printf("[+]Connected to Server.\n\033[0m");

    int compress_signal = send(socket, &COMPRESS, sizeof(int), 0);

    if (compress_signal == -1) error_output("Could Not Send");

    char output[BUFFER_SIZE];
    char user_command[BUFFER_SIZE];

    while (1) {
        SEND_OR_RECEIVE = 0;

        printf("\033[32m> ");

        fflush(stdin);
        fgets(user_command, BUFFER_SIZE, stdin);
        user_command[strcspn(user_command, "\n")] = 0;

        if (COMPRESS) {
            ulong command_size = strlen(user_command) * sizeof(char) + 1;
            ulong command_byte_size = compressBound(command_size);

            char* user_command_compressed = compress_buffer(user_command, command_size, command_byte_size);

            if (send(socket, &command_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (send(socket, &command_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (send(socket, user_command_compressed, command_byte_size, 0) == -1) error_output("Could Not Send");

            if (LOG_SAVE) update_log_compressed(user_command, user_command_compressed, LOG_NAME, SEND_OR_RECEIVE);

            ulong output_size;
            ulong output_byte_size;

            if (recv(socket, &output_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            if (recv(socket, &output_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            
            char* tmp = (char*)malloc(output_byte_size * sizeof(char));

            if (recv(socket, tmp, output_byte_size, 0) == -1) error_output("Could Not Send");

            char* output_uncompressed = uncompress_buffer(tmp, output_size, output_byte_size);
            
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log_compressed(output_uncompressed, tmp, LOG_NAME, SEND_OR_RECEIVE);

            fflush(stdout);
            printf("\033[33m%s\n", output_uncompressed);

            bzero(tmp, output_byte_size);
            bzero(user_command, BUFFER_SIZE);
            bzero(output_uncompressed, output_size);
            bzero(user_command_compressed, command_byte_size);
            free(user_command_compressed);
            free(output_uncompressed);
            free(tmp);
        } else {
            int command_size = strlen(user_command);

            if (send(socket, user_command, command_size, 0) == -1) error_output("Could Not Send");
            
            if (LOG_SAVE) update_log(user_command, LOG_NAME, SEND_OR_RECEIVE);
            
            if (recv(socket, output, BUFFER_SIZE, 0) == -1) error_output("Could Not Receive");
            
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log(output, LOG_NAME, SEND_OR_RECEIVE);
            
            fflush(stdout);
            printf("\033[33m%s\n", output);

            bzero(output, BUFFER_SIZE);
            bzero(user_command, BUFFER_SIZE);
        }
    }
    free(LOG_NAME);
    free(HOST);
    return 0;
}