
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "cvector.h"


int isSingleOperator(char c) {
  return (c == '<' || c == '>' || c == ';');
}

// read chars from src to dst (end is NONINCLUSIVE)
char* copyChars(char* src, char* dest, int start, int end) {
  int i = 0;

  while(start < end) {
    dest[i] = src[start];
    start++;
    i++;
  }

  return dest;
}

// tokenize the input
void tokenize(cvector* cv, char* input, int maxLength) {
  int tokStart;
  int isReading = 0; // false
  int readPipeOrAmp = 0;

  int i = 0;
  char currentChar;
  while (i < strlen(input)) {
      currentChar = input[i];

      // space
      if (isspace(currentChar) || iscntrl(currentChar) || !isprint(currentChar)) {
        if (isReading) {
          // token ends
          char* token = malloc((i - tokStart + 10) * sizeof(char));
          memset(token, 0, (i - tokStart + 10) * sizeof(char));
          token = copyChars(input, token, tokStart, i);
          cvector_push(cv, token);
          free(token);
          isReading = 0;
          readPipeOrAmp = 0;
        }
        tokStart = i + 1;
      }

      // single operator (<, >, ;)
      else if (isSingleOperator(currentChar)) {
        // save token being read
        if (isReading) {
          char* token = malloc((i - tokStart + 10) * sizeof(char));
          memset(token, 0, (i - tokStart + 10) * sizeof(char));

          token = copyChars(input, token, tokStart, i);
          cvector_push(cv, token);
          free(token);
        }
        // save current char
        char* token = malloc(10 * sizeof(char));
        memset(token, 0, 10 * sizeof(char));
        token = copyChars(input, token, i, i + 1);
        cvector_push(cv, token);
        free(token);
        isReading = 0;
        readPipeOrAmp = 0;
        tokStart = i + 1; // ?
      }

      // possible double operator
      else if (currentChar == '|' || currentChar == '&') {
        if (isReading && input[i - 1] == currentChar) {
          // add double operator to tokens
          char* token = malloc(10 * sizeof(char));
          memset(token, 0, 10 * sizeof(char));

          token = copyChars(input, token, i - 1, i + 1);
          isReading = 0;
          readPipeOrAmp = 0;
          tokStart = i + 1;
          cvector_push(cv, token);
          free(token);
        }
        else if (isReading) {
          // save token previously seen
          char* token = malloc((i - tokStart + 10) * sizeof(char));
          memset(token, 0, (i - tokStart + 10) * sizeof(char));

          token = copyChars(input, token, tokStart, i);
          tokStart = i;
          cvector_push(cv, token);
          free(token);
          isReading = 1;
          readPipeOrAmp = 1;
        }
        else {
          isReading = 1;
          readPipeOrAmp = 1;
          tokStart = i;
        }
      }
      // regular char
      else if (isalnum(currentChar) && isprint(currentChar)
      || ispunct(currentChar) && isprint(currentChar)) {
        if (!isReading) {
          isReading = 1;
          tokStart = i;
        }
        if (readPipeOrAmp) {
          char* token = malloc((i - tokStart + 10) * sizeof(char));
          memset(token, 0, (i - tokStart + 10) * sizeof(char));

          token = copyChars(input, token, tokStart, i);
          tokStart = i;
          cvector_push(cv, token);
          free(token);
          isReading = 1;
          readPipeOrAmp = 0;
        }
      }
      i++;
  }
}

/*
int main(int _argc, char* _argv[]) {
  int inputSizeMax = 200;
  char inputLine[inputSizeMax];

  while (1) {
    cvector* cv = new_cvector();
    //input prompt
    printf("tokens$ ");
    // read input line
    char* input = fgets(inputLine, inputSizeMax, stdin);
    if (!input) {
      return 0;
    }

    // tokenize input
    tokenize(cv, input, inputSizeMax);


    // output
    for (int i = 0; i < cv->size; i++) {
      printf("%s\n", cv->items[i]);
    }


    // Go home cvector, you're free now.
    free_cvector(cv);

  }
    return 0;
}

*/
