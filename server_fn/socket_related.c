#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "error_handling.h"

int connect_socket(char* host, int port) {
    int sockfd;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) error_output("Could Not Create Socket");

	printf("\033[34m[+]Server Socket Created Successfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(host);

	bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port Number %d.\n", port);

    if (listen(sockfd, 5) == -1) error_output("Could Not Establish Listen");
    printf("[+]Listening...\n");

    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

    if (newSocket == -1) error_output("Could Not Establish Connection");
    return newSocket;
}