#ifndef __UTIL_H__
#define __UTIL_H__

#define FUNC_TYPE_ODD  1
#define FUNC_TYPE_EVEN 2

// bunch of wip temp hacks

typedef struct {
  float (*f)(float);
  char*    name;
  char*    notes;
  uint32_t flags;
  uint32_t type;
} func_entry_t;

typedef struct {
  uint32_t x0;
  uint32_t x1;
  char*    name;
} func_range_entry_t;

typedef struct {
  char*    name;
  func_range_entry_t* table;
  uint32_t            num;
} func_range_table_t;

#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))
#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S
#define ENTRY(X) { .f=&X, .name=STRINGIFY(X) }


#endif
