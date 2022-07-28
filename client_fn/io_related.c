#include <stdio.h>
#include <stdlib.h>

void print_help(char* argv[]) {
    printf("\nClient-side script to interact with server terminal!\n\n\t"
           "HOW TO USE: %s [OPTIONS]\n\n\t"
           "OPTIONS:\n\t"
           "--help: shows this screen\n\t"
<<<<<<< HEAD
           "-l or --log: creates a log file\n\t"
           "-c ou --compress: compress data size using zlib\n\t"
           "-p or --port: pass a port as a parameter (default is 8000)\n\t"
           "-h or --host: pass a host to access server (default is localhost)\n\n",
=======
           "--log=[LOG_NAME]: enables log saving\n\t"
           "--compress: enables data compression using zlib\n\t"
           "--port=[NEW_PORT]: pass a port as a parameter (default is 4444)\n\t"
           "--host=[NEW_HOST]: pass a host to access server (default is 127.0.0.1)\n\n",
>>>>>>> dev
           argv[0]);

    exit(0);
}

void print_config(int PORT, char* HOST, int COMPRESS, int LOG_SAVE, char* LOG_NAME) {
    printf("\n\033[34mUsing following configurations:\n\t"
        "PORT: %d\n\t"
        "HOST: %s\n\t"
        "COMPRESSING: %s\n\t"
        "SAVING LOGS: %s\n\t"
        "LOG NAME: %s\n\n",
        PORT,
        HOST,
        COMPRESS ? "Yes" : "No",
        LOG_SAVE ? "Yes" : "No",
        LOG_NAME);
}