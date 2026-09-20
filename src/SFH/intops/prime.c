// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2026
// Public Domain under http://unlicense.org, see link for details.
//
// "Fast Primality Testing for Integers That Fit into a Machine Word"
// 

#include <stdint.h>
#include <stdbool.h>

#include "SFH/intops.h"
#include "SFH/intops/montgomery.h"

// table from "Fast Primality.." method: FJ32_256.
static const uint16_t fj32_256[] = {
  15591, 2018,  166, 7429, 8064,16045,10503, 4399,
   1949, 1295, 2776, 3620,  560, 3128, 5212, 2657,
   2300, 2021, 4652, 1471, 9336, 4018, 2398,20462,
  10277, 8028, 2213, 6219,  620, 3763, 4852, 5012,
   3185, 1333, 6227, 5298, 1074, 2391, 5113, 7061,
    803, 1269, 3875,  422,  751,  580, 4729,10239,
    746, 2951,  556, 2206, 3778,  481, 1522, 3476,
    481, 2487, 3266, 5633,  488, 3373, 6441, 3344,
     17,15105, 1490, 4154, 2036, 1882, 1813,  467,
   3307,14042, 6371,  658, 1005,  903,  737, 1887,
   7447, 1888, 2848, 1784, 7559, 3400,  951,13969,
   4304,  177,   41,19875, 3110,13221, 8726,  571,
   7043, 6943, 1199,  352, 6435,  165, 1169, 3315,
    978,  233, 3003, 2562, 2994,10587,10030, 2377,
   1902, 5354, 4447, 1555,  263,27027, 2283,  305,
    669, 1912,  601, 6186,  429, 1930,14873, 1784,
   1661,  524, 3577,  236, 2360, 6146, 2850,55637,
   1753, 4178, 8466,  222, 2579, 2743, 2031, 2226,
   2276,  374, 2132,  813,23788, 1610, 4422, 5159,
   1725, 3597, 3366,14336,  579,  165, 1375,10018,
  12616, 9816, 1371,  536, 1867,10864,  857, 2206,
   5788,  434, 8085,17618,  727, 3639, 1595, 4944,
   2129, 2029, 8195, 8344, 6232, 9183, 8126, 1870,
   3296, 7455, 8947,25017,  541,19115,  368,  566,
   5674,  411,  522, 1027, 8215, 2050, 6544,10049,
    614,  774, 2333, 3007,35201, 4706, 1152, 1785,
   1028, 1540, 3743,  493, 4474, 2521,26845, 8354,
    864,18915, 5465, 2447,   42, 4511, 1660,  166,
   1249, 6259, 2553,  304,  272, 7286,   73, 6554,
    899, 2816, 5197,13330, 7054, 2818, 3199,  811,
    922,  350, 7514, 4452, 3449, 2663, 4708,  418,
   1621, 1171, 3471,   88,11345,  412, 1559,  194
};  

#if 0
// authors reference implementation (mildly hacked)
// https://web.archive.org/web/20231202043758/https://people.ksp.sk/~misof/primes/FJ32_256.cc

bool is_SPRP(uint32_t n, uint32_t a)
{
  uint32_t d = n-1, s = 0;

  while ((d&1)==0) ++s, d>>=1;

  uint64_t c = 1, p = d;

  while (p) { 
    if (p & 1) c = (c*a) % n;
    a = ((uint64_t)a*a) % n;
    p >>= 1;
  }
  
  if (c == 1) return true;

  for (uint32_t r=0; r<s; r++) {
    if (c == n-1) return true;
    c = (c*c) % n;
  }

  return false;
}       


bool is_prime(uint32_t x)
{
  if (x==2 || x==3 || x==5 || x==7) return true;
  if (x%2==0 || x%3==0 || x%5==0 || x%7==0) return false;
  if (x<121) return (x>1);

  uint64_t h = x;
  h = ((h >> 16) ^ h) * 0x45d9f3b;  
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = ((h >> 16) ^ h) & 255;

  return is_SPRP(x,fj32_256[h]);
}
#endif


// montgomery form version. only need to test one base.
static inline int mont_miller_rabin_u32(const uint32_t a, const uint32_t n)
{
  // perform the test either directly in 32-bits or promote to 64-bits
  // and use a simplified reduce. TODO: make an informed selection.
#if 0
  const mont_u32_t k  = mont_init_u32(n);
  const uint32_t   nr = n-k.r;

  uint32_t A = mont_form_u32(a,k);
  uint32_t u = n-1;
  uint32_t s = ctz_32(u);
  uint32_t d = k.r;
  
  u = u >> s;
  
  // compute a^u mod n
  do {
    if (u & 1)
      d = mont_mul_u32(d,A,k);
    
    A= mont_sq_u32(A,k);
  } while (u >>= 1);
  
  if (d == k.r || d == nr) return true;
  
  // walk powers: a^d, a^(2d), ... mod n
  //   test (T1) fast exit very infrequently triggers and
  //   result is correct without so skipping.
  for (uint32_t i=1; i<s; i++) {
    d = mont_sq_u32(d,k);
  //if (d == k.r) return false;       // (T1)
    if (d == nr)  return true;
  }
  
  return false;
#else
  const mont_u64_t k  = mont_init_u64(n);
  const uint64_t   nr = n-k.r;

  uint64_t A = mont_form_u64(a,k);
  uint64_t u = n-1;
  uint64_t s = ctz_64(u);
  uint64_t d = k.r;
  
  u = u >> s;
  
  // compute a^u mod n
  do {
    if (u & 1)
      d = mont_mul_u63(d,A,k);
    
    A= mont_sq_u63(A,k);
  } while (u >>= 1);
  
  if (d == k.r || d == nr) return true;
  
  // walk powers: a^d, a^(2d), ... mod n
  //   test (T1) fast exit very infrequently triggers and
  //   result is correct without so skipping.
  for (uint32_t i=1; i<s; i++) {
    d = mont_sq_u63(d,k);
  //if (d == k.r) return false;       // (T1)
    if (d == nr)  return true;
  }
  
  return false;
#endif  
}


// worker for n on [121,2^32-1] where `n` contains
// no factors of {2,3,5,7}
static inline bool is_prime_core_u32(uint32_t n)
{
  // 981706784 odd integers reach here (45.7143%)
  // and we need to perform one miller-rabin step.
  // paper's hash into base table (black magic!)
  uint64_t h = n;
  h = ((h >> 16) ^ h) * 0x45d9f3b;  
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = ((h >> 16) ^ h) & 255;
  
  // perform the test
  return mont_miller_rabin_u32(fj32_256[h], n);
}


// returns `true` is `n` is prime
bool is_prime_u32(uint32_t n)
{
  // trial divisions: 2,3,5,7
  // TODO: make an informed selection. could also
  // add SIMD trials for ISAs with approp multiply ops.
  // and abstract the trial set constants.
#if 0
  int t2 = (n & 1);
  int t3 = (n % 3) != 0;
  int t5 = (n % 5) != 0;
  int t7 = (n % 7) != 0;
  int t  = (t2 & t3) & (t5 & t7);
#elif 0
  // merged tests probably shouldn't depend on the
  // compiler to produce the mod.
  // merged 3,5 test
  int t2  = (n & 1);
  int t35 = ((0xe996 >> (n % 15)) & 1);
  int t7  = (n % 7) != 0;
  int t   = (t2 & t7) & t35;
#else
  // merged 2,3,5 test
  int ts  = (0xe08a2882 >> (n % 30)) & 1;
  int t7  = (n % 7) != 0;
  int t   = ts & t7;
#endif  
  
  // have we passed the trial divisions?
  if (t != 0) {
    
    // next prime is 11, the smallest composite
    // w/o the trial factors is 11² = 121
    if (n >= 121) {
      return is_prime_core_u32(n);
    }
    else {
      // 1 and primes on [11,113] reach here (smaller
      // composites were caught by the trial divisions).
      return (n != 1);
    }
  }
  
  // to reach here: n is zero or has a factor of 2,3,5,7.
  // For all 'n' the count is 3313260485 (~77.14%)
  // A more interesting number is that 1165776837 odd
  // integers have a factor of 3,5 or 7 (~54.28%).
  // But anyway: anything greater than 7 is composite
  if ((n-1) > 6) return false;

  // for the remaining query a baked bitset of primes
  static const uint32_t bitset = (1u<<2)|(1u<<3)|(1u<<5)|(1u<<7);
  
  return (bitset & (1u << n)) != 0;
}



static inline bool mont_miller_rabin_u64(const uint64_t base[], const int num, const uint64_t n)
{
  // set-up Montgomery form for "mod n"
  const mont_u64_t k  = mont_init_u64(n);
  const uint64_t   nr = n-k.r;
  uint64_t         u  = n-1;
  uint32_t         s  = ctz_64(u);
  
  u >>= s;
  
  for (int j=0; j<num; j++) {
    uint64_t A = mont_form_u64(base[j],k);
    uint64_t d = k.r;
    uint64_t v = u;
    
    if (!A) continue;
    
    // compute a^u mod n
    do {
      if (v & 1)
        d = mont_mul_u64(d,A,k);
      A= mont_sq_u64(A,k);
    } while (v >>= 1);
    
    if (d == k.r || d == nr) continue;
    
    uint32_t i;
    
    // walk powers: a^d, a^(2d), ... mod n
    for (i=1; i<s; i++) {
      d = mont_sq_u64(d,k);
      if (d == k.r) return false;
      if (d == nr)  break;
    }
    
    if (i == s)
      return false;
  }
  
  return true;
}


// 64-bit version is very run-of-the-mill that will be murdered
// by a proper library.
bool is_prime_u64(uint64_t n)
{
  // same outer structure as 32-bit
  int t2 = n & 1;
  int t3 = (n % 3) != 0;
  int t5 = (n % 5) != 0;
  int t7 = (n % 7) != 0;
  int t  = (t2 & t3) & (t5 & t7);
  
  if (t != 0) {
    if (n >= 121) {
      // use 32-bit test if possible. statistically
      // no help for "uniform" inputs but testing
      // very small WRT 2^64 is a reasonable pattern.
      if ((n >> 32) == 0)
        return is_prime_core_u32((uint32_t)n);

      // being lazy about 64-bit and but MR with min base set
      static const uint64_t bases[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};

      // could conditionally use a 63-bit version but meh, that's
      // just a piece of duct-tape.
      return  mont_miller_rabin_u64(bases,7,n);
    }
    else {
      return (n != 1);
    }
  }
  
  if ((n-1) > 6) return false;

  static const uint32_t bitset = (1u<<2)|(1u<<3)|(1u<<5)|(1u<<7);
  
  return (bitset & (1u << n)) != 0;
}
