// Marc B. Reynolds, 2017-2024
// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <stdbool.h>

#include "../bmat_everything.h"
#include "bmat_test.h"

/***
  This file starts by validating that the bridge (data conversion) between bmat_ fixed sized
  dense row-major storage to M4RI's internal representation and back. Then the rest (ideally)
  should be checking as many mbat reference implementations as possible.  It's up to bmat_test.c
  to check optimized vs. reference.

  TODO: (very partial list)
    mzd_submatrix         : block_get variants
    mzd_concat&mzd_stack  : block_set variants


  drivers (names should be cleaned up, so should fun typedefs):
    unary functions:       r=f(a)
      mr_test_eq_ufunc_n:  call(r,a)
      mr_test_eq_iufunc_n: call(a)      : in-place (a is result)
    binary functions:      r=f(a,b)
      mr_test_eq_bfunc_n:  call(r,a,b)
                           call(a,b)    : in-place (a is result)
    number query:          r=f(a)       : integer result..like rank
      mr_test_eq_dfunc_n:  call(a)
			   
*/




//----------------------------------------------------
// build some stuff

typedef bool (mr_test_fn_b1p_t)(mzd_t*);
typedef bool (mr_test_fn_b2p_t)(mzd_t*,mzd_t*);

typedef void (mr_test_fn_v1p_t)(mzd_t*);
typedef void (mr_test_fn_v2p_t)(mzd_t*,mzd_t*);
typedef void (mr_test_fn_v3p_t)(mzd_t*,mzd_t*,mzd_t*);

typedef uint32_t (mr_test_fn_d1p_t)(mzd_t*);

typedef void (to_m4ri_t)(mzd_t*, uint64_t*);
typedef void (from_m4ri_t)(uint64_t*, mzd_t*);



				   
typedef struct {
  uint32_t n;
  
  // conversion routines for the size
  to_m4ri_t*   to_m4ri;
  from_m4ri_t* from_m4ri;

  void     (*set_vec)(mzd_t*, uint64_t);
  uint64_t (*get_vec)(mzd_t*);

  // preallocated scratch matrices 
  mzd_t* a;
  mzd_t* b;
  mzd_t* c;

  mzd_t* v;
  mzd_t* rv;
} m4ri_set_t;


void mr_set_v8 (mzd_t* v, uint64_t x) { m4ri_set_rv_8 (v,(uint8_t) x); }
void mr_set_v16(mzd_t* v, uint64_t x) { m4ri_set_rv_16(v,(uint16_t)x); }
void mr_set_v32(mzd_t* v, uint64_t x) { m4ri_set_rv_32(v,(uint32_t)x); }
void mr_set_v64(mzd_t* v, uint64_t x) { m4ri_set_rv_64(v,          x); }

uint64_t mr_get_v8 (mzd_t* v) { return (uint64_t)m4ri_get_rv_8 (v); }
uint64_t mr_get_v16(mzd_t* v) { return (uint64_t)m4ri_get_rv_16(v); }
uint64_t mr_get_v32(mzd_t* v) { return (uint64_t)m4ri_get_rv_32(v); }
uint64_t mr_get_v64(mzd_t* v) { return           m4ri_get_rv_64(v); }

m4ri_set_t m4ri_set_8  = {.n =  8, .to_m4ri=bmat_to_m4ri_8,  .from_m4ri=bmat_from_m4ri_8,  .set_vec=mr_set_v8,  .get_vec=mr_get_v8};
m4ri_set_t m4ri_set_16 = {.n = 16, .to_m4ri=bmat_to_m4ri_16, .from_m4ri=bmat_from_m4ri_16, .set_vec=mr_set_v16, .get_vec=mr_get_v16};
m4ri_set_t m4ri_set_32 = {.n = 32, .to_m4ri=bmat_to_m4ri_32, .from_m4ri=bmat_from_m4ri_32, .set_vec=mr_set_v32, .get_vec=mr_get_v32};
m4ri_set_t m4ri_set_64 = {.n = 64, .to_m4ri=bmat_to_m4ri_64, .from_m4ri=bmat_from_m4ri_64, .set_vec=mr_set_v64, .get_vec=mr_get_v64};

extern mzd_t* m4ri_alloc_n(uint32_t n);

void m4ri_set_init(m4ri_set_t* set, uint32_t n)
{
  set->a = m4ri_alloc_n(n);
  set->b = m4ri_alloc_n(n);
  set->c = m4ri_alloc_n(n);

  set->v  = mzd_init((rci_t)1,(rci_t)n);
  set->rv = mzd_init((rci_t)1,(rci_t)n);
}


//******************************************************************************
// validate two unary (in-place matrix->matrix) functions are the same: f0(m) = f1(m)

#define MR_EQ_IUTEST(W,BFUNC,MFUNC) mr_test_eq_iufunc_n(prng, &test_fn_set_##W, &m4ri_set_##W, BFUNC, MFUNC, n);

static inline uint32_t mr_test_eq_iufunc_n(prng_t*        prng,
					   test_fn_set_t* mset,
					   m4ri_set_t*    m4ri,
					   test_fn_v1p_t* f0,
					   mr_test_fn_v1p_t* f1,
					   uint32_t       n)
{
  uint64_t m0[64];
  uint64_t r0[64],r1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);
    mset->dup(r0,m0);           // in-place result so duplicate
    f0(r0);

    // move matrix to M4RI, perform op and move result back.
    m4ri->to_m4ri(m4ri->a, m0);
    f1(m4ri->a);
    m4ri->from_m4ri(r1, m4ri->a);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}

//******************************************************************************
// validate two unary (matrix->u32) functions are the same: f0(m) = f1(m)

#define MR_EQ_DTEST(W,BFUNC,MFUNC) mr_test_eq_dfunc_n(prng, &test_fn_set_##W, &m4ri_set_##W, BFUNC, MFUNC, n);

static inline uint32_t mr_test_eq_dfunc_n(prng_t*        prng,
					  test_fn_set_t* mset,
					  m4ri_set_t*    m4ri,
					  test_fn_d1p_t* f0,
					  mr_test_fn_d1p_t* f1,
					  uint32_t       n)
{
  uint64_t m0[64];
  uint32_t r0,r1;
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);
    r0 = f0(m0);

    // move matrix to M4RI, perform op and move result back.
    m4ri->to_m4ri(m4ri->a, m0);
    r1 = f1(m4ri->a);

    if (r0 == r1) continue;
    
    return test_fail();
  }
  return test_pass();
}


//******************************************************************************
// validate two unary (matrix->matrix) functions are the same: f0(m) = f1(m)

// slightly less noisy defs
#define MR_EQ_UTEST(W,BFUNC,MFUNC) mr_test_eq_ufunc_n(prng, &test_fn_set_##W, &m4ri_set_##W, BFUNC, MFUNC, n);

static inline uint32_t mr_test_eq_ufunc_n(prng_t*        prng,
					  test_fn_set_t* mset,
					  m4ri_set_t*    m4ri,
					  test_fn_v2p_t* f0,
					  mr_test_fn_v2p_t* f1,
					  uint32_t       n)
{
  uint64_t m0[64],r0[64],r1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(m0,prng);
    f0(r0,m0);

    // move matrix to M4RI, perform op and move result back.
    m4ri->to_m4ri(m4ri->a, m0);
    f1(m4ri->b,m4ri->a);
    m4ri->from_m4ri(r1, m4ri->b);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}


//******************************************************************************
// validate two functions are the same: f0(a,b) = f1(a,b)

// slightly less noisy defs
#define MR_EQ_BTEST(W,BFUNC,MFUNC) mr_test_eq_bfunc_n(prng, &test_fn_set_##W, &m4ri_set_##W, BFUNC, MFUNC, n);

static inline uint32_t mr_test_eq_bfunc_n(prng_t*        prng,
					  test_fn_set_t* mset,
					  m4ri_set_t*    m4ri,
					  test_fn_v3p_t* f0,
					  mr_test_fn_v3p_t* f1,
					  uint32_t       n)
{
  uint64_t a[64],b[64],r0[64],r1[64];
  
  mset->rsize();
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(a,prng);
    mset->random(b,prng);

    f0(r0,a,b);

    m4ri->to_m4ri(m4ri->a, a);
    m4ri->to_m4ri(m4ri->b, b);

    f1(m4ri->c,m4ri->a,m4ri->b);

    m4ri->from_m4ri(r1, m4ri->c);

    if (mset->equal(r0,r1)) continue;
    
    return test_fail();
  }
  return test_pass();
}



//----------------------------------------------------
// mini prng sanity test. not that it's good. just that
// it's returning stuff.

uint32_t prng_sanity(prng_t* prng)
{
  uint64_t a[64];
  uint64_t b[64];

  uint32_t zmats = 0;

  printf(" TESTING: PRNG sanity ");
  
  for(uint32_t i=0; i<100; i++) {
    bmat_random_64(a,prng);
    bmat_random_64(b,prng);

    if (bmat_is_zero_64(a) || bmat_is_zero_64(b)) {
      if (zmats==0)
	printf("zero matrix! PRNG fail?\n");
      else {
	printf("  zero matrix again: PRNG problem ");
	return test_fail();
      }
      zmats++;
    }
    
    if (bmat_equal_64(a,b)) {
      printf("two identical matrices created (and in a row). PRNG ");
      return test_fail();
    }
  }
  
  return test_pass();
}

//----------------------------------------------------
// round trip testing


static const uint32_t rt_trials = 1024;

static inline uint32_t roundtrip_n(test_fn_set_t* mset, m4ri_set_t* rset, prng_t* prng)
{
  // big enough for all
  uint64_t m[64];
  uint64_t r[64];

  mset->rsize();

  // matrix back-n-forth
  for(uint32_t i=0; i<rt_trials; i++) {
    mset->random(m,prng);
    rset->to_m4ri  (rset->a, m);
    rset->from_m4ri(r, rset->a);
    
    if (mset->equal(m,r)) continue;
    
    return test_fail();
  }

  // row vector back-n-forth
  uint64_t mask = UINT64_C(~0) >> (64-(rset->n));

  for(uint32_t i=0; i<rt_trials; i++) {
    uint64_t v = prng_u64(prng) & mask;
    uint64_t r;

    rset->set_vec(rset->v, v);
    r = rset->get_vec(rset->v);

    if (v == r) continue;

    return test_fail();
  }

  return test_pass();
}

uint32_t roundtrip_8 (prng_t* prng) { return roundtrip_n(&test_fn_set_8,  &m4ri_set_8,  prng); }
uint32_t roundtrip_16(prng_t* prng) { return roundtrip_n(&test_fn_set_16, &m4ri_set_16, prng); }
uint32_t roundtrip_32(prng_t* prng) { return roundtrip_n(&test_fn_set_32, &m4ri_set_32, prng); }
uint32_t roundtrip_64(prng_t* prng) { return roundtrip_n(&test_fn_set_64, &m4ri_set_64, prng); }

uint32_t roundtrip(prng_t* prng)
{
  uint32_t errors = 0;

  test_banner("round trips");
  errors += roundtrip_8 (prng);
  errors += roundtrip_16(prng);
  errors += roundtrip_32(prng);
  errors += roundtrip_64(prng);

  if (errors != 0)
    printf("bailing testing. roundtrips are required to proceed\n");
  
  return errors;
}

//----------------------------------------------------
// set identity testing

void mr_unit(mzd_t* d) { mzd_set_ui(d,1); }
uint32_t t_unit_8 (prng_t* prng, uint32_t n) { return MR_EQ_IUTEST(8,  bmat_set_unit_8_ref,  mr_unit); }
uint32_t t_unit_16(prng_t* prng, uint32_t n) { return MR_EQ_IUTEST(16, bmat_set_unit_16_ref, mr_unit); }
uint32_t t_unit_32(prng_t* prng, uint32_t n) { return MR_EQ_IUTEST(32, bmat_set_unit_32_ref, mr_unit); }
uint32_t t_unit_64(prng_t* prng, uint32_t n) { return MR_EQ_IUTEST(64, bmat_set_unit_64_ref, mr_unit); }

uint32_t test_set_unit(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_set_unit_n_ref");
  errors += t_unit_8 (prng,n);
  errors += t_unit_16(prng,n);
  errors += t_unit_32(prng,n);
  errors += t_unit_64(prng,n);

  return errors;
}


//----------------------------------------------------
// transpose testing

void mr_tpose(mzd_t* d, mzd_t* s) { mzd_transpose(d,s); }


uint32_t t_tpose_8 (prng_t* prng, uint32_t n) { return MR_EQ_UTEST(8,  bmat_transpose_8_ref,  mr_tpose); }
uint32_t t_tpose_16(prng_t* prng, uint32_t n) { return MR_EQ_UTEST(16, bmat_transpose_16_ref, mr_tpose); }
uint32_t t_tpose_32(prng_t* prng, uint32_t n) { return MR_EQ_UTEST(32, bmat_transpose_32_ref, mr_tpose); }
uint32_t t_tpose_64(prng_t* prng, uint32_t n) { return MR_EQ_UTEST(64, bmat_transpose_64_ref, mr_tpose); }

uint32_t test_transpose(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_transpose_n_ref");
  errors += t_tpose_8 (prng,n);
  errors += t_tpose_16(prng,n);
  errors += t_tpose_32(prng,n);
  errors += t_tpose_64(prng,n);

  return errors;
}

//----------------------------------------------------
// rank

uint32_t mr_rank(mzd_t* m) { return (uint32_t)mzd_echelonize(m,0); }

uint32_t t_rank_8 (prng_t* prng, uint32_t n) { return MR_EQ_DTEST(8,  bmat_rank_8_ref,  mr_rank); }
uint32_t t_rank_16(prng_t* prng, uint32_t n) { return MR_EQ_DTEST(16, bmat_rank_16_ref, mr_rank); }
uint32_t t_rank_32(prng_t* prng, uint32_t n) { return MR_EQ_DTEST(32, bmat_rank_32_ref, mr_rank); }
uint32_t t_rank_64(prng_t* prng, uint32_t n) { return MR_EQ_DTEST(64, bmat_rank_64_ref, mr_rank); }

uint32_t test_rank(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_rank_n_ref");
  errors += t_rank_8 (prng,n);
  errors += t_rank_16(prng,n);
  errors += t_rank_32(prng,n);
  errors += t_rank_64(prng,n);

  return errors;
}

//----------------------------------------------------
// 

void mr_mm_mul(mzd_t* c, mzd_t* a, mzd_t* b) { mzd_mul_naive(c,a,b); }

uint32_t t_mm_mul_8 (prng_t* prng, uint32_t n) { return MR_EQ_BTEST(8,  bmat_mul_8_ref,  mr_mm_mul); }
uint32_t t_mm_mul_16(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(16, bmat_mul_16_ref, mr_mm_mul); }
uint32_t t_mm_mul_32(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(32, bmat_mul_32_ref, mr_mm_mul); }
uint32_t t_mm_mul_64(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(64, bmat_mul_64_ref, mr_mm_mul); }

uint32_t test_mm_mul(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_mul_n_ref");
  errors += t_mm_mul_8 (prng,n);
  errors += t_mm_mul_16(prng,n);
  errors += t_mm_mul_32(prng,n);
  errors += t_mm_mul_64(prng,n);
  
  return errors;
}

//----------------------------------------------------
// 

#if 1
void mr_mm_mult(mzd_t* c, mzd_t* a, mzd_t* b) { _mzd_mul_naive(c,a,b,1); }
#else
void mr_mm_mult(mzd_t* c, mzd_t* a, mzd_t* b)
{
  mzd_t* t = mzd_transpose(0,b);
  mzd_mul_naive(c,a,t);
  mzd_free(t);
}
#endif

uint32_t t_mm_mult_8 (prng_t* prng, uint32_t n) { return MR_EQ_BTEST(8,  bmat_mult_8_ref,  mr_mm_mult); }
uint32_t t_mm_mult_16(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(16, bmat_mult_16_ref, mr_mm_mult); }
uint32_t t_mm_mult_32(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(32, bmat_mult_32_ref, mr_mm_mult); }
uint32_t t_mm_mult_64(prng_t* prng, uint32_t n) { return MR_EQ_BTEST(64, bmat_mult_64_ref, mr_mm_mult); }

uint32_t test_mm_mult(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_mult_n_ref");
  errors += t_mm_mult_8 (prng,n);
  errors += t_mm_mult_16(prng,n);
  errors += t_mm_mult_32(prng,n);
  errors += t_mm_mult_64(prng,n);
  
  return errors;
}

//----------------------------------------------------
// c = vM

#define MR_VM_TEST(W,BFUNC,MFUNC) vmmul_driver(prng, &test_fn_set_##W, &m4ri_set_##W, BFUNC, MFUNC, n);

static inline uint32_t vmmul_driver(prng_t*        prng,
				    test_fn_set_t* mset,
				    m4ri_set_t*    m4ri,
				    uint64_t (*f1)(uint64_t, uint64_t*),
				    uint64_t (*f0)(uint64_t, uint64_t*),
				    uint32_t       n)
{
  uint64_t a[64];
  
  mset->rsize();

  uint64_t mask = UINT64_C(~0) >> (64-(m4ri->n));
  
  for(uint32_t i=0; i<n; i++) {
    mset->random(a,prng);

    //mset->set_unit(a);

    uint64_t v  = prng_u64(prng) & mask;
    uint64_t r0 = f0(v,a);
    uint64_t r1 = f1(v,a);

    if (r0==r1) continue;

    printf("%016lx %016lx\n", r0,r1);

    return test_fail();
  }
  return test_pass();
}

uint64_t bmvm_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_8_ref ((uint8_t) v, m); }
uint64_t bmvm_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_16_ref((uint16_t)v, m); }
uint64_t bmvm_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_32_ref((uint32_t)v, m); }
uint64_t bmvm_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_vmul_64_ref((uint64_t)v, m); }

uint64_t mmvm_8 (uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_vm_8 ((uint32_t)v, m); }
uint64_t mmvm_16(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_vm_16((uint32_t)v, m); }
uint64_t mmvm_32(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_vm_32((uint32_t)v, m); }
uint64_t mmvm_64(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_vm_64((uint64_t)v, m); }

uint32_t t_vm_mul_8 (prng_t* prng, uint32_t n) { return MR_VM_TEST(8,  bmvm_8,  mmvm_8);  }
uint32_t t_vm_mul_16(prng_t* prng, uint32_t n) { return MR_VM_TEST(16, bmvm_16, mmvm_16); }
uint32_t t_vm_mul_32(prng_t* prng, uint32_t n) { return MR_VM_TEST(32, bmvm_32, mmvm_32); }
uint32_t t_vm_mul_64(prng_t* prng, uint32_t n) { return MR_VM_TEST(64, bmvm_64, mmvm_64); }

uint32_t test_vm_mul(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_vmul_n_ref");
  errors += t_vm_mul_8 (prng,n);
  errors += t_vm_mul_16(prng,n);
  errors += t_vm_mul_32(prng,n);
  errors += t_vm_mul_64(prng,n);
  
  return errors;
}

#if 1

uint64_t bmmv_8 (uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_8_ref (m,(uint8_t) v); }
uint64_t bmmv_16(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_16_ref(m,(uint16_t)v); }
uint64_t bmmv_32(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_32_ref(m,(uint32_t)v); }
uint64_t bmmv_64(uint64_t v, uint64_t* m) { return (uint64_t)bmat_mulv_64_ref(m,(uint64_t)v); }
uint64_t mmmv_8 (uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_mv_8 (m,(uint32_t)v); }
uint64_t mmmv_16(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_mv_16(m,(uint32_t)v); }
uint64_t mmmv_32(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_mv_32(m,(uint32_t)v); }
uint64_t mmmv_64(uint64_t v, uint64_t* m) { return (uint64_t)m4ri_wrap_mv_64(m,(uint64_t)v); }

uint32_t t_mv_mul_8 (prng_t* prng, uint32_t n) { return MR_VM_TEST(8,  bmmv_8,  mmmv_8);  }
uint32_t t_mv_mul_16(prng_t* prng, uint32_t n) { return MR_VM_TEST(16, bmmv_16, mmmv_16); }
uint32_t t_mv_mul_32(prng_t* prng, uint32_t n) { return MR_VM_TEST(32, bmmv_32, mmmv_32); }
uint32_t t_mv_mul_64(prng_t* prng, uint32_t n) { return MR_VM_TEST(64, bmmv_64, mmmv_64); }

uint32_t test_mv_mul(prng_t* prng, uint32_t n)
{
  uint32_t errors = 0;

  test_banner("bmat_mulv_n_ref");
  errors += t_mv_mul_8 (prng,n);
  errors += t_mv_mul_16(prng,n);
  errors += t_mv_mul_32(prng,n);
  errors += t_mv_mul_64(prng,n);
  
  return errors;
}
#endif

//----------------------------------------------------
// 

int main(void)
{
  prng_t prng;

  uint32_t trials = 0xffff; // temp hack: add minimal command line processing

  trials = 255;
  
  prng.state[0] = 0x1234567;
  prng.state[1] = 0x89abcd1;
  prng.state[2] = __rdtsc();
  prng_u64(&prng);

  m4ri_set_init(&m4ri_set_8,  8);
  m4ri_set_init(&m4ri_set_16,16);
  m4ri_set_init(&m4ri_set_32,32);
  m4ri_set_init(&m4ri_set_64,64);

  printf("M4RI TESTING:\n");

  uint32_t errors = prng_sanity(&prng);
    
  if (errors == 0) {
    // start by testing roundtrips between bmat & M4RI. Additonally
    // ensures we doesn't see any zero matrices. If these fail we
    // can't really do any more.
    errors = roundtrip(&prng);

    if (errors == 0) {
      errors += test_set_unit(&prng, trials);
      errors += test_transpose(&prng, trials);
      errors += test_mm_mul(&prng, trials);
      errors += test_mm_mult(&prng, trials);
      errors += test_rank(&prng, trials);
      errors += test_vm_mul(&prng, trials);
      errors += test_mv_mul(&prng, trials);
    }
  }

  if (errors == 0)
    printf("all tests passed\n");
  else
    printf("oh no! : %u " FAIL "failure%c" ENDC "\n", errors, errors==1?' ':'s');
  
  return (int)errors;
}
