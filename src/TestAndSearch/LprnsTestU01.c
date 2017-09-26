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

// if defined uses top bits of the two uniform integers
// instead of all of the first and remaining for the second.
//#define SANE_DOUBLE_CONVERT

// inject this note at the top of the output...for custom
// mixing or whatever other note desired.
#define NOTATION "default"

// requires my hacked library
#define ADAPTIVE

#define LPRNS_ALT_STREAMS

// Run with specified inital state. Undefined uses __rdtsc()
#define INITAL_STATE 0x9a6e9aa1

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
lprns_stream_t state_backup;
#else
lprns_t state;
lprns_t state_backup;
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
#ifdef SANE_DOUBLE_CONVERT
#ifdef TEST_STREAM
  uint64_t r0 = lprns_stream_next(&state);
  uint64_t r1 = lprns_stream_next(&state);
#else
  uint64_t r0 = lprns_next(&state);
  uint64_t r1 = lprns_next(&state);
#endif
  r0 >>= 6;
  r1 >>= 6;

  return ((r0<<26)|r1) * DBL_EPSILON;
#else
#ifdef TEST_STREAM
  uint64_t r = lprns_stream_next(&state);

  r = (r<<32)|lprns_stream_next(&state);
#else
  uint64_t r = lprns_next(&state);

  r = (r << 32) | lprns_next(&state);
#endif


  return (r >> 12) * DBL_EPSILON;
#endif
}

// these are for the adapative smallcrush and don't really have
// any real work to do at the moment.

static void* get_state(void* p, void* s)
{
  return &state_backup;
}

static void set_state(void* p, void* s)
{
  state = state_backup;
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
#ifdef LPRNS_ALT_STREAMS
  state.m = 0;
#else
  state.m = LPRNS_STREAM_M;
#endif
#endif

  gen->state = 0;
  gen->param = 0;
  gen->name = NAME;
  gen->GetU01  = (void*)&next_f64;
  gen->GetBits = (void*)&next_u32;
  gen->Write = &print_state;

#ifdef ADAPTIVE
  gen->getCurrentState = (void*)&get_state;
  gen->setCurrentState = (void*)&set_state;
#endif

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
  printf("  S0:   %d\n",     LPRNS_S0);
  printf("  S1:   %d\n",     LPRNS_S1);
  printf("  S2:   %d\n",     LPRNS_S2);


#ifdef TEST_STREAM
  printf("  streaming variant: stream param changed each battery test\n");
#endif

#ifdef   INITAL_STATE
  state.i = INITAL_STATE;
#else
  state.i = s;
#endif

  // the state is initialized to a sobol sequence in
  // an attempt to get as good a coverage of the state
  // space as possible.
  
  do {
    // setting the raw state of the generator
    
#ifdef TEST_STREAM
    state.m = lprns_stream_next_k(state.m); // change streams each battery run
    printf("run %d -- state = 0x%08x, mix = 0x%08x\n", -(int32_t)c, state.i, state.m);
#else
    printf("run %d -- state = 0x%08x\n", -(int32_t)c, state.i);
#endif

#if !defined(ADAPTIVE)
    bbattery_SmallCrush(gen);
#else
    bbattery_SmallCrushAdaptive(gen);
#endif

#if defined(NUMBER_OF_RUNS)
	if (0 - c >= NUMBER_OF_RUNS) break;
#endif

    // could improve the domain of the search
    t = lprns_ctz(c) * 2;
    s ^= 0x80000000 >> t;
    c -= 1;

    state.i = s;
  } while (1);

  deleteGenerator(gen);

  return 0;
}
