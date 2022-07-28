// Passes command through popen() to execute it.
//
// Output is saved to output with a maximum size of BUFFER_SIZE.
void command_output(char* command, char* output, int BUFFER_SIZE);
