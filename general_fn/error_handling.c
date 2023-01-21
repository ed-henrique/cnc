#include <stdio.h>
#include <stdlib.h>

void error_output(char *error_message) {
  fprintf(stderr, "\033[31m[+]%s.\n", error_message);
  exit(1);
}