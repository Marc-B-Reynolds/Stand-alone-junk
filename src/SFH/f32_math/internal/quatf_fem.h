// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2026

#pragma once

// (scaled) exponential map: 2/π log(q) for unit q where if q.w < 0
// q is negated prior to taking the log.

// error numbers are on a fixed test set (not an accurate bound) and are
// distance measures vs. reference. This is very close to a half-turn
// angle measure.
//
// There are three approximations methods:
//   quatf_fem_p{n}   : direct polynomials
//   quatf_fem_t{n}   : see comments in ../sollya/quat_fem.sollya
//   quatf_fem_{n}{d} : single digits 'n' & 'd - degrees of rational approximation
//
// Order in file is from least to most accurate.

// ~4.80498423e-03
static inline vec3f_t quatf_fem_p3(quatf_t q)
{
  static const float C[] = { -0x1.2389dcp-1f, 0x1.af0972p-3f };

  float w = fabsf(q[3]);
  float s = C[1];

  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}


// ~7.02742860e-04
static inline vec3f_t quatf_fem_p4(quatf_t q)
{
  static const float C[] = { -0x1.3d7138p-1f, 0x1.82a824p-2f, -0x1.f21db8p-4f };

  float w = fabsf(q[3]);
  float s = C[2];

  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}

// ~2.57426940e-04
static inline vec3f_t quatf_fem_11(quatf_t q)
{
  static const float P[] = { 0xf.fef24p-4f, 0x1.aadd4p-4f };
  static const float Q   = 0xb.bbf97p-4f;
  
  float w = fabsf(q[3]);
  float n = fmaf(w,P[1],P[0]);
  float d = fmaf(w,Q,1.f);

  n = copysignf(n, q[3]);
  
  return (n/d)*quat_bivector(q);
}


// ~1.06546254e-04
static inline vec3f_t quatf_fem_p5(quatf_t q)
{
  static const float C[] = { -0x1.43ff04p-1f, 0x1.d410b8p-2f,
                             -0x1.0b3ae6p-2f, 0x1.2c9e6ap-4f };

  float w = fabsf(q[3]);
  float s = C[3];

  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}

#if 0
// 2.11901271e-05 : inferior to 12
static inline vec3f_t quatf_fem_21(quatf_t q)
{
  static const float P[] = {0xf.ffeap-4f, 0x2.bab85p-4f, -0x5.4f126p-8f};
  static const float Q   = 0xc.e635fp-4f;
  
  float w = fabsf(q[3]);
  float n = P[2];
  float d = Q;

  d = fmaf(d,w,1.f);
  d = copysignf(d, q[3]);

  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]);

  return (n/d)*quat_bivector(q);
}
#endif

// ~1.66997281e-05
static inline vec3f_t quatf_fem_p6(quatf_t q)
{
  static const float C[] = {-0x1.4585b0p-1f, 0x1.f26228p-2f,
                            -0x1.685bb8p-2f, 0x1.76ea28p-3f,
                            -0x1.747022p-5f};
                            

  float w = fabsf(q[3]);
  float s = C[4];

  s = fmaf(s,w,C[3]);
  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}


// ~8.77135897e-06
static inline vec3f_t quatf_fem_12(quatf_t q)
{
  static const float P[] = {0xf.79c3ep-4f, 0x6.278ffp-4f};
  static const float Q[] = {0xf.79cc8p-4f, 1.f, 0x2.8054cp-4f};
  
  float w = fabsf(q[3]);
  float n = P[1];
  float d = Q[2];

  n = fmaf(n,w,P[0]);
  n = copysignf(n, q[3]);

  d = fmaf(d,w,Q[1]);
  d = fmaf(d,w,Q[0]);
  
  return (n/d)*quat_bivector(q);
}


// ~2.83623552e-06
static inline vec3f_t quatf_fem_p7(quatf_t q)
{
  static const float C[] = {-0x1.45dbeep-1f, 0x1.fc2172p-2f,
                            -0x1.95fe04p-2f, 0x1.18651ap-2f,
                            -0x1.088334p-3f, 0x1.d579a6p-6f };
                            
  float w = fabsf(q[3]);
  float s = C[5];

  s = fmaf(s,w,C[4]);
  s = fmaf(s,w,C[3]);
  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}


// ~7.27805800e-07
static inline vec3f_t quatf_fem_22(quatf_t q)
{
  static const float P[] = {0xd.29a2p-4f, 0x7.9ef08p-4f, 0x3.88fe3cp-8f};
  static const float Q[] = {0xd.29a26p-4f, 0x1p+0f, 0x3.d4be2p-4f};
  
  float w = fabsf(q[3]);
  float n = P[2];
  float d = Q[2];

  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]);
  n = copysignf(n, q[3]);
  
  d = fmaf(d,w,Q[1]);
  d = fmaf(d,w,Q[0]);
  
  return (n/d)*quat_bivector(q);
}

// ~6.49013571e-07
static inline vec3f_t quatf_fem_p8(quatf_t q)
{
  static const float C[] = {-0x1.45ee46p-1f, 0x1.fef822p-2f,
                            -0x1.a8acb8p-2f, 0x1.50c710p-2f,
                            -0x1.b42dcep-3f, 0x1.7600eap-4f,
                            -0x1.2b94d2p-6f};

  float w = fabsf(q[3]);
  float s = C[6];

  s = fmaf(s,w,C[5]);
  s = fmaf(s,w,C[4]);
  s = fmaf(s,w,C[3]);
  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}


// ~3.69837095e-07
static inline vec3f_t quatf_fem_t7(quatf_t q)
{
  static const float K[] = {
    0x1.8acb1ep-10f, -0x1.d9caccp-8f,
    0x1.1b0ed0p-6f,  -0x1.fe55eep-6f,
    0x1.cf093ap-5f,  -0x1.17c788p-3f,
    0x1.fffffep-1f};

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  p = fmaf(p, w, K[1]);
  p = fmaf(p, w, K[2]);
  p = fmaf(p, w, K[3]);
  p = fmaf(p, w, K[4]);
  p = fmaf(p, w, K[5]);
  p = fmaf(p, w, K[6]);
  p = p*a;
  
  return p*quat_bivector(q);
}


// ~3.47871111e-07
static inline vec3f_t quatf_fem_32(quatf_t q)
{
  static const float K   = 0xb.f0cc8p-4f;
  static const float P[] = {0x8.65fd3p-4f, 0x7.0007bp-8f, -0x6.bd959p-12f};
  static const float Q   = 0x4.a7595p-4f;
  
  float w = fabsf(q[3]);
  float n = P[2];
  float d = Q;

  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]);
  n = fmaf(n,w,K);
  n = copysignf(n,q[3]);
  
  d = fmaf(d,w,1.f);
  d = fmaf(d,w,K);
  
  return (n/d)*quat_bivector(q);
}

// ~3.37359953e-07
static inline vec3f_t quatf_fem_33(quatf_t q)
{
  static const float K   = 0x9.81b88p-4f;
  static const float P[] = {0x9.f2a54p-4f, 0x2.2b9cfp-4f, 0x7.32ff48p-12f};
  static const float Q[] = {0x7.9a5a8p-4f, 0xe.7215p-8f};
  
  float w = fabsf(q[3]);
  float n = P[2];
  float d = Q[1];

  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]); 
  n = fmaf(n,w,K);
  
  d = fmaf(d,w,Q[0]);
  d = fmaf(d,w,1.f);
  d = fmaf(d,w,K);
  d = copysignf(d,q[3]);
  
  return (n/d)*quat_bivector(q);
}

// ~3.14340340e-07
static inline vec3f_t quatf_fem_43(quatf_t q)
{
  static const float K   = 0x8.ba474p-4f;
  static const float P[] = {0xa.719d4p-4f, 0x2.e42348p-4f, 0x1.1fe134p-8f, -0xa.50946p-16f};
  static const float Q[] = {0x8.b698p-4f, 0x1.52488p-4f};
  
  float w = fabsf(q[3]);
  float n = P[3];
  float d = Q[1];

  n = fmaf(n,w,P[2]);
  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]); 
  n = fmaf(n,w,K);
  
  d = fmaf(d,w,Q[0]);
  d = fmaf(d,w,1.f);
  d = fmaf(d,w,K);
  d = copysignf(d,q[3]);
  
  return (n/d)*quat_bivector(q);
}


#if 0
// ~3.21175336e-07 (underperforms 43)
static inline vec3f_t quatf_fem_44(quatf_t q)
{
  static const float K   = 0x7.8b5p-4f;
  static const float P[] = { 0xb.327dp-4f, 0x4.ba5c9p-4f, 0x8.88ep-8f, 0xe.f8f54p-16f };
  static const float Q[] = { 0xb.244cdp-4f, 0x2.d40efp-4f, 0x3.1e5e2p-8f };
  
  float w = fabsf(q[3]);
  float n = P[3];
  float d = Q[2];

  n = fmaf(n,w,P[2]);
  n = fmaf(n,w,P[1]);
  n = fmaf(n,w,P[0]); 
  n = fmaf(n,w,K);
  
  d = fmaf(d,w,Q[1]);
  d = fmaf(d,w,Q[0]);
  d = fmaf(d,w,1.f);
  d = fmaf(d,w,K);
  d = copysignf(d,q[3]);
  
  return (n/d)*quat_bivector(q);
}
#endif

// ~3.11947188e-07
static inline vec3f_t quatf_p9(quatf_t q)
{
  static const float C[] = {-0x1.45f212p-1f, 0x1.ffbd38p-2f,
                            -0x1.af6d2ap-2f, 0x1.6ccb1cp-2f,
                            -0x1.182382p-2f, 0x1.51cb5cp-3f,
                            -0x1.088bd8p-4f, 0x1.81fabap-7f};

  float w = fabsf(q[3]);
  float s = C[7];

  s = fmaf(s,w,C[6]);
  s = fmaf(s,w,C[5]);
  s = fmaf(s,w,C[4]);
  s = fmaf(s,w,C[3]);
  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}


// ~2.67495559e-07
static inline vec3f_t quatf_fem_p10(quatf_t q)
{
  static const float C[] = {
    -0x1.45f2d6p-1f, 0x1.ffefaep-2f,
    -0x1.b1a52cp-2f, 0x1.78d972p-2f,
    -0x1.3c4fc6p-2f, 0x1.d01406p-3f,
    -0x1.03f656p-3f, 0x1.75d394p-5f,
    -0x1.f46fdcp-8f };

  float w = fabsf(q[3]);
  float s = C[8];

  s = fmaf(s,w,C[7]);
  s = fmaf(s,w,C[6]);
  s = fmaf(s,w,C[5]);
  s = fmaf(s,w,C[4]);
  s = fmaf(s,w,C[3]);
  s = fmaf(s,w,C[2]);
  s = fmaf(s,w,C[1]);
  s = fmaf(s,w,C[0]);
  s = fmaf(s,w,1.f);
  s = copysignf(s, q[3]);
  
  return s*quat_bivector(q);
}

