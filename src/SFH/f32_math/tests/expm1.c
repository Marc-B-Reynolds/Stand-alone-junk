// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
//
//

#if defined(__linux__) || defined(__unix__)
// for thread affinity
#define _GNU_SOURCE 
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "util.h"


// testing test uniform range
//#define SAMPLE_X0 -1.f
//#define SAMPLE_X1  1.f


//────────────────────────────────────────────────────────────────────────────────────

float libm(float x) { return expm1f(x); }


//────────────────────────────────────────────────────────────────────────────────────


//────────────────────────────────────────────────────────────────────────────────────
// SEE: https://core-math.gitlabpages.inria.fr for licence

#include "core_math_expand.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#pragma STDC FENV_ACCESS ON

/* __builtin_roundeven was introduced in gcc 10:
   https://gcc.gnu.org/gcc-10/changes.html,
   and in clang 17 */
#if ((defined(__GNUC__) && __GNUC__ >= 10) || (defined(__clang__) && __clang_major__ >= 17)) && !defined(_MSC_VER) && (defined(__aarch64__) || defined(__x86_64__) || defined(__i386__))
# define roundeven_finite(x) __builtin_roundeven (x)
#else
/* round x to nearest integer, breaking ties to even */
static double
roundeven_finite (double x)
{
  double ix;
# if (defined(__GNUC__) || defined(__clang__)) && (defined(__AVX__) || defined(__SSE4_1__) || (__ARM_ARCH >= 8))
#  if defined __AVX__
   __asm__("vroundsd $0x8,%1,%1,%0":"=x"(ix):"x"(x));
#  elif __ARM_ARCH >= 8
   __asm__ ("frintn %d0, %d1":"=w"(ix):"w"(x));
#  else /* __SSE4_1__ */
   __asm__("roundsd $0x8,%1,%0":"=x"(ix):"x"(x));
#  endif
# else
  ix = __builtin_round (x); /* nearest, away from 0 */
  if (__builtin_fabs (ix - x) == 0.5)
  {
    /* if ix is odd, we should return ix-1 if x>0, and ix+1 if x<0 */
    union { double f; uint64_t n; } u, v;
    u.f = ix;
    v.f = ix - __builtin_copysign (1.0, x);
    /* Warning: v.n is 0 when x=0.5; while u.n cannot be zero since ix
       is rounded away from zero. */
    if (v.n == 0 || __builtin_ctzll (v.n) > __builtin_ctzll (u.n))
      ix = v.f;
  }
# endif
  return ix;
}
#endif

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;

float cr_expm1f(float x){
  static const double c[] =
    {1, 0x1.62e42fef4c4e7p-6, 0x1.ebfd1b232f475p-13, 0x1.c6b19384ecd93p-20};
  static const double ch[] =
    {0x1.62e42fefa39efp-6, 0x1.ebfbdff82c58fp-13, 0x1.c6b08d702e0edp-20, 0x1.3b2ab6fb92e5ep-27,
     0x1.5d886e6d54203p-35, 0x1.430976b8ce6efp-43};
  static const double td[] =
    {0x1p+0, 0x1.059b0d3158574p+0, 0x1.0b5586cf9890fp+0, 0x1.11301d0125b51p+0,
     0x1.172b83c7d517bp+0, 0x1.1d4873168b9aap+0, 0x1.2387a6e756238p+0, 0x1.29e9df51fdee1p+0,
     0x1.306fe0a31b715p+0, 0x1.371a7373aa9cbp+0, 0x1.3dea64c123422p+0, 0x1.44e086061892dp+0,
     0x1.4bfdad5362a27p+0, 0x1.5342b569d4f82p+0, 0x1.5ab07dd485429p+0, 0x1.6247eb03a5585p+0,
     0x1.6a09e667f3bcdp+0, 0x1.71f75e8ec5f74p+0, 0x1.7a11473eb0187p+0, 0x1.82589994cce13p+0,
     0x1.8ace5422aa0dbp+0, 0x1.93737b0cdc5e5p+0, 0x1.9c49182a3f09p+0, 0x1.a5503b23e255dp+0,
     0x1.ae89f995ad3adp+0, 0x1.b7f76f2fb5e47p+0, 0x1.c199bdd85529cp+0, 0x1.cb720dcef9069p+0,
     0x1.d5818dcfba487p+0, 0x1.dfc97337b9b5fp+0, 0x1.ea4afa2a490dap+0, 0x1.f50765b6e454p+0};
  const double iln2 = 0x1.71547652b82fep+5, big = 0x1.8p52;
  b32u32_u t = {.f = x};
  double z = x;
  uint32_t ux = t.u, ax = ux<<1;
  if(__builtin_expect(ax<0x7c400000u,1)){ // |x| < 0.15625
    if(__builtin_expect(ax<0x676a09e8u, 0)){ // |x| < 0x1.6a09e8p-24
      if(__builtin_expect(ax==0x0u, 0)) return x; // x = +-0
      float res = __builtin_fmaf(__builtin_fabsf(x),0x1p-25f,x);
#ifdef CORE_MATH_SUPPORT_ERRNO
      // expm1(x) underflows for |x| < 2^-126, and for x=-0x1p-126 and RNDZ
      if (__builtin_fabsf (x) < 0x1p-126f || __builtin_fabsf (res) < 0x1p-126f)
        errno = ERANGE; // underflow
#endif
      return res;
    }
    static const double b[] =
      {0x1.fffffffffffc2p-2, 0x1.55555555555fep-3, 0x1.555555559767fp-5, 0x1.1111111098dc1p-7,
       0x1.6c16bca988aa9p-10, 0x1.a01a07658483fp-13, 0x1.a05b04d2c3503p-16, 0x1.71de3a960b5e3p-19};
    double z2 = z*z, z4 = z2*z2;
    double r = z + z2*((b[0]+z*b[1]) + z2*(b[2]+z*b[3]) + z4*((b[4]+z*b[5]) + z2*(b[6]+z*b[7])));
    return r;
  }
  if(__builtin_expect(ax>=0x8562e430u, 0)){  // |x| >= 0x1.62e43p+6 ~ 88.72
    if(ax>(0xffu<<24)) return x + x; // nan
    if(__builtin_expect(ux>>31, 0)){ // x < 0
      if(ax==(0xffu<<24)) return -1.0f;
      /* The expression -1.0f + 0x1p-26f should be evaluated at run
         time, with the current rounding mode, and not constant folded at
         compile time. See the comment about -frounding-math in README.
         Also, the expression -1.0f + 0x1p-26f is valid on a larger range:
         for x < -0x1.154244p+4 for rndn, and for x < -0x1.0a2b22p+4 for
         directed roundings. But this would need a different test for
         x > 0 and x < 0. */
      return -1.0f + 0x1p-26f;
    }
    if(ax==(0xffu<<24)) return x * x; // +/-inf
#ifdef CORE_MATH_SUPPORT_ERRNO
    errno = ERANGE;
#endif
    return 0x1.fffffep127f * x;
  }
  double a = iln2*z, ia = roundeven_finite(a), h = a - ia, h2 = h*h;
  b64u64_u u = {.f = ia + big};
  double c2 = c[2] + h*c[3], c0 = c[0] + h*c[1];
  const uint64_t *tdl = (uint64_t *)((void*)td);
  b64u64_u sv = {.u = tdl[u.u&0x1f] + ((u.u>>5)<<52)};
  double r = (c0 + h2*c2)*sv.f - 1.0;
  float ub = r, lb = r - sv.f*0x1.3b3p-33;
  if(__builtin_expect(ub != lb, 0)){
    if(__builtin_expect(ux>0xc18aa123u, 0)) // x < -17.32
      return -1.0f + 0x1p-26f;
    const double iln2h = 0x1.7154765p+5, iln2l = 0x1.5c17f0bbbe88p-26;
    double s = sv.f;
    h = (iln2h*z - ia) + iln2l*z;
    h2 = h*h;
    double w = s*h;
    r = (s-1) + w*((ch[0] + h*ch[1]) + h2*((ch[2] + h*ch[3]) + h2*(ch[4] + h*ch[5])));
    ub = r;
  }
  return ub;
}

#pragma GCC diagnostic pop
//────────────────────────────────────────────────────────────────────────────────────


func_entry_t func_table[] =
{
  ENTRY(libm),
};

const char* func_name = "expm1";

float cr_func(float x) { return cr_expm1f(x); }

#include "common.h"


//────────────────────────────────────────────────────────────────────────────────────

// move to common.h

void test_spot(void)
{
}

void test_all(void)
{
}

void test_sanity(void)
{
}

int main(int argc, char** argv)
{
  printf("empty stub\n");
  return test_run(argc, argv);
}
