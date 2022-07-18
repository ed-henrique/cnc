#include <stdio.h>
#include <errno.h>
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
	printf("\033[34m[+]Server Socket Created Successfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port Number %d.\n", PORT);

    listen(sockfd, 5);
    printf("[+]Listening...\n");

    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
    printf("[+]Connection with Client was Established.\n\033[0m");
    
    char command[1024];
    char output[1024];
    
    while (1) {
        recv(newSocket, command, 1024, 0);

        if (strcmp(command, "") == 0) {
            printf("\033[34m[+]Client Disconnected.\n");
            break;
        }

        char tmp[1024];
        strcpy(tmp, command);

        printf("\033[34m[+]Received Command: \033[32m%s\n", tmp);

        FILE* pipe;

        if ((pipe = popen(tmp, "r")) == NULL) {
            fprintf(stderr, "\033[31m[+]Unable to Open Pipe.\n");
            return 1;
        }

        while (fgets(output, 1024, pipe)) {
            if (output[strspn(output, " ")] == '\n') continue;
        }
        
        output[strcspn(output, "\n")] = 0;
        printf("\033[34m[+]Ouput: \033[33m%s\n", output);

        if (strcmp(output, "") == 0) {
            printf("\033[31m[+]Something Went Wrong.\n");
            bzero(output, 1024);
            strcpy(output, "Output was either blank or command doesn't exist. Please, retry!");
        }

        pclose(pipe);

        send(newSocket, output, strlen(output), 0);
        printf("\033[34m[+]Sent Desired Output.\n");

        bzero(tmp, 1024);
        bzero(output, 1024);
        bzero(command, 1024);
    }
    return 0;
}