// This is a battery test for:
//   http://github.com/Marc-B-Reynolds/Stand-alone-junk/blob/master/src/SFH/prns.h
//
// The docs and structure are here:
//   http://marc-b-reynolds.github.io/shf/2016/04/19/prns.html
//
// The testing will run until stopped, unless NUMBER_OF_RUNS is defined
//
// Building requires either of these libraries:
//   http://github.com/Marc-B-Reynolds/TestU01x
//   http://simul.iro.umontreal.ca/testu01/tu01.html

// inject this note at the top of the output...for custom
// mixing or whatever other note desired.
#define NOTATION "standard"

// if defined the lower 32-bit results are used for
// integer test, otherwise the upper.  See the docs
// on dropping final right-xorshift.
//#define USE_LOWER_BITS

// if defined run Smallcrush, otherwise Crush
//#define SMALLCRUSH

// Run with specified inital state. Undefined uses __rdtsc()
//#define INITAL_STATE 0x1L

// Setup whatever configuration to be tested

//#define PRNS_MIX_13

#define PRNS_NO_FINAL_XORSHIFT

// if defined then the value is the number of time to run the
// battery...otherwise will continue until stopped.
#define NUMBER_OF_RUNS 100


#ifdef  PRNS_WEYL
// just to prevent compile time errors. The battery only test standard
// member access (walking forward).  Backward is identical statistical
// properties.
#define PRNS_WEYL_I 0x1L
#endif

#define NAME "prns"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include "../SFH/prns.h"


#if defined(_MSC_VER)
#define inline _inline
#define I2F (1.0/((1.0*(1<<22))*(1.0*(1<<30))))
_inline uint32_t __builtin_ctz(uint32_t x)  { unsigned long r; _BitScanForward(&r, (unsigned long)x); return (uint32_t)r; }
_inline uint32_t __builtin_clzl(uint64_t x) { unsigned long r; _BitScanReverse64(&r, x); return (uint64_t)r; }
#else
#include <x86intrin.h>
#define I2F 0x1p-52f
#endif

#include "util.h"
#include "unif01.h"
#include "swrite.h"

prns_t state;

// for 32-bit integer tests
static uint32_t next_u32(void* p, void* s)
{
  uint64_t r = prns_next(&state);

#if defined(USE_LOWER_BITS)  
  return (uint32_t)r;
#else
  return (uint32_t)(r >> 32);
#endif
}

// for double tests
static double next_f64(void* p, void* s)
{
  uint64_t r = prns_next(&state);
  // map to [0,1)
  return (r >> 12) * I2F;
}

// dump start of test state: useful if one wanted to
// check questionable p-values (or to repeat failures
// sanity checks)
static void print_state(void* s)
{
  printf("  S = 0x%0" PRIx64 "\n", state.i);
}

// 
unif01_Gen* createGenerator()
{
  unif01_Gen* gen = util_Malloc(sizeof(unif01_Gen));

  gen->state = 0;
  gen->param = 0;
  gen->name = NAME;
  gen->GetU01 = (void*)&next_f64;
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

int main(void)
{
  unif01_Gen* gen = createGenerator();
  uint64_t    s = __rdtsc();
  uint32_t    c = -1;
  uint32_t    t;

  swrite_Basic = FALSE; // only print summary

  uint64_t z = s;

  // dump out the configuration
  printf("PRNS with configuation options ("  NOTATION  ") \n");
  printf("  WEYL: 0x%0" PRIx64 "\n", PRNS_WEYL);
  printf("  S0:   %d\n", PRNS_MIX_S0);
  printf("  S1:   %d\n", PRNS_MIX_S1);
#ifndef PRNS_NO_FINAL_XORSHIFT
  printf("  S2:   %d\n", PRNS_MIX_S2);
#else
  printf("  drops last right-xorshift\n");
#endif
  printf("  M0:   0x%0" PRIx64 "\n", PRNS_MIX_M0);
  printf("  M1:   0x%0" PRIx64 "\n", PRNS_MIX_M1);

#ifdef USE_LOWER_BITS
  printf("  low bits\n");
#else
  printf("  high bits\n");
#endif

  // the state is initialized to a sobol sequence in
  // an attempt to get as good a coverage of the state
  // space as possible.
  
  do {
    // setting the raw state of the generator
    state.i = s;

    printf("run %d -- state = 0x%0" PRIx64 "\n", -(int32_t)c, state.i);

#if defined(SMALLCRUSH)
    bbattery_SmallCrush(gen);
#else
    bbattery_Crush(gen);
#endif

#if defined(NUMBER_OF_RUNS)
	if (0 - c >= NUMBER_OF_RUNS) break;
#endif

    // could improve the domain of the search
    t = __builtin_ctz(c) * 2;
    s ^= 0x8000000000000000UL >> t;
    c -= 1;
  } while (1);

  deleteGenerator(gen);

  return 0;
}
