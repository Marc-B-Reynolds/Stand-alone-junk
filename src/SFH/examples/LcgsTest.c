
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define LCGS_IMPLEMENTATION
#define LCGS_MLCG
#define LCGS_64

#include "../lcgs.h"

#define L_POW 4
#define LEN (1<<L_POW)

uint64_t history[LEN];


#if defined(LCGS_64)
#define LCGS_UINT uint64_t
#define LCGS_SINT int64_t
#else
#define LCGS_UINT uint32_t
#define LCGS_SINT int32_t
#endif

#if 1 // LAZY HERE
#include <x86intrin.h>
#endif

void spot_check(LCGS_UINT e, LCGS_UINT a, char* msg)
{
  if (e==a) return;

#if defined(LCGS_64)
  printf("error: 0x%0" PRIx64  " 0x%0" PRIx64 " %s\n", e, a, msg);
#else
  printf("error: 0x%08x 0x%08x %s\n", e, a, msg);
#endif
}

// minimal spot checks
int main(int argc, char** argv)
{
  lcgs_t rng;
  lcgs_t rng2;
  lcgs_t rng3;

  uint32_t i = 0;

#if defined(LCGS_64)
  uint64_t o = __rdtsc();

  printf("LCGS: init state: 0x%0" PRIx64 "\n", o);

#else
  uint64_t TSC = __rdtsc();
  uint32_t o   = (uint32_t)((TSC>>32)^TSC);

  printf("LCGS: init state: 0x%08x\n", o);
#endif

  //*** PRNG testing

  lcgs_state_set(&rng, (LCGS_UINT)1);
  lcgs_next(&rng);
  lcgs_prev(&rng);
  spot_check(rng.i, (LCGS_UINT)1, "SANITY CHECK FAILED");

  o=1L;
  
  lcgs_state_set(&rng, o);

  // history = LEN members starting at 'o'
  // test forward seeking at same time
  for(i=0; i<LEN; i++) {
    // set rng2's state to intial 'o' and seek forward by 'i'
    rng2.i = o;
    lcgs_seek(&rng2, i);
    spot_check(rng.i, rng2.i, "forward seek");
    history[i] = lcgs_next(&rng);
  }

  lcgs_prev(&rng); // back-up by one
  
  LCGS_UINT t = rng.i;
  
  lcgs_state_set(&rng2, lcgs_state_get(&rng)); // rng2.i = rng.i

  // spot check 'lcgs_prev' and backwards seeking
  for(i=LEN-1; i<LEN; i--) {
    rng3.i = t;
    lcgs_seek(&rng3, ((LCGS_SINT)i-LEN+1));
    spot_check(rng2.i, rng3.i, "backward seek");
    spot_check(history[i], lcgs_prev(&rng2), "prev");
  }

  lcgs_next(&rng2); // forward by one
  spot_check(rng2.i, o, "blah");

}


