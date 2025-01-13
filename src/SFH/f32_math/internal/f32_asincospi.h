// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

// classic style asinpi/cospi approximations

#ifndef F32_ASINCOSPI_H
#define F32_ASINCOSPI_H



//**************************************************************
// polynomial P approximations of asin(x)/pi on [-1/2, 1/2]
// if P(x) is one of the following then:
// asinpi(x) ~= x/pi + x^3 P(x^2)

static inline float f32_asinpi_k5(float x)
{
  static const float C[] =
    {0x1.0482b4p-6f, 0x1.a1959ep-7f, 0x1.8940acp-6f, 0x1.b2921ep-5f};

  return f32_horner_3(x,C);
}

static inline float f32_asinpi_k6(float x)
{
  static const float C[] =
    {0x1.b7ef3ep-7f, 0x1.f83e04p-8f, 0x1.da4aa4p-7f, 0x1.86e4a2p-6f, 0x1.b299f2p-5f};

  return f32_horner_4(x,C);
}

static inline float f32_asinpi_k7(float x)
{
  static const float C[] = {0x1.85fd46p-7f, 0x1.2f42f4p-8f, 0x1.4b767p-7f, 0x1.d05edp-7f, 0x1.872a08p-6f, 0x1.b29954p-5f};

  return f32_horner_5(x,C);
}

static inline float f32_asinpi_k8(float x)
{
  static const float C[] = {0x1.731ba8p-7f, 0x1.15e3b2p-9f, 0x1.03d32ep-7f, 0x1.39829cp-7f, 0x1.d1dfccp-7f, 0x1.8722a4p-6f, 0x1.b2996p-5f};

  return f32_horner_6(x,C);
}


//**************************************************************
// asinpi expansions

// branchs on the two ranges. stays in binary32
// 1) no promotion makes the errors much larger for |x| > 1/2
//    than |x| <= 1/2 for a given kernel
// 2) it's sad if the input is random WRT ranges
static inline float f32_asinpi_x0(float (*P)(float), float x)
{
  float a = fabsf(x);

  // |x| <= 0.5
  if (a <= 0.5f) {
    float t2 = x*x;
    float r  = P(t2);
    // extended precision 1/pi lead term
    r = fmaf(r,t2, f32_mul_k_pi_i.l);
    r = fmaf(x,    f32_mul_k_pi_i.h, x*r);

    return r;
  }

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asinpi( sqrt(1-x)/1 )

  // extract the sign bit and apply early to break dep-chain
  // removes application at end at cost of 1 basic op.
  float sx = f32_xor(x,a);
  float m  = f32_xor(-2.0f, sx);
  float c  = f32_xor( 0.5f, sx);
  
  float t2 = 0.5f * (1.f-a);             // exact: Sterbenz lemma
  float t  =    m * f32_sqrt(t2);
  float r  = P(t2);
  
  r = fmaf(r,t2, f32_mul_k_pi_i.l);
  r = fmaf(t,    f32_mul_k_pi_i.h, t*r);
  
  // merging this and t*r into fma increases error
  return r + c;
}

// branchs on the two ranges. promotes to binary64 on |x| > 1/2
// 1) drastically improves error numbers vs. 'x0'
// 2) it's sad if the input is random WRT ranges
static inline float f32_asinpi_x1(float (*P)(float), float x)
{
  float a = fabsf(x);

  // |x| <= 0.5
  if (a <= 0.5f) {
    float t2 = x*x;
    float r  = P(t2);
    r = fmaf(r,t2, f32_mul_k_pi_i.l);
    r = fmaf(x,    f32_mul_k_pi_i.h, x*r);

    return r;
  }

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asinpi( sqrt(1-x)/2 )
  float  sx = f32_xor(x,a);
  float  tf = 0.5f * (1.f-a);        // exact: Sterbenz lemma
  double t2 = (double)tf;
  double t  = -2.0 * f64_sqrt(t2);
  double r  = (double)P(tf);
  
  r = fma(t2, r, f64_mul_k_pi_i.h);
  r = fma(t,  r, 0.5);

  // dep-chain of sign application could be removed. requires
  // a bit of thinking and restructuring though.
  return f32_xor((float)r, sx);
}


// always promote and branchfree. always promote doesn't have a
// huge impact on error measures so skipping on branchy version
// 1) lowest errors (for a given kernel) but not vastly better
//    than 'x1'
// 2) performance isn't a concern OR:
// 3) is a performance concern but want (1) and input isn't 
//    predictable WRT reducation ranges.
static inline float f32_asinpi_x2(float (*P)(float), float v)
{
  double x = (double)v;
  double a = fabs(x);

  double r;

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asinpi( sqrt(1-x)/2 )
  double   r0 = 0.5f * (1.f-a);       // exact: Sterbenz lemma (when used)
  double   r1 = -2.0 * f64_sqrt(r0);
  double   im = 0.5-a;                // select based on: |x| <= 1/2
  double   sx = f64_xor(x,a);         // sign bit

  // perform the range selection
  double   c  = f64_sign_select(0.5, 0.0, im);
  double   t  = f64_sign_select(r1,  a,   im);
  double   t2 = f64_sign_select(r0,  x*x, im);

  // apply sign: break dep-chain instead of apply at end
  t = f64_xor(t, sx);
  c = f64_xor(c, sx);
  
  // |x| <= 0.5 : asinpi(x) = a/pi + a^3 P(a^2), a = |x|
  // |x| >  0.5 : asinpi(x) = 1/2 - 2 asinpi( sqrt(1-x)/2 )
  r  = (double)P((float)t2);
  r = fma(t2, r, f64_mul_k_pi_i.h);
  r = fma(t,  r, c);

  return (float)r;
}


//**************************************************************
// acospi expansions:
//   |x| <  0.5 : acospi(x) = 1/2 -   asinpi(x)
//    x  < -0.5 : acospi(x) =   1 - 2 asinpi( sqrt((1+x)/2 )
//    x  >  0.5 : acospi(x) =       2 asinpi( sqrt((1-x)/2 )
//
//  m = ( x  < 0.5) ?   2  : 1 {hummm}
//  s = ( x  < 0.5) ?  -1  : 1
//  a = (|x| < 0.5) ? -1/2 : 0
//
//  acospi(x) = sm()

// WIP: but in working state
static inline float f32_acospi_x1(float (*P)(float), float x)
{
  float a = fabsf(x);

  // |x| <= 0.5
  if (a <= 0.5f) {
    float t2 = x*x;
    float r  = P(t2);
    r =  fmaf(r,t2, f32_mul_k_pi_i.l);
    r =  fmaf(x,    f32_mul_k_pi_i.h, x*r);

    // merging via FMA with x*r increase error
    return 0.5f-r;
  }

  float  sx = f32_xor(x,a);
  float  tf = 0.5f * (1.f-a);        // exact: Sterbenz lemma
  double t2 = (double)tf;
  double t  = 2.0*f64_sqrt(t2);
  double r  = (double)P(tf);
  double c  = (x < 0.f) ? -1.f : 0.f;
  
  r = fma(t2, r, f64_mul_k_pi_i.h);
  r = fma(t,  r, c);

  return f32_xor((float)r, sx);
}

#endif
