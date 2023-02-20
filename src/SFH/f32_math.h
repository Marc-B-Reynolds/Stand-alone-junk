// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2023

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

static inline void f32_quadratic(f32_pair_t* r, float a, float b, float c)
{
  float t0 = f32_sqrt(f32_mms(b,b,4.f*a,c));
  float t1 = b+copysignf(t0,b);

  r->h = t1/(-2.f*a);
  r->l = (-2.f*c)/t1;
}

#endif

