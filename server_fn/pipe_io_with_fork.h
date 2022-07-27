// Passes command through a pipe(), using fork() and execl() to execute it.
//
// Output is saved to output with a maximum size of BUFFER_SIZE.
void command_output_with_fork(char* command, char* output, int BUFFER_SIZE);