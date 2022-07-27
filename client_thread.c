#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//#include <unistd.h>
#include <pthread.h>
//#include <sys/socket.h>
#include "client_fn/io_related.h"
#include "client_fn/log_related.h"
#include "client_fn/socket_related.h"
#include "client_fn/options_related.h"
#include "general_fn/error_handling.h"
#include "general_fn/compress_related.h"

#define BUFFER_SIZE 16384
#define LEN 32

int PORT = 4444; // Set as default

int COMPRESS = 0;
int LOG_SAVE = 0;
int SEND_OR_RECEIVE = 0; // 0 for send and 1 for receive

char* LOG_NAME = NULL;
char* HOST = "127.0.0.1"; // Set as default

volatile sig_atomic_t flag = 0;
int socket_connection;

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void send_msg_handler() {
    char user_command[BUFFER_SIZE];

    while(1) {
  	    printf("\033[32m> ");

        if (fgets(user_command, BUFFER_SIZE, stdin) == NULL) {
            flag = 1;
			break;
        }
        
        user_command[strcspn(user_command, "\n")] = 0;
        if (send(socket_connection, user_command, strlen(user_command), 0) == -1) error_output("Could Not Send");
        SEND_OR_RECEIVE = 0;
        if (LOG_SAVE) update_log(user_command, LOG_NAME, SEND_OR_RECEIVE);

		bzero(user_command, BUFFER_SIZE);
        
        sleep(1);
    }

    catch_ctrl_c_and_exit(SIGINT);
}

void send_compressed_msg_handler() {
    char user_command[BUFFER_SIZE];

    while(1) {
  	    printf("\033[32m> ");

        if (fgets(user_command, BUFFER_SIZE, stdin) == NULL) {
            flag = 1;
			break;
        }

        user_command[strcspn(user_command, "\n")] = 0;

        ulong command_size = strlen(user_command) * sizeof(char) + 1;
        ulong command_byte_size = compressBound(command_size);

        char* user_command_compressed = compress_buffer(user_command, command_size, command_byte_size);        

        if (send(socket_connection, &command_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
        if (send(socket_connection, &command_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
        if (send(socket_connection, user_command_compressed, command_byte_size, 0) == -1) error_output("Could Not Send");

        SEND_OR_RECEIVE = 0;
        if (LOG_SAVE) update_log_compressed(user_command, user_command_compressed, LOG_NAME, SEND_OR_RECEIVE);

        bzero(user_command, BUFFER_SIZE);
        bzero(user_command_compressed, command_byte_size);
        free(user_command_compressed);

        sleep(1);
    }

    catch_ctrl_c_and_exit(SIGINT);
}


void recv_msg_handler() {
    char output[BUFFER_SIZE];

    while (1) {
		int receive = recv(socket_connection, output, BUFFER_SIZE, 0);

        if (receive > 0) {
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log(output, LOG_NAME, SEND_OR_RECEIVE);

            printf("\033[33m%s\n", output);
        } else if (receive == 0) break;
        else error_output("Could Not Receive");

		bzero(output, BUFFER_SIZE);
    }
}

void recv_compressed_msg_handler() {
    while (1) {
        ulong output_size, output_byte_size;

        char* tmp = NULL;
        char* output_uncompressed = NULL;
        
        int receive = recv(socket_connection, &output_size, sizeof(ulong), 0);
        
        if (receive > 0) {
            if (recv(socket_connection, &output_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send");
            
            tmp = (char*)malloc(output_byte_size * sizeof(char));

            if (recv(socket_connection, tmp, output_byte_size, 0) == -1) error_output("Could Not Send");

            output_uncompressed = uncompress_buffer(tmp, output_size, output_byte_size);
            
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log_compressed(output_uncompressed, tmp, LOG_NAME, SEND_OR_RECEIVE);

            printf("\033[33m%s\n", output_uncompressed);
        } else if (receive == 0) break;
        else error_output("Could Not Receive");
        
        bzero(tmp, output_byte_size);
        bzero(output_uncompressed, output_size);
        free(output_uncompressed);
        free(tmp);
    }
}

int main(int argc, char **argv){
    test_if_options_passed_client(argc, argv, &LOG_NAME, &PORT, &HOST, &COMPRESS, &LOG_SAVE);
    print_config(PORT, HOST, COMPRESS, LOG_SAVE, LOG_NAME);

	signal(SIGINT, catch_ctrl_c_and_exit);

    socket_connection = connect_socket_client(PORT, HOST);
    printf("[+]Connected to Server.\n");

    char name[LEN];
    printf("Type your username: ");
    fgets(name, LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    if (send(socket_connection, name, LEN, 0) == -1) error_output("Could Not Send Name");;

    int compress_signal = send(socket_connection, &COMPRESS, sizeof(int), 0);
    if (compress_signal == -1) error_output("Could Not Send");

	printf("=== TERMINAL STARTED ===\n");

    pthread_t send_msg_thread, recv_msg_thread;

    if (COMPRESS) {
        if(pthread_create(&send_msg_thread, NULL, (void*) send_compressed_msg_handler, NULL) != 0) error_output("Could Not Create Thread");
        if(pthread_create(&recv_msg_thread, NULL, (void*) recv_compressed_msg_handler, NULL) != 0) error_output("Could Not Create Thread");
    } else {
        if(pthread_create(&send_msg_thread, NULL, (void*) send_msg_handler, NULL) != 0) error_output("Could Not Create Thread");
        if(pthread_create(&recv_msg_thread, NULL, (void*) recv_msg_handler, NULL) != 0) error_output("Could Not Create Thread");
    }

	while (1) {
		if (flag) {
			printf("\n\033[34m[+]Disconnecting...\n");
			break;
        }
	}

	close(socket_connection);

	return 0;
}