// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2002-2026
// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <assert.h>

//────────────────────────────────────────────────────────────────────────────────────
// Binomial Coefficients
// 
// • uint64_t binomial_u64(uint64_t n, uint64_t k)
//   computes the binomial coefficient (without overflow) for n on [0,67].
//   This limitation is because 67 is the largest 'n' for which all 'k' values
//   can fit in a uint64_t.
//     log2(binomial(67,67/2)) = ~63.6358
//     log2(binomial(68,68/2)) = ~64.6252
//
// • void binomial_init(void)
//   initialize data table (if selected) nop otherwise. By default this should
//   happen automatically at init time (see below).
//
// • int64_t binomial_i64(int64_t n, int64_t k)
//   wrapper than extends to negative n inputs. |n| ≤ 66
//     log2(binomial(66,66/2)) = ~62.6466
//
// The default implementation is table based which is init time initialized. If at
// compile time the "magic" for making that happen isn't found then a warning will
// be produced and `binomial_init()` will need to be called by user before
// calling `binomial_u64`.
// 
// Internally there are three versions:
// • binomial_ref:   minimal code size version for internal testing only.
// • binomial_table: very small amount of computation to map into an 8.25K table (default)
// • binomial_small: hybrid: textbook for cases that can't overflow and table
//   lookup for inputs that would overflow. Actually there's a commented out
//   overflow path that's basically an optimized version of `binomial_ref`.
//   This is purely for entertainment purposes. This version is used if
//   `SFH_BINOMIAL_SMALL` is defined. This is very sad though. But it is small:
//    data table entries = 52 vs. 1056 for table driven.
// 
//────────────────────────────────────────────────────────────────────────────────────

// scaffolding stuff I don't even want to look at
#ifndef __has_include
#define __has_include(X) 0
#endif


// largest 64-bit isn't a binomial coefficient
static const uint64_t binomial_error = UINT64_C(0xFFFFFFFFFFFFFFFF);

// if using `binomial_table` then none of the hints matter very much.
// there's one branch free select hint that almost certainly compiles
// that way without it.
#if __has_include("SFH/compiler_hints.h")
#include "SFH/compiler_hints.h"
#else
#define hint_no_unroll
#endif


// if not testing then fixup the name of the actual call
#ifndef BINOMIAL_TEST
#define binomial_table binomial_u64
#define binomial_small binomial_u64
#endif

// if don't have cmov/csel like hints: throw some together (see notes above)
#if !defined(hint_select)
#if  defined(COMPILER_HINTS_H)
#warning "found and loaded 'SFH/compiler_hints.h' but don't have 'hint_select' et al"
#endif

#if __has_builtin(__builtin_unpredictable)
#define hint_select_u64(C,A,B) ({__builtin_unpredictable(C) ? A : B;})
#elif defined(__GNUC__)
#define hint_select_u64(C,A,B) ({int _hsc=(C); typeof(A) _hsa=A,_hsb=B,_r; asm("":"+r"(_hsc),"+r"(_hsa),"+r"(_hsb)); _r=_hsc?_hsa:_hsb;_r;})
#else
#define hint_select_u64(C,A,B) _Generic((C),default: (C) ? A:B)
#endif

#define hint_cswap_u64(C,X,Y) do { int _csc=(C); uint64_t _csx=X,_csy=Y; X=hint_select_u64(_csc,_csy,_csx); Y=hint_select_u64(_csc,_csx,_csy); } while(0)
#endif


// need a couple of support functions if using small or doing internal testing
#if defined(SFH_BINOMIAL_SMALL) || defined(BINOMIAL_TEST)

#if !defined(_MSC_VER)
// GCC/clang can match to single opcode
static inline uint32_t binomial_ctz(uint64_t x) { return (x!=0) ? (uint32_t)__builtin_ctzl(x) : 64; }
static inline uint32_t binomial_clz(uint64_t x) { return (x!=0) ? (uint32_t)__builtin_clzl(x) : 64; }
#else
//#include <intrin.h>
#pragma intrinsic(_tzcnt_u64)
#pragma intrinsic(_lzcnt_u64)
static inline uint32_t binomial_ctz(uint64_t x) { return (uint32_t)_tzcnt_u64(x);  }
static inline uint32_t binomial_clz(uint64_t x) { return (uint32_t)_lzcnt_u64(x);  }
#endif
#endif


//────────────────────────────────────────────────────────────────────────────────────
// table based version

#if !defined(SFH_BINOMIAL_SMALL) || defined(BINOMIAL_TEST)

// binomial table:
// uses symmettry and easy from 'n' to reduce size.
// Sum[Max[Floor[n/2]-1,0], {n,0,67}] = 1056 (8.25K)
//   (sum bounds for clairity. some zero terms)
static uint64_t binomial_data[1056] = {0};

uint64_t binomial_table(uint64_t n, uint64_t k)
{
  // illegal 'n' silently returning 0
  if (n <= 67) {
    // rework these
    if (k > n)              return 0;
    if (k == 0 || k == n)   return 1;
    if (k == 1 || k == n-1) return (uint64_t)n;
    
    if (k > n/2) k = n-k;

    // k on [2,floor(n/2)] & [2,33] at max
    // n on [4,67] for all

    // compute index into reduced table
    uint64_t h  = n >> 1;
    uint64_t id = h*((n-1)>>1) - n+2;

    id  = hint_select_u64(n>=4, id, 0);
    id += k-2;
    
    return binomial_data[id];
  }

  return 0;
}

// warn at compile time if haven't found macro for hooking-up init time
#ifndef  register_init_time_function
#define  register_init_time_function(f) void f(void)
#warning "WARNING: 'binomial_init' not registered to run at init time. user must manually call"
#define NEEDS_USER_INIT
#endif


// initialization time function that builds
// the binomial data table. Pascal's rule
register_init_time_function(binomial_init_)
{
  uint64_t c0,c1;
  uint64_t id  = 0;

  binomial_data[id++] = 6;  // C(4,2)
  binomial_data[id++] = 10; // C(5,2)
  binomial_data[id++] = 15; // C(6,2)
  binomial_data[id++] = 20; // C(6,3)
  
  for (uint64_t n=7; n<=67; n++) {
    uint64_t e = n >> 1;

    c0 = n-1; // C(n-1,k-1) for k=2
    
    // C(n,k) = C(n-1,k-1) + C(n-1,k)
    hint_no_unroll
    for (uint64_t k=2; k<=e; k++) {
      // this could be faster but we only
      // reach here 1052 times: meh
      c1 = binomial_table(n-1, k);
      binomial_data[id++] = c0 + c1;
      c0 = c1;
    }
  }
}
#endif


void binomial_init(void)
{
#if defined(BINOMIAL_USER_INIT)  
  if (binomial_data[0] != 6)
    binomial_init_();
#endif
}


//────────────────────────────────────────────────────────────────────────────────────
// data reduced version. don't use. for entertainment purposes only.
// • non-overflow cases are textbook implementation (commented out) with
//   the divide replaced by multiply.
// • overflow cases have commented out "optimized" version of reference
//   but uses a table look-up.
// • total data size: 16+36 8 byte entries tables (416 bytes total)

#if defined(SFH_BINOMIAL_SMALL) || defined(BINOMIAL_TEST)

static inline uint64_t binomial_mulhi(uint64_t x, uint64_t y)
{
#if defined(_MSC_VER)
  return __umulh(x, y);
#else
  __uint128_t xl = x, yl = y;
  __uint128_t rl = xl * yl;
  return (uint64_t)(rl >> 64);
#endif
}

uint64_t binomial_small(uint64_t n, uint64_t k)
{  
  if ((k <= n) && (n <= 67)) {  
    if ((!k) || (k==n)) return 1;
    if (k > n/2) k = n-k; 
    
    // can we do the "easy" compute without spurious overflow?
    // It happens that detection of the overflow case is easy.
    // one input (63,28) falls out to the overflow case that 
    // wouldn't overflow: meh. 
    if (n < 63 || k+n < 91) {
      uint64_t r = n-k+1;
      uint64_t t = r;

      for (uint64_t i=2; i<=k; i++) {
        t++;
        r *= t;
#if 0
        // what we're doing below
        r  = r/i;
#else
        // "magic" constants for divide by 'i'. first entry is junk padding (16 entries)
        static const uint64_t m[] = {
          1,                  0x5555555555555556, 0x999999999999999a, 0x2492492492492493,
          0xc71c71c71c71c71d, 0x745d1745d1745d18, 0x3b13b13b13b13b14, 0x1111111111111112,
          0xe1e1e1e1e1e1e1e2, 0xaf286bca1af286bd, 0x8618618618618619, 0x642c8590b21642c9,
          0x47ae147ae147ae15, 0x2f684bda12f684be, 0x1a7b9611a7b9611b, 0x0842108421084211
        };

        // doing the "divide by multiply" the hard way so all are handled the same way
        uint32_t s = binomial_ctz(i);              // shift to make 'i' odd
        uint64_t o = i >> s;                       // 'i' reduced to odd
        uint64_t l = o >> 1;                       //   index to magic data
        uint32_t a = 64 - binomial_clz(l);         // compute the shift for the "divide by multiply"
        uint64_t q = binomial_mulhi(r,m[l]);       // high 64-bit result of product by magic
        uint64_t t = (((r-q)>>1)+q) >> a;          // complete the division by odd

        // if we divided by '1' then the result is garbage so use 'r' instead of 't'
        // and complete the divide by 'i' 
        r = hint_select_u64(o==1,r,t) >> s;
#endif
      }
      return r;
    }
    
#if 0
    // overflow case: This is basically the reference version with a
    // built-in GCD. Didn't bother removing the division (as above)
    // since moving from a 17 table entry to 36 entry one & directly
    // grabbing the result: no brainer.
    
    // mod inverse of odd integers on [1,33]. since the divisions are
    // exact we can multiply by the mod inverse instead of divide.
    // floor(67/2) = 33 : 17 entries
    static const uint64_t div[] = {0x0000000000000001, 0xaaaaaaaaaaaaaaab,
                                   0xcccccccccccccccd, 0x6db6db6db6db6db7,
                                   0x8e38e38e38e38e39, 0x2e8ba2e8ba2e8ba3,
                                   0x4ec4ec4ec4ec4ec5, 0xeeeeeeeeeeeeeeef,
                                   0xf0f0f0f0f0f0f0f1, 0x86bca1af286bca1b,
                                   0xcf3cf3cf3cf3cf3d, 0xd37a6f4de9bd37a7,
                                   0x8f5c28f5c28f5c29, 0x84bda12f684bda13,
                                   0x34f72c234f72c235, 0xef7bdef7bdef7bdf,
                                   0xf83e0f83e0f83e1};
    
    uint64_t r = 1;
    
    // same as above but divide through by the GCD first
    for (uint64_t i=1, t=n-k+1; i<=k; i++, t++) {
      uint64_t s = binomial_ctz(t|i);
      uint64_t u = t >> binomial_ctz(t);
      uint64_t v = i;
      
      // GCD: no zero inputs and produce GCD(t,i) = 2^s u
      // expected number of iterations:
      //   0.706*log2(67) = 6.06609 (actual is 6)
      // Could have done Eulid (modulo): needs 66 entry
      // table for naive. Performing exactly one would
      // need 5 and reduced expected iterations by 1.
      do {
        v >>= binomial_ctz(v);
        hint_cswap_u64(u>v,u,v);
        v = v - u;
      } while (v != 0);
      
      // u is odd: lose the bottom bit to half the
      // mod-inverse table size.
      u >>= 1;
      r /= (i>>s)*div[u];   // r /= i/GCD(t,i)
      r *= (t>>s)*div[u];   // r *= t/GCD(t,i)
    }
    
    return r;
#else
    // completely table drive all inputs that have spurious overflow.
    // data table is 36 entries (288 bytes). notice the compiler should
    // have already computed "k+n-91" to reach here: so a subtract and
    // two table lookups.
    static const uint64_t  d63[] = { 0x08bbc08e2bfcde3d, 0x0a8a52534f924a03,
                                     0x0bf2190915ea0f9d, 0x0cb764f927d82123 };
    static const uint64_t  d64[] = { 0x0bbfdbd8a3bb35c0, 0x0f86aba76aa51950,
                                     0x134612e17b8f2840, 0x167c6b5c657c59a0,
                                     0x18a97e023dc230c0, 0x196ec9f24fb04246 };
    static const uint64_t  d65[] = { 0x0de9f0223b27cd20, 0x14190586c7397da0,
                                     0x1b4687800e604f10, 0x22ccbe88e6344190,
                                     0x29c27e3de10b81e0, 0x2f25e95ea33e8a60, 
                                     0x321847f48d727306 };
    static const uint64_t  d66[] = { 0x0e8f00e1a1cc9c90, 0x16f5329ee19b45a8,
                                     0x2202f5a902614ac0, 0x2f5f8d06d599ccb0,
                                     0x3e134608f49490a0, 0x4c8f3cc6c73fc370, 
                                     0x58e8679c844a0c40, 0x613e315330b0fd66,
                                     0x64308fe91ae4e60c};
    static const uint64_t  d67[] = { 0x0d80a95b9d023c28, 0x173975372cc66778,
                                     0x258433808367e238, 0x38f82847e3fc9068,
                                     0x516282afd7fb1770, 0x6d72d30fca2e5d50, 
                                     0x8aa282cfbbd45410, 0xa577a4634b89cfb0,
                                     0xba2698efb4fb09a6, 0xc56ec13c4b95e372};
    
    static const uint64_t* data[] = {d63,d64,d65,d66,d67};

    return data[n-63][k+n-91];
#endif
  }
  if ((!k) || (k==n)) return 1;

  return 0;
}

#endif


//────────────────────────────────────────────────────────────────────────────────────

// direct to table based if running internal test
#ifdef  BINOMIAL_TEST
#define binomial_u64 binomial_table
#endif

// extend to signed 'n'. requires |n| <= 66 for no overflow
int64_t binomial_i64(int64_t n, int64_t k)
{
  if (k >= 0) {
    if (n >= 0) {
      if (n <= 66)
        return (int64_t)binomial_u64((uint64_t)n,(uint64_t)k);
      else
        return (int64_t)binomial_error;
    }

    // transform to standard
    uint64_t r = binomial_u64((uint64_t)(k-n-1),(uint64_t)k);
    uint64_t s = -(k & 1);
    
    return (int64_t)((r^s)-s);
  }

  return 0;
}



#if defined(BINOMIAL_TEST)

#include <stdio.h>
#include <inttypes.h>

static inline uint64_t binomial_gcd(uint64_t u, uint64_t v)
{
  uint32_t s = binomial_ctz(u|v);
  
  u >>= binomial_ctz(u);
  
  do {
    v >>= binomial_ctz(v);
    hint_cswap_u64(u>v,u,v);
    v = v - u;
  } while (v != 0);
  
  return u<<s;
}

// reference version is even smaller!
uint64_t binomial_ref(uint64_t n, uint64_t k)
{
  if (k > n)          return 0;
  if ((!k) || (k==n)) return 1;

  if (k > n/2)
    k = n - k;

  uint64_t r = 1;

  // textbook but performs GCD reductions at each step
  for (uint64_t i=1, t=n-k+1; i<=k; i++, t++) {
    uint64_t a = binomial_gcd(t,i);
    r /= i/a;
    r *= t/a;
  }
  
  return r;
}

int main(void)
{
  printf("test: binomial_u64 variants\n");

  if (binomial_data[0] != 6) {
    printf("  WARNING: autoinit didn't happen. initalizing.\n");
    binomial_init();
  }
  
  uint32_t serrors = 0;
  uint32_t terrors = 0;

  for(uint64_t n=0; n<=67; n++) {
    uint64_t e = n;
    
    for(uint64_t k=0; k<=e; k++) {
      // checking three implementations for agreement on all valid inputs seems sufficient.
      uint64_t r0 = binomial_ref(n,k);
      uint64_t r1 = binomial_small(n,k);
      uint64_t r2 = binomial_table(n,k);

      if ((r0 == r1) && (r0 == r2)) continue;

      if (r0 != r1) { serrors++; printf("  s %2u : (%2" PRIu64 ", %2" PRIu64 ") = %3" PRIu64 " : %" PRIu64 " %" PRIu64 "\n", serrors,n,k,n+k,r0,r1);}
      if (r0 != r2) { terrors++; printf("  t %2u : (%2" PRIu64 ", %2" PRIu64 ") = %3" PRIu64 " : %" PRIu64 " %" PRIu64 "\n", terrors,n,k,n+k,r0,r1);}
    }
  }

  if (serrors) printf("  small errors: %u\n", serrors);

  if (!terrors) {
    printf("  done\ntest: binomial_i64\n");
    uint32_t nerrors = 0;

    // run some positive results
    for(uint32_t n=1; n<=66; n++) {
      int64_t r0 = (int64_t)binomial_table(3+(uint64_t)n,(uint64_t)(n-1));
      int64_t r1 = binomial_i64(-((int64_t)n), 4);
      if (r0 == r1) continue;
      printf("  fail: (-%u,%2u) = %3i : got %3i\n",n,n-1,(int32_t)r0,(int32_t)r1);
      if (++nerrors > 33) { printf("  (bailing)"); break; }
    }

    nerrors = 0;
    
    // run some negative results
    for(uint32_t n=1; n<=66; n++) {
      int64_t r0 = -(int64_t)binomial_table(2+(uint64_t)n,(uint64_t)(n-1));
      int64_t r1 = binomial_i64(-((int64_t)n), 3);
      if (r0 == r1) continue;
      printf("  fail: (-%u,%2u) = %3i : got %3i\n",n,n-1,(int32_t)r0,(int32_t)r1);
      if (++nerrors > 33) { printf("  (bailing)"); break; }
    }
  }
  else
    printf("  table errors: %u (other routines testing skipped)\n", terrors);

  printf("  done\n");

  return 0;
}

#endif
