#ifndef CVECTOR_H
#define CVECTOR_H

typedef struct cvector {
  int size;
  int capacity;
  char** items;
} cvector;


// Allocate a new cvector
cvector* new_cvector();

// free memory occupied by cvector
void free_cvector(cvector* cv);

// get the item at index i from cvector cv
char* cvector_get(cvector* cv, int i);

// put item at index i in cvector cv
void cvector_put(cvector* cv, int i, char* item);

// append item to cvector
void cvector_push(cvector* cv, char* item);

// remove last item
void cvector_pop(cvector* cv);

// reset a cvector. free all contents and reinitialize
void reset(cvector* cv);

// does cv contain item?
// 1 if true
int contains(cvector* cv, char* item);

#endif
