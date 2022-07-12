#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int PORT = 8000;

bool COMPRESS = false;
bool LOG_SAVE = false;
bool SEND_OR_RECEIVE = false; // false for send and true for receive

char LOG_NAME[100];
char* HOST = "localhost";

void print_help(char* argv[]) {
    printf("\nClient-side script to interact with server terminal!\n\n\t"
           "HOW TO USE: %s [OPTIONS]\n\n\t"
           "OPTIONS:\n\t"
           "--help: shows this screen\n\t"
           "-l or --log: creates a log file\n\t"
           "-c ou --compress: compress data size using zlib\n\t"
           "-p or --port: pass a port as a parameter (default is 8000)\n\t"
           "-h or --host: pass a host to access server (default is localhost)\n\n",
           argv[0]);

    exit(0);
}

void save_log_as_file(char* log_new_name) {
    int str_size = strlen(log_new_name) - 2;
    char log_name[str_size + 5];
    strncpy(log_name, &log_new_name[6], str_size);
    log_name[str_size - 4] = '.';
    log_name[str_size - 3] = 't';
    log_name[str_size - 2] = 'x';
    log_name[str_size - 1] = 't';
    log_name[str_size] = '\0';

    strcpy(LOG_NAME, log_name);

    time_t raw_time = time(NULL);
    char* time_str = ctime(&raw_time);

    time_str[strlen(time_str) - 1] = '\0';

    FILE* fp;

    fp = fopen(log_name, "w");

    if (fp != NULL) fprintf(fp, "[%s] LOG WAS CREATED\n", time_str);

    fclose(fp);
}

void update_log(char* command) {
    FILE* fp;
    
    fp = fopen(LOG_NAME, "a");

    time_t raw_time = time(NULL);
    char* time_str = ctime(&raw_time);

    time_str[strlen(time_str) - 1] = '\0';

    if (fp != NULL) {
        if (!SEND_OR_RECEIVE) fprintf(fp, "[%s] SENT # BYTES: %s\n", time_str, command);
        else fprintf(fp, "[%s] RECEIVED # BYTES: %s\n", time_str, command);
    }

    fclose(fp);
}

void change_port(char* new_port) {
    int str_size = strlen(new_port) - 7;
    char tmp_str[str_size + 1];
    strncpy(tmp_str, &new_port[7], str_size);
    tmp_str[str_size] = '\0';
    PORT = strtol(tmp_str, NULL, 10);
}

void change_host(char* new_host) {
    int str_size = strlen(new_host) - 7;
    char tmp_str[str_size + 1];
    strncpy(tmp_str, &new_host[7], str_size);
    tmp_str[str_size] = '\0';
    strcpy(HOST, tmp_str);
}

void test_if_options_passed(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], "--help") != NULL) print_help(argv);
        else if (strstr(argv[i], "--log") != NULL) {
            LOG_SAVE = true;
            save_log_as_file(argv[i]);
        }
        else if (strstr(argv[i], "--compress") != NULL) COMPRESS = true;
        else if (strstr(argv[i], "--port") != NULL) change_port(argv[i]);
        else if (strstr(argv[i], "--host") != NULL) change_host(argv[i]);
        else printf("No options passed\n");
    }
}

int main(int argc, char* argv[]) {
    test_if_options_passed(argc, argv);
    
    printf("\n\033[31mUsing following configurations:\033[0m\n\t"
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

    while (1) {
        SEND_OR_RECEIVE = false;

        printf("\033[32m> ");

        char user_command[100];

        fgets(user_command, 100, stdin);

        user_command[strcspn(user_command, "\n")] = 0;

        update_log(user_command);
        printf("\033[33mComando enviado...\n", user_command);

        SEND_OR_RECEIVE = true;

        // ADICIONAR COMO ENVIAR COMANDO PRA REDE AQUI
    }

    return 0;
}