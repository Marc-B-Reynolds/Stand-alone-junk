// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#include "f32_utils.h"


// ∙
//────────────────────────────────────────────────────────────────────────────────────
// atan(x) for x on [-1,1] (unit extent) specialized versions
//
// SEE: f32_atapi.c unit extent notes

//    |ulp| correctly  | faithful| >1  |%correctly|%faithful |max abs error|
// 1) | 1 | 1064311582 | 1041635 |  0  | 99.902226| 0.097774 | 5.960464e-08|
// 2) | 27| 1043332699 | 6910022 |many |97.933031 | 0.648613 | 1.609325e-06|
// 3) | 1 | 1064938767 |  414450 |  0  | 99.961097| 0.038903 | 5.960464e-08|

// compute: P(x²)/Q(x²) 
static inline float f32_atan_ue_k(float x2)
{
  // rational approximation: of atan(x)/x - x
  static const float N[] = {-0xd.459ab8p-44f,-0x3.97c714p-4f,-0x3.2d77ep-4f,-0x7.a811dp-8f};
  static const float D[] = {0xa.c7554p-4f,0x6.6a9d1p-4f,0x9.576e8p-8f};

  float n = N[3];
  float d = D[2];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}

// atan(x) for x on [-1,1] (unit extent)
float f32_atan_ue(float x)
{
  // x P(x²)/Q(x²) + x
  return fmaf(x, f32_atan_ue_k(x*x), x);
}


float f32_atan_ue_a(float x)
{
  static const float N[] = {-0x3.ac262cp-36f,-0x5.367ca8p-4f,-0x2.351a5p-4f};
  static const float D[] = {0xf.a37ep-4f,0x2.f0444p-4f};
  float x2 = x*x;

  float n = N[2];
  float d = D[1];

  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x,(n/d),x);
}

// Same degree polynomials as `f32_atan_ue` just everything is in doubles.
// The raison d'etre for this kernel is that it's about the same speed as
// the default and the extended xxxxx
// SEE: table entry 3
double f32_atan_ue_dk(double x2)
{
  static const double N[] = {-0xb.65d3cb953908p-48,-0x3.975debc74bb1p-4,
                             -0x3.2db6edaee4d64p-4,-0x7.aaf3b0c3bf22p-8};
  static const double D[] = { 0xa.c619c60a22078p-4, 0x6.6badb3354c53p-4,
                              0x9.5b472df81dfdp-8};
  double n = N[3];
  double d = D[2];

  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]);

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.0); 
  d = fma(d, x2, D[0]);

  // (float)fma(x,(n/d),x);
  return (n/d);
}


//────────────────────────────────────────────────────────────────────────────────────


// atan(x) : x on [-tan(π/8),tan(π/8)]
// ∙ internal kernel
static inline float f32_atan_k(float x)
{
  // atan(x) ≈ x³ P(x²) + x
  static const float C[] = {-0x1.555454p-2f, 0x1.9924dap-3f, -0x1.1c380cp-3f, 0x1.49e74cp-4f};

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0]);
  r = fmaf(r*x,x2, x);
  
  return r;
}

// standard (full domain)
// ∙ max error = 1 ulp (faithfully rounded)
float f32_atan(float x)
{
  static const float pi_hi     =  3.1415927410125732421875f;
  static const float pi_lo     = -8.74227765734758577309548854827880859375e-8f;
  static const float tan_3pi_8 =  2.414213657379150390625f;
  static const float tan_pi_8  =  0.4142135623730950f;
  
  uint32_t sx = f32_sign_bit(x);
  float    lo = 0.f;
  float    hi = 0.f;
  
  x = fabsf(x);

  // x > tan(3π/8)
  if (x > tan_3pi_8) {
    hi = pi_hi/2.f;
    lo = pi_lo/2.f;
    x  = -(1.f/x);
  }
  // x > tan(π/8)
  else if (x > tan_pi_8) {
    hi = pi_hi/4.f;
    lo = pi_lo/4.f;
    x  = (float)(((double)x-1.0)/((double)x+1.0));
  }

  float r = hi + (f32_atan_k(x) + lo);
  
  return f32_mulsign(r,sx);
}



