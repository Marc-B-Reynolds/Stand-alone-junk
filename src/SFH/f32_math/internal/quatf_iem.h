// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2026

#pragma once

static inline quatf_t quatf_iem_ref(vec3f_t V)
{
  vec3d_t v = vec3f_promote(V);
  double  d = vec3_norm_fma(v);

#if 0  
  double  t = ssimd_rsqrt_f64(d+0x1.0p-1020);
  double  a = (M_PI/2.0)*t*d;
  double  s = sin(a);
  double  k = s*t;
  quatd_t r = quatd_bs(k*v,cos(a));
#else
  double  x = sqrt(d);
  double  a = (0.5*M_PI)*x;
  double  w = cos(a);
  double  s = sin(a)/(x+0x1.0p-1020);
  quatd_t r = quatd_bs(s*v,w);
#endif  

  return quatd_demote(r);
}

// ~ 2.63713986e-07
static inline quatf_t quatf_iem_naive(vec3f_t v)
{
  float d = vec3_norm_fma(v);
  float x = sqrtf(d);
  float a = ((float)(0.5*M_PI))*x;
  float w = cosf(a);
  float s = sinf(a)/x;               // not worrying about limit x approaches zero
  return quatf_bs(s*v,w);            //   since not for real use
}


// exp(π/2 V) special cased for 'V' in unit ball
//  given V = ΘU (in unit ball, U = unit bivector)
//  returns cos(π/2 Θ) + sin(π/2 Θ) U
// ~2.06779305e-07
static inline quatf_t quatf_iem_p5(vec3f_t v)
{
  // cos(π/2 Θ) ≈ 1 + Θ²P(Θ²) where Θ² = d = v∙v
  //   coefficients of polynomial P
  static const float C[] = {-0x1.8c0b5p-16f, 0x1.e0dbdep-11f,
                            -0x1.55ce5cp-6f, 0x1.03c1d8p-2f,
                            -0x1.3bd3ccp0f};

  // computation of 's'
  // sin(π/2 Θ)/Θ = sqrt(1-w²)/Θ        : from cosine
  //              = sqrt((1-w²)/d)      : pull Θ inside sqrt
  //              = sqrt((1-(1+dp)²)/d) : w = 1+dp
  //              = sqrt(-dp²-2p)       : reduce
  //
  // s only NaN for very wrong inputs (d ≥ 0x1.f520fp+1, ~3.915)
  float d = vec3_norm_fma(v);           // v∙v
  float p = f32_horner_4(d,C);          // P(Θ²)
  float s = sqrtf(-fmaf(d,p*p,p+p));    // sin(π/2 Θ)/Θ
  float w = fmaf(d,p,1.f);              // cos(π/2 Θ) = 1+dp

  // { sin(π/2 a)/a v, cos(π/2 a) }
  return quatf_bs(s*v,w);
}


// ~2.30486819e-07
static inline quatf_t quatf_iem_p4(vec3f_t v)
{
  static const float C[] = {0x1.c2a574p-11f, -0x1.5502d4p-6f,
                            0x1.03bd9ep-2f,  -0x1.3bd3bp0f};

  float d = vec3_norm_fma(v);
  float p = f32_horner_3(d,C);
  float s = sqrtf(-fmaf(d,p*p,p+p));
  float w = fmaf(d,p,1.f);

  return quatf_bs(s*v,w);
}

// ~1.77031452e-05
static inline quatf_t quatf_iem_p3(vec3f_t v)
{
  static const float C[] = {-0x1.39de84p-6f, 0x1.02c086p-2f, -0x1.3bc928p0f };

  float d = vec3_norm_fma(v);
  float p = f32_horner_2(d,C);
  float s = sqrtf(-fmaf(d,p*p,p+p));
  float w = fmaf(d,p,1.f);

  return quatf_bs(s*v,w);
}



