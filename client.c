#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "client_fn/io_related.h"
#include "client_fn/log_related.h"
#include "client_fn/socket_related.h"
#include "client_fn/options_related.h"
#include "general_fn/error_handling.h"
#include "general_fn/compress_related.h"
#include "general_fn/conversion_related.h"

#define BUFFER_SIZE 16384

int PORT = 4444; // Set as default

int COMPRESS = 0;
int LOG_SAVE = 0;
int SEND_OR_RECEIVE = 0; // 0 for send and 1 for receive

char* LOG_NAME = NULL;
char* HOST = "127.0.0.1"; // Set as default

int main(int argc, char* argv[]) {
    test_if_options_passed(argc, argv, &LOG_NAME, &PORT, &HOST, &COMPRESS, &LOG_SAVE);
    print_config(PORT, HOST, COMPRESS, LOG_SAVE, LOG_NAME);

    int socket = connect_socket(PORT, HOST);
    printf("[+]Connected to Server.\n\033[0m");

    char signal[2];
    int compress_signal = 0;

    if (COMPRESS) sprintf(signal, "%d", 1);
    else sprintf(signal, "%d", 0);

    compress_signal = send(socket, signal, 2, 0);

    if (compress_signal == -1) error_output("Could Not Send");

    //char* output = NULL;

    //if (!COMPRESS) {
        char output[BUFFER_SIZE];
    //}

    char user_command[BUFFER_SIZE];

    while (1) {
        SEND_OR_RECEIVE = 0;

        printf("\033[32m> ");

        fflush(stdin);
        fgets(user_command, BUFFER_SIZE, stdin);
        user_command[strcspn(user_command, "\n")] = 0;

        //if (COMPRESS) {

        //} else {
            int command_size = strlen(user_command);

            if (send(socket, user_command, command_size, 0) == -1) error_output("Could Not Send");
            
            if (LOG_SAVE) update_log(user_command, LOG_NAME, SEND_OR_RECEIVE);
            
            if (recv(socket, output, BUFFER_SIZE, 0) == -1) error_output("Could Not Receive");
        //}

        SEND_OR_RECEIVE = 1;
        if (LOG_SAVE) update_log(output, LOG_NAME, SEND_OR_RECEIVE);
        
        fflush(stdout);
        printf("\033[33m%s\n", output);

        bzero(output, BUFFER_SIZE);
        bzero(user_command, BUFFER_SIZE);
    }

    free(LOG_NAME);
    free(HOST);
    return 0;
}