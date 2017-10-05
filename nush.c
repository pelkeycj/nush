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



void execute(cvector* cv);
void userLoop();
void scriptLoop(char* argv[]);
int backgroundProcess(cvector* cv);

void parseSemicolon(cvector* cv);
void parseBool(cvector* cv);
void parseRedirIn(cvector* cv);
void parseRedirOut(cvector* cv)


// is this a backgroundProcess?
int backgroundProcess(cvector* cv) {
  return strcmp(cv->items[cv->size - 1], "&") == 0;
}


// execute a command
void execute(cvector* cv) {
  int cpid;
  int bg = 0;

  // check if it should run in background
  if (backgroundProcess(cv)) {
    cvector_pop(cv);
    bg = 1;
  }


  if ((cpid = fork())) { // parent
    // Child may still be running until we wait.
    int status;

    if (!bg) {
      waitpid(cpid, &status, 0);
    }
    else {
      return;
    }

    //waitpid(cpid, &status, 0);
  }
  else { //child
    // check for cd
    char* cmd = cv->items[0];
    if (strcmp(cmd, "cd") == 0 && cv->size >= 2) {
      chdir(cv->items[1]);
      return;
    }

    // create arguments array and populate
    char* args[cv->size  + 1];

    for (int i = 0; i < cv->size; i++) {
      args[i] = cv->items[i];
    }
    args[cv->size] = 0; // null terminate

    execvp(cmd, args);
  }
}

// handle redirect output operator
void parseRedirOut(cvector* cv) {

}

// handle redirect input operator
void parseRedirIn(cvector* cv) {
  int stdin_dup = dup(0);
  int stdout_dup = dup(1);

  // < operator: find and open file, execute command arg
  if (contains(cv, "<")) {
    char* command;
    char* file;
    // find <
    for (int i = 0; i < cv->size; i++) {
      if(strcmp(cv->items[i], "<") == 0) {
        command = cv->items[i - 1];
        file = cv->items[i + 1];
        break;
      }
    }

    // open file in place of stdin
    close(0);
    open(file, O_RDONLY);

    cvector* sub = new_cvector();
    cvector_push(sub, command);
    //execute cmd
    parseRedirOut(sub);
    free_cvector(sub);
    dup2(stdin_dup, 0); // reopen stdin
  }
  else {
    parseRedirOut(cv);
  }
}

// handle boolean ops
// if contains && or || -> split
// && -> check if one is false, if so do not execute any,
// else execute all
// || -> check if one is true, if so execute all, else execute none
void parseBool(cvector* cv) {
  cvector* sub = new_cvector();

  // does it contain && bool?
  if (contains(cv, "&&") && !contains(cv, "false")) {
    for (int i = 0; i < cv->size; i++) {
      // if we hit "&&" execute first command, continue to read second
      if (strcmp(cv->items[i], "&&") == 0) {
        parseRedir(sub);
        reset(sub);
      }
      else {
        cvector_push(sub, cv->items[i]);
      }
    }
  }
  else if (contains(cv, "||")) {
    if (strcmp(cv->items[0], "true") == 0) {
      free_cvector(sub);
      return;
    }
    //execute first that is not false or true
    for (int i = 0; i < cv->size; i++) {
      // if we hit "&&" execute first command, continue to read second
      if (strcmp(cv->items[i], "||") == 0) {
        parseRedir(sub);
        reset(sub);
      }
      else {
        cvector_push(sub, cv->items[i]);
      }
    }
  }
  else if (!contains(cv, "&&") && !contains(cv, "||")) {
    parseRedir(cv);
  }
  if (sub->size > 0) {
    parseRedir(sub);
  }
  free_cvector(sub);
}


// read and copy a cvector until a semicolon is reached, execute on semicolon
// repeat
void parseSemicolon(cvector* cv) {
  cvector* sub = new_cvector();
  for (int i = 0; i < cv->size; i++) {
    if (strcmp(cv->items[i], ";") == 0) {
      // execute and reset
      parseBool(sub);
      reset(sub);
    }
    else {
      cvector_push(sub, cv->items[i]);
    }
  }
  if (sub->size > 0) {
    parseBool(sub);
  }
  free_cvector(sub);
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
    // exit?
    if (strcmp(cv->items[0], "exit") == 0) {
      free_cvector(cv);
      return;
    }

    // parse semicolons and execute
    parseSemicolon(cv);
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
    // exit?
    if (strcmp(cv->items[0], "exit") == 0) {
      free_cvector(cv);
      return;
    }

    // parse semicolons and execute
    parseSemicolon(cv);
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


//TODO redirect operator < : locate redir operator, open file right after it
        //                   assign file to to input descriptor, dup2 call?
//TODO redirect operator >
//TODO pipe | -> redirect output to tmp file, read file, rm file?
