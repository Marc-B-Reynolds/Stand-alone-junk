// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#ifndef F32_ACOSPI_SB_H
#define F32_ACOSPI_SB_H

// fast branchfree acospi using non-standard approximation method
// based on being able to compute a square root in parallel with
// a polynomial. the construction puts some limitiations on
// accuracy at the top end.
//
// SEE:
// * (put post link here)
//
// core approximation on [0,1] using expressions:
//    acospi(x) ~= sqrt(1-x) P(x), x >= 0
//               = 1 - acospi(-x), x <  0
//
// * defines sets of polynomial approximations P(x)
//     f32_acospi_sb_kr{n} = n^th degree where P(0) ~= 1/2  {abs error}
//     f32_acospi_sb_ke{n} = n^th degree where P(0)  = 1/2  {abs error}
//     f32_acospi_sb_re{n} = n^th degree where P(0)  = 1/2  {rel error}
//   the P(0) ~= 1/2 has slightly lower error with the one more
//   degree of freedom and the P(0)=1/2 is exact for inputs
//   at (and near) zero. This make a huge difference on total
//   counts (which can be misleading is not mentally noted)
// * defines functions for expanding the P(x) into usable
//   functions:
//     f32_acospi_sb_xp   = expand an approximation for positive inputs
//     f32_acospi_sb_xf   = expand an approximation for full range
//     f32_acopis_bs_xf_l =   alternate full range (see below)
// * also promote-to-double flavors of some of the previous

// NOTES:
//   hybrid (stdfunc/bitops) only reduction. see f32_reduce_odd

//**********************************************************************
// f(0) relaxed spitball polynomials: P(x)
// not forcing result of f(0) to 1/2 gives an additional degree of
// freedom and produces slightly more accurate result.
// some of these should probably be nuked

static inline float f32_acospi_sb_kr1(float x)
{
  static const float C[] = {-0x1.b6bfa6p-5f, 0x1.fef466p-2f };
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sb_kr2(float x)
{
  static const float C[] = { 0x1.0c014ep-6f, -0x1.0bed48p-4f, 0x1.ffe4cap-2f };
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sb_kr3(float x)
{
  static const float C[] = { -0x1.b3d094p-8f, 0x1.9107f6p-6f, -0x1.158bb6p-4f, 0x1.fffcd4p-2f };
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sb_kr4(float x)
{
  static const float C[] = { 0x1.961c7cp-9f, -0x1.88640ap-7f, 0x1.bea05ep-6f, -0x1.1760fcp-4f, 0x1.ffff9ap-2f };
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_kr5(float x)
{
  static const float C[] = {
    -0x1.9989cep-10f, 0x1.adfc26p-8f, -0x1.def6dep-7f,
     0x1.cbd07cp-6f, -0x1.17b844p-4f, 0x1.fffff2p-2f };
  
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sb_kr6(float x)
{
  static const float C[] = {
    0x1.b354bcp-11f, -0x1.f4673p-9f, 0x1.2176c4p-7f, -0x1.008af8p-6f,
    0x1.cf49aap-6f, -0x1.17c876p-4, 0x1.fffffep-2f };
  
  return f32_horner_6(x,C);
}


//**********************************************************************
// f(0) exact spitball polynomials: P(x)  { minimized for abs error }

static inline float f32_acospi_sb_ke1(float x)
{
  static const float C[] = {-0x1.c95b42p-5f, 0.5f};
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sb_ke2(float x)
{
  static const float C[] = { 0x1.16bf4p-6f, -0x1.0e77f6p-4f, 0.5f};
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sb_ke3(float x)
{
  static const float C[] = {-0x1.c374b8p-8f, 0x1.96b008p-6f, -0x1.161a08p-4f, 0.5f};
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sb_ke4(float x)
{
  static const float C[] = {0x1.a30de4p-9f,-0x1.8eb0fp-7f,0x1.c09deap-6f,-0x1.177d92p-4f, 0.5f};
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_ke5(float x)
{
  static const float C[] = {
    -0x1.a58d5ep-10f, 0x1.b5572ep-8f, -0x1.e22fap-7f, 0x1.cc6afp-6f, -0x1.17bdf6p-4f, 0.5f
  };
  
  return f32_horner_5(x,C);
}


//**********************************************************************
// f(0) exact spitball polynomials: P(x) { minimized for relative error }

static inline float f32_acospi_sb_re4(float x)
{
  static const float C[] =
    {0x1.732054p-9f, -0x1.7ac4b8p-7f, 0x1.bbbd74p-6f, -0x1.1753ap-4f, 0.5f };
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_re5(float x)
{
  static const float C[] =
    { -0x1.729abcp-10f, 0x1.99a6f8p-8f, -0x1.d7f70ep-7f, 0x1.caf59ap-6f, -0x1.17b5e6p-4, 0.5f };
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sb_re6(float x)
{
  static const float C[] =
    { 0x1.87057p-11f, -0x1.d77f8p-9f, 0x1.1a9f46p-7f, -0x1.fe45dep-7f, 0x1.cf0faap-6f, -0x1.17c80ep-4f, 0.5f };
  return f32_horner_6(x,C);
}


//**********************************************************************
// binary64 kernels (for binary32 results) both are exact at f(0)

#ifndef   F64_HORNER2
#include "f64_horner2.h"
#endif

static inline double f32_acospi_dp_7(double x)
{
  static const double C[] = {
    -0x1.a25e75102a784p-12,
     0x1.14f922ea95169p-9,
    -0x1.6392dee5c3e07p-8,
     0x1.41e900ddb9fbep-7,
    -0x1.059da0937915ep-6,
     0x1.d008947285f77p-6,
    -0x1.17cb0f25e856bp-4,
     0x1.ffffff82e9620p-2
  };
  // order-2 horner
  return f64_horner2_7(x,C);
}

static inline double f32_acospi_dp_8(double x)
{
  static const double C[] = {
     0x1.d96812d527187p-13,
    -0x1.53ee1c1b10406p-10,
     0x1.d2e6213801f55p-9,
    -0x1.b349d19615f9ap-8,
     0x1.546a48855e022p-7,
    -0x1.07f1875d82e60p-6,
     0x1.d051fb78ab1c9p-6,
    -0x1.17cbeaa7bc3e2p-4,
     0x1.ffffffede992fp-2
  };

  // order-2 horner
  return f64_horner2_8(x,C);
}

//**********************************************************************


// given f = P(x) expand approximation restricted to positive input.
// return NaN for negative inputs.
static inline float f32_acospi_sb_xp(float (*f)(float), float x)
{
  float t = f32_sqrt(1.f-x);
  float p = f(x);

  return t*p;
}

static inline float f32_acospi_d_xp(double (*f)(double), float x)
{
  double v = (double)x;
  double t = f64_sqrt(1.0-v);
  double p = f(x);

  return (float)(t*p);
}

// compiler hand holding to keep it branchfree. placeholder for being
// to make it easy to flip a compile time switch if the compiler + arch
// is doing better than the work around.
//
// WARNING: f32_acospi_c/f32_acospi_cw & f32_acospi_sx need to work
// together to handle -0 (acutally both zeroes) inputs correctly. Using:
//   1 - acospi(|x|) 
//
// so for x=-0 we need either (sx,c) = {set,1} or {clear,0} (same for +0)

#define F32_ACOS_SB_BITOPS

static inline float f32_acospi_sx(float x, float a)
{
#if defined(F32_ACOS_SB_BITOPS)
  return f32_xor(x,a);
#else
  a=a;
  return (x < 0.f) ? -0.f : 0.f;
#endif  
}

static inline double f32_acospi_sxw(double x, double a)
{
#if defined(F32_ACOS_SB_BITOPS)
  return f64_xor(x,a);
#else
  a=a;
  return (x < 0.0) ? -0.0 : 0.0;
#endif  
}

static inline float f32_acospi_c(float x)
{
#if defined(F32_ACOS_SB_BITOPS)
  uint32_t m = sgn_mask_u32(f32_to_bits(x));
  return f32_from_bits(m & f32_to_bits(1.f));
#else
  return (x < 0.f) ? 1.f : 0.f; 
#endif  
}

static inline double f32_acospi_cw(float x)
{
#if defined(F32_ACOS_SB_BITOPS)
  uint64_t m = f32_sign_mask_u64(x);
  return f64_from_bits(m & f64_to_bits(1.0));
#else
  return (x < 0.f) ? 1.0 : 0.0; 
#endif  
}



// given f = P(x) expand full range approximation
static inline float f32_acospi_sb_xf(float (*f)(float), float x)
{
  float a  = fabsf(x);
  float sx = f32_acospi_sx(x,a);
  float c  = f32_acospi_c(x);
  float t  = f32_sqrt(1.f-a);
  float p  = f(a);
  
  // combine the subresults: here the sign is being applied the polynomial
  // result 'p' but could instead be applied to sqrt result 't'. ideally
  // it would applied to the sub-result that's expected to complete first.
  // roughly very low degree P apply to 'p', otherwise to 't' (as below)

  return fmaf(t, f32_xor(p,sx), c);  
}

// longer polynomial version (see note above)
static inline float f32_acospi_sb_xf_l(float (*f)(float), float x)
{
  float a  = fabsf(x);
  float sx = f32_acospi_sx(x,a);
  float c  = f32_acospi_c(x);
  float t  = f32_sqrt(1.f-a);
  float p  = f(a);
  
  return fmaf(f32_xor(t,sx), p, c);  
}


// full range, branchfree double promote expansions 
static inline float f32_acospi_d_xf(double (*P)(double), float v)
{
  double x  = (double)v;
  double a  = fabs(x);
  double sx = f32_acospi_sxw(x,a);
  double c  = f32_acospi_cw(v);
  double t  = f64_sqrt(1.0-a);
  double p  = P(a);

  return (float)fma(f64_xor(t,sx), p, c);
}



#endif
