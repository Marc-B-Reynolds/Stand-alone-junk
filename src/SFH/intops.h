// Marc B. Reynolds, 2016-2023
// Public Domain under http://unlicense.org, see link for details.

#ifndef INTOPS_H
#define INTOPS_H

//  name_(u|s)?(d+)

#ifndef   BITOPS_H
#include "bitops.h"
#endif


#if !defined(_MSC_VER)
#else
#include <intrin.h>
#endif

static inline uint32_t shr_u32(uint32_t x, uint32_t n) { return (uint32_t)(((int32_t)x) >> n); }
static inline uint64_t shr_u64(uint64_t x, uint32_t n) { return (uint64_t)(((int64_t)x) >> n); }

static inline uint32_t sgn_mask_u32(uint32_t x) { return shr_u32(x,31); }
static inline uint64_t sgn_mask_u64(uint64_t x) { return shr_u64(x,63); }

static inline void cs_adder_64(uint64_t* h, uint64_t* l, uint64_t a, uint64_t b, uint64_t c)
{
  uint64_t s = a^b;
  h[0] = (a & b)|(s & c);
  l[0] = s^c;
}

static inline uint32_t log2_32(uint32_t x)       { return (31 - clz_32(x));   }
static inline uint64_t log2_64(uint64_t x)       { return (63 - clz_64(x));   }
static inline uint32_t log2_ceil_32(uint32_t x)  { return (32 - clz_32(x-1)); }
static inline uint64_t log2_ceil_64(uint64_t x)  { return (64 - clz_64(x-1)); }

// floor and ceiling of (a+b)/2 without overflow
static inline uint32_t ave_u32(uint32_t a, uint32_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint64_t ave_u64(uint64_t a, uint64_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint32_t ave_ceil_u32(uint32_t a, uint32_t b) { return (a|b) - ((a^b)>>1); }
static inline uint64_t ave_ceil_u64(uint64_t a, uint64_t b) { return (a|b) - ((a^b)>>1); }


//-------------------------------------------------------------------------------
// mod int ops assume inputs in the ring [0,n)

// (x+1) % n
static inline uint32_t inc_mod_n_u32(uint32_t x, uint32_t n)
{
  uint32_t t = x+1;
  return (x<n) ? t : 0;
}

static inline uint64_t inc_mod_n_u64(uint64_t x, uint64_t n)
{
  uint64_t t = x+1;
  return (x<n) ? t : 0;
}

// (x-1) % n
static inline uint32_t dec_mod_n_u32(uint32_t x, uint32_t n)
{
  uint32_t t = (0<x) ? x : n;
  return t-1;
}

static inline uint64_t dec_mod_n_u64(uint64_t x, uint64_t n)
{
  uint64_t t = (0<x) ? x : n;
  return t-1;
}

static inline uint32_t neg_mod_n_u32(uint32_t x, uint32_t n)
{
  return (x != 0) ? n-x : 0;
}

static inline uint64_t neg_mod_n_u64(uint64_t x, uint64_t n)
{
  return (x != 0) ? n-x : 0;
}


//-------------------------------------------------------------------------------

// multiplicative inverse: find i such that i*a = 1 (requires odd 'a')
// SEE: https://marc-b-reynolds.github.io/math/2017/09/18/ModInverse.html

static inline uint32_t mod_inverse_u32(uint32_t a)
{
  uint32_t x = (3*a)^2; 
  uint32_t y  = 1 - a*x;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y);
  return x;
}

static inline uint64_t mod_inverse_u64(uint64_t a)
{
  uint64_t x = (3*a)^2; 
  uint64_t y  = 1 - a*x;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y);
  return x;
}

// two for one trick: mod inverse of a & b
static inline void mod_inverse_x2_u32(uint32_t a, uint32_t b, uint32_t* ia, uint32_t* ib)
{
  uint32_t c  = a*b;
  uint32_t ic = mod_inverse_u32(c);
  *ia = ic * b;
  *ib = ic * a;
}

static inline void mod_inverse_x2_u64(uint64_t a, uint64_t b, uint64_t* ia, uint64_t* ib)
{
  uint64_t c  = a*b;
  uint64_t ic = mod_inverse_u64(c);
  *ia = ic * b;
  *ib = ic * a;
}

// 128-bit result of 64-bit product
static inline uint64_t mul_hilo_64(uint64_t a, uint64_t b, uint64_t* hi)
{
#if defined(__GNUC__) || defined(__clang__)
  __uint128_t r = (__uint128_t)a * (__uint128_t)b;
  *hi = (uint64_t)(r >> 64);
  return (uint64_t)r;
#elif defined(_MSC_VER) && defined(_M_IX64)
#pragma intrinsic(_umul128)
  return _umul128(a,b,hi);
#else
  static_assert(0, "mul_hilo_64: not written");
#endif    
}


#if !defined(INTOPS_IMPLEMENTATION)

extern uint32_t gcd_nz_u32(uint32_t u, uint32_t v);
extern uint64_t gcd_nz_u64(uint64_t u, uint64_t v);

#else

#define INTOPS_SWAP(T,X,Y) { T t = X; X=Y; Y=t; }

// GCD when neither input is zero
uint32_t gcd_nz_u32(uint32_t u, uint32_t v)
{
  uint32_t s = ctz_32(u|v);
  
  u >>= ctz_32(u);
  
  do {
    v >>= ctz_32(v);
    if (u > v) { INTOPS_SWAP(uint32_t, u,v); }
    v = v - u;
  } while (v != 0);
  
  return u<<s;
}

uint64_t gcd_nz_u64(uint64_t u, uint64_t v)
{
  uint32_t s = ctz_64(u|v);
  
  u >>= ctz_64(u);
  
  do {
    v >>= ctz_64(v);
    if (u > v) { INTOPS_SWAP(uint64_t, u,v); }
    v = v - u;
  } while (v != 0);
  
  return u<<s;
}

#endif

static inline uint32_t gcd_u32(uint32_t a, uint32_t b)
{
  if (a != 0 && b != 0)
    return gcd_nz_u32(a,b);

  return a+b;
}

static inline uint64_t gcd_u64(uint64_t a, uint64_t b)
{
  if (a != 0 && b != 0)
    return gcd_nz_u64(a,b);

  return a+b;
}

#undef INTOPS_SWAP

#endif
