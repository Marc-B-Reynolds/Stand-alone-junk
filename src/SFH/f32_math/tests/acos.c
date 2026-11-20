// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_acosf) version:
// ************************************
//
// The CORE-MATH routine fall under:
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#if defined(__linux__) || defined(__unix__)
// for thread affinity
#define _GNU_SOURCE 
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "internal/f32_asincos.h"

#include "util.h"

// testing test uniform range
#define SAMPLE_X0 -1.f
#define SAMPLE_X1  1.f


//────────────────────────────────────────────────────────────────────────────────────

float libm(float x)   { return acosf(x); }
float libm_d(float x) { return (float)acos((double)x); }

static const float
pio2_hi = 1.5707962513e+00f, /* 0x3fc90fda */
pio2_lo = 7.5497894159e-08f, /* 0x33a22168 */
pS0 =  1.6666586697e-01f,
pS1 = -4.2743422091e-02f,
pS2 = -8.6563630030e-03f,
qS1 = -7.0662963390e-01f;

// fdlibm asin/acos core rational approximation
static float R(float z)
{
  float_t p, q;
  p = z*(pS0+z*(pS1+z*pS2));
  q = 1.0f+z*qS1;
  return p/q;
}

// fdlibm asin/acos core rational approximation
//   ~asin(x) = fma(x, fdlibm_asincos_k(x*x), x) on |x| < 1/2
static inline float fdlibm_asincos_k(float x2)
{
  static const float P[] = {-8.6563630030e-3f, -4.2743422091e-2f, 1.6666586697e-1f};
  static const float Q   = -7.0662963390e-1f;

  float n = x2 * f32_horner_2(x2,P);
  float d = fmaf(x2, Q, 1.f);
  return n/d;
}


// almost "unmodified"
float fdlibm_acosfu(float x)
{
  float z,w,s,c,df;
  uint32_t hx,ix;
  
  hx = f32_to_bits(x);
  ix = hx & 0x7fffffff;

  /* |x| >= 1 or nan */
  if (ix >= 0x3f800000) {
    if (ix == 0x3f800000) {
      if (hx >> 31)
	return 2*pio2_hi + 0x1p-120f;
      return 0;
    }
    return 0/(x-x);
  }
  
  /* |x| < 0.5 */
  if (ix < 0x3f000000) {
    return pio2_hi - (x - (pio2_lo-x*R(x*x)));
  }
  
  /* x < -0.5 */
  if (hx >> 31) {
    z = (1+x)*0.5f;
    s = f32_sqrt(z);
    w = R(z)*s-pio2_lo;
    return 2*(pio2_hi - (s+w));
  }
  
  /* x > 0.5 */
  z = (1-x)*0.5f;
  s = f32_sqrt(z);
  hx = f32_to_bits(s);
  df = f32_from_bits(hx&0xfffff000);
  c = (z-df*df)/(s+df);
  w = R(z)*s+c;
  return 2*(df+w);
}



float fdlibm_acosf(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;

  // |x| < 0.5
  if (ax <= 0x3f000000) {
    float x2 = x*x;
    return pio2_hi - (x + fmaf(x,R(x2),-pio2_lo) );
  }

  float z,w,s,c,df;
  
  // x < -0.5
  if (sx) {
    z = (1+x)*0.5f;
    s = f32_sqrt(z);
    w = fmaf(s,R(z),-pio2_lo);
    return 2*(pio2_hi - (s+w));
  }
  
  if (ax == 0x3f800000) { if (!sx) return 0.f; }
  
  // x > 0.5
  z  = (1-x)*0.5f;
  s  = f32_sqrt(z);
  ix = f32_to_bits(s);

  df = f32_from_bits(ix&0xfffff000);
  c  = -fmaf(df,df,-z)/(s+df);
  w  = fmaf(s,R(z),c);

  return 2*(df+w);
}


#if 1

// hacked for local defs
static inline float mulsignf(float x, float y) {
  return f32_from_bits(f32_to_bits(x) ^ (f32_to_bits(y) & (1u << 31)));
}

typedef struct { float x,y; } Sleef_float2;

static inline Sleef_float2 df(float h, float l) {
  Sleef_float2 ret;
  ret.x = h; ret.y = l;
  return ret;
}

static inline Sleef_float2 dfadd_f2_f_f(float x, float y) {
  // |x| >= |y|
  Sleef_float2 r;
  r.x = x + y;
  r.y = x - r.x + y;
  return r;
}

static inline Sleef_float2 dfadd_f2_f2_f(Sleef_float2 x, float y) {
  // |x| >= |y|
  Sleef_float2 r;
  r.x = x.x + y;
  r.y = x.x - r.x + y + x.y;

  return r;
}

float sleef_acosf(float d)
{
  int o = fabsf(d) < 0.5f;
  float x2 = o ? (d*d) : ((1-fabsf(d))*0.5f), u;
  float x = o ? fabsf(d) : f32_sqrt(x2);
  x = fabsf(d) == 1.0 ? 0 : x;

  u = +0.4197454825e-1f;
  u = fmaf(u, x2, +0.2424046025e-1f);
  u = fmaf(u, x2, +0.4547423869e-1f);
  u = fmaf(u, x2, +0.7495029271e-1f);
  u = fmaf(u, x2, +0.1666677296e+0f);

  u *= x * x2;
  
  float y = 3.1415926535897932f/2 - (mulsignf(x, d) + mulsignf(u, d));
  x += u;
  float r = o ? y : (x*2);
  if (!o && d < 0) r = dfadd_f2_f2_f(df(3.1415927410125732422f,-8.7422776573475857731e-08f), -r).x;

  return r;
}
#endif

// much worse than abs error version.  hummm
static inline float f32_asincos_x0(float x2)
{
  // rel error - 
  static const float C[] = {
     0x1.b7070cp-11f,
    -0x1.723450p-8f,
     0x1.2f2922p-6f,
    -0x1.59358ep-5f,
     0x1.5f9f50p-4f,
    -0x1.b6199ap-3f,
     0x1.921b80p0f
  };

  return f32_horner_6(x2,C);
}


static inline float f32_asincos_x1(float x2)
{
  // abs error - 
  static const float C[] = {
     0x1.d0b534p-11f,
    -0x1.803a5ap-8f,
     0x1.357982p-6f,
    -0x1.5c356cp-5f,
     0x1.606a40p-4f,
    -0x1.b635e4p-3f,
     0x1.921be8p0f
  };

  return f32_horner_6(x2,C);
}

float f32_acos_(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;
  float    a  = f32_from_bits(ax);

  // |x| < 0.5 : acos(x) = pi/2 - asin(x)
  if (ax <= 0x3f000000) {
    float x2 = x*x;
    float k  = f32_asincos_k5(x2);
    float r  = fmaf(x,x2*k,x);        
    return f32_add_half_pi(-r);
  }

  float t2 = 0.5f*(1.f-a);
  float p  = f32_asincos_k5(t2);

  // x < -0.5 : acos(x) = pi - 2 asin( sqrt((1+x)/2 )
  if (sx) {
    // k5 CR/FR:  7851474/537135 (93.596853/6.403147%)
    float t = f32_sqrt(t2);
    float r = fmaf(t,t2*p,t);

    return 2.f*f32_add_half_pi(-r);
  }
  
  if (ax == 0x3f800000) { if (!sx) return 0.f; }
  
  // x > 0.5 : acos(x) = 2 asin( sqrt((1-x)/2 )

#if 0
  float e = -fmaf(t,t,-t2);
  float l =  t/(2.f*e);

#endif

#if 0
  // k5 CR/FR: 8265638/122971 (98.534072/1.465928%)
  // top 12-bits of 't'
  ix = f32_to_bits(t);
  df = f32_from_bits(ix&0xfffff000);

  c  = -fmaf(df,df,-t2)/(t+df);
  w  = fmaf(t, t2*p, c);

  return 2*(df+w);
#elif 1
  // k5 CR/FR:  8335179/53430 (99.363065/0.636935%)
  double dp  = (double)p;
  double dt2 = (double)t2;
  double dt  = f64_sqrt(dt2);
  double dr  = 2.f*fma(dt,dt2*dp,dt);

  return (float)dr;
#elif 1
  // meh
  k = f32_asincos_k6(1.f-x);
  return fmaf(f32_sqrt(1.f-x),k,0.f);
#else  
  // k5 CR/FR: 6321580/2067029 (75.359097/24.640903%)
  float r = fmaf(t,t2*k,t);
  return 2.f*r;
#endif
}


#if 1
double f32_atan_ue_dk(double x2)
{
  static const double N[] = {-0xb.65d3cb953908p-48,-0x3.975debc74bb1p-4,
                             -0x3.2db6edaee4d64p-4,-0x7.aaf3b0c3bf22p-8};
  static const double D[] = { 0xa.c619c60a22078p-4, 0x6.6badb3354c53p-4,
                              0x9.5b472df81dfdp-8};
  double n = N[3];
  double d = D[2];

  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]);

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.0); 
  d = fma(d, x2, D[0]);

  // (float)fma(x,(n/d),x)   // this would complete
  return (n/d);
}

float acos_bf(float a)
{
  double   x  = (double)fabsf(a);
  double   d  = (1.0+x);
  double   n  = (1.0-x)*d;
  double   r  = f32_atan_ue_dk(d/(n*n));
  double   t  = sqrt(n)/d;
  double   y  = fma(t,r,t);

  // temp hack
  if (a >= 0)
    return (float)(2.0*y);

  return (float)(f64_pi-2.0*y);
}
#endif


float f32_acos(float x)
{
  static const float C[] = {0x1.55554cp-3f, 0x1.33389ep-4f,
                            0x1.6caabcp-5f, 0x1.04aedp-5f,
                            0x1.d79aa8p-7f, 0x1.33d094p-5f };
  float a = fabsf(x);
  float r, t;

  // approximate using asin kernel on [-1/2, 1/2]
  //   acos(x) = π   - 2 asin(sqrt((1-x)/2))   : x < -1/2          : 0
  //   acos(x) = π/2 -   asin(x)               : x on [-1/2,1/2]   : 1
  //                   2 asin(sqrt((1-x)/2))   : x >  1/2          : 2
  
  t = sqrtf(fmaf(.5f, -a, 0.5f));
  t = (a > 0.5f) ? t : a;
  t = f32_mul_by_sign(t, x);
  t = (a > 0.5f) ? t : -t;
  
  // asin(t) ≈ x + x³P(x²)) on [-1/2,1/2]
  float t2 = t*t;

#if 1
  r =             C[5];
  r = fmaf(r, t2, C[4]);
  r = fmaf(r, t2, C[3]);
  r = fmaf(r, t2, C[2]);
  r = fmaf(r, t2, C[1]);
  r = fmaf(r, t2, C[0]);
  r = r * t2;
  r = fmaf(r, t, t);
#else
  r = t2* f32_asincos_k4(t2);
  r = fmaf(r, t, t);
#endif  
  
  // r + π/2 (extended precision in one fma)
  float ar = f32_add_half_pi(r);
  
  // complete range reduction transforms
  r = (x <= 0.5f) ? ar    : r;
  r = (a >  0.5f) ? (r+r) : r;
  
  return r;
}


//────────────────────────────────────────────────────────────────────────────────────
/*
 * Copyright (C) 2023-2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Fast version of acosf
 * Max ULP: 1.53
 * This routine does not handle special cases such as NaN/INF
 *
 * Signature:
 *   float acosf(float x)
 *
 * Spec:
 *   acosf(0)    = pi/2
 *   acosf(1)    = 0
 *   acosf(-1)   = pi
 *   acosf(+inf) = NaN
 *   acosf(-inf) = NaN
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute acosf(float x)
 *
 * Based on the value of x, acosf(x) is calculated as,
 *
 * 1. If x > 0.5
 *      acosf(x) = 2 * asinf(sqrt((1 - x) / 2))
 *
 * 2. If x < -0.5
 *      acosf(x) = pi - 2asinf(sqrt((1 + x) / 2))
 *
 * 3. If x <= |0.5|
 *      acosf(x) = pi / 2 - asinf(x)
 *
 * 4.   acosf(-x) = 1 / 2 * pi + asinf(x)
 *
 * acosf(x) is calculated using the polynomial,
 *      x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
 *
 */

#include "amd_source.h"

static struct {
  float const piby2, pi;
  float half;
  float a[2], b[2], poly_acosf[5];
} acosf_data = {
  .piby2      = 1.5707963705e+00F,
  .pi         = 3.1415926535897933e+00F,
  .half       = 0x1p-1,
  .a          = {
    0,
    0x1.921fb6p-1,
  },
  .b          = {
    0x1.921fb6p0,
    0x1.921fb6p-1,
  },
  /* Polynomial coefficients*/
  .poly_acosf = {
    0x1.5555fcp-3,
    0x1.32f8d8p-4,
    0x1.7525aap-5,
    0x1.86e46ap-6,
    0x1.5d456cp-5,
  },
};

#define ALM_ACOSF_PIBY2 acosf_data.piby2
#define ALM_ACOSF_PI    acosf_data.pi
#define ALM_ACOSF_HALF  acosf_data.half

#define A acosf_data.a
#define B acosf_data.b

#define C1 acosf_data.poly_acosf[0]
#define C2 acosf_data.poly_acosf[1]
#define C3 acosf_data.poly_acosf[2]
#define C4 acosf_data.poly_acosf[3]
#define C5 acosf_data.poly_acosf[4]

float amd_acos(float x)
{
    float y, z, poly, result;
    unsigned int ux, aux, xneg;
    int xexp;
    int32_t n = 0;

    ux   = asuint32 (x);

    /* Get absolute value of input */
    aux  = (ux & ~SIGNBIT_SP32);

    /* Get sign of input */
    xneg = (ux & SIGNBIT_SP32);

    /* Get the exponent part */
    xexp = (int)((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;

    if (xexp < -26)
    {
        /* Input small enough that arccos(x) = pi/2 */
        return ALM_ACOSF_PIBY2;
    }
    else if (xexp >= 0)
    {   /* abs(x) >= 1.0 */
        if (x == 1.0F)
            return 0.0F;
        else if (x == -1.0F)
            return ALM_ACOSF_PI;
    }

    y = asfloat(aux);

    if (y > ALM_ACOSF_HALF)
    {
        z = ALM_ACOSF_HALF * (1.0f - y);
        y = -2.0f * ALM_PROTO_KERN(sqrtf)(z);
    }
    else
    {
        n = 1;
        z = y * y;
    }
    /*
     *  Compute acosf(x) using the polynomial
     *  x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
     */
    poly = POLY_EVAL_5(z, C1, C2, C3, C4, C5);
    poly = y + y * z * poly;

    if (xneg)
        result = (B[n] + poly) + B[n];
    else
        result = (A[n] - poly) + A[n];

    return result;
}

// re-enable any disabled warnings from "amd_source.h"
#pragma GCC diagnostic pop
// end of AMD

//────────────────────────────────────────────────────────────────────────────────────
// SEE: https://core-math.gitlabpages.inria.fr
// and license info at top of file.

#include "core_math_expand.h"

#include <fenv.h>


typedef union {float f; unsigned u;} b32u32_u;

float cr_acosf(float x){
  const double pi = 0x1.921fb54442d18p+1;
  const float pih = 0x1.921fb6p+1, pil = -0x1.777a5cp-24;
  b32u32_u t = {.f = x};
  int e = (t.u>>23)&0xff;
  double r;
  if(__builtin_expect(e>=127, 0)){
    if(t.u == (0x7fu<<23)) return 0.0f; // x=1
    if(t.u == (0x17fu<<23)) return pih + pil;  // x=-1
    if(e==0xff && (t.u<<9)) return x; // nan
    //errno = EDOM;               // MBR: nobody needs this
    //feraiseexcept(FE_INVALID);  // MBR: don't need for testing
    return __builtin_nanf("1");
  }
  if (e<126){
    static const struct {union{float arg; unsigned uarg;}; float rh, rl;} st[] = {
      {{0x1.110b46p-26f}, 0x1.921fb6p+0f, -0x1.fffffep-25},
      {{0x1.04c444p-12f}, 0x1.920f6ap+0f, -0x1.fffffep-25}
    };
    double z = x, z2 = z*z, z4 = z2*z2, z8 = z4*z4;
    if (__builtin_expect(e<127-12, 0)){
      if(__builtin_expect(t.u == st[0].uarg, 0)) return st[0].rh + st[0].rl;
      return pi/2 - z;
    }
    if(__builtin_expect(t.u == st[1].uarg, 0)) return st[1].rh + st[1].rl;

    static const double c[] =
      {0x1.555555555529cp-3, 0x1.333333337e0ddp-4, 0x1.6db6db3b4465ep-5, 0x1.f1c72e13ac306p-6,
       0x1.6e89cebe06bc4p-6, 0x1.1c6dcf5289094p-6, 0x1.c6dbbcc7c6315p-7, 0x1.8f8dc2615e996p-7,
       0x1.a5833b7bf15e8p-8, 0x1.43f44ace1665cp-6, -0x1.0fb17df881c73p-6, 0x1.07520c026b2d6p-5};
    double c0 = c[0] + z2*c[1];
    double c2 = c[2] + z2*c[3];
    double c4 = c[4] + z2*c[5];
    double c6 = c[6] + z2*c[7];
    double c8 = c[8] + z2*c[9];
    double c10 = c[10] + z2*c[11];
    c0 += c2*z4;
    c4 += c6*z4;
    c8 += c10*z4;
    c0 += z8*(c4 + z8*c8);
    r = (pi/2 - z) - (z*z2)*c0;
  } else {
    float ax = __builtin_fabsf(x);
    double z = 1.0 - ax, s = __builtin_sqrt(z);
    static const double c[] =
      {0x1.6a09e667f3bcbp+0, 0x1.e2b7dddff2db9p-4, 0x1.b27247ab42dbcp-6, 0x1.02995cc4e0744p-7,
       0x1.5ffb0276ec8eap-9, 0x1.033885a928decp-10, 0x1.911f2be23f8c7p-12, 0x1.4c3c55d2437fdp-13,
       0x1.af477e1d7b461p-15, 0x1.abd6bdff67dcbp-15, -0x1.1717e86d0fa28p-16, 0x1.6ff526de46023p-16};
    double z2 = z*z, z4 = z2*z2;
    double c0 = c[0] + z*c[1];
    double c2 = c[2] + z*c[3];
    double c4 = c[4] + z*c[5];
    double c6 = c[6] + z*c[7];
    double c8 = c[8] + z*c[9];
    double c10 = c[10] + z*c[11];
    c0 += c2*z2;
    c4 += c6*z2;
    c8 += z2*c10;
    c0 += z4*(c4 + z4*c8);
    r = s*c0;
    if(x<0.0f) r = pi - r;
  }
  return r;
}

#pragma GCC diagnostic pop


//────────────────────────────────────────────────────────────────────────────────────

float f32_acos_x0_r1(float x) { return f32_acos_x0(x, &f32_asincos_r1); }
float f32_acos_x0_r2(float x) { return f32_acos_x0(x, &f32_asincos_r2); }

float f32_acos_x0_3(float x)  { return f32_acos_x0(x, &f32_asincos_k3); }
//float f32_acos_x1_3(float x)  { return f32_acos_x1(x, &f32_asincos_k3); }
//float f32_acos_x3_3(float x)  { return f32_acos_x0(x, &f32_asincos_k3); }

float f32_acos_x0_4(float x)  { return f32_acos_x0(x, &f32_asincos_k4); }
float f32_acos_px_4(float x)  { return f32_acos_px(x, &f32_asincos_k4); }
//float f32_acos_x3_4(float x)  { return f32_acos_x0(x, &f32_asincos_k4); }

float f32_acos_x0_5(float x)  { return f32_acos_x0(x, &f32_asincos_k5); }
//float f32_acos_x1_5(float x)  { return f32_acos_x1(x, &f32_asincos_k5); }
//float f32_acos_x3_5(float x)  { return f32_acos_x0(x, &f32_asincos_k5); }

float f32_acos_x0_6(float x)  { return f32_acos_x0(x, &f32_asincos_k6); }
//float f32_acos_x1_6(float x)  { return f32_acos_x1(x, &f32_asincos_k6); }
//float f32_acos_x3_6(float x)  { return f32_acos_x0(x, &f32_asincos_k6); }


// abs error: 
float f32_acos_a0(float x)
{
  static const float c0 = 0.17539402770089185f;

  float h = f32_pi/2.f;                      // π/2
  float k = f32_mul_by_sign(h,x);            // sign(x) * π/2
  float a = fabsf(x);                        // |x|
  float t = sqrtf(1.f-a);                    // sqrt(1-|x|)
  float p = fmaf(x,c0,-k);                   // P(x)

  // the cancellation of π/2 terms in the positive
  // case is on purpose. Removing it increase the work
  // needed...notably for handling ±0.
  return h-fmaf(t,p,k);
}

// abs error: 
float f32_acos_a1(float x)
{
  static const float c1 = -0.05344910528338329f;
  static const float c0 =  0.20744683027439512f;

  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,c1,c0),-k);

  return h-fmaf(t,p,k);
}

// abs error: 
float f32_acos_a2(float x)
{
  static const float c2 =  0.021641525571976804f;
  static const float c1 = -0.07798149543591731f;
  static const float c0 =  0.21330134383396135f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,c2,c1),c0),-k);

  return h-fmaf(t,p,k);
}

// abs error: 
float f32_acos_a3(float x)
{
  static const float c3 = -0.010044210004718978f; 
  static const float c2 =  0.03822427750032387f;
  static const float c1 = -0.0860213463845701f;
  static const float c0 =  0.2143665611826344f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,c3,c2),c1),c0),-k);

  return h-fmaf(t,p,k);
}

// abs error: 
float f32_acos_a4(float x)
{
  static const float c4 =  0x1.4bd458p-8f;
  static const float c3 = -0x1.57f016p-6f;
  static const float c2 =  0x1.7ae748p-5f;
  static const float c1 = -0x1.69a5acp-4f;
  static const float c0 =  0x1.b76b9ap-3f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,fmaf(a,c4,c3),c2),c1),c0),-k);

  return h-fmaf(t,p,k);
}

// abs error: 
float f32_acos_a5(float x)
{
  static const float c5 = -0x1.64fd5ap-9f;
  static const float c4 =  0x1.941a0ap-7f;
  static const float c3 = -0x1.cce15cp-6f;
  static const float c2 =  0x1.949beap-5f;
  static const float c1 = -0x1.6c11d2p-4f;
  static const float c0 =  0x1.b77e22p-3f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,fmaf(a,fmaf(a,c5,c4),c3),c2),c1),c0),-k);

  return h-fmaf(t,p,k);
}


func_entry_t func_table[] =
{
  //ENTRY(libm_d),
  //ENTRY(fdlibm_acosfu),
  //ENTRY(fdlibm_acosf),
  //ENTRY(sleef_acosf),
  //ENTRY(amd_acos),          // out 2 ulp and slower than local faithfully rounded
  //ENTRY(f32_acos_),
  //ENTRY(f32_acos),
  ENTRY(libm),
#if 1
  ENTRY(f32_acos_a0),
  ENTRY(f32_acos_a1),
  ENTRY(f32_acos_a2),
  ENTRY(f32_acos_a3),
  ENTRY(f32_acos_a4),
  ENTRY(f32_acos_a5),

  ENTRY(f32_acos_x0_r1),

  ENTRY(f32_acos_x0_3),
  //ENTRY(f32_acos_x1_3),
  //ENTRY(f32_acos_x3_3),
  
  ENTRY(f32_acos_x0_r2),

//ENTRY(f32_acos_px_4),
  ENTRY(f32_acos_x0_4),
  //ENTRY(f32_acos_x1_4),
  //ENTRY(f32_acos_x3_4),

  ENTRY(f32_acos_x0_5),
  //ENTRY(f32_acos_x1_5),
  //ENTRY(f32_acos_x3_5),

  ENTRY(f32_acos_x0_6),
  //ENTRY(f32_acos_x1_6),
  //ENTRY(f32_acos_x3_6),
#else
  ENTRY(f32_acos),
#endif  
};

const char* func_name = "acos";

float cr_func(float x) { return cr_acosf(x); }

#include "common.h"

//────────────────────────────────────────────────────────────────────────────────────
// validating range that f(x) = pi/2
//  f(x) = pi/2 on [0x1.110b46p-26,-0x1.bbbd2ep-24]
void scan_constant(void)
{
  uint32_t ix = f32_to_bits(0x1.p-28f);
  float    r;

  do {
    float x = f32_from_bits(++ix);
    r = cr_func(x);
  } while(r == 0.5f*f32_pi);

  uint32_t x0 = ix-1;
  ix = f32_to_bits(-0x1.p-28f);
  
  do {
    float x = f32_from_bits(++ix);
    r = cr_func(x);
  } while(r == 0.5f*f32_pi);

  uint32_t x1 = ix-1;

  printf("f(x) = pi/2 on [%a,%a]\n", f32_from_bits(x0), f32_from_bits(x1));
}

void test_spot(void)
{
#if 0
  test_1pot_pn(1.f/32.f);
  test_1pot_pn(1.f/16.f);
  test_1pot_pn(1.f/ 8.f);
  test_1pot_pn(1.f/ 4.f);
  test_1pot_pn(1.f/ 2.f);
#else
  //test_scan_peak(f32_acos_a0, f32_to_bits(-1.f/32.f), f32_to_bits(-1.f));
  
  test_force(f32_to_bits(-1.f/32.f), f32_to_bits(-1.f));
  test_force(f32_to_bits( 1.f/32.f), f32_to_bits( 1.f));
#endif
}

void test_all(void)
{
  const uint32_t xa = f32_to_bits( 0x1.110b46p-26f);
  const uint32_t xb = f32_to_bits(-0x1.bbbd2ep-24f);

  // handle constant output range
  test_const_range(0x00000000,xa, 0.5f*f32_pi);
  test_const_range(0x80000000,xb, 0.5f*f32_pi);

  uint32_t x1 = f32_to_bits(1.0f/32.f);

  test_force(xa, x1);
  test_force(xb, x1 ^ 0x80000000);

  test_spot();
}

void test_sanity(void)
{
}

#include "dref.h"

void foo(void)
{
  uint32_t x0 = f32_to_bits(0.125f);
  uint32_t x1 = f32_to_bits(1.f);

  do {
    float x  = f32_from_bits(x0);
    float r0 = f32_acos_x0_4(x);
    float r1 = f32_acos_px_4(x);
    if (r0 != r1) { printf("%f %f %f\n", x,r0,r1); }
  } while(x0++ < x1);
}


int main(int argc, char** argv)
{
  float x = -0.75f;
  x = -1.0f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x));
  x = -0.5f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x));
  x = -0.0f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x));
  x =  0.0f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x));
  x =  0.5f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x));
  x =  1.0f; printf("%f %f\n", f32_acos_a0(x), cr_acosf(x)); //return 0;
  
  //foo(); return 0;
  //scan_constant(); return 0;
  return test_run(argc, argv);
}
