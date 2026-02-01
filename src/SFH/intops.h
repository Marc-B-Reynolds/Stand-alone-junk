// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2016-2026
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


// yes. this is evil except for undordered and very special cases of ordered 
static inline pair_u32_t pair_u32(uint32_t a, uint32_t b) { return (pair_u32_t){.a=a, .b=b }; }
static inline pair_u64_t pair_u64(uint64_t a, uint64_t b) { return (pair_u64_t){.a=a, .b=b }; }

static inline uint32_t shr_u32(uint32_t x, uint32_t n) { return (uint32_t)(((int32_t)x) >> n); }
static inline uint64_t shr_u64(uint64_t x, uint32_t n) { return (uint64_t)(((int64_t)x) >> n); }

static inline uint32_t sgn_mask_u32(uint32_t x) { return shr_u32(x,31); }
static inline uint64_t sgn_mask_u64(uint64_t x) { return shr_u64(x,63); }

static inline pair_u64_t cs_adder_64(uint64_t a, uint64_t b, uint64_t c)
{
  uint64_t s = a^b;
  return (pair_u64_t){.hi= (a & b)|(s & c), .lo=s^c };
}

static inline pair_u32_t cs_adder_32(uint32_t a, uint32_t b, uint32_t c)
{
  uint32_t s = a^b;
  return (pair_u32_t){.hi= (a & b)|(s & c), .lo=s^c };
}

// returns 1 for even 2 for odd
static inline uint32_t even_off_u32(uint32_t x) { return 2-(x&1); }
static inline uint64_t even_off_u64(uint64_t x) { return 2-(x&1); }

// returns 1 for odd 2 for even
static inline uint32_t odd_off_u32(uint32_t x)  { return 1+(x&1); }
static inline uint64_t odd_off_u64(uint64_t x)  { return 1+(x&1); }

static inline uint32_t min_u32(uint32_t x, uint32_t y) { return (x<y) ? x : y; }
static inline uint32_t max_u32(uint32_t x, uint32_t y) { return (x>y) ? x : y; }
static inline uint64_t min_u64(uint64_t x, uint64_t y) { return (x<y) ? x : y; }
static inline uint64_t max_u64(uint64_t x, uint64_t y) { return (x>y) ? x : y; }
static inline int32_t  min_s32( int32_t x,  int32_t y) { return (x<y) ? x : y; }
static inline int32_t  max_s32( int32_t x,  int32_t y) { return (x>y) ? x : y; }
static inline int64_t  min_s64( int64_t x,  int64_t y) { return (x<y) ? x : y; }
static inline int64_t  max_s64( int64_t x,  int64_t y) { return (x>y) ? x : y; }

// floor and ceiling log_2(x)
static inline uint32_t log2_u32(uint32_t x)       { return (31 - clz_32(x));   }
static inline uint64_t log2_u64(uint64_t x)       { return (63 - clz_64(x));   }
static inline uint32_t log2_ceil_u32(uint32_t x)  { return (32 - clz_32(x-1)); }
static inline uint64_t log2_ceil_u64(uint64_t x)  { return (64 - clz_64(x-1)); }

// floor and ceiling of (a+b)/2 without overflow
static inline uint32_t ave_u32(uint32_t a, uint32_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint64_t ave_u64(uint64_t a, uint64_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint32_t ave_ceil_u32(uint32_t a, uint32_t b) { return (a|b) - ((a^b)>>1); }
static inline uint64_t ave_ceil_u64(uint64_t a, uint64_t b) { return (a|b) - ((a^b)>>1); }


//────────────────────────────────────────────────────────────────────────────────────
// mod int ops assume inputs are in the ring [0,n) 

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

static inline uint32_t add_mod_n_u32(uint32_t x, uint32_t y, uint32_t n)
{
  uint32_t t = n-y;
  return (t > x) ? x+y : x-t;
}

static inline uint64_t add_mod_n_u64(uint64_t x, uint64_t y, uint64_t n)
{
  uint64_t t = n-y;
  return (t > x) ? x+y : x-t;
}

static inline uint32_t sub_mod_n_u32(uint32_t x, uint32_t y, uint32_t n)
{
  uint32_t t = (y > x) ? 0 : n;
  return x-y+t;
}

static inline uint64_t sub_mod_n_u64(uint64_t x, uint64_t y, uint64_t n)
{
  uint64_t t = (y > x) ? 0 : n;
  return x-y+t;
}


// 2^64 mod k
static inline uint64_t mod_k_base_u64(uint64_t k)
{
  static const uint64_t N = UINT64_C(1) << 63;
  
  if (k <= N) {
    uint64_t r = (N % k) << 1;
    
    return r < k ? r : r-k;
  }
  
  return -k;  
}

// 2^32 mod k
static inline uint32_t mod_k_base_u32(uint32_t k)
{
#if 1
  static const uint32_t N = UINT32_C(1) << 31;
  
  if (k <= N) {
    uint32_t r = (N % k) << 1;
    
    return r < k ? r : r-k;
  }
  
  return -k;
#else
  static const uint64_t N = UINT64_C(1) << 32;
  return (uint32_t)(N % k);
#endif
}

//────────────────────────────────────────────────────────────────────────────────────

// 128-bit result of 64-bit product (access via hi and lo elements)
static inline pair_u64_t mul_hilo_64(uint64_t a, uint64_t b)
{
  uint64_t hi,lo;

#if defined(__GNUC__)
  __uint128_t r = (__uint128_t)a * (__uint128_t)b;
  hi = (uint64_t)(r >> 64);
  lo = (uint64_t)r;
#elif defined(_MSC_VER)
  lo = _umul128(a,b,&hi);
#else
  static_assert(0);
  hi = lo = 0;
#endif

  return (pair_u64_t){.hi=hi, .lo=lo};
}

// high 64-bit result of 64-bit product
static inline uint64_t mul_u64_hi(uint64_t a, uint64_t b) { return mul_hilo_64(a,b).hi; }


typedef struct {uint64_t r; int64_t x,y; } gcd_u64_t;
typedef struct {uint32_t r; int32_t x,y; } gcd_u32_t;

#if !defined(INTOPS_IMPLEMENTATION)

extern uint32_t gcd_nz_u32(uint32_t u, uint32_t v);
extern uint64_t gcd_nz_u64(uint64_t u, uint64_t v);

extern gcd_u64_t gcd_extended_u64(uint64_t u, uint64_t v);
extern gcd_u32_t gcd_extended_u32(uint32_t u, uint32_t v);

extern const uint8_t mod_inverse_table_8[];

#else

// 8-bit mod inverse table. only odd elements accessed for legal input
// (could shift and half the same of the table..but meh)
const uint8_t mod_inverse_table_8[] =
{
  0,0x01,0,0xab,0,0xcd,0,0xb7,
  0,0x39,0,0xa3,0,0xc5,0,0xef,
  0,0xf1,0,0x1b,0,0x3d,0,0xa7,
  0,0x29,0,0x13,0,0x35,0,0xdf,
  0,0xe1,0,0x8b,0,0xad,0,0x97,
  0,0x19,0,0x83,0,0xa5,0,0xcf,
  0,0xd1,0,0xfb,0,0x1d,0,0x87,
  0,0x09,0,0xf3,0,0x15,0,0xbf,
  0,0xc1,0,0x6b,0,0x8d,0,0x77,
  0,0xf9,0,0x63,0,0x85,0,0xaf,
  0,0xb1,0,0xdb,0,0xfd,0,0x67,
  0,0xe9,0,0xd3,0,0xf5,0,0x9f,
  0,0xa1,0,0x4b,0,0x6d,0,0x57,
  0,0xd9,0,0x43,0,0x65,0,0x8f,
  0,0x91,0,0xbb,0,0xdd,0,0x47,
  0,0xc9,0,0xb3,0,0xd5,0,0x7f,
  0,0x81,0,0x2b,0,0x4d,0,0x37,
  0,0xb9,0,0x23,0,0x45,0,0x6f,
  0,0x71,0,0x9b,0,0xbd,0,0x27,
  0,0xa9,0,0x93,0,0xb5,0,0x5f,
  0,0x61,0,0x0b,0,0x2d,0,0x17,
  0,0x99,0,0x03,0,0x25,0,0x4f,
  0,0x51,0,0x7b,0,0x9d,0,0x07,
  0,0x89,0,0x73,0,0x95,0,0x3f,
  0,0x41,0,0xeb,0,0x0d,0,0xf7,
  0,0x79,0,0xe3,0,0x05,0,0x2f,
  0,0x31,0,0x5b,0,0x7d,0,0xe7,
  0,0x69,0,0x53,0,0x75,0,0x1f,
  0,0x21,0,0xcb,0,0xed,0,0xd7,
  0,0x59,0,0xc3,0,0xe5,0,0x0f,
  0,0x11,0,0x3b,0,0x5d,0,0xc7,
  0,0x49,0,0x33,0,0x55,0,0xff
};

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

gcd_u32_t gcd_extended_u32(uint32_t u, uint32_t v)
{
  int32_t  x0 = 0, y0 = 1;
  int32_t  x1 = 1, y1 = 0;
  uint32_t a1 = u, a2 = v;
  uint32_t q  = 0;
  
  while (a2 != 0) {
    int32_t  x2 = x0 - (int32_t)q*x1;
    int32_t  y2 = y0 - (int32_t)q*y1;
    uint32_t a0 = a1;

    x0=x1; y0=y1;
    x1=x2; y1=y2; a1=a2;
    
    q  = a0/a1;
    a2 = a0 - q*a1;
  }

  return (gcd_u32_t){.r=a1, .x=x1, .y=y1};
}

gcd_u64_t gcd_extended_u64(uint64_t u, uint64_t v)
{
  int64_t  x0 = 0, y0 = 1;
  int64_t  x1 = 1, y1 = 0;
  uint64_t a1 = u, a2 = v;
  uint64_t q  = 0;
  
  while (a2 != 0) {
    int64_t  x2 = x0 - (int64_t)q*x1;
    int64_t  y2 = y0 - (int64_t)q*y1;
    uint64_t a0 = a1;

    x0=x1; y0=y1;
    x1=x2; y1=y2; a1=a2;
    
    q  = a0/a1;
    a2 = a0 - q*a1;
  }

  return (gcd_u64_t){.r=a1, .x=x1, .y=y1};
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


//────────────────────────────────────────────────────────────────────────────────────
// multiplicative inverse: find i such that i*a = 1 (requires odd 'a')
// SEE: https://marc-b-reynolds.github.io/math/2017/09/18/ModInverse.html

static inline uint32_t mod_inverse_u32(uint32_t a)
{
  uint32_t x = (3*a)^2; 
  uint32_t y = 1 - a*x;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y);
  return x;
}

static inline uint64_t mod_inverse_u64(uint64_t a)
{
  uint64_t x = (3*a)^2; 
  uint64_t y = 1 - a*x;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y); y *= y;
  x = x*(1+y);
  return x;
}

// two for one trick: mod inverse of a & b (pair order as input)
static inline pair_u32_t mod_inverse_x2_u32(uint32_t a, uint32_t b)
{
  uint32_t c  = a*b;
  uint32_t ic = mod_inverse_u32(c);
  return pair_u32(ic*b, ic*a);
}

static inline pair_u64_t mod_inverse_x2_u64(uint64_t a, uint64_t b)
{
  uint64_t c  = a*b;
  uint64_t ic = mod_inverse_u64(c);
  return pair_u64(ic*b, ic*a);
}

#undef INTOPS_SWAP

#endif
