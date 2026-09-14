// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2023-2026
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <stdint.h>

// for the full products ATM. make local copies
#include "SFH/intops.h"

//────────────────────────────────────────────────────────────────────────────────────
// Single header file Montgomery form arithmetic (1). The reduce routines are
// using postive mod inverse as in (2,3)
//
// Brief: choose constant R (here 2^{32} and 2^{64}) and given an odd modulus N,
// then we can represent a value 'a' as:
//   a' = aR mod N
//
// 1) https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
// 2) https://en.algorithmica.org/hpc/number-theory/montgomery/
// 3) https://jeffhurchalla.com/2022/04/28/montgomery-redc-using-the-positive-inverse-mod-r/
// 4) https://jeffhurchalla.com/2022/04/29/optimized-montgomery-multiplication-with-smaller-modulus-sizes/
// 5) https://jeffhurchalla.com/2022/05/01/the-montgomery-multiply-accumulate/


#if !defined(MONT_DEBUG)
#define mont_assert(...)
#define mont_check_canonical(...)
#define mont_check_relaxed(...)
#else
#include <assert.h>
#include <stdio.h>

// all the asserts are temp hacks
#define mont_assert(X) assert(X)
#define mont_check_canonical(V,K) do { assert(V < K.n);        } while(0)
#define mont_check_relaxed(V,K)   do { assert(V < (K.n+K.n-1));} while(0)
#endif


// precomputed data for working in Montgomery form for odd modulus 'n'.
typedef struct {
  uint32_t n;        // odd modulus
  uint32_t i;        // n^{-1} mod R  (R = 2^32)
  uint32_t r;        // R   mod n
  uint32_t r2;       // R^2 mod n
} mont_u32_t;


typedef struct {
  uint64_t n;        // odd modulus
  uint64_t i;        // n^{-1} mod R  (R = 2^64)
  uint64_t r;        // R   mod n
  uint64_t r2;       // R^2 mod n
} mont_u64_t;


#include <assert.h>
#include <stdint.h>



// one in Montgomery form
static inline uint32_t mont_one_u32(const mont_u32_t k) { return k.r; }
static inline uint64_t mont_one_u64(const mont_u64_t k) { return k.r; }

//   m = (T mod R) N^-1 
//   t = ((T - mN) / R) mod N

static inline uint32_t mont_reduce_u32(uint64_t T, const mont_u32_t k)
{
  uint32_t l = (uint32_t)(T);
  uint32_t h = (uint32_t)(T>>32);

  mont_check_canonical(h,k);

  uint32_t m = (uint32_t)mul_hi_u32(l*k.i, k.n);
  uint32_t t = (h + k.n) - m;
  uint32_t d = h - m;
  uint32_t r = (h < m) ? t : d;

  mont_check_canonical(r,k);

  return r;
}


static inline uint64_t mont_reduce_u64(pair_u64_t T, const mont_u64_t k)
{
  uint64_t l = T.lo;
  uint64_t h = T.hi;

  mont_check_canonical(h,k);

  uint64_t m = mul_hi_u64(l*k.i, k.n);
  uint64_t t = (h + k.n) - m;
  uint64_t d = h - m;
  uint64_t r = (h < m) ? t : d;

  mont_check_canonical(r,k);

  return r;
}


// result on [0,2n-2] 
static inline uint32_t mont_reduce_u31(uint64_t T, const mont_u32_t k)
{
  uint32_t l = (uint32_t)(T);                // T mod R
  uint32_t h = (uint32_t)(T>>32);            // T / R
  
  mont_assert(k.n < UINT64_C(0x80000000));

  uint32_t m = l * k.i;                      // (T mod R)N^-1 mod R
  uint32_t t = (uint32_t)mul_hi_u32(m, k.n); // (T - mN) / R
  uint32_t r = h + k.n - t;
  
  mont_check_relaxed(r,k);

  return r;
}

static inline uint64_t mont_reduce_u63(pair_u64_t T, const mont_u64_t k)
{
  uint64_t l = T.lo;
  uint64_t h = T.hi;

  mont_assert(k.n < UINT64_C(0x8000000000000000));
  
  uint64_t m = l * k.i;
  uint64_t t = mul_hi_u64(m, k.n);
  uint64_t r = h + k.n - t;
  
  mont_check_relaxed(r,k);

  return r;
}


// special case reduction: 'x' fits in a word
static inline uint32_t mont_sreduce_u32(uint32_t x, const mont_u32_t k)
{
  uint32_t m = (uint32_t)mul_hi_u32(x*k.i, k.n);
  uint32_t r = m ? k.n - m : 0;

  mont_check_canonical(r,k);

  return r;
}

static inline uint64_t mont_sreduce_u64(uint64_t x, const mont_u64_t k)
{
  uint64_t m = mul_hi_u64(x*k.i, k.n);
  uint64_t r = m ? k.n - m : 0;
  
  mont_check_canonical(r,k);
  
  return r;
}


//─────────────────────────────────────────────────────────────────────────────────

static inline uint32_t mont_sq_u32(uint32_t, const mont_u32_t);
static inline uint64_t mont_sq_u64(uint64_t, const mont_u64_t);


static inline mont_u32_t mont_init_u32(uint32_t n)
{
  uint64_t   t = n;                       // promote (for 2^64 mod n)
  mont_u32_t k;

  mont_assert((n & 1)==1 && "modulus must be odd");
  
  k.n  = n;                               // modulus (odd)
  k.i  = mod_inverse_u32(n);              // n^{-1} mod R
  k.r2 = (uint32_t)((-t) % t);            // R^2 mod n
  k.r  = mont_sreduce_u32(k.r2,k);        // R   mod n
  
  return k;
}


static inline uint64_t pow2_128_mod_u64(uint64_t n)
{
#if 0
  // this is probably better for ARM
  static const __uint128_t K = ~(__uint128_t)0;  // 2^128-1
  
  uint64_t r = (uint64_t)(K % n);

  // 2^128 mod n = ((2^128 - 1) mod n + 1) mod n
  return (r == n - 1) ? 0 : r + 1;
#else
  uint64_t   r = (-n) % n;
  pair_u64_t m = mul_full_u64(r,r);
  pair_u64_t t = divmod_u128_u64(m,n);

  return t.r;
#endif  
}

static inline mont_u64_t mont_init_u64(uint64_t n)
{
  uint64_t   t = pow2_128_mod_u64(n);     // R^2 mod n (64-bit)
  mont_u64_t k;

  assert((n & 1)==1 && "modulus must be odd");
  
  k.n  = n;                               // modulus (odd)
  k.i  = mod_inverse_u64(n);              // n^{-1} mod R
  k.r2 = t;                               // R^2 mod n
  k.r  = mont_sreduce_u64(k.r2,k);        // R   mod n
  
  return k;
}

//─────────────────────────────────────────────────────────────────────────────────

// ab mod n
static inline uint32_t mont_mul_u32(uint32_t a, uint32_t b, const mont_u32_t k)
{
  return mont_reduce_u32(mul_full_u32(a,b),k);
}

static inline uint64_t mont_mul_u64(uint64_t a, uint64_t b, const mont_u64_t k)
{
  return mont_reduce_u64(mul_full_u64(a,b),k);
}

static inline uint32_t mont_mul_u31(uint32_t a, uint32_t b, const mont_u32_t k)
{
  return mont_reduce_u31(mul_full_u32(a,b),k);
}

static inline uint64_t mont_mul_u63(uint64_t a, uint64_t b, const mont_u64_t k)
{
  return mont_reduce_u63(mul_full_u64(a,b),k);
}

static inline uint32_t mont_sq_u32(uint32_t a, const mont_u32_t k)
{
  mont_check_canonical(a,k);
  return mont_reduce_u32(mul_full_u32(a,a),k);
}

static inline uint64_t mont_sq_u64(uint64_t a, const mont_u64_t k)
{
  mont_check_canonical(a,k);
  return mont_reduce_u64(mul_full_u64(a,a),k);
}

static inline uint32_t mont_sq_u31(uint32_t a, const mont_u32_t k)
{
  return mont_reduce_u31(mul_full_u32(a,a),k);
}

static inline uint64_t mont_sq_u63(uint64_t a, const mont_u64_t k)
{
  return mont_reduce_u63(mul_full_u64(a,a),k);
}


//─────────────────────────────────────────────────────────────────────────────────

// convert 'x' to Montgomery form. requires x on [0,n)
//   xR mod n = (x R^2 mod n)R^{-1} mod n

static inline uint32_t mont_form_u32(uint32_t x, const mont_u32_t k)
{
  mont_check_canonical(x,k);
  return mont_reduce_u32(mul_full_u32(x, k.r2), k);
}

static inline uint64_t mont_form_u64(uint64_t x, const mont_u64_t k)
{
  mont_check_canonical(x,k);
  return mont_reduce_u64(mul_full_u64(x, k.r2), k);
}


// montgomery form to normal integer.
static inline uint32_t mont_to_u32(uint32_t x, const mont_u32_t k)
{
  uint32_t r = mont_sreduce_u32(x,k);
  mont_check_canonical(r,k);

  return r;
}

static inline uint64_t mont_to_u64(uint64_t x, const mont_u64_t k)
{
  uint64_t r = mont_sreduce_u64(x,k);
  mont_check_canonical(r,k);

  return r;
}


// "normalize" value to [0,n) range assuming it can be on [0,2n]
static inline uint32_t mont_canonicalize_u32(uint32_t x, const mont_u32_t k)
{
  mont_check_relaxed(x,k);
  uint32_t r = (x < k.n) ? x : x-k.n;
  mont_check_canonical(r,k);
  
  return r;
}

static inline uint64_t mont_canonicalize_u64(uint64_t x, const mont_u64_t k)
{
  mont_check_relaxed(x,k);
  uint64_t r =  (x < k.n) ? x : x-k.n;
  mont_check_canonical(r,k);
  
  return r;
}

//─────────────────────────────────────────────────────────────────────────────────
// basic ops: {negate,inc,dec,add,sub} are same as non montgomery form.
// These assume input(s) are on the canonical range: [0,n)

static inline uint32_t mont_neg_u32(uint32_t a, const mont_u32_t k)
{
  mont_check_canonical(a,k);
  uint32_t r = a ? k.n-a : 0;
  mont_check_canonical(r,k);

  return r;
}

static inline uint64_t mont_neg_u64(uint64_t a, const mont_u64_t k)
{
  mont_check_canonical(a,k);
  uint64_t r = a ? k.n-a : 0;
  mont_check_canonical(r,k);

  return r;
}

static inline uint32_t mont_dec_u32(uint32_t x, mont_u32_t k)
{
  mont_check_canonical(x,k);
  
  uint32_t t = (x!=0) ? x : k.n;
  return t-1;
}

static inline uint64_t mont_dec_u64(uint64_t x, mont_u64_t k)
{
  mont_check_canonical(x,k);
  
  uint64_t t = (x!=0) ? x : k.n;
  return t-1;
}

static inline uint32_t mont_inc_u32(uint32_t x, mont_u32_t k)
{
  mont_check_canonical(x,k);
  
  uint32_t t = x+1;
  return (x < k.n) ? t : 0;
}

static inline uint64_t mont_inc_u64(uint64_t x, mont_u64_t k)
{
  mont_check_canonical(x,k);
  
  uint64_t t = x+1;
  return (x < k.n) ? t : 0;
}

static inline uint32_t mont_add_u32(uint32_t x, uint32_t y, mont_u32_t k)
{
  mont_check_canonical(x,k);
  mont_check_canonical(y,k);
  
  uint32_t t = k.n-y;
  return (t > x) ? x+y : x-t;
}

static inline uint64_t mont_add_u64(uint64_t x, uint64_t y, mont_u64_t k)
{
  mont_check_canonical(x,k);
  mont_check_canonical(y,k);
  
  uint64_t t = k.n-y;
  uint64_t r = (t > x) ? x+y : x-t;

  return r;
}

static inline uint32_t mont_sub_u32(uint32_t x, uint32_t y, mont_u32_t k)
{
  mont_check_canonical(x,k);
  mont_check_canonical(y,k);
  
  uint32_t t = (y < x) ? 0 : k.n;
  uint32_t r = x-y+t;

  return r;
}

static inline uint64_t mont_sub_u64(uint64_t x, uint64_t y, mont_u64_t k)
{
  mont_check_canonical(x,k);
  mont_check_canonical(y,k);
  
  uint64_t t = (y < x) ? 0 : k.n;
  uint64_t r = x-y+t;

  return r;
}

// 2a mod n
static inline uint32_t mont_mul2_u32(uint32_t a, const mont_u32_t k)
{
  mont_check_canonical(a,k);
  uint32_t t = k.n-a;

  return (t > a) ? a+a : a-t;
}

static inline uint64_t mont_mul2_u64(uint64_t a, const mont_u64_t k)
{
  mont_check_canonical(a,k);
  uint64_t t = k.n-a;

  return (t > a) ? a+a : a-t;
}

//────────────────────────────────────────────────────────────────────────────────────


static inline uint64_t mont_fma_u32_i(uint32_t x, uint32_t y, uint32_t c, const mont_u32_t k)
{
  uint64_t m = mul_full_u32(x,y);
  uint64_t h = m >> 32;
  uint64_t d = (h < k.n-c) ? c : c-k.n;

  m += (d << 32);
  
  return m;
}

static inline pair_u64_t mont_fma_u64_i(uint64_t x, uint64_t y, uint64_t c, const mont_u64_t k)
{
  pair_u64_t m = mul_full_u64(x,y);
  uint64_t   d = (m.hi < k.n-c) ? c : c-k.n;

  m.hi += d;
  
  return m;
}

static inline uint64_t mont_fms_u32_i(uint32_t x, uint32_t y, uint32_t c, const mont_u32_t k)
{
  uint64_t m = mul_full_u32(x,y);
  uint64_t h = m >> 32;
  uint64_t d = (h >= c) ? c : c-k.n;

  m -= (d << 32);
  
  return m;
}

static inline pair_u64_t mont_fms_u64_i(uint64_t x, uint64_t y, uint64_t c, const mont_u64_t k)
{
  pair_u64_t m = mul_full_u64(x,y);
  uint64_t   d = (m.hi >= c) ? c : c-k.n;

  m.hi -= d;
  
  return m;
}

static inline uint32_t mont_fma_u32(uint32_t a, uint32_t b, uint32_t c, const mont_u32_t k)
{
  return mont_reduce_u32(mont_fma_u32_i(a,b,c,k),k);
}

static inline uint32_t mont_fms_u32(uint32_t a, uint32_t b, uint32_t c, const mont_u32_t k)
{
  return mont_reduce_u32(mont_fms_u32_i(a,b,c,k),k);
}

static inline uint64_t mont_fma_u64(uint64_t a, uint64_t b, uint64_t c, const mont_u64_t k)
{
  return mont_reduce_u64(mont_fma_u64_i(a,b,c,k),k);
}

static inline uint64_t mont_fms_u64(uint64_t a, uint64_t b, uint64_t c, const mont_u64_t k)
{
  return mont_reduce_u64(mont_fms_u64_i(a,b,c,k),k);
}

