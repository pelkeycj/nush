#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cvector.h"
#include "tokens.h"

void
execute(char* cmd)
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

    cvector* cv = new_cvector();
    tokenize(cv, cmd, strlen(cmd));
    cmd = cv->items[0];

    // create arguments array and init with command
    char* args[cv->size  + 2];
    args[0] = cmd;

    for (int i = 1; i < cv->size; i++) {
      args[i + 1] = cv->items[i];
    }
    args[cv->size + 1] = 0; // null terminate

    execvp(cmd, args);

    // free cvector
    free_cvector(cv);
  }
}

int
main(int argc, char* argv[])
{

  while (1) {
    char cmd[256];

    if (argc == 1) {
      printf("nush$ ");
      fflush(stdout);
      char* x = fgets(cmd, 256, stdin);
      if (!x) {
        return 0;
      }
    }
    else {
      memcpy(cmd, "echo", 5);
    }

    execute(cmd);


  }
  return 0;
}


//TODO: programs to execute, pass args
//TODO: commands such as pwd, echo -> run as programs w/ execute
//TODO: cd, exit
