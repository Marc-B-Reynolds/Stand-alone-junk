// Marc B. Reynolds, 2017-2022
// Public Domain under http://unlicense.org, see link for details.

// put link here

// NOTES:
// * no real effort at portability

#ifndef __CARRYLESS_H__
#define __CARRYLESS_H__

#ifndef __BITOPS_H__
#include "bitops.h"
#endif


//-----------------------------------------------------------
// helper for building scalar functions:

typedef __m128i cl_128_t;

// move two 64-bit input to SIMD, perform
// the carryless product & SIMD (128-bit) result

static inline cl_128_t cl_mul_base(cl_128_t a, cl_128_t b)
{
  return _mm_clmulepi64_si128(a,b, 0);
}

static inline cl_128_t cl_load_64(uint64_t a)
{
  return _mm_cvtsi64_si128((int64_t)a);
}

static inline cl_128_t cl_load_32(uint32_t a)
{
  return _mm_cvtsi32_si128((int32_t)a);
}

static inline cl_128_t cl_mul_64_(uint64_t a, uint64_t b)
{
  return cl_mul_base(cl_load_64(a),cl_load_64(b));
}

static inline cl_128_t cl_mul_32_(uint32_t a, uint32_t b)
{
  return cl_mul_base(cl_load_32(a),cl_load_32(b));
}

static inline uint32_t cl_lo_32(cl_128_t x)
{
  return (uint32_t)_mm_cvtsi128_si32(x);
}

static inline uint32_t cl_hi_32(cl_128_t x)
{
  return (uint32_t)_mm_extract_epi32(x,1);
}

static inline uint64_t cl_lo_64(cl_128_t x)
{
  return (uint64_t)_mm_cvtsi128_si64(x);
}

static inline uint64_t cl_hi_64(cl_128_t x)
{
  return (uint64_t)_mm_extract_epi64(x,1);
}


//-----------------------------------------------------------

static inline uint32_t cl_mul_32(uint32_t a, uint32_t b)
{
  return cl_lo_32(cl_mul_32_(a,b));
}

static inline uint64_t cl_mul_64(uint64_t a, uint64_t b)
{
  return cl_lo_64(cl_mul_64_(a,b));
}

// hi results of standard carryless products

// NOTE: b is intentionally defined as 64-bit as is called product
static inline uint32_t cl_mul_hi_32(uint32_t a, uint64_t b)
{
  return cl_hi_32(cl_mul_64_(a,b));
}

static inline uint64_t cl_mul_hi_64(uint64_t a, uint64_t b)
{
  return cl_hi_64(cl_mul_64_(a,b));
}


// inverses return either 0 or 1<<(bits-1) if
// the input is a divisor (not-invertiable)
// does not use special case squaring since
// that requires moving between xmm and standard
// reg set and inflates code footprint.

// --change to the wrappers

static inline uint32_t cl_mul_inv_32(uint32_t v)
{
  cl_128_t x = cl_load_32(v);
  cl_128_t r = x;
  
  for(int i=0; i<4; i++) {
    x = cl_mul_base(x,x);
    r = cl_mul_base(r,x);
  }
  return cl_lo_32(r);
}

static inline uint64_t cl_mul_inv_64(uint64_t v)
{
  cl_128_t x = cl_load_64(v);
  cl_128_t r = x;

  for(int i=0; i<5; i++) {
    x = cl_mul_base(x,x);
    r = cl_mul_base(r,x);
  }
  return cl_lo_64(r);
}


static inline uint32_t cl_pow2_32(uint32_t x) { return bit_scatter_32(x, bit_set_even_1_32); }
static inline uint64_t cl_pow2_64(uint64_t x) { return bit_scatter_64(x, bit_set_even_1_64); }

// misnamed: only if 'x' is a square
static inline uint32_t cl_sqrt_32(uint32_t x) { return bit_gather_32(x, bit_set_even_1_32); }
static inline uint64_t cl_sqrt_64(uint64_t x) { return bit_gather_64(x, bit_set_even_1_64); }


//-----------------------------------------------------------

static inline uint32_t cr_mul_32(uint32_t a, uint32_t b)
{
  return cl_mul_hi_32(a, ((uint64_t)b)<<1);
}

static inline uint64_t cr_mul_64(uint64_t a, uint64_t b)
{
  uint64_t r = cl_mul_hi_64(a, b<<1);

  // handle the bit that got discarded by (k<<1)
  r = ((int64_t)b < 0) ? r^a : r;

  return r;
}

static inline uint32_t cr_pow2_32(uint32_t x)
{
  return bit_scatter_32(x>>16, bit_set_even_1_32<<1);
}

static inline uint64_t cr_pow2_64(uint64_t x)
{
  return bit_scatter_64(x>>32, bit_set_even_1_64<<1);
}


//-----------------------------------------------------------

static inline uint32_t cc_mul_32(uint32_t a, uint32_t b)
{
  cl_128_t p = cl_mul_32_(a,b);
  uint32_t r = cl_hi_32(p);
  uint32_t l = cl_lo_32(p);

  return r^l;
}

static inline uint64_t cc_mul_64(uint64_t a, uint64_t b)
{
  cl_128_t p = cl_mul_64_(a,b);
  uint64_t r = cl_hi_64(p);
  uint64_t l = cl_lo_64(p);

  return r^l;
}


//-----------------------------------------------------------

// inverse of crc32c(x,0):
//   crc32c_inv(crc32c(x,0)) == crc32c(crc32c_inv(x),0) == x
//   note: crc32c(a,b) == crc32c(a,0)^crc32c(b,0)
static inline uint32_t crc32c_inv(uint32_t x)
{
  x = cr_mul_32(x, 0x82febcde);
  x = cl_mul_32(x, 0x05ec76f1); 
  return x;
}



//-----------------------------------------------------------

#if !defined(CARRYLESS_IMPLEMENTATION)

extern uint32_t cl_gcd_32(uint32_t u, uint32_t v);
extern uint64_t cl_gcd_64(uint64_t u, uint64_t v);

extern uint32_t cl_divrem_32(uint32_t a, uint32_t b, uint32_t* r);
extern uint64_t cl_divrem_64(uint64_t a, uint64_t b, uint64_t* r);
extern uint32_t cl_rem_32(uint32_t a, uint32_t b);
extern uint64_t cl_rem_64(uint64_t a, uint64_t b);

extern uint32_t cl_mul_order_32(uint32_t x);
extern uint32_t cl_mul_order_64(uint32_t x);

#else

#define CL_SWAP(T,X,Y) { T t = X; X=Y; Y=t; }

uint32_t cl_gcd_32(uint32_t u, uint32_t v)
{
  uint32_t s;
  
  if (u != 0 && v != 0) {
    s   = ctz_32(u|v);
    u >>= ctz_32(u);
    
    do {
      v >>= ctz_32(v);
      if (u > v) { CL_SWAP(uint32_t, u,v); }
      v = v ^ u;
    } while (v != 0);
    
    return u<<s;
  }
  
  return u^v;
}

uint64_t cl_gcd_64(uint64_t u, uint64_t v)
{
  uint64_t s;
  
  if (u != 0 && v != 0) {
    s   = ctz_64(u|v);
    u >>= ctz_64(u);
    
    do {
      v >>= ctz_64(v);
      if (u > v) { CL_SWAP(uint64_t, u,v); }
      v = v ^ u;
    } while (v != 0);
    
    return u<<s;
  }
  
  return u^v;
}


uint32_t cl_divrem_32(uint32_t a, uint32_t b, uint32_t* r)
{
  // euclidean division:
  uint32_t q  = 0;

  if (b != 0) {
    uint32_t lb = clz_32(b);
    uint32_t t  = lb - clz_32(a);
    
    while ((int32_t)t >= 0) {
      a ^= b << t;
      q ^= 1 << t;
      t  = lb - clz_32(a);
    }
  }
    
  *r = a;
  
  return q;
}

uint64_t cl_divrem_64(uint64_t a, uint64_t b, uint64_t* r)
{
  uint64_t q  = 0;

  if (b != 0) {
    uint64_t lb = clz_64(b);
    uint64_t t  = lb - clz_64(a);
    
    while ((int64_t)t >= 0) {
      a ^= b << t;
      q ^= UINT64_C(1) << t;
      t  = lb - clz_64(a);
    }
  }
    
  *r = a;
  
  return q;
}

uint32_t cl_rem_32(uint32_t a, uint32_t b)
{
  if (b != 0) {
    uint32_t lb = clz_32(b);
    uint32_t t  = lb - clz_32(a);
    
    while ((int32_t)t >= 0) {
      a ^= b << t;
      t  = lb - clz_32(a);
    }
  }
    
  return a;
}

uint64_t cl_rem_64(uint64_t a, uint64_t b)
{
  if (b != 0) {
    uint32_t lb = clz_64(b);
    uint32_t t  = lb - clz_64(a);
    
    while ((int32_t)t >= 0) {
      a ^= b << t;
      t  = lb - clz_64(a);
    }
  }
    
  return a;
}


// temp hack: kill log2 usuage
#ifndef __INTOPS_H__
#include "intops.h"
#endif

// cl/cr/cc (32-bit) all have:
//
// card of units by 'n' (same counts for divisors)
// x^n = 1 (smallest n)
//   n =  2 00010000 (     65536)
//   n =  4 00ff0000 (  16711680)
//   n =  8 0f000000 ( 251658240)
//   n = 16 30000000 ( 805306368)
//   n = 32 40000000 (1073741824)

uint32_t cl_mul_order_32(uint32_t x)
{
  x = (x & UINT32_C(~1));
  return 32 >> log2_32(ctz_32(x));
}

uint64_t cl_mul_order_64(uint64_t x)
{
  x = (x & UINT64_C(~1));
  return 64 >> log2_64(ctz_64(x));
}

#endif

static inline uint32_t cl_div_32(uint32_t a, uint32_t b)
{
  uint32_t t;
  return cl_divrem_32(a,b,&t);
}

static inline uint64_t cl_div_64(uint64_t a, uint64_t b)
{
  uint64_t t;
  return cl_divrem_64(a,b,&t);
}


static inline uint32_t cr_gcd_32(uint32_t u, uint32_t v)
{
  // lazy version
  u = bit_reverse_32(u);
  v = bit_reverse_32(v);
  return bit_reverse_32(cl_gcd_32(u,v));
}

static inline uint64_t cr_gcd_64(uint64_t u, uint64_t v)
{
  // lazy version
  u = bit_reverse_64(u);
  v = bit_reverse_64(v);
  return bit_reverse_64(cl_gcd_64(u,v));
}



//-----------------------------------------------------------
// bit operation built on carryless ops

// TODO: switch for software carryless impl (not really planning on doing that)
#if 1
 // TODO: (wait! are these named backward?...need covind)
static inline uint32_t bit_prefix_sum_32(uint32_t a) { return cr_mul_32(a, UINT32_C(~0)); }
static inline uint32_t bit_suffix_sum_32(uint32_t a) { return cl_mul_32(a, UINT32_C(~0)); }

static inline uint64_t bit_prefix_sum_64(uint64_t a) { return cr_mul_64(a, UINT64_C(~0)); }
static inline uint64_t bit_suffix_sum_64(uint64_t a) { return cl_mul_64(a, UINT64_C(~0)); }

#else

static inline uint32_t bit_prefix_sum_32(uint32_t a)
{
  a ^= a >>  1;
  a ^= a >>  2;
  a ^= a >>  4;
  a ^= a >>  8;
  a ^= a >> 16;
  return a;
}

static inline uint32_t bit_suffix_sum_32(uint32_t a)
{
  a ^= a <<  1;
  a ^= a <<  2;
  a ^= a <<  4;
  a ^= a <<  8;
  a ^= a << 16;
  return a;
}

static inline uint64_t bit_prefix_sum_64(uint64_t a)
{
  a ^= a >>  1;
  a ^= a >>  2;
  a ^= a >>  4;
  a ^= a >>  8;
  a ^= a >> 16;
  a ^= a >> 32;
  return a;
}

static inline uint64_t bit_suffix_sum_32(uint64_t a)
{
  a ^= a <<  1;
  a ^= a <<  2;
  a ^= a <<  4;
  a ^= a <<  8;
  a ^= a << 16;
  a ^= a << 32;
  return a;
}

#endif


#undef CL_SWAP
#endif
