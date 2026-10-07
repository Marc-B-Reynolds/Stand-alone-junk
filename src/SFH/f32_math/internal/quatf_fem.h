// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2026

#pragma once

// forward (scaled) exponential map:
//   2/π log(Q) provided Q is unit and Q.w >= 0.
//
// routine handles Q.w < 0 as: 2/π log(-Q)
//
// Using the exponential map to perform unit quaternion to unit ball:
//   Q = A + w    (unit with w ≥ 0)
//   B = f(w) A   (result)
//
// error numbers are on a fixed test set (not an accurate bound) and are
// distance measures vs. reference. This is very close to a half-turn
// angle measure.
//
// There are three approximations methods:
//   quatf_fem_p{n}   : direct polynomials
//   quatf_fem_t{n}   : see comments in ../sollya/quat_fem.sollya and/or quatf_fem_t8 below
//                    : (underperform in accuracy and throughput. sadface)
//   quatf_fem_{n}{d} : single digits 'n' & 'd - degrees of rational approximation
//                      produced using: https://gitlab.inria.fr/sfilip/rminimax
//                      example command line: ratapprox --function="2*(acos(x)/(sqrt(1-x^2)))/pi" --dom=[0,0.99999999] --denF=[SG] --numF=[SG] --num=[1,x,x^2] --den=[1,x,x^2] --output=fem_22.sollya
//                      was used to produce quatf_fem_22
// 
// Order in file is from most to least accurate (again: these are not tight measures)
// 
// example sloppy (no other work) throughput numbers. 'median' results are very consistent across runs.
// ┌───────────────────┬──────────────────────────────┬────────────────┬─────────┬─────────┬─────────┐
// │ function          │       mean ± std (cycles)    │                │    min  │  median │    max  │
// ├───────────────────┼──────────────────────────────┼────────────────┼─────────┼─────────┼─────────┤
// │ quatf_fem_atan2   │    237.286110 ±     85.361152│ (1 ± 0.359739) │   194.32│   199.68│   601.24│
// │ quatf_fem_acos    │    172.101064 ±     56.776856│ (1 ± 0.329904) │   144.37│   148.17│   380.10│ ← error bound opponent
// │ quatf_fem_p9      │     94.896225 ±     32.630150│ (1 ± 0.343851) │    81.34│    84.26│   242.50│
// │ quatf_fem_t8      │    101.008263 ±     36.335606│ (1 ± 0.359729) │    83.21│    86.26│   260.00│
// │ quatf_fem_43      │     99.573567 ±     43.225816│ (1 ± 0.434109) │    79.50│    83.75│   302.22│
// │ quatf_fem_33      │     91.987692 ±     32.089290│ (1 ± 0.348843) │    78.65│    81.24│   242.92│ ← last to outperform WRT error
// │ quatf_fem_32      │     88.814970 ±     31.370987│ (1 ± 0.353217) │    77.75│    80.60│   240.92│
// │ quatf_fem_t7      │     91.870110 ±     23.546673│ (1 ± 0.256304) │    81.59│    84.51│   197.31│
// │ quatf_fem_p8      │     92.399062 ±     33.003662│ (1 ± 0.357186) │    79.50│    82.54│   249.71│
// │ quatf_fem_22      │     85.212349 ±     20.143333│ (1 ± 0.236390) │    75.61│    78.17│   173.63│
// │ quatf_fem_t6      │     91.878303 ±     26.653994│ (1 ± 0.290101) │    81.10│    83.49│   223.76│
// │ quatf_fem_p7      │     99.325965 ±     43.515739│ (1 ± 0.438110) │    77.27│    79.53│   238.40│
// │ quatf_fem_t5      │     92.289456 ±     25.466262│ (1 ± 0.275939) │    81.05│    83.16│   190.07│
// │ quatf_fem_12      │     89.846032 ±     35.520483│ (1 ± 0.395348) │    75.55│    77.84│   234.54│
// │ quatf_fem_p6      │     84.262025 ±     25.383608│ (1 ± 0.301246) │    75.87│    78.19│   232.89│
// │ quatf_fem_t4      │     88.751430 ±     22.782335│ (1 ± 0.256698) │    80.09│    82.27│   221.25│
// │ quatf_fem_p5      │     83.046694 ±     24.823861│ (1 ± 0.298914) │    73.83│    75.94│   190.78│
// │ quatf_fem_11      │     85.579393 ±     28.585593│ (1 ± 0.334024) │    72.96│    75.99│   189.96│
// │ quatf_fem_t3      │     92.623971 ±     31.657638│ (1 ± 0.341787) │    79.51│    81.79│   240.60│
// │ quatf_fem_p4      │     86.843931 ±     36.746100│ (1 ± 0.423128) │    72.34│    74.48│   222.12│
// │ quatf_fem_p3      │     95.227661 ±     46.983090│ (1 ± 0.493376) │    71.12│    73.41│   229.71│
// └───────────────────┴──────────────────────────────┴────────────────┴─────────┴─────────┴─────────┘


// calling this: ground truth
static inline vec3f_t quatf_fem_ref(quatf_t Q)
{
  quatd_t q = quatf_promote(Q);
  double  w = q[3];

  if (w < 0.0) {
    q = -q;
    w = -w;
  }
  
  double  a = quat_bnorm_fma(q);
  double  b = ssimd_rsqrt_f64(a+0x1.0p-1024);
  double  k = a*b;
  double  s = (2.0/M_PI)*atan2(k,w)*b;
  vec3d_t v = s*quat_bivector(q);

  return vec3d_demote(v);
}


// simple: scale standard log.
// ~2.54740144e-07
static inline vec3f_t quatf_fem_atan2(quatf_t q)
{
  static const float K = 0x1.45f306p-1f; // 2/π
  float x = q[3];                        // m cos(Θ)
  float y = sqrtf(quat_bnorm_fma(q));    // |V| = m sin(Θ)
  float Θ = atan2f(y,x);                 //   this does all the heavy lifting
  float s = Θ/y;                         // scale factor

  // small y can be zero or a normal number and in the
  // limit the scale factor approaches one.
  s = (y != 0.f) ? s : 1.f;

  return (K*s)*quat_bivector(q);
}

// standard functions: move to acos and computing
// |V| as sqrt(1-w²)
static inline vec3f_t quatf_fem_acos(quatf_t q)
{
  static const float K = 0x1.45f306p-1f;

  float k = copysignf(K,q[3]);     
  float w = fabsf(q[3]);

  w = (w <= 1.f) ? w : 1.f;
  
  float y = sqrtf(-fmaf(w,w,-1.f));      //   3.40125329e-07
//float y = sqrtf((1.f-w)*(1.f+w));      //   3.63783706e-07
//float y = sqrtf(1.f-w*w);              //   5.11455530e-07
//float y = sqrtf(quat_bnorm_fma(q));    //   4.68131871e-06
  float Θ = acosf(w);
  float s = Θ/y;
  
  s  = (y != 0.f) ? s : 1.f;
  s *= k;

  return s*quat_bivector(q);
}


//────────────────────────────────────────────────────────────────────────────────────
// direct methods

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

// ~3.11947188e-07
static inline vec3f_t quatf_fem_p9(quatf_t q)
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


// these forms are underperforming as noted above. keeping since
// a fast & accurate 1/sqrt(x) hardware op could make it interesting again
// 
// one way to approximation acos directly on [0,1] can be
// formed by:  acos(x) ≈ P(x) sqrt(1-x)
//
//   f(w) ≈ P(w) sqrt(1-w)/sqrt(1-w²)
//        ≈ P(w)/sqrt(1+w)
//
// ~3.02827380e-07
static inline vec3f_t quatf_fem_t8(quatf_t q)
{
  static const float K[] = {
    -0x1.b86cc6p-11f,0x1.1ec656p-8f,
    -0x1.6a761ap-7f, 0x1.4455cap-6f,
    -0x1.060ff2p-5f, 0x1.d01d6cp-5f,
    -0x1.17cb74p-3f
  };

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  // compute f(w): 
  //   actual polynomial used is P(w) = 1 + w K(w), with
  //   a = 1/sqrt(1+w) → f(w) = a + aw K(w)
  p = fmaf(p,  w, K[1]);
  p = fmaf(p,  w, K[2]);
  p = fmaf(p,  w, K[3]);
  p = fmaf(p,  w, K[4]);
  p = fmaf(p,  w, K[5]);
  p = fmaf(p,  w, K[6]);
  p = fmaf(a*p,w,a);
  
  return p*quat_bivector(q);
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
  n = copysignf(n,q[3]);
  
  d = fmaf(d,w,Q[0]);
  d = fmaf(d,w,1.f);
  d = fmaf(d,w,K);
  
  return (n/d)*quat_bivector(q);
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

// ~8.78031246e-07
static inline vec3f_t quatf_fem_t6(quatf_t q)
{
  static const float K[] = {
    -0x1.7274aep-9f, 0x1.98e5a2p-7f,
    -0x1.d7319cp-6f, 0x1.caaf8ep-5f,
    -0x1.17b15p-3f,  0x1.ffffecp-1f }; 

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  p = fmaf(p, w, K[1]);
  p = fmaf(p, w, K[2]);
  p = fmaf(p, w, K[3]);
  p = fmaf(p, w, K[4]);
  p = fmaf(p, w, K[5]);
  p = a*p;
  
  return p*quat_bivector(q);
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

// ~ 4.82217785e-06
static inline vec3f_t quatf_fem_t5(quatf_t q)
{
  static const float K[] = {
    0x1.70ba6ap-8f, -0x1.78d516p-6f,
    0x1.babc12p-5f, -0x1.173b6p-3f,
    0x1.ffff66p-1f };

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  p = fmaf(p, w, K[1]);
  p = fmaf(p, w, K[2]);
  p = fmaf(p, w, K[3]);
  p = fmaf(p, w, K[4]);
  p = a*p;
  
  return p*quat_bivector(q);
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


// ~3.65338093e-05
static inline vec3f_t quatf_fem_t4(quatf_t q)
{
  static const float K[] = {
    -0x1.8ea382p-7f, 0x1.863354p-5f,
    -0x1.14d17ep-3f, 0x1.fffb3cp-1f };

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  p = fmaf(p, w, K[1]);
  p = fmaf(p, w, K[2]);
  p = fmaf(p, w, K[3]);
  p = a*p;
  
  return p*quat_bivector(q);
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


// ~3.14763605e-04
static inline vec3f_t quatf_fem_t3(quatf_t q)
{
  static const float K[] = { 0x1.efb86ap-6f, -0x1.088fd6p-3f, 0x1.ffd6c4p-1f };

  float w = fabsf(q[3]);
  float a = copysignf(sqrtf(1.f/(1.f+w)), q[3]);
  float p = K[0];

  p = fmaf(p, w, K[1]);
  p = fmaf(p, w, K[2]);
  p = a*p;
  
  return p*quat_bivector(q);
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
