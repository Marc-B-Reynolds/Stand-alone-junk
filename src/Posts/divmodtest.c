// -*- coding: utf-8 -*-

// Brute force validation for "Moving integer division to floating-point is trivial"
//   https://marc-b-reynolds.github.io/math/2026/08/10/IntDivByFP.html
//
// Checks a worst case a/b for every 'r' (power-of-two sizes of b).
// Exceptions are the last for each which set a=2^p-1 and b=2^p
// which forces p bits beyond the decimal. For p=24 r=23 and p=53
// r={51,52} meeting the requirements is impossible so I wing it.
// Otherwise as follows:
// • a: is odd so LSB is set
// • b: any divisor on range except power-of-two (the first of subset)
//      binary32 uses the second of the set because it works up to
//      r=21, r=22 just found by a search, and remaining noted above.
//      binary64 use the same scheme for r on [21,51]. For r on [1,20]
//      I use b with two bits set which keeps it's associate 'a' very
//      near the max 53 bit integer.
// • a & b are coprime and internal checking validates:
//   r + ceiling(log2(a/b)) = p
// • Checking requirements are met can be enabled by defining
//   'CHECK_TEST_PAIRS' (see below)
// • The fact these are hitting worst case input can be also validated
//   by defining 'BREAK_IT' (see below) which changes the rounding mode.

// define to check the test pairs for meeting requirements
//#define CHECK_TEST_PAIRS

// define this to make all the proper vectors fail. it changes
// the rounding mode so you'll probably have to pass '-frounding-math'
// or equivalent to make this work
//#define BREAK_IT

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <stdint.h>
#include <stdio.h>

#include <fenv.h>

#if defined(CHECK_TEST_PAIRS)


#if defined(__GNUC__) || defined(__clang__)
// GCC & clang will match to single op
static inline uint32_t clz_64(uint64_t x) { return (x!=0) ? (uint32_t)__builtin_clzl(x) : 64; }
static inline uint32_t ctz_64(uint64_t x) { return (x!=0) ? (uint32_t)__builtin_ctzl(x) : 64; }
#elif defined(_MSC_VER)
static inline uint32_t clz_64(uint64_t x) { return (uint32_t)__lzcnt64(x);  }
static inline uint32_t ctz_64(uint64_t x) { return (uint32_t)__tzcnt64(x);  }
#else
#error "fill in these two functions"
#endif

static inline uint32_t log2_ceiling(uint64_t x)
{
  return 64-clz_64(x);
}

uint64_t gcd(uint64_t u, uint64_t v)
{
  uint32_t s = ctz_64(u|v);
  uint64_t t;

  u >>= ctz_64(u);

  do { 
    t = v >> ctz_64(v);
    v = (u > t) ? (u - t) : (t - u);
    u = (u < t) ? u : t;
  } while (v);

  return u<<s;
}

// check that the test values are actually doing what
// they are supposed to.
// 0: a is even
// 1: a is illegal size
// S: quotient isn't full sized
// C: a & b aren't coprime
// L: significant digits of quotient isn't (p-r)
// R: mod isn't (b-1) so not a worst case
#define INTERNAL_CHECK()                       \
{                                              \
  if ((a&1)==0)               { printf("0"); } \
  if (a > m)                  { printf("1"); } \
  if (log2_ceiling(q)+r != p) { printf("S"); } \
  if (gcd(a,b) != 1)          { printf("C"); } \
  if (a%b != (b-1))           { printf("R"); } \
}

#else
#define INTERNAL_CHECK() do { (void)q; } while(0)
#endif

// verify:
// X: the division was correct
// L: the quotient has (p-r) significant digits
// R: remainder must be (b-1) 
#define DIV_CHECK()                              \
{                                                \
  if (Q != q)                 { printf("X"); }   \
}


// constructs a worst case a and b for every r
// and validates the binary32 computation returns
// same result as integer division.
//
// use the first odd divisor (second element of subset)
// for each 'r' and make the maximum possible
// numerator 'a' which is odd. This produces 'a'
// with bits 0 & 23 set.
//
// binary64 does a more complicated thing for the
// first part of its 'r' range. skipping that 
// here because it's no better.

// binary32 test vector
typedef struct {
  uint32_t a;
  uint32_t b;
} test_binary32_t;

void binary32_check_test(void)
{
  static const uint32_t p = 24;
  static const uint32_t m = (1u<<24);

  // build a test vector to handle the final few r values
  // in the same way
  test_binary32_t v[26];

  for(uint32_t r=1; r<22; r++) {
    uint32_t n = (1u << (p-r-1))+2;
    uint32_t b = (m+1)/n;
    uint32_t a = n*b-1;
  //uint32_t q = n-1;
    v[r].a = a;
    v[r].b = b;
  }

  v[22] = (test_binary32_t){.a=0xfffffb, .b=0x555554};  // meets requirements
  v[23] = (test_binary32_t){.a=0xffffff, .b=0xfffffd};  // can't meet (so wing it)
  v[24] = (test_binary32_t){.a=0xffffff, .b=0x1000000}; // just use b=2^24

  // perform the correctness test
  printf("binary32: check all magnitudes on [0, 2^24] (note no -1 on upper)\n");
  printf("┌──┬───────────────────────────────────┐\n");
  printf("│r │      a         b     a/b      FP  │\n");
  printf("├──┼───────────────────────────────────┤\n");

  for(uint32_t r=1; r<=24; r++) {
    uint32_t  b = v[r].b;
    uint32_t  a = v[r].a;
    uint32_t  q = a/b;

    float     A = (float)a;
    float     B = (float)b;
    uint32_t  Q = (uint32_t)(A/B);

    printf("│%2i│%7x / %7x = %7x %7x│ ", r,a,b,q,Q);
    DIV_CHECK();

#if defined(CHECK_TEST_PAIRS)
    INTERNAL_CHECK();
    if (r==23) { printf(" ← expected"); }
#endif    

    printf("\n");
  }

  printf("└──┴───────────────────────────────────┘\n\n");
}

// binary64 test vector
typedef struct {
  uint64_t a;
  uint64_t b;
} test_binary64_t;


// constructs worst case a and b for every r
void binary64_check_test(void)
{
  static const uint64_t p = 53;
  static const uint64_t m = (UINT64_C(1)<<53);

  // build the test vector here instead of mucking around multiple times
  test_binary64_t v[55];

  // the scheme for binary32 doesn't work for small 'r'
  // since 'p' is odd.
  for(uint32_t r=1; r<=20; r++) {
    // wtf am I doing here? I should have kept notes.
    uint64_t t = (m+1)/((UINT64_C(1) << r)+1);
    uint64_t n = (t & 1) ? t-1 : t;
    uint64_t b = (m+1)/n;
    uint64_t a = n*b-1;
  //uint64_t q = n-1;

    v[r].a = a;
    v[r].b = b;
  };

  // switch schemes for the remaining (except impossible cases)
  for(uint32_t r=21; r<52; r++) {
    uint64_t n = (1u << (p-r-1))+2;
    uint64_t b = (m+1)/n;
    uint64_t a = n*b-1;
  //uint64_t q = n-1;

    v[r].a = a;
    v[r].b = b;
  }

  v[51] = (test_binary64_t){.a=m-1, .b=0xaaaaaaaaaaaab};
  v[52] = (test_binary64_t){.a=m-1, .b=m-2};
  v[53] = (test_binary64_t){.a=m-1, .b=m};

  // start the actual test

  printf("binary64: check all magnitudes on [0,2^53] (note no -1 on upper)\n");
  printf("┌──┬─────────────────────────────────────────────────────────────┐\n");
  printf("│r │              a               b        a/b          FP       │\n");
  printf("├──┼─────────────────────────────────────────────────────────────┤\n");

  for(uint32_t r=1; r<=p; r++) {
    uint64_t a = v[r].a;
    uint64_t b = v[r].b;
    uint64_t q = a/b;

    double   A = (double)a;
    double   B = (double)b;
    uint64_t Q = (uint64_t)(A/B);

    printf("│%2i│%15lx /%14lx = %13lx %13lx│ ", r,a,b,q,Q);
    DIV_CHECK();

#if defined(CHECK_TEST_PAIRS)
    INTERNAL_CHECK();
    if (r==52 || r==51) { printf(" ← expected"); }
#endif    
    
    printf("\n");
  }

  printf("└──┴─────────────────────────────────────────────────────────────┘\n\n");
}

// too bad this doesn't do anything.
#pragma STDC FENV_ACCESS ON

int main(void)
{
#if defined(BREAK_IT)  
  fesetround(FE_UPWARD);
#endif  
  
  binary32_check_test();
  binary64_check_test();
  return 0;
}


