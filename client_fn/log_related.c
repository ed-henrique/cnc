#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *create_log(char *log_new_name, int PORT, char *HOST, int COMPRESS) {
  int str_size = strlen(log_new_name) - 2;

  char log_name[str_size + 5];
  char *LOG_NAME = (char *)malloc(sizeof(char) * (str_size + 5));

  strncpy(log_name, &log_new_name[6], str_size);
  log_name[str_size - 4] = '.';
  log_name[str_size - 3] = 't';
  log_name[str_size - 2] = 'x';
  log_name[str_size - 1] = 't';
  log_name[str_size] = '\0';

  strcpy(LOG_NAME, log_name);

  time_t raw_time = time(NULL);
  char *time_str = ctime(&raw_time);

  time_str[strlen(time_str) - 1] = '\0';

  FILE *fp;

  if ((fp = fopen(log_name, "w")) != NULL) {
    fprintf(fp,
            "[%s] LOG WAS CREATED USING FOLLOWING CONFIGURATIONS:\n\t"
            "PORT: %d\n\t"
            "HOST: %s\n\t"
            "COMPRESSING: %s\n\t"
            "SAVING LOGS: %s\n\t"
            "LOG NAME: %s\n\n",
            time_str, PORT, HOST, COMPRESS ? "Yes" : "No", "Yes", LOG_NAME);

    fclose(fp);
  }

  return LOG_NAME;
}

void update_log(char *command, char *LOG_NAME, int SEND_OR_RECEIVE) {
  time_t raw_time = time(NULL);
  char *time_str = ctime(&raw_time);

  time_str[strlen(time_str) - 1] = '\0';

  FILE *fp;

  if ((fp = fopen(LOG_NAME, "a")) != NULL) {
    if (!SEND_OR_RECEIVE)
      fprintf(fp, "[%s] SENT %zu BYTES:\n%s\n", time_str,
              strlen(command) * sizeof(char), command);
    else
      fprintf(fp, "[%s] RECEIVED %zu BYTES:\n%s\n", time_str,
              strlen(command) * sizeof(char), command);

    fclose(fp);
  }
}

void update_log_compressed(char *command, char *compressed_command,
                           char *LOG_NAME, int SEND_OR_RECEIVE) {
  time_t raw_time = time(NULL);
  char *time_str = ctime(&raw_time);

  time_str[strlen(time_str) - 1] = '\0';

  FILE *fp;

  if ((fp = fopen(LOG_NAME, "a")) != NULL) {
    if (!SEND_OR_RECEIVE) {
      fprintf(fp, "[%s] SENT %zu BYTES:\n", time_str,
              strlen(command) * sizeof(char));
      fprintf(fp, "COMMAND: %s\n", command);
      fprintf(fp, "COMPRESSED COMMAND: %s\n", compressed_command);
    } else {
      fprintf(fp, "[%s] RECEIVED %zu BYTES:\n", time_str,
              strlen(command) * sizeof(char));
      fprintf(fp, "OUTPUT:\n%s\n", command);
      fprintf(fp, "COMPRESSED OUTPUT:\n%s\n", compressed_command);
    }

    fclose(fp);
  }
}