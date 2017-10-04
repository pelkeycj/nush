#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

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

// execute a command
void execute(cvector* cv)
{
  int cpid;

  if ((cpid = fork())) {
    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);
  }
  else {
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

// loop function to read and process user input
void userLoop() {
  while (1) {
    char cmd[256];
    cvector* cv = new_cvector();

    // get input
    printf("nush$ ");
    fflush(stdout);
    char* x = fgets(cmd, 256, stdin);
    if (!x) {
      free_cvector(cv);
      return;
    }

    // tokenize input
    tokenize(cv, cmd, strlen(cmd));
    if (exitcmd(cv)) {
      free_cvector(cv);
      return;
    }

    // execute
    execute(cv);
    free_cvector(cv);
  }

}

// loop function to read and process script input
void scriptLoop(char* argv[]) {
  // open file
  int file = open(argv[1], O_WRONLY);

//TODO loop on input?

  while (1) {
    char cmd[256];
    cvector* cv = new_cvector();

    //TODO read file
    /*
    if (!x) {
      free_cvector(cv);
      return;
    }
    */
    // tokenize input
    tokenize(cv, cmd, strlen(cmd));
    if (exitcmd(cv)) {
      free_cvector(cv);
      return;
    }

    // execute
    execute(cv);
    free_cvector(cv);
  }
}

int main(int argc, char* argv[])
{
  if (argc == 1) {
      userLoop();
    }
  else {
    scriptLoop(argv);
  }

  return 0;
}
