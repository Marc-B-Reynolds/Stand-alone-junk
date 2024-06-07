// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2024

// WIP: tons of stuff to be minorly cleaned up before a proper push

#ifndef F32_MATH
#define F32_MATH

#ifndef   BITOPS_H
#include "bitops.h"
#endif

#ifndef   F32_UTIL_H
#include "f32_util.h"
#endif


extern float f32_rsqrt_hq(float x);
extern float f32_rsqrt_cr(float x);

extern float f32_rcbrt(float x);
extern float f32_rcbrt_fr(float x);

extern float f32_cbrt(float x);
extern float f32_cbrt_fr(float x);

//*************************************************************************
// solve quadratic equation ax^2+bx+c for real roots
// complex roots will yield a standard NaN

// larger magnitude root
static inline float f32_quadratic_max(float a, float b, float c)
{
  float t0 = f32_sqrt(f32_mms(b,b,4.f*a,c));
  float t1 = b+copysignf(t0,b);
  return t1/(-2.f*a);
}

// smaller magnitude root
static inline float f32_quadratic_min(float a, float b, float c)
{
  float t0 = f32_sqrt(f32_mms(b,b,4.f*a,c));
  float t1 = b+copysignf(t0,b);
  return (-2.f*c)/t1;
}

static inline f32_pair_t f32_quadratic(float a, float b, float c)
{
  float t0 = f32_sqrt(f32_mms(b,b,4.f*a,c));
  float t1 = b+copysignf(t0,b);

  f32_pair_t r;

  r.h = t1/(-2.f*a);
  r.l = (-2.f*c)/t1;

  return r;
}

static inline f32_pair_t f32_quadratic_hq(float A, float B, float C)
{
  double a = (double)A;
  double b = (double)B;
  double c = (double)C;

  // compute the sqrt of the discriminant. for finite
  // input b^2 & 4ac cannot overflow nor underflow due
  // to the wider dynamic range of binary64. Both terms
  // are exact, can contain no more than 48 binary digits
  // and therefore both have at least 5 trailing zeroes.
  // catastrophic cancellation cannot occur since both
  // are exact. since b^2 cannot over/underflow:
  // d = |b| exactly if either a or c is zero.
  // the FMA usage is solely for performance reasons
  // aside the effective sum/diff will be exactly
  // when b^2 and -4ac are close by exponents due
  // to the 5 trialing zeros..can't be bother to
  // come up with a bound since it does seem useful.

  double d = fma(b,b,-4.0*a*c);
  
  d = sqrt(d);
  
  // the pair of roots could be computed using the
  // textbook equation since -b +/- sqrt(bb-4ac)
  // cannot hit catastrophic cancellation. b has
  // at most 24 binary digits and root term up to
  // 53 so any rounding errors cannot become
  // significant WRT the final 24 bit result. This
  // would only be useful, however, if we additionally
  // eliminate the pair of divisions in favor of
  // multipling by the reciprocal which would add
  // one more rounding step (almost no impact on
  // result since still in binary64) BUT we'd also
  // need to handle the a=0 case explictly (via
  // two cases) instead of the chosen method below
  // which can (should) lower into a conditional
  // move. judgement call which is better.
  double t  = b + copysign(d,b+0.f);

  f32_pair_t r;

  r.l = (float)( (-2.0*c) / t );
  r.h = (float)( t / (-2.0*a) );

  // a = 0 case: second root is duplicate of first
  // but evaluates to NaN. copy to meet contract
  if (a == 0.0) r.h = r.l;
  
  return r;
}


#endif

