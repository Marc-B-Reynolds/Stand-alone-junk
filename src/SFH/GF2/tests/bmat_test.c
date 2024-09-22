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

// functions that need wrappers for testing
#define SET_PX(A,B,W)     {.f0=A##_##W,.f1=B##_##W}
#define FUNC_SET_X(N,A,B) {.name = N, .p={SET_PX(A,B,8), SET_PX(A,B,16), SET_PX(A,B,32), SET_PX(A,B,64)} }

// signatures
//   gen_func       void     f(uint64_t*, uint64_t)
//   u1func         void     f(uint64_t*)
//   unary_2param   void     f(uint64_t*, uint64_t*)
//   binary_3param  void     f(uint64_t*, uint64_t*, uint64_t*)
//   mvprod         uint64_t f(uint64_t, uint64_t*)


//******************************************************************************

// the pair of functions to test
typedef struct {
  void (*f0)(uint64_t*, uint64_t);
  void (*f1)(uint64_t*, uint64_t);
} gen_func_pair_t;

// a full set of pairs for all sizes
typedef struct {
  char* name;
  gen_func_pair_t p[4];
} gen_func_set_t;


static inline uint32_t test_gen(prng_t*        prng,
				test_fn_set_t* mset,
				void (*f0)(uint64_t*, uint64_t),
				void (*f1)(uint64_t*, uint64_t),
				uint32_t       n)
{
  bmat_def_64(r0);
  bmat_def_64(r1);
  
  mset->rsize();

  uint64_t mask = UINT64_C(~0) >> (64-(mset->n));
  
  for(uint32_t i=0; i<n; i++) {
    uint64_t v  = prng_u64(prng) & mask;

    f0(r0,v);
    f1(r1,v);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}

// "dumbest" version ever is perfect for our purposes
// (only used for 8 & 16 bit testing)
uint32_t my_first_carryless_product_32(uint32_t a, uint32_t b)
{
  uint32_t r = 0;

  // not going to check which has smaller popcount
  while (b != 0) {

    // not going to fast-forward past trailing zeros
    if (b & 1)
      r ^= a;

    a <<= 1;
    b >>= 1;
  }

  // because it just doesn't matter! (yes I'm attempting
  // to amuse myself with hacking-n-slashing testing code)
  return r;
}


#if 0
// humm..don't have to matrix from function for 8 & 16 ATM
void wcl_8 (uint64_t* m, uint64_t v) { bmat_set_cl_mul_8 (m,(uint8_t) v); }
void wcl_16(uint64_t* m, uint64_t v) { bmat_set_cl_mul_16(m,(uint16_t)v); }
void wcl_32(uint64_t* m, uint64_t v) { bmat_set_cl_mul_32(m,(uint32_t)v); }
void wcl_64(uint64_t* m, uint64_t v) { bmat_set_cl_mul_64(m,(uint64_t)v); }

void wcc_8 (uint64_t* m, uint64_t v) { bmat_set_cc_mul_8 (m,(uint8_t) v); }
void wcc_16(uint64_t* m, uint64_t v) { bmat_set_cc_mul_16(m,(uint16_t)v); }
void wcc_32(uint64_t* m, uint64_t v) { bmat_set_cc_mul_32(m,(uint32_t)v); }
void wcc_64(uint64_t* m, uint64_t v) { bmat_set_cc_mul_64(m,(uint64_t)v); }

void wcr_8 (uint64_t* m, uint64_t v) { bmat_set_cr_mul_8 (m,(uint8_t) v); }
void wcr_16(uint64_t* m, uint64_t v) { bmat_set_cr_mul_16(m,(uint16_t)v); }
void wcr_32(uint64_t* m, uint64_t v) { bmat_set_cr_mul_32(m,(uint32_t)v); }
void wcr_64(uint64_t* m, uint64_t v) { bmat_set_cr_mul_64(m,(uint64_t)v); }

#else

uint8_t  id_8 (uint8_t  x) { return x; }
uint16_t id_16(uint16_t x) { return x; }
uint32_t id_32(uint32_t x) { return x; }
uint64_t id_64(uint64_t x) { return x; }

// temp hack: too lazy to make unused macro
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
void wid_8 (uint64_t* m, uint64_t v) { bmat_set_unit_8 (m); }
void wid_16(uint64_t* m, uint64_t v) { bmat_set_unit_16(m); }
void wid_32(uint64_t* m, uint64_t v) { bmat_set_unit_32(m); }
void wid_64(uint64_t* m, uint64_t v) { bmat_set_unit_64(m); }

void fid_8 (uint64_t* m, uint64_t v) { bmat_from_ufunc_8 (m,id_8 ); }
void fid_16(uint64_t* m, uint64_t v) { bmat_from_ufunc_16(m,id_16); }
void fid_32(uint64_t* m, uint64_t v) { bmat_from_ufunc_32(m,id_32); }
void fid_64(uint64_t* m, uint64_t v) { bmat_from_ufunc_64(m,id_64); }

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if 1
gen_func_set_t gen_funcs[] = {
  FUNC_SET_X("set_unit",wid,fid),
};

uint32_t test_all_gen(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<(sizeof(gen_funcs)/sizeof(gen_funcs[0])); i++) {
    gen_func_set_t* set = gen_funcs+i;
    test_banner(set->name);

    for(uint32_t t=0; t<4; t++) {
      errors += test_gen(prng, fn_set[t], set->p[t].f0, set->p[t].f1, trials);
    }
  }

  return errors;
}
#endif
#endif

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
  bmat_def_64(m0);
  bmat_def_64(r0);
  bmat_def_64(r1);
  
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
  bmat_def_64(m0);
  bmat_def_64(m1);
  
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
  bmat_def_64(m0);
  bmat_def_64(m1);
  bmat_def_64(r0);
  bmat_def_64(r1);

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
// matrix/vector products

// the pair of functions to test
typedef struct {
  uint64_t (*f0)(uint64_t, uint64_t*);
  uint64_t (*f1)(uint64_t, uint64_t*);
} mvfunc_pair_t;

// a full set of pairs for all sizes
typedef struct {
  char* name;
  mvfunc_pair_t p[4];
} mvfunc_set_t;

// need wrappers
uint64_t mvr_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_8_ref (m,(uint8_t) v); }
uint64_t mvr_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_16_ref(m,(uint16_t)v); }
uint64_t mvr_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_32_ref(m,(uint32_t)v); }
uint64_t mvr_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_64_ref(m,(uint64_t)v); }
uint64_t mva_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_8 (m,(uint8_t) v); }
uint64_t mva_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_16(m,(uint16_t)v); }
uint64_t mva_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_32(m,(uint32_t)v); }
uint64_t mva_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_64(m,(uint64_t)v); }

uint64_t vmr_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_8_ref ((uint8_t) v,m); }
uint64_t vmr_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_16_ref((uint16_t)v,m); }
uint64_t vmr_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_32_ref((uint32_t)v,m); }
uint64_t vmr_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_64_ref((uint64_t)v,m); }
uint64_t vma_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_8 ((uint8_t) v,m); }
uint64_t vma_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_16((uint16_t)v,m); }
uint64_t vma_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_32((uint32_t)v,m); }
uint64_t vma_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_64((uint64_t)v,m); }


static inline uint32_t test_mvprod(prng_t*        prng,
				   test_fn_set_t* mset,
				   uint64_t (*f0)(uint64_t, uint64_t*),
				   uint64_t (*f1)(uint64_t, uint64_t*),
				   uint32_t       n)
{
  bmat_def_64(a);

  mset->rsize();

  uint64_t mask = UINT64_C(~0) >> (64-(mset->n));

  for(uint32_t i=0; i<n; i++) {
    mset->random(a,prng);

    uint64_t v  = prng_u64(prng) & mask;
    uint64_t r0 = f0(v,a);
    uint64_t r1 = f1(v,a);

    if (r0==r1) continue;
    
    return test_fail();
  }
  return test_pass();
}


// table of matrix/vector product funcs
mvfunc_set_t mvp_funcs[] = {
  FUNC_SET_X("bmat_mulv_n", mvr, mva),
  FUNC_SET_X("bmat_vmul_n", vmr, vma),
};

uint32_t test_all_mvprod(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<(sizeof(mvp_funcs)/sizeof(mvp_funcs[0])); i++) {
    mvfunc_set_t* set = mvp_funcs+i;
    test_banner(set->name);

    for(uint32_t t=0; t<4; t++) {
      errors += test_mvprod(prng, fn_set[t], set->p[t].f0, set->p[t].f1, trials);
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
  errors += test_all_mvprod(&prng, trials);
  errors += test_all_gen(&prng, trials);
  
  if (errors == 0)
    printf("all tests passed\n");
  else
    printf("oh no! : %u " FAIL "failure%c" ENDC "\n", errors, errors==1?' ':'s');
  
  return (int)errors;
}
