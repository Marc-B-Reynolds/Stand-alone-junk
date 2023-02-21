// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

// Build your own libm level quality acos and asin



#ifndef F32_ASINCOS_H
#define F32_ASINCOS_H

//**************************************************************
// classic polynomial approximations of asin(x) on [-1/2, 1/2]
// if P(x) is one of the following then:
// asin(x) ~= x + x^3 P(x^2)

// NOTES:
// * only uses hybrid method of sign range reduction (SEE: f32_odd_reduce)

// too short to meet faithfully rounded (can hit 2 ulp)
static inline float f32_asincos_k3(float x2)
{
  static const float C[] = {
    0x1.993a88p-5f,
    0x1.47f5c4p-5f,
    0x1.34dc56p-4f,
    0x1.554fa2p-3f,
  };

  return f32_horner_3(x2,C);
}

// minimial faithfully rounded version
static inline float f32_asincos_k4(float x2)
{
  static const float C[] = {
    0x1.5959aap-5f,
    0x1.8c415ap-6f,
    0x1.747b72p-5f,
    0x1.330206p-4f,
    0x1.5555c8p-3f
  };

  return f32_horner_4(x2,C);
}

// tighter faithfully rounded
static inline float f32_asincos_k5(float x2)
{
  static const float C[] = {
    0x1.33d432p-5f,
    0x1.d7924cp-7f,
    0x1.04af34p-5f,
    0x1.6caab6p-5f,
    0x1.33389ep-4f,
    0x1.55554cp-3f
  };

  return f32_horner_5(x2,C);
}

// minimal improvement vs k5
static inline float f32_asincos_k6(float x2)
{
  static const float C[] = {
    0x1.1bc652p-5f,
    0x1.eca486p-8f,
    0x1.930fa2p-6f,
    0x1.ed6104p-6f,
    0x1.6dda80p-5f,
    0x1.3332acp-4f,
    0x1.555556p-3f
  };

  return f32_horner_6(x2,C);
}

//**************************************************************
// asin expansions

// SEE: f32_asin_x1. stripped of double promotion. can hit
// 2 ulp and could be cleaned & sped up...but meh.
static inline float f32_asin_x0(float x, float (*P)(float))
{
  float a = fabsf(x);

  // |x| <= 0.5
  if (a <= 0.5f) {
    float x2 = x*x;
    float r  = x2*P(x2);
    return fmaf(r,x,x);
  }

  // |x| > 0.5 : asin(x) = pi/2 - 2 asin( 0.5*sqrt(1-x) )
  float sx = f32_xor(x,a);
  float t2 = 0.5f * (1.f-a);            // exact: Sterbenz lemma
  float t  = -2.f*f32_sqrt(t2); 
  float r  = t2*P(t2);
  
  r = fmaf(r,t,t);
  r = f32_add_half_pi(r);               // FMA: r + pi/2

  return f32_xor((float)r,sx);
}


// classic core for asin:
// P is an approximation of asin on x on [-1/2,1/2] where:
//   asin(x) = x + x^3 P(x^2)
//           = fma(x, (x*x)*P(x*x), x)
//
// * broken into two sets of ranges: |x| < 1/2, |x| > 1/2
//   using P for both.
// * |x| > 1/2 has a sadface of the square root not having enough
//   precision to meet target. punt to doubles.
static inline float f32_asin_x1(float x, float (*P)(float))
{
  float a = fabsf(x);

  // |x| <= 0.5
  if (a <= 0.5f) {
    float x2 = x*x;
    float r  = x2*P(x2);
    return fmaf(r,x,x);
  }

  // |x| > 0.5 : asin(x) = pi/2 - 2 asin( 0.5*sqrt(1-x) )
  float  sx = f32_xor(x,a);
  float  t2 = 0.5f * (1.f-a);           // exact: Sterbenz lemma
  double d2 = (double)t2;
  double t  = -2.0*sqrt(d2); 
  double r  = d2*(double)P(t2);

  r = fma(r,t,t);
  r = f64_half_pi + r;

  return f32_xor((float)r,sx);
}

//**************************************************************
// acos expansions

// classic core for acos:
// P is an approximation of asin on x on [-1/2,1/2] where:
//   asin(x) = x + x^3 P(x^2)
//           = fma(x, (x*x)*P(x*x), x)
//
// * broken into three intervals: |x| < 1/2, x > 1/2, x < -1/2
//   using P for all three via identities
static inline float f32_acos_x1(float x, float (*P)(float))
{
  float a = fabsf(x);

  // |x| < 0.5 : acos(x) = pi/2 - asin(x)
  if (a <= 0.5f) {
    float x2 = x*x;
    float k  = P(x2);
    float r  = fmaf(x,x2*k,x);        
    return f32_add_half_pi(-r);         // FMA addition
  }

  // 1-|x| is exact (Sterbenz lemma)
  //float sx = f32_xor(x,a);
  float t2 = 0.5f*(1.f-a);
  float p  = f32_asincos_k5(t2);

  // x < -0.5 : acos(x) = pi - 2 asin( sqrt((1+x)/2 )
  if (x < 0.f) {
    float t = f32_sqrt(t2);
    float r = fmaf(t,t2*p,t);

    return 2.f*f32_add_half_pi(-r);     // FMA addition
  }
 
  // x >  0.5 : acos(x) = 2 asin( sqrt((1-x)/2 )
  // this is the troublesome interval. the square root
  // doesn't have enough precision for a tight result.
  // requires 2 promotes and one demote.
  double dp  = (double)p;
  double dt2 = (double)t2;
  double dt  = f64_sqrt(dt2);
  double dr  = 2.f*fma(dt,dt2*dp,dt);

  return (float)dr;
}

// modified classic: same as 'f32_acos_x1' except the 
// two |x| > 1/2 intervals are merged to be less branchy.
// this means the x < -0.5 is likewise burdened with the
// double promote/demote latencies. If the expection is
// that under high utilization that the inputs are not
// highly correlated then this is probably the better
// choice (and vise-versa if highly correlated).
// lowers average error for (x < -0.5) due to higher
// precision computations.
static inline float f32_acos_x2(float x, float (*P)(float))
{
  float a = fabsf(x);

  // |x| < 0.5 : acos(x) = pi/2 - asin(x)
  if (a <= 0.5f) {
    float x2 = x*x;
    float k  = P(x2);
    float r  = fmaf(x,x2*k,x);        
    return f32_add_half_pi(-r);       // FMA addition
  }

  // x < -0.5 : acos(x) = pi - 2 asin( sqrt((1+x)/2 )
  // x >  0.5 : acos(x) =      2 asin( sqrt((1-x)/2 )

  uint32_t sx = f32_xor_to_bits(x,a);

  // the whole point of this version is not to branch on
  // the sign of the input. otherwise might as well use
  // the 'x1' expansion. sledgehammer it ATM.
  
  // we simply want this branchfree:
//double   c  = (x < 0.f) ? -M_PI : 0.0;
  uint64_t m  = f32_sign_mask_u64(x);
  double   c  = f64_from_bits(m & f64_to_bits(-f64_pi));

  float  z  = 0.5f*(1.f-a);         // exact: Sterbenz lemma
  double t2 = (double)z;
  double t  = 2.0*f64_sqrt(t2);
  double p  = (double)P(z);
  double r  = fma(t, t2*p, t+c);
  
  return f32_mulsign((float)r, sx);
}

#endif
