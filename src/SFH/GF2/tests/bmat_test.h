// Marc B. Reynolds, 2017-2024
// Public Domain under http://unlicense.org, see link for details.

// types and builders defs. copy & paste madness. formating is for eye scanning. sorry..not sorry.

#pragma once

#include <stdio.h>
#include "prng_small.h"

// massive temp hack
#define HEADER     "\033[95m"
#define OKBLUE     "\033[94m"
#define OKCYAN     "\033[96m"
#define OKGREEN    "\033[92m"
#define WARNING    "\033[93m"
#define FAIL       "\033[91m"
#define ENDC       "\033[0m"
#define BOLD       "\033[1m"
#define UNDERLINE  "\033[4m"

void test_banner(char* str)
{
  printf(BOLD " TESTING: %s" ENDC "\n", str);
}


// note: pass returns zero and fail one.
uint32_t test_fail(void) { printf(FAIL "FAIL!" ENDC "\n");     return 1; }
uint32_t test_pass(void) { printf(OKGREEN "passed" ENDC "\n"); return 0; }

uint32_t test_skipped(char* reason)
{
  printf(WARNING "skipped" ENDC ": %s\n", reason);
  return 0;
}

typedef void (test_fn_vv_t)(void);

void  test_header_8 (void) { printf("    8x8: "); fflush(stdout); }
void  test_header_16(void) { printf("  16x16: "); fflush(stdout); }
void  test_header_32(void) { printf("  32x32: "); fflush(stdout); }
void  test_header_64(void) { printf("  64x64: "); fflush(stdout); }


typedef bool (test_fn_b1p_t)(uint64_t*);
typedef bool (test_fn_b2p_t)(uint64_t*,uint64_t*);

typedef void (test_fn_v1p_t)(uint64_t*);
typedef void (test_fn_v2p_t)(uint64_t*,uint64_t*);
typedef void (test_fn_v3p_t)(uint64_t*,uint64_t*,uint64_t*);

typedef uint32_t (test_fn_d1p_t)(uint64_t*);

typedef void (test_fn_rsample_t)(uint64_t*, prng_t*);

// pair this down. what was I thinking?
typedef struct {
  uint32_t n;
  test_fn_vv_t* rsize;
  
  test_fn_v2p_t* dup;

  test_fn_v3p_t* add;
  test_fn_v2p_t* sum;
  
  test_fn_b2p_t* equal;
  test_fn_b1p_t* is_zero;

  test_fn_d1p_t* rank;

  test_fn_rsample_t* random;  
} test_fn_set_t;

#define TEST_FN_E(N,W) .N = bmat_##N##_##W

#define TEST_FN_SET(W)      \
{                           \
  .n     = W,         	    \
  .rsize = test_header_##W, \
  TEST_FN_E(dup,W),	    \
  TEST_FN_E(add,W),	    \
  TEST_FN_E(sum,W),	    \
  TEST_FN_E(equal,W),	    \
  TEST_FN_E(is_zero,W),     \
  TEST_FN_E(rank,W),        \
  TEST_FN_E(random,W),      \
}

       
test_fn_set_t test_fn_set_8  = TEST_FN_SET( 8);
test_fn_set_t test_fn_set_16 = TEST_FN_SET(16);
test_fn_set_t test_fn_set_32 = TEST_FN_SET(32);
test_fn_set_t test_fn_set_64 = TEST_FN_SET(64);

test_fn_set_t* fn_set[] =
{
  &test_fn_set_8, 
  &test_fn_set_16,
  &test_fn_set_32,
  &test_fn_set_64,
};


//******************************************************************************
// validate two functions are inverses: m = f1(f0(m)) = f0(f1(m))
static inline uint32_t test_rt_ufunc_n(prng_t*        prng,
				       test_fn_set_t* mset,
				       test_fn_v2p_t* f0,
				       test_fn_v2p_t* f1,
				       uint32_t       n)
{
  uint64_t m0[64],r0[64],r1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);

    f0(r0,m0);
    f1(r1,r0);
    if (mset->equal(m0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}




//******************************************************************************
// validate two functions are the same: f0(m0,m1) = f1(m0,m1)
static inline uint32_t test_eq_bfunc_n(prng_t*        prng,
				       test_fn_set_t* mset,
				       test_fn_v3p_t* f0,
				       test_fn_v3p_t* f1,
				       uint32_t       n)
{
  uint64_t m0[64],m1[64],r0[64],r1[64];
  
  mset->rsize();

  if (f0 == f1)
    return test_skipped("this configuration uses the reference");
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);
    mset->random(m1,prng);

    f0(r0,m0,m1);
    f1(r1,m0,m1);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}



