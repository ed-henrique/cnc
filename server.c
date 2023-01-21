#include "general_fn/compress_related.h"
#include "general_fn/error_handling.h"
#include "server_fn/options_related.h"
#include "server_fn/pipe_io_with_fork.h"
#include "server_fn/socket_related.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zlib.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 16384
#define HOST "127.0.0.1"
#define LEN 32

int PORT = 4444; // Set as default, can be changed using --port

// This variable is atomic because there is a need for multiple threads to
// access it simultaneously
static _Atomic unsigned int cli_count = 0;

static int uid = 10;

// Struct to hold important clients data
typedef struct {
  struct sockaddr_in address; // Client IP address
  int sockfd;                 // Client socket
  int uid;                    // Client ID
  int compress;               // If client has data compression activated
  char name[LEN];             // Client name
} client_t;

// Number of threads to uphold a maximum number of clients
client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Print IP address using
void print_client_addr(struct sockaddr_in addr) {
  printf("%d.%d.%d.%d", addr.sin_addr.s_addr & 0xff,
         (addr.sin_addr.s_addr & 0xff00) >> 8,
         (addr.sin_addr.s_addr & 0xff0000) >> 16,
         (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

// Add clients to queue
void queue_add(client_t *cl) {
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i]) {
      clients[i] = cl;
      break;
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

// Remove clients from queue
void queue_remove(int uid) {
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i]) {
      if (clients[i]->uid == uid) {
        clients[i] = NULL;
        break;
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

// Handles all communication with the client
void *handle_client(void *client) {
  // This flag indicates when client disconnects
  int leave_flag = 0;

  // Updates client count
  cli_count++;

  // Parses client from void* to client_t*
  client_t *cli = (client_t *)client;

  // Receives both compression flag and client's name
  if (recv(cli->sockfd, cli->name, LEN, 0) == -1)
    error_output((char *)"Could Not Receive Client's Name");
  if (recv(cli->sockfd, &cli->compress, sizeof(int), 0) == -1)
    error_output((char *)"Could Not Receive Compress Flag");

  // Prints out client's name and address, announcing its connection
  printf("\033[34m%s (", cli->name);
  print_client_addr(cli->address);
  printf(") connected to the server.\n");

  // Creating buffers
  char output[BUFFER_SIZE];
  char command[BUFFER_SIZE];

  while (1) {
    // Checking if disconnection flag is up
    if (leave_flag)
      break;

    // Handling client if data compression is activated
    if (cli->compress) {
      // Creating variables to store command size and compressed command size
      ulong command_size;
      ulong command_byte_size;

      // This part involves three receives, which are necessary for
      // decompression The first one receives the command original size, before
      // compression The second one receives the compressed command size The
      // third one receives the compressed command

      // Receives command size and disconnects if no input is found
      int receive = recv(cli->sockfd, &command_size, sizeof(ulong), 0);
      if (receive == -1)
        error_output((char *)"Could Not Receive Command Size");
      else if (receive == 0) {
        // bzero() set bytes to 0, flushing this buffer
        bzero(output, BUFFER_SIZE);

        // Flag to disconnect is set
        leave_flag = 1;
        continue;
      }

      if (recv(cli->sockfd, &command_byte_size, sizeof(ulong), 0) == -1)
        error_output((char *)"Could Not Receive Compressed Command Size");

      // Allocates enough memory for decompressed command size
      char *command = (char *)malloc(command_byte_size * sizeof(char));

      if (recv(cli->sockfd, command, command_byte_size, 0) == -1)
        error_output((char *)"Could Not Receive Compressed Command");

      // Decompresses command using zlib's uncompress()
      char *tmp = uncompress_buffer(command, command_size, command_byte_size);

      // Prints out client's name, address and command given
      printf("\033[32m> %s (", cli->name);
      print_client_addr(cli->address);
      printf(") sent command %s.\n", tmp);

      // Uses pipe(), fork() and execl() to execute command
      command_output_with_fork(tmp, output, BUFFER_SIZE);

      printf("\033[33mOUTPUT:\n%s\033[0m\n", output);

      // Getting output size and compressed output size
      ulong output_size = strlen(output) * sizeof(char) + 1;
      ulong output_byte_size = compressBound(output_size);

      // Compresses  using zlib's compress()
      char *output_compressed =
          compress_buffer(output, output_size, output_byte_size);

      // This part involves three sends, which are necessary to uncompress on
      // the server's side The first one sends the output original size, before
      // compression The second one sends the compressed output size The third
      // one sends the compressed output

      if (send(cli->sockfd, &output_size, sizeof(ulong), 0) == -1)
        error_output((char *)"Could Not Send Output Size");
      if (send(cli->sockfd, &output_byte_size, sizeof(ulong), 0) == -1)
        error_output((char *)"Could Not Send Compressed Output Size");
      if (send(cli->sockfd, output_compressed, output_byte_size, 0) == -1)
        error_output((char *)"Could Not Send Compressed Output");

      // bzero() set bytes to 0, flushing this buffer
      bzero(tmp, command_size);
      bzero(command, command_byte_size);
      bzero(output_compressed, output_byte_size);
      free(output_compressed);
      free(command);
      free(tmp);
    } else {
      // Receives command
      int receive = recv(cli->sockfd, command, BUFFER_SIZE, 0);
      if (receive == -1)
        error_output((char *)"Could Not Receive Uncompressed Command");
      else if (receive == 0) {
        // bzero() set bytes to 0, flushing this buffer
        bzero(output, BUFFER_SIZE);
        bzero(command, BUFFER_SIZE);

        // Flag to disconnect is set
        leave_flag = 1;
        continue;
      }

      // Prints out client's name, address and command given
      printf("\033[32m> %s (", cli->name);
      print_client_addr(cli->address);
      printf(") sent command %s.\n", command);

      // Uses pipe(), fork() and execl() to execute command
      command_output_with_fork(command, output, BUFFER_SIZE);

      printf("\033[33mOUTPUT:\n%s\033[0m\n", output);

      // Output is sent
      if (send(cli->sockfd, output, strlen(output), 0) == -1)
        error_output((char *)"Could Not Send Uncompressed Output");

      printf("\033[34m[+]Sent Output.\n");

      // bzero() set bytes to 0, flushing this buffer
      bzero(output, BUFFER_SIZE);
      bzero(command, BUFFER_SIZE);
    }
  }

  // Prints out client's name and address, announcing its disconnection
  printf("\033[34m%s (", cli->name);
  print_client_addr(cli->address);
  printf(") disconnected from the server.\n");

  // Delete client from queue, decreases client count and yield thread
  close(cli->sockfd);
  queue_remove(cli->uid);
  free(cli);
  cli_count--;
  pthread_detach(pthread_self());

  return NULL;
}

int main(int argc, char *argv[]) {
  // Test for --port
  test_if_options_passed_server(argc, argv, &PORT);

  // Socket option set to 1
  int option = 1;

  // File descriptors for listen
  int listen_fd;

  // Server and client addresses
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  // Thread id
  pthread_t tid;

  // Setting up server IP and port
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(HOST);
  serv_addr.sin_port = htons(PORT);

  // Start signal catching
  signal(SIGPIPE, SIG_IGN);

  // Set socket options
  if (setsockopt(listen_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),
                 (char *)&option, sizeof(option)) < 0)
    error_output((char *)"Could Not Set Socket Options");

  // Bind server to IP
  if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error_output((char *)"Could Not Bind Server");

  // Listening for clients
  if (listen(listen_fd, 10) < 0)
    error_output((char *)"Could Not Listen");

  printf("=== SERVER TERMINAL ===\n");

  while (1) {
    // File descriptor for connect
    int conn_fd;

    // Get client address length
    socklen_t client_len = sizeof(cli_addr);

    // Connecting to client
    conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &client_len);
    if (conn_fd == -1)
      error_output((char *)"Could Not Connect To Client");

    // Check if maximum client number is reached
    if ((cli_count + 1) == MAX_CLIENTS) {
      printf("Maximum number of clients reached. Rejected: ");
      print_client_addr(cli_addr);
      printf(":%d\n", cli_addr.sin_port);
      close(conn_fd);
      continue;
    }

    // Client settings
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->address = cli_addr;
    cli->sockfd = conn_fd;
    cli->uid = uid++;
    cli->compress = 0;

    // Add client to queue and create its thread
    queue_add(cli);
    pthread_create(&tid, NULL, &handle_client, (void *)cli);

    sleep(1);
  }

  // Destroy mutex and close socket
  pthread_mutex_destroy(&clients_mutex);
  close(listen_fd);

  return 0;
}