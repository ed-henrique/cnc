#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include "client_fn/io_related.h"
#include "client_fn/log_related.h"
#include "client_fn/socket_related.h"
#include "client_fn/options_related.h"
#include "general_fn/error_handling.h"
#include "general_fn/compress_related.h"

#define BUFFER_SIZE 16384
#define LEN 32

int PORT = 4444; // Set as default, can be changed using --port

int COMPRESS = 0; // 1 if compressing
int LOG_SAVE = 0; // 1 if saving logs
int SEND_OR_RECEIVE = 0; // 0 for send and 1 for receive
int WAITING_RESPONSE = 0; // 1 if response is not received yet
int EMPTY_COMMAND = 0; // 1 if command is empty

char* LOG_NAME = NULL; // Log name is changed if --log option is used
char* HOST = "127.0.0.1"; // Set as default, can be changed using --host

volatile sig_atomic_t disconnect = 0; // Signal variable
int socket_connection;

pthread_mutex_t response_mutex = PTHREAD_MUTEX_INITIALIZER;

// Signal handler for when a SIGINT is received, activating flag to disconnect from server
void catch_sigint(int sig) {
    disconnect = 1;
}

// Thread handler to send uncompressed messages using send()
void send_msg() {
    char user_command[BUFFER_SIZE];

    while(1) {
        // This loop waits for the response to be displayed before prompting user for input again
        if (!EMPTY_COMMAND) while (WAITING_RESPONSE) {};

        EMPTY_COMMAND = 0;

  	    printf("\033[32m> ");

        // Gets command
        if (fgets(user_command, BUFFER_SIZE, stdin) == NULL) {
            disconnect = 1;
			break;
        } else if (user_command[0] == '\n' && user_command[1] == '\0') {
    		bzero(user_command, BUFFER_SIZE);
            EMPTY_COMMAND = 1;
			continue;
        }
        
        user_command[strcspn(user_command, "\n")] = 0;
        
        // Sends command
        if (send(socket_connection, user_command, strlen(user_command), 0) == -1) error_output("Could Not Send Command Without Compression");

        // Change log flag to send
        SEND_OR_RECEIVE = 0;
        if (LOG_SAVE) update_log(user_command, LOG_NAME, SEND_OR_RECEIVE);

        // bzero() set bytes to 0, flushing this buffer
		bzero(user_command, BUFFER_SIZE);
    }

    catch_sigint(SIGINT);
}

// Thread handler to send compressed messages using send()
// Compression uses zlib compress()
void send_compressed_msg() {
    char user_command[BUFFER_SIZE];

    while(1) {
        // This loop waits for the response to be displayed before prompting user for input again
        if (!EMPTY_COMMAND) while (WAITING_RESPONSE) {};

        EMPTY_COMMAND = 0;

  	    printf("\033[32m> ");

        // Gets command
        if (fgets(user_command, BUFFER_SIZE, stdin) == NULL) {
            disconnect = 1;
			break;
        } else if (user_command[0] == '\n' && user_command[1] == '\0') {
    		bzero(user_command, BUFFER_SIZE);
            EMPTY_COMMAND = 1;
			continue;
        }

        user_command[strcspn(user_command, "\n")] = 0;

        // Getting command size and compressed command size
        ulong command_size = strlen(user_command) * sizeof(char) + 1;
        ulong command_byte_size = compressBound(command_size);

        // Compresses command using zlib's compress()
        char* user_command_compressed = compress_buffer(user_command, command_size, command_byte_size);       

        // This part involves three sends, which are necessary to uncompress on the server's side
        // The first one sends the command original size, before compression
        // The second one sends the compressed command size
        // The third one sends the compressed command

        if (send(socket_connection, &command_size, sizeof(ulong), 0) == -1) error_output("Could Not Send Command Size");
        if (send(socket_connection, &command_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send Compressed Command Size");
        if (send(socket_connection, user_command_compressed, command_byte_size, 0) == -1) error_output("Could Not Send Compressed Command");

        // Change log flag to send
        SEND_OR_RECEIVE = 0;
        if (LOG_SAVE) update_log_compressed(user_command, user_command_compressed, LOG_NAME, SEND_OR_RECEIVE);

        // bzero() set bytes to 0, flushing buffers
        bzero(user_command, BUFFER_SIZE);
        bzero(user_command_compressed, command_byte_size);
        free(user_command_compressed);
    }

    catch_sigint(SIGINT);
}


// Thread handler to receive uncompressed messages using recv()
void recv_msg() {
    char output[BUFFER_SIZE];

    while (1) {
        // Under a mutex for mutual exclusion
        pthread_mutex_lock(&response_mutex);
        WAITING_RESPONSE = 1;
        pthread_mutex_unlock(&response_mutex);

        // Receives output
		int receive = recv(socket_connection, output, BUFFER_SIZE, 0);

        if (receive > 0) {
            // Change log flag to receive
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log(output, LOG_NAME, SEND_OR_RECEIVE);

            printf("\033[33m%s\n", output);
        } else if (receive == 0) break;
        else error_output("Could Not Receive Output Without Compression");

        // bzero() set bytes to 0, flushing this buffer
		bzero(output, BUFFER_SIZE);

        // Under a mutex for mutual exclusion
        pthread_mutex_lock(&response_mutex);
        WAITING_RESPONSE = 0;
        pthread_mutex_unlock(&response_mutex);
    }
}

// Thread handler to receive uncompressed messages using recv()
// Decompression uses zlib uncompress()
void recv_compressed_msg() {
    while (1) {
        // Under a mutex for mutual exclusion
        pthread_mutex_lock(&response_mutex);
        WAITING_RESPONSE = 1;
        pthread_mutex_unlock(&response_mutex);

        // Output size and output size after compression
        ulong output_size, output_byte_size;

        char* tmp = NULL;
        char* output_uncompressed = NULL;
        
        // This part involves three receives, which are necessary for decompression
        // The first one receives the output original size, before compression
        // The second one receives the compressed output size
        // The third one receives the compressed output

        int receive = recv(socket_connection, &output_size, sizeof(ulong), 0);
        
        if (receive > 0) {
            if (recv(socket_connection, &output_byte_size, sizeof(ulong), 0) == -1) error_output("Could Not Send Compressed Output Size");
            
			// Allocates enough memory for decompressed command size
            tmp = (char*)malloc(output_byte_size * sizeof(char));

            if (recv(socket_connection, tmp, output_byte_size, 0) == -1) error_output("Could Not Send Compressed Output");

            // Decompresses output using zlib's uncompress()
            output_uncompressed = uncompress_buffer(tmp, output_size, output_byte_size);
            
            // Change log flag to receive
            SEND_OR_RECEIVE = 1;
            if (LOG_SAVE) update_log_compressed(output_uncompressed, tmp, LOG_NAME, SEND_OR_RECEIVE);

            printf("\033[33m%s\n", output_uncompressed);
        } else if (receive == 0) break;
        else error_output("Could Not Receive Output Size");
        
        // bzero() set bytes to 0, flushing buffers
        bzero(tmp, output_byte_size);
        bzero(output_uncompressed, output_size);
        free(output_uncompressed);
        free(tmp);

        // Under a mutex for mutual exclusion
        pthread_mutex_lock(&response_mutex);
        WAITING_RESPONSE = 0;
        pthread_mutex_unlock(&response_mutex);
    }
}

int main(int argc, char* argv[]){
    // Test for --port, --log, --help, --host and/or --compress
    test_if_options_passed_client(argc, argv, &LOG_NAME, &PORT, &HOST, &COMPRESS, &LOG_SAVE);

    // Print configurations defined by the user
    print_config(PORT, HOST, COMPRESS, LOG_SAVE, LOG_NAME);

    // Start signal catching
	signal(SIGINT, catch_sigint);

    // Connect to server through TCP/IP socket
    socket_connection = connect_socket_client(PORT, HOST);
    printf("Connected to Server.\n");

    // Getting user name to register on server's side
    char name[LEN];
    printf("Type your username: ");
    fgets(name, LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    if (send(socket_connection, name, LEN, 0) == -1) error_output("Could Not Send Client's Name");

    // Passing flag to indicate if this client uses compressed data or not
    int compress_flag = send(socket_connection, &COMPRESS, sizeof(int), 0);
    if (compress_flag == -1) error_output("Could Not Send Compress Flag");

	printf("=== TERMINAL STARTED ===\n");

    // Two threads are initiated per client, one to send messages and another one to receive messages
    pthread_t send_msg_thread, recv_msg_thread;

    // Checking if data compression is activated to choose the correct thread handlers
    if (COMPRESS) {
        if(pthread_create(&send_msg_thread, NULL, (void*)send_compressed_msg, NULL) != 0) error_output("Could Not Create Compressed Send Message Thread");
        if(pthread_create(&recv_msg_thread, NULL, (void*)recv_compressed_msg, NULL) != 0) error_output("Could Not Create Compressed Receive Message Thread");
    } else {
        if(pthread_create(&send_msg_thread, NULL, (void*)send_msg, NULL) != 0) error_output("Could Not Create Uncompressed Send Message Thread");
        if(pthread_create(&recv_msg_thread, NULL, (void*)recv_msg, NULL) != 0) error_output("Could Not Create Uncompressed Receive Message Thread");
    }

    // Waiting for the client to press ^D and disconnect from server and destroy mutex
	while (1) {
		if (disconnect) {
			printf("\n\033[34mDisconnecting...\n");
			break;
        }
	}

    // Destroy mutex and close socket
    pthread_mutex_destroy(&response_mutex);
	close(socket_connection);

	return 0;
}