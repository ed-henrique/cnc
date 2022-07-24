#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../general_fn/error_handling.h"

int change_port(char* new_port) {
    int str_size = strlen(new_port) - 7;

    char port[str_size + 1];

    strncpy(port, &new_port[7], str_size);
    port[str_size] = '\0';

    return strtol(port, NULL, 10);
}

char* change_host(char* new_host) {
    int str_size = strlen(new_host) - 7;

    char* host = (char*)malloc(sizeof(char) * (str_size + 1));

    strncpy(host, &new_host[7], str_size);
    host[str_size] = '\0';

    return host;
}

int connect_socket(int PORT, char* HOST) {
    int clientSocket;
	struct sockaddr_in serverAddr;

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) error_output("Could Not Create Socket");

	printf("\033[34m[+]Client Socket Created Successfully.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(HOST);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) error_output("Could Not Connect");

    return clientSocket;
}