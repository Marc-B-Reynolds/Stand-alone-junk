// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

// atanpi(x) = atan(x)/π

#include "f32_utils.h"

//────────────────────────────────────────────────────────────────────────────────────
// atanpi(x) for x on [-1,1] (unit extent) specialized versions
//
// rational approximations for atanpi(x) for x on [-1,1]
//  
//   atan(x)/π ≈ x/4 + x P(x²)/Q(x²)
// 
// the peak value of atan(x)/π - x/4 occurs at x = sqrt((4-π)/π) ≈ 0.522723
// and its value is ≈ 0.02263649.
// 
// rational approximations can hit the full range without range reduction
// with low degree polynomials P & Q which can be computed in parallel.
// 

//    |ulp| correctly| faithful| >1 |%correctly|%faithful|max abs error|
// 1) |  1| 976353883| 88999334| 0  |91.646026 | 8.353974| 1.490116e-08|
// 2) | 31|   2046260|  3458308|many| 0.192073 | 0.324616| 1.937151e-07|
// 3) |  1|1025226194| 40127023| 0  |96.233454 | 3.766546| 1.490116e-08|


static inline float f32_atanpi_ue_k(float x2)
{
  // rational approximation: of atan(x)/(πx) - x/4
  static const float N[] = {0xc.26c30p-8f,-0x1.63129p-8f,-0x9.248c0p-8f,-0x1.9f247p-8f};
  static const float D[] = {0xb.1e2f8p-4f, 0x6.203fep-4f, 0x8.4d802p-8f};

  float n  = N[3];
  float d  = D[2];
  
  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}

// faithfully rounded.  SEE: table entry (1) for error information
float f32_atanpi_ue(float x2)
{
  return fmaf(x, f32_atanpi_ue_k(x*x), 0.25f*x);
}


// max rel error = 31 ulp (19 correct digits)
// max abs error = 1.937151e-07
float f32_atanpi_ue_a(float x)
{
  static const float N[] = {0x1.17cb8p-4f,-0xa.0fe9p-8f,-0x7.6ccfp-8f};
  static const float D[] = {0xf.a3ba5p-4f,0x2.a398cp-4f};

  float x2 = x*x;
  float n  = N[2];
  float d  = D[1];

  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[0]);
  d = fmaf(d, x2, 1.f);

  return fmaf(x,(n/d), 0.25f*x);
}


// same degree polynomials as `f32_atanpi_ue_k` but optimized for and working
// in doubles. 
double f32_atanpi_ue_ex(double x2)
{
  static const double N[] = {0xc.21f873a99eep-8,-0x1.5ba1886c85f3p-8,-0x9.24fd56d356cp-8,-0x1.a1599469c291cp-8 };
  static const double D[] = {0xb.19cd4548071dp-4,0x6.23ec81fac27b8p-4,0x8.5a5211bbc10b8p-8};

  double n = N[3];
  double d = D[2];

  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]);

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.f);
  d = fma(d, x2, D[0]);

  // fma(x, (n/d), 0.25*x) 
  return n/d;
}


//────────────────────────────────────────────────────────────────────────────────────
// full range single parameter


// binary32 kernel for faithfully rounding for x on [-tan(π/8),tan(π/8)]
static inline float f32_atanpi_k4_fr(double x, double a)
{
  // atanpi(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const double C[] = {-0x1.555453812e9b7p-2, 0x1.9924bc804e959p-3,
                             -0x1.1c3701132244fp-3, 0x1.49e167123486cp-4};

  static const double ipi = 0x1.45f306dc9c883p-2;  // 1/π

  double x2 = x*x;
  double r;
  
  r = C[3];
  r = fma(r,  x2, C[2]);
  r = fma(r,  x2, C[1]);
  r = fma(r,  x2, C[0]);
  r = fma(r*x,x2, x);
  r = fma(ipi, r, a);
  
  return (float)r;
}

// standard (full domain)
// ∙ max error = 1 ulp (faithfully rounded)
// ∙ a couple of places required extended precision so it's less work to
//   just promote to doubles for the whole computation. Specifically the
//   (x-1)/(x+1) range reduction and for small results the final few
//   computations in the kernel that work in 1/π blow up the number
//   of computations required.
float f32_atanpi(float X)
{
  uint32_t sx = f32_sign_bit(X);
  double   x  = (double)fabsf(X);
  double   a  = 0.f;
  
  // x > tan(3π/8)
  if (x > 2.414213657379150390625f) {
    a = 0.5;
    x = -(1.0/x);
  }
  // x > tan(π/8)
  else if (x > 0.4142135623730950f) {
    a = 0.25;
    x = (x-1.0)/(x+1.0);
  }

  x = f32_atanpi_k4_fr(x,a);
  
  return f32_mulsign((float)x,sx);
}



//────────────────────────────────────────────────────────────────────────────────────
// 


