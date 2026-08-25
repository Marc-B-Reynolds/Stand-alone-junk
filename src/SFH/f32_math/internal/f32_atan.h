// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2024-2026

#pragma once


//────────────────────────────────────────────────────────────────────────────────────
// gross approximation of atan(x) on [-1,1] (all absolute error)

// atan on [-1,1] :peak abs error  0.003347723744809627532958984375
static inline float atan_u8(float x)
{
  float ax = fabsf(x);

  return x * (fmaf(ax, -0x1.10517p-2, 0x1.0dff9cp0));
}


//────────────────────────────────────────────────────────────────────────────────────
// rational approximation of atan(x) on [-1,1] (all relative error)
//
// Using: https://gitlab.inria.fr/sfilip/rminimax. Example first built with:
// ratapprox --function="(atan(x)-x)/x" --dom=[-1,1] --denF=[SG] --numF=[SG] --num=[x^2,x^4,x^6] --den=[1,x^2,x^4,x^6] --output=atan_3_3.sollya --log


// faithful
// |1 | 1064307333 | 1045884 || 99.901827 | 0.098173 || 5.960464e-08|
static inline float f32_atan_r33_k(float x2)
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

  return n/d;
}

// 3-ulp
// |3| 1058549070 | 6388298 | 415801 | 48 | 99.361325 | 0.599641 | 0.039029 | 0.000005 | 1.788139e-07|
static inline float f32_atan_r23_k(float x2)
{
  static const float N[] = {-0x4.a0e21p-4f,-0x2.8e7198p-4f};
  static const float D[] = { 0xd.e2a6ap-4,0x3.a791ap-4,-0xf.57cdap-12};

  float n = N[1];
  float d = D[2];

  n = fmaf(n, x2, N[0]) * x2;

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}

// 27-ulp
// |27 | 1043386989 | 6725214 | 3403259 | 3456502 | 8381253|97.938127 | 0.631266 | 0.319449 | 0.324447 | 0.786711 | 1.609325e-06|
static inline float f32_atan_r22_k(float x2)
{
  static const float N[] = {-0x5.364128p-4f, -0x2.353c7p-4f };
  static const float D[] = { 0xf.a2cb1p-4f,   0x2.f08114p-4f};

  float n = N[1];
  float d = D[1];

  n = fmaf(n, x2, N[0]) * x2;
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}


//────────────────────────────────────────────────────────────────────────────────────
// classic polynomial approximations of atan(x) [-tan(π/8),tan(π/8)]
//   atan(x) ≈ x³ P(x²) + x


// |11 | 49380158 | 10010970 | 3894914 | 4092189 | 18308084|57.628990 | 11.683278 | 4.545550 | 4.775779 | 21.366404 | 2.682209e-07|
static inline float f32_atan_sp3(float x)
{
  static const float P[] = {-0x1.5540d2p-2f, 0x1.93beeep-3f, -0x1.cbc8acp-4f};
  
  float x2 = x*x;
  float r  = P[2];

  r = fmaf(r,x2,P[1]);
  r = fmaf(r,x2,P[0]);
  r = fmaf(r*x,x2, x);

  return r;
}

// |1 | 80403902 | 5282413 | 0 | 0 | 0|93.835173 | 6.164827 | 0.000000 | 0.000000 | 0.000000 | 2.980232e-08|
static inline float f32_atan_sp4(float x)
{
  static const float P[] = {-0x1.555454p-2f, 0x1.9924dap-3f,
                            -0x1.1c380cp-3f, 0x1.49e74cp-4f};
  
  float x2 = x*x;
  float r  = P[3];

  r = fmaf(r,x2,P[2]);
  r = fmaf(r,x2,P[1]);
  r = fmaf(r,x2,P[0]);
  r = fmaf(r*x,x2, x);

  return r;
}

//────────────────────────────────────────────────────────────────────────────────────
