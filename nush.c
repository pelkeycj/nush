#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cvector.h"
#include "tokens.h"


// does cv contain 'exit' token?
int exitcmd(cvector* cv) {
  for (int i = 0; i < cv->size; i++) {
    if (strcmp("exit", cv->items[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

void execute(cvector* cv)
{
  int cpid;

  if ((cpid = fork())) {
    // parent process
    //printf("Parent pid: %d\n", getpid());
    //printf("Parent knows child pid: %d\n", cpid);

    // Child may still be running until we wait.

    int status;
    waitpid(cpid, &status, 0);

    //printf("== executed program complete ==\n");

    //printf("child returned with wait code %d\n", status);
    if (WIFEXITED(status)) {
      //printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
    }
  }
  else {
    // child process
    //printf("Child pid: %d\n", getpid());
    //printf("Child knows parent pid: %d\n", getppid());

/*
    for (int ii = 0; ii < strlen(cmd); ++ii) {
      if (cmd[ii] == ' ') {
        cmd[ii] = 0;
        break;
      }
    }
    */



    // The argv array for the child.
    // Terminated by a null pointer.
    //char* args[] = {cmd, "one", 0};

    printf("== executed program's output: ==\n");

    char* cmd = cv->items[0];

    // create arguments array and init with command
    char* args[cv->size  + 1];

    for (int i = 1; i < cv->size; i++) {
      args[i] = cv->items[i];
    }
    args[cv->size] = 0; // null terminate

    execvp(cmd, args);
  }
}

int main(int argc, char* argv[])
{

  while (1) {
    char cmd[256];
    cvector* cv = new_cvector();

    if (argc == 1) {
      printf("nush$ ");
      fflush(stdout);
      char* x = fgets(cmd, 256, stdin);
      if (!x) {
        break;
      }
      // tokenize input
      tokenize(cv, cmd, strlen(cmd));
      if (exitcmd(cv)) {
        break;
      }
    }
    else {
      // read file
      int file = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY);
    }


    execute(cv);
    free_cvector(cv);

  }
  return 0;
}


//TODO: cd, exit
