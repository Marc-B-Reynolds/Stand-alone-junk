// This is a battery test for:
//   http://github.com/Marc-B-Reynolds/Stand-alone-junk/blob/master/src/SFH/lprns.h
//
// The testing will run until stopped, unless NUMBER_OF_RUNS is defined
//
// Building requires either of these libraries:
//   http://github.com/Marc-B-Reynolds/TestU01x
//   http://simul.iro.umontreal.ca/testu01/tu01.html


// if define test stream version, changing to a new stream
// each battery run.
//#define TEST_STREAM

// inject this note at the top of the output...for custom
// mixing or whatever other note desired.
#define NOTATION "default"

// if defined run Smallcrush, otherwise Crush
#define SMALLCRUSH

// Run with specified inital state. Undefined uses __rdtsc()
//#define INITAL_STATE 0x1L

// Setup whatever configuration to be tested


// if defined then the value is the number of time to run the
// battery...otherwise will continue until stopped.
//#define NUMBER_OF_RUNS 100

#define NAME "prns"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>


#if defined(_MSC_VER)
#define inline _inline
#define I2F (1.0/((1.0*(1<<22))*(1.0*(1<<30))))
#else
#include <x86intrin.h>
#define I2F 0x1p-52f
#endif

#include "util.h"
#include "unif01.h"
#include "swrite.h"

#include "../SFH/lprns.h"
#include <float.h>

// not really needed
#ifdef TEST_STREAM
lprns_stream_t state;
#else
lprns_t state;
#endif

static uint32_t next_u32(void* p, void* s)
{
#ifdef TEST_STREAM
  return lprns_stream_next(&state);
#else
  return lprns_next(&state);
#endif
}

// for double tests
static double next_f64(void* p, void* s)
{
#ifdef TEST_STREAM
  uint64_t r = lprns_stream_next(&state);

  r = (r<<32)|lprns_stream_next(&state);
#else
  uint64_t r = lprns_next(&state);

  r = (r << 32) | lprns_next(&state);
#endif

  return (r >> 12) * DBL_EPSILON;
}

// dump start of test state: useful if one wanted to
// check questionable p-values (or to repeat failures
// sanity checks)
static void print_state(void* s)
{
  printf("  S = 0x%08x\n", state.i);
}

// 
unif01_Gen* createGenerator()
{
  unif01_Gen* gen = util_Malloc(sizeof(unif01_Gen));

#ifdef TEST_STREAM
  state.m = LPRNS_STREAM_M;
#endif

  gen->state = 0;
  gen->param = 0;
  gen->name = NAME;
  gen->GetU01  = (void*)&next_f64;
  gen->GetBits = (void*)&next_u32;
  gen->Write = &print_state;

  return gen;
}

void deleteGenerator(unif01_Gen* gen)
{
  if (gen != NULL) {
    //util_Free(gen->param);
    //util_Free(gen->state);
    util_Free(gen);
  }
}


#include "bbattery.h"
#include "scomp.h"

int main(void)
{
  unif01_Gen* gen = createGenerator();
  uint32_t    s = (uint32_t)__rdtsc();
  uint32_t    c = -1;
  uint32_t    t;

  swrite_Basic = FALSE; // only print summary

  uint64_t z = s;

  // dump out the configuration
  printf("PRNS with configuation options ("  NOTATION  ") \n");
  printf("  WEYL: 0x%08x\n", LPRNS_WEYL);
  printf("  M0:   0x%08x\n", LPRNS_M0);
  printf("  M1:   0x%08x\n", LPRNS_M1);
#ifndef PRNS_NO_FINAL_XORSHIFT
  //printf("  S2:   %d\n", PRNS_MIX_S2);
#else
  //printf("  drops last right-xorshift\n");
#endif
  //printf("  M0:   0x%0" PRIx64 "\n", PRNS_MIX_M0);
  //printf("  M1:   0x%0" PRIx64 "\n", PRNS_MIX_M1);

#ifdef TEST_STREAM
  printf("  streaming variant: stream param changed each battery test\n");
#endif


  // the state is initialized to a sobol sequence in
  // an attempt to get as good a coverage of the state
  // space as possible.
  
  do {
    // setting the raw state of the generator
    state.i = s;
#ifdef TEST_STREAM
    state.m = lprns_stream_next_k(state.m); // change streams each battery run
    printf("run %d -- state = 0x%08x, mix = 0x%08x\n", -(int32_t)c, state.i, state.m);
#else
    printf("run %d -- state = 0x%08x\n", -(int32_t)c, state.i);
#endif

#if defined(SMALLCRUSH)
    bbattery_SmallCrush(gen);
#else
    bbattery_Crush(gen);
#endif

#if defined(NUMBER_OF_RUNS)
	if (0 - c >= NUMBER_OF_RUNS) break;
#endif

    // could improve the domain of the search
    t = lprns_ctz(c) * 2;
    s ^= 0x80000000 >> t;
    c -= 1;
  } while (1);

  deleteGenerator(gen);

  return 0;
}
