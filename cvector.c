#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvector.h"

// based on svec code from lecture

// Allocate a new cvector
cvector* new_cvector() {
  cvector* cv = malloc(sizeof(cvector));
  cv->size = 0;
  cv->capacity = 20;
  cv->items = malloc(cv->capacity * sizeof(char*));
  memset(cv->items, 0, cv->capacity * sizeof(char*)); //set all chars to 0
  return cv;
}

// free memory occupied by cvector
// each items is freed separately
// then the items pointer is freed
// then the cvector itself is freed
void free_cvector(cvector* cv) {
  for (int i = 0; i < cv->size; i++) {
    if (cv->items[i] != 0) {
      free(cv->items[i]);
    }
  }
  free(cv->items);
  free(cv);
}

// get the item at index i from cvector cv
char* cvector_get(cvector* cv, int i) {
  if (i > 0 && i < cv->size) {
    return cv->items[i];
  }
}

// put item at index i in cvector cv
void cvector_put(cvector* cv, int i, char* item) {
  if (i >= 0 && i <= cv->size) {
    cv->items[i] = strdup(item);
    cv->size++;
  }
}

// append item to cvector
void cvector_push(cvector* cv, char* item) {
  // at capacity -> increase and insert
  if (cv->size >= cv->capacity) {
    // double capacityand reallocate
    cv->capacity *= 2;
    cv->items = (char**) realloc(cv->items, cv->capacity * sizeof(char*));
  }

  //increment size and insert
  cvector_put(cv, cv->size, item);
}

// remove last item
char* cvector_pop(cvector* cv) {
  if (cv->size > 0) {
    free(cv->items[cv->size]--);
  }
}
