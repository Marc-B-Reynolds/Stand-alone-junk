#ifndef __UTIL_H__
#define __UTIL_H__

typedef struct {
  float (*f)(float);
  char*   name;
} func_entry_t;

#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))
#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S
#define ENTRY(X) { .f=&X, .name=STRINGIFY(X) }


#endif
