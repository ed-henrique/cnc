#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    while (1) {
        char command[100];

        strcpy(command, " ping -c 2 -W 3 google.com");
        
        FILE* pipe;

        pipe = popen(command, "r");

        if (pipe == NULL) {
            puts("Unable to open process");
            return 1;
        }

        while (1) {
            char ch = fgetc(pipe);

            if (ch == EOF) break;

            putchar(ch);
        }

        pclose(pipe);
        break;

        // ADICIONAR COMO ENVIAR COMANDO PRA REDE AQUI
    }

    return 0;
}