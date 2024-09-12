// Marc B. Reynolds, 2017-2024
// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <stdbool.h>

#include "../bmat_everything.h"
#include "bmat_test.h"


//******************************************************************************

// macros to populate structures of 'real' (for this configuration)
// vs. reference version.
//#define SET_N(N,W) SFH_CAT(bmat_,N,_,W)
#define SET_N(N,W) bmat_##N##_##W
#define SET_R(N,W) bmat_##N##_##W##_ref
#define SET_P(N,W) {.f0=SET_R(N,W),.f1=SET_N(N,W)}

#define FUNC_SET(N) {.name = #N, .p={SET_P(N,8), SET_P(N,16), SET_P(N,32), SET_P(N,64)} }



//******************************************************************************
// validate two unary functions are the same: f0(m) = f1(m)
//   where input and output differ: func(r,m)

// the pair of functions to test
typedef struct {
  void (*f0)(uint64_t*, uint64_t*);
  void (*f1)(uint64_t*, uint64_t*);
} ufunc_pair_t;

// a full set of pairs for all sizes
typedef struct {
  char* name;
  ufunc_pair_t p[4];
} ufunc_set_t;


static inline uint32_t test_unary_2param(prng_t*        prng,
					 test_fn_set_t* mset,
					 void (*f0)(uint64_t*, uint64_t*),
					 void (*f1)(uint64_t*, uint64_t*),
					 uint32_t       n)
{
  uint64_t m0[64],r0[64],r1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);

    f0(r0,m0);
    f1(r1,m0);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}

// table of 2 parameter unary functions
ufunc_set_t ufuncs[] = {
  FUNC_SET(transpose),
  FUNC_SET(flip_h),
  FUNC_SET(flip_v),
};

uint32_t test_all_unary_2param(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<(sizeof(ufuncs)/sizeof(ufuncs[0])); i++) {
    ufunc_set_t* set = ufuncs+i;
    test_banner(set->name);

    for(uint32_t t=0; t<4; t++) {
      errors += test_unary_2param(prng, fn_set[t], set->p[t].f0, set->p[t].f1, trials);
    }
  }

  return errors;
}

//******************************************************************************
// validate two unary functions are the same: f0(m) = f1(m)
//   where result is in-place: func(m)

// the pair of functions to test
typedef struct {
  void (*f0)(uint64_t*);
  void (*f1)(uint64_t*);
} u1func_pair_t;

// a full set of pairs for all sizes
typedef struct {
  char* name;
  u1func_pair_t p[4];
} u1func_set_t;

// table of 1 parameter unary functions
u1func_set_t u1funcs[] = {
  FUNC_SET(set_unit),
  FUNC_SET(set_exchange),
};

static inline uint32_t test_unary_1param(prng_t*        prng,
					 test_fn_set_t* mset,
					 void (*f0)(uint64_t*),
					 void (*f1)(uint64_t*),
					 uint32_t       n)
{
  uint64_t m0[64],m1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);
    mset->dup(m1,m0);

    f0(m0);
    f1(m1);

    if (mset->equal(m0,m1)) continue;
    
    return test_fail();
  }
  return test_pass();
}

uint32_t test_all_unary_1param(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<(sizeof(u1funcs)/sizeof(u1funcs[0])); i++) {
    u1func_set_t* set = u1funcs+i;
    test_banner(set->name);

    for(uint32_t t=0; t<4; t++) {
      errors += test_unary_1param(prng, fn_set[t], set->p[t].f0, set->p[t].f1, trials);
    }
  }

  return errors;
}

//******************************************************************************
// validate two binary functions are the same: r = f0(a,b) = f1(a,b)
//   call(r,a,b)

// the pair of functions to test
typedef struct {
  void (*f0)(uint64_t*, uint64_t*, uint64_t*);
  void (*f1)(uint64_t*, uint64_t*, uint64_t*);
} bfunc_pair_t;

// a full set of pairs for all sizes
typedef struct {
  char* name;
  bfunc_pair_t p[4];
} bfunc_set_t;


static inline uint32_t test_binary_3param(prng_t*        prng,
					  test_fn_set_t* mset,
					  void (*f0)(uint64_t*, uint64_t*, uint64_t*),
					  void (*f1)(uint64_t*, uint64_t*, uint64_t*),
					  uint32_t       n)
{
  uint64_t m0[64],m1[64];
  uint64_t r0[64],r1[64];

  mset->rsize();
  
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

// table of 3 parameter binary functions
bfunc_set_t bfuncs[] = {
  FUNC_SET(mul),
  FUNC_SET(mult),
};

uint32_t test_all_binary_3param(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<(sizeof(bfuncs)/sizeof(bfuncs[0])); i++) {
    bfunc_set_t* set = bfuncs+i;
    test_banner(set->name);

    for(uint32_t t=0; t<4; t++) {
      errors += test_binary_3param(prng, fn_set[t], set->p[t].f0, set->p[t].f1, trials);
    }
  }

  return errors;
}


//******************************************************************************

int main(void)
{
  prng_t prng;

  uint32_t trials = 0xffff; // temp hack
  
  prng.state[0] = __rdtsc();
  prng.state[1] = UINT64_C(0x3ba0d900b9aaf028);
  prng.state[2] = UINT64_C(0xab7837b9aa423d86);
  prng_u64(&prng);

  printf(HEADER "BMAT TESTING:\n" ENDC);

  uint32_t errors = 0;

  errors += test_all_unary_2param(&prng, trials);
  errors += test_all_unary_1param(&prng, trials);
  errors += test_all_binary_3param(&prng, trials);
  
  if (errors == 0)
    printf("all tests passed\n");
  else
    printf("oh no! : %u " FAIL "failure%c" ENDC "\n", errors, errors==1?' ':'s');
  
  return (int)errors;
}
