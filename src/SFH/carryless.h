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

// move two 64-bit input to SIMD, perform
// the carryless product & SIMD (128-bit) result
static inline __m128i cl_mul_base(uint64_t a, uint64_t b)
{
  __m128i x = _mm_cvtsi64_si128((int64_t)a);
  __m128i y = _mm_cvtsi64_si128((int64_t)b);

  return _mm_clmulepi64_si128(x,y, 0);
}

static inline uint32_t cl_lo_32(__m128i x)
{
  return (uint32_t)_mm_cvtsi128_si32(x);
}

static inline uint32_t cl_hi_32(__m128i x)
{
  return (uint32_t)_mm_extract_epi32(x,1);
}

static inline uint64_t cl_lo_64(__m128i x)
{
  return (uint64_t)_mm_cvtsi128_si64(x);
}

static inline uint64_t cl_hi_64(__m128i x)
{
  return (uint64_t)_mm_extract_epi64(x,1);
}


//-----------------------------------------------------------

static inline uint32_t cl_mul_32(uint32_t a, uint32_t b)
{
  return cl_lo_32(cl_mul_base(a,b));
}

static inline uint64_t cl_mul_64(uint64_t a, uint64_t b)
{
  return cl_lo_64(cl_mul_base(a,b));
}

// hi results of standard carryless products

// NOTE: b is intentionally defined as 64-bit
static inline uint32_t cl_mul_hi_32(uint32_t a, uint64_t b)
{
  return cl_hi_32(cl_mul_base(a,b));
}

static inline uint64_t cl_mul_hi_64(uint64_t a, uint64_t b)
{
  return cl_hi_64(cl_mul_base(a,b));
}


// inverses return either 0 or 1<<(bits-1) if
// the input is a divisor (not-invertiable)
// does not use special case squaring since
// that requires moving between xmm and standard
// reg set and inflates code footprint.

static inline uint32_t cl_mul_inv_32(uint32_t v)
{
  __m128i x = _mm_cvtsi32_si128((int32_t)v);
  __m128i r = x;

  for(int i=0; i<4; i++) {
    x = _mm_clmulepi64_si128(x,x, 0);
    r = _mm_clmulepi64_si128(r,x, 0);
  }
  return (uint32_t)_mm_cvtsi128_si32(r);
}

static inline uint64_t cl_mul_inv_64(uint64_t v)
{
  __m128i x = _mm_cvtsi64_si128((int64_t)v);
  __m128i r = x;

  for(int i=0; i<5; i++) {
    x = _mm_clmulepi64_si128(x,x, 0);
    r = _mm_clmulepi64_si128(r,x, 0);
  }
  return (uint64_t)_mm_cvtsi128_si64(r);
}


static inline uint32_t cl_pow2_32(uint32_t x) { return bit_scatter_32(x, bit_set_even_1_32); }
static inline uint64_t cl_pow2_64(uint64_t x) { return bit_scatter_64(x, bit_set_even_1_64); }

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
  __m128i  p = cl_mul_base(a,b);
  uint32_t r = (uint32_t)_mm_extract_epi32(p,1);
  uint32_t l = (uint32_t)_mm_cvtsi128_si32(p);

  return r^l;
}

static inline uint64_t cc_mul_64(uint64_t a, uint64_t b)
{
  __m128i  p = cl_mul_base(a,b);
  uint64_t r = (uint64_t)_mm_extract_epi64(p,1);
  uint64_t l = (uint64_t)_mm_cvtsi128_si64(p);

  return r^l;
}


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
      q ^= 1 << t;
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
    uint64_t lb = clz_64(b);
    uint64_t t  = lb - clz_64(a);
    
    while ((int64_t)t >= 0) {
      a ^= b << t;
      t  = lb - clz_64(a);
    }
  }
    
  return a;
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


#undef CL_SWAP
#endif
