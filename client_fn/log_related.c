#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* create_log(char* log_new_name, int PORT, char* HOST, bool COMPRESS, bool LOG_SAVE) {
    int str_size = strlen(log_new_name) - 2;


    char log_name[str_size + 5];
    char* LOG_NAME = (char*)malloc(sizeof(char) * (str_size + 5));
    
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

    if ((fp = fopen(log_name, "w")) != NULL) {
        fprintf(fp, "[%s] LOG WAS CREATED USING FOLLOWING CONFIGURATIONS:\n\t"
            "PORT: %d\n\t"
            "HOST: %s\n\t"
            "COMPRESSING: %s\n\t"
            "SAVING LOGS: %s\n\t"
            "LOG NAME: %s\n\n",
            time_str,
            PORT,
            HOST,
            COMPRESS ? "Yes" : "No",
            LOG_SAVE ? "Yes" : "No",
            LOG_NAME);
    }

    fclose(fp);

    return LOG_NAME;
}

void update_log(char* command, char* LOG_NAME, bool SEND_OR_RECEIVE) {
    time_t raw_time = time(NULL);
    char* time_str = ctime(&raw_time);
    
    time_str[strlen(time_str) - 1] = '\0';

    FILE* fp;
    
    if ((fp = fopen(LOG_NAME, "a")) != NULL) {
        if (!SEND_OR_RECEIVE) fprintf(fp, "[%s] SENT %ld BYTES:\n%s\n", time_str, strlen(command) *sizeof(char), command);
        else fprintf(fp, "[%s] RECEIVED %ld BYTES:\n%s\n", time_str, strlen(command) *sizeof(char), command);
    }

    fclose(fp);
}