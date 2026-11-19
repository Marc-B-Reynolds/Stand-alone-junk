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

float libm(float x) { return 0; }


//────────────────────────────────────────────────────────────────────────────────────


//────────────────────────────────────────────────────────────────────────────────────
// SEE: https://core-math.gitlabpages.inria.fr for licence

#include "core_math_expand.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#pragma STDC FENV_ACCESS ON

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;

static __attribute__((noinline)) float as_special(float x){
  b32u32_u t = {.f = x};
  if(t.u==0xbf800000u){// +0.0
#ifdef CORE_MATH_SUPPORT_ERRNO
    errno = ERANGE;
#endif
    return -1.0f/0.0f; // to raise FE_DIVBYZERO
  }
  if(t.u == 0x7f800000u) return x; // +inf
  uint32_t ax = t.u<<1;
  if(ax > 0xff000000u) return x + x; // nan
#ifdef CORE_MATH_SUPPORT_ERRNO
  errno = EDOM;
#endif
  return 0.0f/0.0f; // to raise FE_INVALID
}

float cr_log1pf(float x) {
  // the reciprocal 1/(1+(j+0.5)/32) is rounded to 23 bits
  static const double x0[] = {
    0x1.f81f80p-1, 0x1.e9131cp-1, 0x1.dae608p-1, 0x1.cd8568p-1, 0x1.c0e070p-1, 0x1.b4e81cp-1,
    0x1.a98ef8p-1, 0x1.9ec8e8p-1, 0x1.948b10p-1, 0x1.8acb90p-1, 0x1.818180p-1, 0x1.78a4c8p-1,
    0x1.702e04p-1, 0x1.681680p-1, 0x1.605818p-1, 0x1.58ed24p-1, 0x1.51d080p-1, 0x1.4afd6cp-1,
    0x1.446f88p-1, 0x1.3e22ccp-1, 0x1.381380p-1, 0x1.323e34p-1, 0x1.2c9fb4p-1, 0x1.27350cp-1,
    0x1.21fb78p-1, 0x1.1cf06cp-1, 0x1.181180p-1, 0x1.135c80p-1, 0x1.0ecf58p-1, 0x1.0a6810p-1,
    0x1.0624dcp-1, 0x1.020408p-1};

  // the logarithm of the reciprocal is offset by 0x1.7654p-37 so log1p_fast(x) - log1p(x) > 0
  static const double lix[] = {
    0x1.fc0b0b1599ce4p-7, 0x1.77457a64a42abp-5, 0x1.341d74627847dp-4, 0x1.a926d8a568810p-4,
    0x1.0d77e8cd667aap-3, 0x1.44d2b38d15679p-3, 0x1.7ab886a16b2b3p-3, 0x1.af3c9b686996dp-3,
    0x1.e27075e30cc37p-3, 0x1.0a3250a767d98p-2, 0x1.229423bd2662ep-2, 0x1.3a64c596c3292p-2,
    0x1.51aadd530e505p-2, 0x1.686c85e9e0177p-2, 0x1.7eaf7df859caep-2, 0x1.94793ee2403b3p-2,
    0x1.a9cec5a9cf512p-2, 0x1.beb4d3baa086fp-2, 0x1.d32fe2a03d8b5p-2, 0x1.e744257d97431p-2,
    0x1.faf58cf7bdfe7p-2, 0x1.0723e6d1e5599p-1, 0x1.109f3b52ec2f3p-1, 0x1.19ee6a7693fc5p-1,
    0x1.23130d9c03597p-1, 0x1.2c0e9cc4604f1p-1, 0x1.34e28bd9e5837p-1, 0x1.3d9028a72cd5fp-1,
    0x1.4618b9c1dd52dp-1, 0x1.4e7d825b8d20bp-1, 0x1.56bf9fab56a02p-1, 0x1.5ee02ab258ccap-1};
  static const double b[] =
    {0x1p+0, -0x1p-1, 0x1.5555555556f6bp-2, -0x1.00000000029b9p-2,
     0x1.9999988d176e4p-3, -0x1.55555418889a7p-3, 0x1.24adeca50e2bcp-3, -0x1.001ba33bf57cfp-3};
  static const double c[] =
    {0x1.ffffffe1eac82p-1, -0x1.ffffff7da1724p-2, 0x1.5564d8fa59d0cp-2, -0x1.001219d3dba2ap-2};

  double z = x;
  b32u32_u t = {.f = x};
  uint32_t ux = t.u;
  if(__builtin_expect(ux>=0xbf800000u, 0)) return as_special(x); // x<=-1, x=-inf, x=-nan
  uint32_t ax = ux&(~0u>>1);
  if(__builtin_expect(ax>=0x7f800000u, 0)) return as_special(x); // x=+inf, x=+nan
  if(__builtin_expect(ax <0x3c880000u, 1)){ // |x| < 0x1.1p-6
    if(__builtin_expect(ax<0x33000000u, 0)){ // |x| < 0x1p-25
      if(!ax) return x;
      float res = __builtin_fmaf(x,-x,x);
#ifdef CORE_MATH_SUPPORT_ERRNO
      /* log1p(x) underflows exactly when |res| < 2^-126
	 and for x=-0x1.fffffcp-127 and rounding downwards */
      if(__builtin_expect(ax <= 0x00800000, 0)){
	if(__builtin_expect(ax == 0x00800000, 0)){
	  if(__builtin_fabsf(res) < 0x1p-126f ) errno = ERANGE; // underflow
	} else {
	  errno = ERANGE; // underflow
	}
      }
#endif
      return res;
    }
    double z2 = z*z, z4 = z2*z2;
    double f = z2*((b[1] + z*b[2]) + z2*(b[3] + z*b[4]) + z4*(b[5] + z*(b[6] + z*b[7])));
    b64u64_u r = {.f = z + f};
    if(__builtin_expect((r.u&0xfffffffll) == 0, 0)) r.f += 0x1p14*(f + (z - r.f));
    return r.f;
  } else {
    b64u64_u tp = {.f = z + 1.0};
    int e = (tp.u>>52)-0x3ff;
    uint64_t m52 = tp.u&(~0ull>>12);
    unsigned j = (tp.u >> (52-5))&31;
    b64u64_u xd = {.u = m52 | ((uint64_t)0x3ff<<52)};
    z = xd.f*x0[j] - 1; // z is exact for x<0x1.0cp+30
    const double ln2 = 0x1.62e42fefa39efp-1;
    double z2 = z*z, r = (ln2*e + lix[j]) + z*((c[0] + z*c[1]) + z2*(c[2] + z*c[3]));
    const double eps = 2.1555e-11;
    float ub = r, lb = r - eps;
    if(__builtin_expect(ub != lb, 0)){
      double z4 = z2*z2, f = z2*((b[1] + z*b[2]) + z2*(b[3] + z*b[4]) + z4*(b[5] + z*(b[6] + z*b[7])));
      double lj = lix[j] - 0x1.7654p-37; // subtract the offset
      const double ln2l = 0x1.7f7d1cf79abcap-20, ln2h = 0x1.62e4p-1;
      double Lh = ln2h * e, Ll = ln2l * e;
      Ll += z;
      double rh = Lh + lj, rl = ((Lh - rh) + lj) + (Ll + f);
      float fh = rh + rl;
      double Fl = (rh - (double)fh) + rl;
      float fl = Fl, tfl = fl*2.0f;
      if((fh + tfl)-fh == tfl)
	fl += __builtin_copysignf(0.5f,(float)(Fl-(double)fl))*__builtin_fabsf(fl);
      ub = fh + fl;
    }
    return ub;
  }
}

#pragma GCC diagnostic pop
//────────────────────────────────────────────────────────────────────────────────────


func_entry_t func_table[] =
{
  ENTRY(libm),
};

const char* func_name = "lol";

float cr_func(float x) { return 0.f; }

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
  //return test_run(argc, argv);
}
