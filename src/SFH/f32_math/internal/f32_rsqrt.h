// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#ifndef F32_RSQRT_H
#define F32_RSQRT_H

// The "best" simple way to compute 1/sqrt(x) is sqrt(1/x) which is
// faithfully rounded (f32_rsqrt in f32_util.h).
// 
//   correctly:   86.991060% (14594678)
//   faithfully:  13.008940% (2182538)
//
// SEE:
// * https://gist.github.com/Marc-B-Reynolds/9fb24a7a4ee915e6e973bf9f4d08c404
// * "Fast Compensated Algorithms for the Reciprocal Square Root,
//   the Reciprocal Hypotenuse, and Givens Rotations", Carlos F. Borges, 2021
//   https://arxiv.org/abs/2103.08694
// * "High-level algorithms for correctly-rounded reciprocal square roots",
//   Borges, Jeannerod & Muller, 2022
//   https://hal.inria.fr/hal-03728088

// Newton-Raphson step for 1/sqrt(x) using FMA when
//   x = input to function
//   h = -x/2
//   r = RN(1/x)
static inline float f32_rsqrt_nr_step(float x, float h, float r)
{
  float s = fmaf(h,r,0.5f);
  float t = fmaf(x,x, -r);
  float v = fmaf(h,t,  s);
  return fmaf(x,v,x);
}

// Halley method step for 1/sqrt(x)
//   x = input to function
//   h = -x/2
//   r = RN(1/x)
static inline float f32_rsqrt_hm_step(float x, float h, float r)
{
  float s = fmaf(r,h,0.5f);
  float t = fmaf(x,x,-r);
  float v = fmaf(h,t, s);
  float w = fmaf(1.5f*v,v,v);
  return fmaf(x,w,x);
}

// correctly:   99.999994% (16777215)
// faithfully:   0.000006% (1)
// Borges Algorithm 2 (rsqrtNewton in Borges/Jeannerod/Muller)
// +4 fma & +1 product vs. standard
static inline float f32_rsqrt_hq(float x)
{
  float r = 1.f/x;
  float h = -0.5f*x;

  x = f32_sqrt(r);
  x = f32_rsqrt_nr_step(x,h,r);

  return x;
}

// correctly rounded (in round to even/ties to nearest)
// Borges Algorithm 3 (rsqrtHalley in Borges/Jeannerod/Muller)
// +5 fma & +1 product vs. standard
// DOUBLE PROMOTION WORKS HERE! (correct this oversight)
// BUT FOR A NARROWER RANGE...find it.
static inline float f32_rsqrt_cb(float x)
{
  float r = 1.f/x;
  float h = -0.5f*x;

  x = f32_sqrt(r);
  x = f32_rsqrt_hm_step(x,h,r); 

  return x;
}

#endif
