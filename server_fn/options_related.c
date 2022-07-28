#include <stdio.h>
#include <string.h>
#include "socket_related.h"

void test_if_options_passed_server(int argc, char* argv[], int* PORT) {
    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], "--port") != NULL) *PORT = change_port_server(argv[i]);
    }
}