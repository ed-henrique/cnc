#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int main() {
  int link[2];
  pid_t pid;
  char foo[4096];

  if (pipe(link)==-1)
    die("pipe");

  if ((pid = fork()) == -1)
    die("fork");

  if(pid == 0) {

    dup2 (link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    char* command = "df";
    execl("/bin/bash", "sh", "-c", command,(char *)0);
    die("execl");

  } else {

    close(link[1]);
    int nbytes = read(link[0], foo, sizeof(foo));
    printf("Output: (%.*s)\n", nbytes, foo);
    wait(NULL);

  }
  return 0;
}
