// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#include "f32_utils.h"


// atan(x) : x on [-tan(π/8),tan(π/8)]
static inline float f32_atan_k(float x)
{
  // atan(x) ≈ x³ P(x²) + x
  static const float C[] = {-0x1.555454p-2f, 0x1.9924dap-3f, -0x1.1c380cp-3f, 0x1.49e74cp-4f};

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0])
  r = fmaf(r*x,x2, x);
  
  return r;
}

// faithfully rounded: max error = 1 ulp
static inline float f32_atan(float x)
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



