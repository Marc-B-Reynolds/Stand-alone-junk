// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

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

// branch on the major ranges. stay in binary32
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

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asin( 0.5*sqrt(1-x) )
  float sx = f32_xor(x,a);
  float t2 = 0.5f * (1.f-a);            // exact: Sterbenz lemma
  float t  = -2.f * f32_sqrt(t2);
  float r  = P(t2);

  r = fmaf(r,t2, f32_mul_k_pi_i.l);
  r = fmaf(t,    f32_mul_k_pi_i.h, t*r);
  
  // merging this and t*r into fma increases error
  r = 0.5f + r;                         

  // sign application could be folded above. lack of motivation ATM
  // (apply to t and 1/2)
  return f32_xor(r,sx);
}

// as x0 but promote to binary64 on troublesome: sqrt needs more bits
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

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asin( 0.5*sqrt(1-x) )
  float  sx = f32_xor(x,a);
  float  tf = 0.5f * (1.f-a);
  double t2 = (double)tf;
  double t  = -2.0 * f64_sqrt(t2);
  double r  = (double)P(tf);

  r = fma(t2, r, f64_mul_k_pi_i.h);
  r = fma(t,  r, 0.5);

  // sign application again
  return f32_xor((float)r,sx);
}


static inline double f64_mask_select(double a, double b, uint64_t m)
{
  uint64_t ia = f64_to_bits(a);
  uint64_t ib = f64_to_bits(b);
  uint64_t ir = (ia & m)|(ib & ~m);
  return f64_from_bits(ir);
}


// wip: always promote to branchfree
static inline float f32_asinpi_x2(float (*P)(float), float v)
{
  double x = (double)v;
  double a = fabs(x);

  double r;

  uint64_t im = f64_sign_mask(0.5-a);
  double   sx = f64_from_bits(f64_sign_bit(x));

  // |x| > 0.5 : asinpi(x) = 1/2 - 2 asin( 0.5*sqrt(1-x) )
  double   r0 = 0.5f * (1.f-a);            // exact: Sterbenz lemma (when used)
  double   r1 = -2.0 * f64_sqrt(r0);

  // select based on: |x| <= 1/2
  double   c  = f64_mask_select(0.5, 0.0, im);
  double   t  = f64_mask_select(r1,  a,   im);
  double   t2 = f64_mask_select(r0,  x*x, im);

  t = f64_xor(t, sx);
  c = f64_xor(c, sx);
  
  r  = (double)P((float)t2);
  r = fma(t2, r, f64_mul_k_pi_i.h);
  r = fma(t,  r, c);

  return (float)r;
}



#endif
