#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4444

int main() {
    int sockfd;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+]Server Socket Created Successfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port number %d.\n", PORT);

    listen(sockfd, 5);
    printf("[+]Listening...\n");

    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
    printf("[+]Connection with client was established.\n");
    
    char command[1024];
    char output[1024];
    
    while (1) {
        recv(newSocket, command, 1024, 0);

        if (strcmp(command, "") == 0) {
            printf("[+]Client disconnected.\n");
            break;
        }

        char* tmp;
        strcpy(tmp, command);

        printf("[+]Received command: %s\n", tmp);

        FILE* pipe;

        pipe = popen(tmp, "r");

        if (pipe == NULL) {
            printf("Unable to open process.\n");
            return 1;
        }
        fgets(output, 1024, pipe);
        pclose(pipe);

        printf("[+]Ouput: %s\n", output);

        send(newSocket, output, strlen(output), 0);
        printf("[+]Sent desired output.\n");

        bzero(output, 1024);
        bzero(command, 1024);
    }
    return 0;
}