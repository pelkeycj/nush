#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "cvector.h"
#include "tokens.h"


#define LINE_BUFFER 256

// does cv contain 'exit' token?
int exitcmd(cvector* cv) {
  for (int i = 0; i < cv->size; i++) {
    if (strcmp("exit", cv->items[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

void execute(cvector* cv);
void userLoop();
void scriptLoop(char* argv[]);

/*
// is this a backgroundProcess?
int backgroundProcess(cvector* cv) {
  return strcmp(cv->items[cv->size - 1], "&") == 0;
}
*/

// execute a command
void execute(cvector* cv) {
  int cpid;

  if ((cpid = fork())) { // parent
    // Child may still be running until we wait.
    int status;

    char* cmd = cv->items[0];
    if (strcmp(cmd, "cd") == 0 && cv->size >= 2) {
      chdir(cv->items[1]);
      return;
    }

    // check if cd

    // create arguments array and populate
    char* args[cv->size  + 1];

    for (int i = 0; i < cv->size; i++) {
      args[i] = cv->items[i];
    }
    args[cv->size] = 0; // null terminate

    execvp(cmd, args);
  }
}

// loop function to read and process user input
void userLoop() {
  while (1) {
    char cmd[LINE_BUFFER];
    cvector* cv = new_cvector();

    // get input
    printf("nush$ ");
    fflush(stdout);
    char* x = fgets(cmd, LINE_BUFFER, stdin);
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
  FILE* file = fopen(argv[1], "r");
  if (!file) {
    printf("Could not open file: %s\n", argv[1]);
  }

  while (1) {
    char cmd[LINE_BUFFER];
    cvector* cv = new_cvector();

    char* x = fgets(cmd, LINE_BUFFER, file);
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
