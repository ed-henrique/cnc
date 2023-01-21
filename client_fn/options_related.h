// Test for options passed when executing client:
//
// --log=[LOG_NAME] and also activates log saving (LOG_SAVE = 1)
// --port=[PORT]
// --host=[HOST]
// --compress activates date compression (COMPRESS = 1)
void test_if_options_passed_client(int argc, char *argv[], char **LOG_NAME,
                                   int *PORT, char **HOST, int *COMPRESS,
                                   int *LOG_SAVE);