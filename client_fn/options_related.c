#include "io_related.h"
#include "log_related.h"
#include "socket_related.h"
#include <stdio.h>
#include <string.h>

void test_if_options_passed_client(int argc, char *argv[], char **LOG_NAME,
                                   int *PORT, char **HOST, int *COMPRESS,
                                   int *LOG_SAVE) {
  for (int i = 1; i < argc; i++) {
    if (strstr(argv[i], "--help") != NULL)
      print_help(argv);
    else if (strstr(argv[i], "--log") != NULL) {
      *LOG_SAVE = 1;
      *LOG_NAME = create_log(argv[i], *PORT, *HOST, *COMPRESS);
    } else if (strstr(argv[i], "--compress") != NULL)
      *COMPRESS = 1;
    else if (strstr(argv[i], "--port") != NULL)
      *PORT = change_port_client(argv[i]);
    else if (strstr(argv[i], "--host") != NULL)
      *HOST = change_host(argv[i]);
  }
}