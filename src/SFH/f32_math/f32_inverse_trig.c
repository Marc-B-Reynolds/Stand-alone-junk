// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#include "f32_utils.h"


//────────────────────────────────────────────────────────────────────────────────────
// atan(x) and atanpi(x) for x on [-1,1] (unit extent) specialized versions
// where atanpi(x) = atan(x)/π
// 
// rational approximations for atan/atanpi(x) for x on [-1,1]
//  
//   atan(x)   ≈ x   + x P(x²)/Q(x²)
//   atan(x)/π ≈ x/4 + x P(x²)/Q(x²)
// 
// the peak value of atan(x)/π - x/4 occurs at x = sqrt((4-π)/π) ≈ 0.522723
// and its value is ≈ 0.02263649.
// 
// rational approximations can hit the full range without range reduction
// with low degree polynomials P & Q which can be computed in parallel.
// Very SIMD friendly but likely to be a scalar win as well. Compare to scalar
// full range single parameter. There the larger range transform
// (|x| > tan(3π/8)) can be dropped but the other requires a division
// and a 5 term polynomial evaluation. It also needs to be performed
// in double precision for reasons noted below.
//
// unit extent error table:
// 
// atan:
//    |ulp|correctly | faithful| >1 |%correctly|%faithful|max abs error|
// 1) |  1|1064307333| 1045884 |  0 | 99.901827| 0.098173| 5.960464e-08|
// 2) |  3|1058549070| 6388298 |many| 99.361325| 0.599641| 1.788139e-07|
// 3) | 27|1043386989| 6725214 |many| 97.938127| 0.631266| 1.609325e-06|
// 4) |  1|1065142796|  210421 |  0 | 99.980249| 0.019751| 5.960464e-08|
//
// atanpi:
//    |ulp| correctly| faithful| >1 |%correctly|%faithful|max abs error|
// 1) |  1| 976353883| 88999334|  0 | 91.646026| 8.353974| 1.490116e-08|
// 2) |  3|  35877683|702388931|many|  3.367680|65.930146| 2.980232e-08|
// 3) | 31|   2046260|  3458308|many|  0.192073| 0.324616| 1.937151e-07|


// compute: the tan P(x²)/Q(x²) 
static inline float f32_atan_ue_k(float x2)
{
  static const float N[] = {-0x3.96927p-4f,-0x3.2e30f0p-4f,-0x7.b089ap-8f};
  static const float D[] = { 0xa.c3b75p-4,  0x6.6dbda8p-4f, 0x9.62bc0p-8f};

  float n = N[2];
  float d = D[2];

  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]) * x2;

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  // to complete x*(n/d)+x
  return n/d;
}

// compute: the tanpi P(x²)/Q(x²) 
static inline float f32_atanpi_ue_k(float x2)
{
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

  // to complete x*(n/d)+x/4
  return n/d;
}


// atan(x) for x on [-1,1] (unit extent)
//  atan table entry (1) : 1 ulp
float f32_atan_ue(float x)
{
  // x P(x²)/Q(x²) + x
  return fmaf(x, f32_atan_ue_k(x*x), x);
}

// atanpi(x) for x on [-1,1]
//   atanpi table entry (1) : 1 ulp
float f32_atanpi_ue(float x)
{
  return fmaf(x, f32_atanpi_ue_k(x*x), 0.25f*x);
}

// atan(x) for x on [-1,1]
//   atan table entry (2) : 3 ulp
float f32_atan_ue_a(float x)
{
  static const float N[] = {-0x4.a0e21p-4f,-0x2.8e7198p-4f};
  static const float D[] = { 0xd.e2a6ap-4,0x3.a791ap-4,-0xf.57cdap-12};
  
  float x2 = x*x;
  float n  = N[1];
  float d  = D[2];

  n = fmaf(n, x2, N[0]) * x2;

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x,(n/d),x);
}

// atanpi(x) for x on [-1,1]
//   atanpi table entry (2) : 3 ulp
float f32_atanpi_ue_a(float x)
{
  static const float N[] = {0xf.c87a6p-8f,-0x7.07068p-8,-0x8.c173ep-8f};
  static const float D[] = {0xe.70d46p-4f,0x3.63be64p-4f,-0x1.0d5b14p-8f};
    
  float n  = N[2];
  float d  = D[2];
  float x2 = x*x;
  
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x, n/d, 0.25f*x);
}


// atan(x) for x on [-1,1] 
//   atan table entry (2) : 27 ulp
float f32_atan_ue_a2(float x)
{
  static const float N[] = {-0x5.364128p-4f, -0x2.353c7p-4f };
  static const float D[] = { 0xf.a2cb1p-4f,   0x2.f08114p-4f};

  float x2 = x*x;
  float n  = N[2];
  float d  = D[1];

  n = fmaf(n, x2, N[0]) * x2;
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x,(n/d),x);
}

// atanpi(x) for x on [-1,1] : approximation (table entry x)
//   atanpi table entry (2) : 31 ulp (19 correct digits)
float f32_atanpi_ue_a2(float x)
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


//────────────────────────────────────────────────────────────────────────────────────
// Same degree polynomials as `f32_atan_ue` just everything is in doubles.
// The raison d'etre for this kernel is that it's about the same speed as
// the default and the extended precision for working in doubles can
// be a useful building block.
//
// a simple wrapper as follows would produce table entry 3
//
// float f32_atan_ue_dk(float a)
// {
//   double x = (double)a;
//
//  return (float)fma(x, f32_atan_ue_dk(x*x) ,x);
// }
//

double f32_atan_ue_dk(double x2)
{
  static const double N[] = {-0x3.97489796d0896p-4,-0x3.2dc3b8351325ap-4,-0x7.ab8ca571d35e0p-8};
  static const double D[] = { 0xa.c5d9c94ca1630p-4, 0x6.6be5a606541ecp-4, 0x9.5c13ff8f20cf8p-8};

  double n = N[2];
  double d = D[2];

  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]) * x2;

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.f);
  d = fma(d, x2, D[0]);

  // (float)fma(x,(n/d),x)   // this would complete
  return (n/d);
}

double f32_atanpi_ue_dk(double x2)
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
// atan(x) variants

// atan(x) : x on [-tan(π/8),tan(π/8)]
// ∙ internal polynomial kernel
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

// atan(x) : x on [-tan(π/8),tan(π/8)]
// ∙ internal polynomial kernel
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


// atan(x) on full domain
// ∙ max error = 1 ulp (faithfully rounded)
// ∙ the range reduction on (tan(π/8), tan(3π/8)] needs
//   extended precision divide to meet faithful rounding
//   so promotes to doubles and back for that step. Also
//   need an extended precision add but that's easy here.
// ∙ should have error number here and the full version
//   in the reports directory.
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

// atanpi(x) on full domain
// ∙ max error = 1 ulp (faithfully rounded)
// ∙ a couple of places required extended precision so it's less work to
//   just promote to doubles for the whole computation. Specifically the
//   (x-1)/(x+1) range reduction and for small results the final few
//   computations in the kernel that work in 1/π blow up the number
//   of computations required.
// ∙ full error report should be in the testing/reports directory
//   but I haven't saved it yet.
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
// atan2(y,x) variants should be put here


//════════════════════════════════════════════════════════════════════════════════════
// blah
