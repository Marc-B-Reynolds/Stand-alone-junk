#include <inttypes.h>

#include <flint/nmod.h>
#include "SFH/intops/montgomery.h"

#define  PRNG_IMPLEMENTATION
#include "prng_small_global.h"

#if 0
#define TEST_DIVISORS     0x1ffff
#define TEST_PER_DIVISOR  0x1fff
#else
#define TEST_DIVISORS     0xffff
#define TEST_PER_DIVISOR  0xff
#endif

#define FM_HEX_U64 "%016" PRIx64
#define FM_HEX_U32 "%08" PRIx32

#define MM_HEX_U64 "16^^%016" PRIx64
#define MM_HEX_U32 "16^^%08" PRIx32


void mont_u32_print(const mont_u32_t k)
{
  printf("mont_u32("FM_HEX_U32" {i="FM_HEX_U32", r="FM_HEX_U32", r2="FM_HEX_U32"})\n", k.n,k.i,k.r,k.r2);
}

void mont_u64_print(const mont_u64_t k)
{
  printf("mont_u64("FM_HEX_U64" {i="FM_HEX_U64", r="FM_HEX_U64", r2="FM_HEX_U64"})\n", k.n,k.i,k.r,k.r2);
}


typedef struct {
  char* name;
  char* op;
  uint64_t (*ref)(uint64_t,const nmod_redc_ctx_t);
  uint32_t (*f)(uint32_t,const mont_u32_t);
  uint32_t s;
} ufunc_32_t;

typedef struct {
  char* name;
  char* op;
  uint64_t (*ref)(uint64_t,const nmod_redc_ctx_t);
  uint64_t (*f)(uint64_t,const mont_u64_t);
  uint32_t s;
} ufunc_64_t;

typedef struct {
  char* name;
  char* op;
  uint64_t (*ref)(uint64_t, uint64_t, const nmod_redc_ctx_t);
  uint32_t (*f)(uint32_t, uint32_t, const mont_u32_t);
  uint32_t s;
} bfunc_32_t;

typedef struct {
  char* name;
  char* op;
  uint64_t (*ref)(uint64_t, uint64_t, const nmod_redc_ctx_t);
  uint64_t (*f)(uint64_t, uint64_t, const mont_u64_t);
  uint32_t s;
} bfunc_64_t;

typedef struct {
  char* name;
  char* op1;
  char* op2;
  uint64_t (*ref)(uint64_t, uint64_t, uint64_t, const nmod_redc_ctx_t);
  uint32_t (*f)(uint32_t, uint32_t, uint32_t, const mont_u32_t);
  uint32_t s;
} tfunc_32_t;

typedef struct {
  char* name;
  char* op1;
  char* op2;
  uint64_t (*ref)(uint64_t,uint64_t,uint64_t,const nmod_redc_ctx_t);
  uint64_t (*f)(uint64_t,uint64_t,uint64_t,const mont_u64_t);
  uint32_t s;
} tfunc_64_t;


void test_ufunc_32(const ufunc_32_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  uint32_t dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint32_t   n = (prng_u32() >> e->s)|1; 
    mont_u32_t k = mont_init_u32(n);

    nmod_redc_ctx_init_ui(flint, n);
    
    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint32_t a  = prng_u32() % n;
      uint32_t ma = mont_form_u32(a,k);
      uint64_t fa = nmod_redc_set_ui(a,flint);
      
      uint64_t r0 = e->ref(fa,flint);
      uint32_t r1 = e->f(ma,k);

      r0 = nmod_redc_get_nmod(r0,flint);
      r1 = mont_to_u32(r1,k);
      
      cnt++;

      if ((uint32_t)r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }
      printf("  Mod[%s" MM_HEX_U32 ", "MM_HEX_U32 "] " FM_HEX_U32 " " FM_HEX_U32"\n",
             e->op,
             a,k.n,(uint32_t)r0,r1);
      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break;
    }
  }
  printf("  done\n");
}


void test_ufunc_64(const ufunc_64_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  int      dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint64_t   n = (prng_u64() >> e->s)|1; 
    mont_u64_t k = mont_init_u64(n);
    
    nmod_redc_ctx_init_ui(flint, n);

    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint64_t a  = prng_u64() % n;
      uint64_t r0 = nmod_redc_get_nmod(e->ref(nmod_redc_set_ui(a,flint),flint),flint);
      uint64_t r1 = mont_to_u64(e->f(mont_form_u64(a,k),k),k);

      cnt++;
      
      if (r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }

      // OPPS
      printf("  Mod[%s"
             "16^^"    FM_HEX_U64
             ", 16^^" FM_HEX_U64 "] "
             FM_HEX_U64 " " FM_HEX_U64 "\n",
             e->op,
             a,
             k.n,
             r0,r1);
      
      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break; // change divisor
    }
  }
  printf("  done\n");
}


void test_bfunc_32(const bfunc_32_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  int      dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint32_t   n = (prng_u32() >> e->s)|1; 
    mont_u32_t k = mont_init_u32(n);

    nmod_redc_ctx_init_ui(flint, n);
    
    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint32_t a  = prng_u32() % n;
      uint32_t b  = prng_u32() % n;

      uint64_t fa = nmod_redc_set_ui(a,flint);
      uint64_t fb = nmod_redc_set_ui(b,flint);
      uint32_t ma = mont_form_u32(a,k);
      uint32_t mb = mont_form_u32(b,k);

      uint64_t r0 = e->ref(fa,fb,flint);
      uint32_t r1 = e->f(ma,mb,k);

      r0 = nmod_redc_get_nmod(r0,flint);
      r1 = mont_to_u32(r1,k);

      cnt++;
      
      if ((uint32_t)r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }

      printf("  Mod[" MM_HEX_U32"%s"MM_HEX_U32", "MM_HEX_U32"] " FM_HEX_U32 " " FM_HEX_U32 "\n",
             a,
             e->op,b,k.n,(uint32_t)r0,r1);

      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break;
    }
  }
  printf("  done\n");
}


void test_bfunc_64(const bfunc_64_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  int      dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint64_t   n = (prng_u64() >> e->s)|1; 
    mont_u64_t k = mont_init_u64(n);

    nmod_redc_ctx_init_ui(flint, n);
    
    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint64_t a  = prng_u64() % n;
      uint64_t b  = prng_u64() % n;

      uint64_t fa = nmod_redc_set_ui(a,flint);
      uint64_t fb = nmod_redc_set_ui(b,flint);
      uint64_t ma = mont_form_u64(a,k);
      uint64_t mb = mont_form_u64(b,k);

      uint64_t r0 = e->ref(fa,fb,flint);
      uint64_t r1 = e->f(ma,mb,k);

      r0 = nmod_redc_get_nmod(r0,flint);
      r1 = mont_to_u64(r1,k);

      cnt++;
      
      if (r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }

      printf("  Mod[" MM_HEX_U64"%s"MM_HEX_U64", "MM_HEX_U64"] " FM_HEX_U64 " " FM_HEX_U64 "\n",
             a,
             e->op,b,k.n,r0,r1);

      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break;
    }
  }
  printf("  done\n");
}


void test_tfunc_32(const tfunc_32_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  int      dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint32_t   n = (prng_u32() >> e->s)|1; 
    mont_u32_t k = mont_init_u32(n);
    
    nmod_redc_ctx_init_ui(flint, n);

    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint32_t a  = prng_u32() % n;
      uint32_t b  = prng_u32() % n;
      uint32_t c  = prng_u32() % n;

      uint64_t fa = nmod_redc_set_ui(a,flint);
      uint64_t fb = nmod_redc_set_ui(b,flint);
      uint64_t fc = nmod_redc_set_ui(c,flint);
      uint32_t ma = mont_form_u32(a,k);
      uint32_t mb = mont_form_u32(b,k);
      uint32_t mc = mont_form_u32(c,k);
      
      uint64_t r0 = e->ref(fa,fb,fc,flint);
      uint32_t r1 = e->f(ma,mb,mc,k);

      r0 = nmod_redc_get_nmod(r0,flint);
      r1 = mont_to_u32(r1,k);

      cnt++;
      
      if ((uint32_t)r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }

      printf("  Mod["
             "16^^"   FM_HEX_U32
             "%s16^^" FM_HEX_U32
             "%s16^^" FM_HEX_U32
             ", 16^^" FM_HEX_U32
             "] "
             FM_HEX_U32 " " FM_HEX_U32"\n",
             a,e->op1,
             b,e->op2,c,
             k.n,
             (uint32_t)r0,r1);

      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break;
    }
  }
  printf("  done\n");
}

void test_tfunc_64(const tfunc_64_t* e)
{
  printf("%-10s ", e->name); fflush(stdout);

  uint32_t err = 0;
  uint32_t cnt = 0;
  int      dis = 1;

  nmod_redc_ctx_t flint;

  for(int j=0; j<TEST_DIVISORS; j++) {
    uint64_t   n = (prng_u64() >> e->s)|1; 
    mont_u64_t k = mont_init_u64(n);

    nmod_redc_ctx_init_ui(flint, n);

    for(int i=0; i<TEST_PER_DIVISOR; i++) {
      uint64_t a  = prng_u64() % n; a = 0;
      uint64_t b  = prng_u64() % n;
      uint64_t c  = prng_u64() % n;

      uint64_t fa = nmod_redc_set_ui(a,flint);
      uint64_t fb = nmod_redc_set_ui(b,flint);
      uint64_t fc = nmod_redc_set_ui(c,flint);
      uint64_t ma = mont_form_u64(a,k);
      uint64_t mb = mont_form_u64(b,k);
      uint64_t mc = mont_form_u64(c,k);

      uint64_t r0 = e->ref(fa,fb,fc,flint);
      uint64_t r1 = e->f(ma,mb,mc,k);

      r0 = nmod_redc_get_nmod(r0,flint);
      r1 = mont_to_u64(r1,k);

      cnt++;
      
      if (r0 == r1) continue;

      if (dis) { printf("\n"); dis = 0; }

      printf("  Mod["
             "16^^"   FM_HEX_U64
             "%s16^^" FM_HEX_U64
             "%s16^^" FM_HEX_U64
             ", 16^^" FM_HEX_U64
             "] "
             FM_HEX_U64 " " FM_HEX_U64"\n",
             a,e->op1,
             b,e->op2,c,
             k.n,
             r0,r1);

      if (++err > 5) {printf("  bail after %u trials\n", cnt); return; }
      break;
    }
  }

  printf("  done\n");
}


uint64_t wrap_mul_two(uint64_t a, const nmod_redc_ctx_t ctx)
{
  // fast version is no good for full 64-bit inputs..so just
  // use add.
  return nmod_redc_add(a,a,ctx);
}

uint64_t wrap_sq(uint64_t a, const nmod_redc_ctx_t ctx)
{
  return nmod_redc_mul(a,a,ctx);
}

// I'm assuming FLINT limbs are 64-bit

#define LENGTHOF(T) (sizeof(T)/sizeof((T)[0]))

const ufunc_32_t ufunc_32[] = {
  {.name="32 neg",  .s=0, .op="-",  .ref=nmod_redc_neg, .f=mont_neg_u32},
  {.name="32 mul2", .s=0, .op="2*", .ref=wrap_mul_two,  .f=mont_mul2_u32},
  {.name="32 sq",   .s=0, .op=" ",  .ref=wrap_sq,       .f=mont_sq_u32},
};

const ufunc_64_t ufunc_64[] = {
  {.name="64 neg",  .s=0, .op="-",  .ref=nmod_redc_neg,.f=mont_neg_u64},
  {.name="64 mul2", .s=0, .op="2*", .ref=wrap_mul_two, .f=mont_mul2_u64},
  {.name="64 sq",   .s=0, .op=" ",  .ref=wrap_sq,      .f=mont_sq_u64},
};

const bfunc_32_t bfunc_32[] = {
  {.name="32 add", .s=0, .op="+", .ref=nmod_redc_add, .f=mont_add_u32},
  {.name="32 sub", .s=0, .op="-", .ref=nmod_redc_sub, .f=mont_sub_u32},
  {.name="32 mul", .s=0, .op="*", .ref=nmod_redc_mul, .f=mont_mul_u32},
  {.name="31 mul", .s=1, .op="*", .ref=nmod_redc_mul, .f=mont_mul_u31},
};

const bfunc_64_t bfunc_64[] = {
  {.name="64 add", .s=0, .op="+", .ref=nmod_redc_add, .f=mont_add_u64},
  {.name="64 sub", .s=0, .op="-", .ref=nmod_redc_sub, .f=mont_sub_u64},
  {.name="64 mul", .s=0, .op="*", .ref=nmod_redc_mul, .f=mont_mul_u64},
  {.name="63 mul", .s=1, .op="*", .ref=nmod_redc_mul, .f=mont_mul_u63},
};


uint64_t wrap_fma(uint64_t a, uint64_t b, uint64_t c, const nmod_redc_ctx_t ctx)
{
  return nmod_redc_add(nmod_redc_mul(a,b,ctx),c,ctx);
}

uint64_t wrap_fms(uint64_t a, uint64_t b, uint64_t c, const nmod_redc_ctx_t ctx)
{
  return nmod_redc_sub(nmod_redc_mul(a,b,ctx),c,ctx);
}

const tfunc_32_t tfunc_32[] = {
  {.name="32 fma", .s=0, .op1="*",.op2="+", .ref=wrap_fma, .f=mont_fma_u32},
  {.name="32 fms", .s=0, .op1="*",.op2="-", .ref=wrap_fms, .f=mont_fms_u32},
};

const tfunc_64_t tfunc_64[] = {
  {.name="64 fma", .s=0, .op1="*",.op2="+", .ref=wrap_fma, .f=mont_fma_u64},
  {.name="64 fms", .s=0, .op1="*",.op2="-", .ref=wrap_fms, .f=mont_fms_u64},
};


int main(void)
{
  printf("testing: montgomery form\n");
  for(size_t i=0; i<LENGTHOF(ufunc_32); i++) test_ufunc_32(ufunc_32 + i);
  for(size_t i=0; i<LENGTHOF(ufunc_64); i++) test_ufunc_64(ufunc_64 + i);
  for(size_t i=0; i<LENGTHOF(bfunc_32); i++) test_bfunc_32(bfunc_32 + i);
  for(size_t i=0; i<LENGTHOF(bfunc_64); i++) test_bfunc_64(bfunc_64 + i);
  for(size_t i=0; i<LENGTHOF(tfunc_32); i++) test_tfunc_32(tfunc_32 + i);
  for(size_t i=0; i<LENGTHOF(tfunc_64); i++) test_tfunc_64(tfunc_64 + i);

  return 0;
}
