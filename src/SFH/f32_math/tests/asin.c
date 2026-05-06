// -*- coding: utf-8 -*-

// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_asinf) version:
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "internal/f32_asincos.h"

#include "util.h"

//**********************************************************************

float libm(float x) { return asinf(x); }

float sleef_asinf(float d)
{
  uint32_t ix = f32_to_bits(d);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;
  float    a  = f32_from_bits(ax);
  int      o  = a < 0.5f;
  float    x2 = o ? (d*d) : ((1.f-a)*0.5f);
  float    x  = o ? a     : sqrtf(x2);
  float    u;
  
  u = +0.4197454825e-1f;
  u = fmaf(u, x2, +0.2424046025e-1f);
  u = fmaf(u, x2, +0.4547423869e-1f);
  u = fmaf(u, x2, +0.7495029271e-1f);
  u = fmaf(u, x2, +0.1666677296e+0f);
  u = fmaf(u, x * x2, x);
  
  float r = o ? u : (f32_pi/2.f - 2.f*u);
  r = f32_mulsign(r, sx);
  
  return r;
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

// faithfully rounded
float fdlibm_asinf(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;

  // |x| < 0.5
  if (ax < 0x3f000000)
    return fmaf(x, fdlibm_asincos_k(x*x), x);
  
  // |x| >= 0.5
  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  double t  = sqrt((double)t2);
  float  r  = fdlibm_asincos_k(t2);
  
  x= (float)(f64_half_pi - 2*(t+t*r));
  
  return f32_mulsign(x,sx);
}


float cephes_asinf(float x)
{
  // asin(x) ~= x + x^3 A(x^2)  on [0,1/2]
  
  static const float C[] = {
    4.2163199048e-2f,
    2.4181311049e-2f,
    4.5470025998e-2f,
    7.4953002686e-2f,
    1.6666752422e-1f };  
  
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;
  float    a  = f32_from_bits(ax);
  float    t;
  int flag;
  
  if (a > 0.5f) {
    t = 0.5f * (1.f-a);
    x = sqrtf(t);
    flag = 1;
  }
  else {
    x = a;
    t = x*x;
    flag = 0;
  }

  float r = f32_horner_4(t,C);
  t = fmaf(r*t,x,x);

  if (flag != 0) {
    t = t + t;
    t = f32_add_half_pi(-t);
  }
  
  return f32_mulsign(t,sx);
}

// https://mastodon.gamedev.place/@eniko/115813868508988177
// max abs error ~= 1.140559e-02
// uica: 33.00/33.00 (clang 21.1.0 / gcc 15.2)
float eniko(float x)
{
  float c = 1.f - sqrtf(-fmaf(x,x,-1.f));
  float a = f32_mul_by_sign(f32_pi*0.5,x);
  float r = f32_lerp(x,a,c*c);
  return r;
}

// max abs error ~= 4.261762e-03
// uica: 22.11/21.00  (skylake predicted throughput)
float gross_0(float x)
{
  // Hastings 1955 style except forcing the constant
  // term of P so f(0) & f(1) are exact
  // positive 'x':
  //   π/2 + sqrt(1-x)P(x)
  //   P(x) = c₀x - π/2
  // and fold through by product with sign(x) to handle
  // negative x inputs (and x's become |x|).

  static const float c0 = 0.17539402770089185f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);   // sign(x) * π/2
  float a = fabsf(x);                        // |x|
  float t = sqrtf(1.f-a);                    // sqrt(1-|x|)
  float p = fmaf(x,c0,-k);                   // P(x)
  return fmaf(t,p,k);
}

// max abs error ~= 4.024506e-04
// uica: 22.11/21.00 
float gross_1(float x)
{
  // like above but 2nd order P
  //   P(x) = (c₁x + c₀)x - π/2

  static const float c1 = -0.05344910528338329f;
  static const float c0 =  0.20744683027439512f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,c1,c0),-k);

  return fmaf(t,p,k);
}


// max abs error ~= 4.506111e-05
// uica: 22.78/21.70
float gross_2(float x)
{
  // like above but 3rd order P
  //   P(x) = ((c₂x + c₁)x + c₀)x - π/2

  static const float c2 =  0.021641525571976804f;
  static const float c1 = -0.07798149543591731f;
  static const float c0 =  0.21330134383396135f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,c2,c1),c0),-k);

  return fmaf(t,p,k);
}


// max abs error ~= 5.662441e-06
// uica: 23.00/21.60
float gross_3(float x)
{
  // like above but 4th order P
  //   P(x) = ((((c₃ + c₂)x + x + c₁)x + c₀)x - π/2

  static const float c3 = -0.010044210004718978f; 
  static const float c2 =  0.03822427750032387f;
  static const float c1 = -0.0860213463845701f;
  static const float c0 =  0.2143665611826344f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,c3,c2),c1),c0),-k);

  return fmaf(t,p,k);
}


// expand classic constructions
float asin_x0_k3(float x) { return f32_asin_x0(x, &f32_asincos_k3); }
float asin_x0_k4(float x) { return f32_asin_x0(x, &f32_asincos_k4); }
float asin_x0_k5(float x) { return f32_asin_x0(x, &f32_asincos_k5); }
float asin_x0_k6(float x) { return f32_asin_x0(x, &f32_asincos_k6); }

float asin_x1_k3(float x) { return f32_asin_x1(x, &f32_asincos_k3); }
float asin_x1_k4(float x) { return f32_asin_x1(x, &f32_asincos_k4); }
float asin_x1_k5(float x) { return f32_asin_x1(x, &f32_asincos_k5); }
float asin_x1_k6(float x) { return f32_asin_x1(x, &f32_asincos_k6); }


//**********************************************************************
// SEE: https://core-math.gitlabpages.inria.fr
// and license info at top of file.

// oh my!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wimplicit-float-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include <fenv.h>

typedef union {float f; unsigned u;} b32u32_u;

float cr_asinf(float x){
  const double pi = 0x1.921fb54442d18p+1;
  b32u32_u t = {.f = x};
  int e = (t.u>>23)&0xff;
  double r;
  if(__builtin_expect(e>=127, 0)){
    if(__builtin_fabsf(x)==1.0f) return __builtin_copysign(pi/2, (double)x);
    if(e==0xff && (t.u<<9)) return x; // nan
    //errno = EDOM;               // MBR: SMH
    //feraiseexcept(FE_INVALID);  // MBR: not needed for testing
    return __builtin_nanf("1");
  }
  if (e<126){
    if (__builtin_expect(e<127-12, 0))
      return __builtin_fmaf(x, 0x1p-25, x);
    static const double c[] =
      {0x1.555555555529cp-3, 0x1.333333337e0ddp-4, 0x1.6db6db3b4465ep-5, 0x1.f1c72e13ac306p-6,
       0x1.6e89cebe06bc4p-6, 0x1.1c6dcf5289094p-6, 0x1.c6dbbcc7c6315p-7, 0x1.8f8dc2615e996p-7,
       0x1.a5833b7bf15e8p-8, 0x1.43f44ace1665cp-6, -0x1.0fb17df881c73p-6, 0x1.07520c026b2d6p-5};
    double z = x, z2 = z*z, z4 = z2*z2, z8 = z4*z4;
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
    r = z + (z*z2)*c0;
  } else {
    static const struct {union{float arg; unsigned uarg;}; float rh, rl;} st[] = {
      {{0x1.107434p-1f}, 0x1.1f4b64p-1f, 0x1.fffffep-26f},
      {{0x1.55688ap-1f}, 0x1.75b8a2p-1f, 0x1.c5ca9ap-53f}
    };
    unsigned at = t.u&(~0u>>1), sgn = t.u>>31;
    for(int i=0;i<2;i++) {
      if(__builtin_expect(st[i].uarg == at, 0)){
	if(sgn)
	  return -st[i].rh - st[i].rl;
	else
	  return  st[i].rh + st[i].rl;
      }
    }
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
    r = pi/2 - s*c0;
    r = __builtin_copysign(r,(double)x);
  }
  return r;
}

#pragma GCC diagnostic pop

//********************************************************


func_entry_t func_table[] =
{
#if 0  
  ENTRY(libm),
  ENTRY(fdlibm_asinf),
  //ENTRY(cephes_asinf),
  //ENTRY(sleef_asinf),
  
  ENTRY(asin_x0_k3),
  ENTRY(asin_x0_k4),
  ENTRY(asin_x0_k5),
  ENTRY(asin_x0_k6),

  ENTRY(asin_x1_k3),
  ENTRY(asin_x1_k4),
  ENTRY(asin_x1_k5),
  ENTRY(asin_x1_k6),
#endif
  ENTRY(eniko),
  ENTRY(gross_0),
  ENTRY(gross_1),
  ENTRY(gross_2),
  ENTRY(gross_3),
};

const char* func_name = "asin";

float cr_func(float x) { return cr_asinf(x); }

#include "common.h"

//********************************************************

// asinf(x) = x on |x| < 0x1.d12edp-12
void test_simple(void)
{
  uint32_t xe = f32_to_bits(0x1.d12edp-12f);
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=0; xi<xe; xi++) {
      float x = f32_from_bits(xi);
      float r = func_table[fi].f(x);
      float d = x-r;
      
      if (d == 0.f) { func_error[fi].ulp[0]++; continue; };
      
      uint32_t ulp = f32_ulp_dist_ss(x, r);
      
      if (ulp > func_error[fi].max) { func_error[fi].max = ulp; }
      
      if (ulp > 3) ulp = 3;
      
      func_error[fi].ulp[ulp]++;
    }
  }
}

// clean up testing 
void test_spot(void)
{
  uint32_t x0 = f32_to_bits(0.5f);
  uint32_t x1 = f32_to_bits(1.0f);
  test_force(x0,x1);
}

void test_all(void)
{
  uint32_t x0 = f32_to_bits(0.0f);
  uint32_t x1 = f32_to_bits(0x1.d12edp-12f);

  test_identity_range(x0,x1);
  test_force(x1+1, f32_to_bits(0.5f)-1);
  test_spot();
}

void test_sanity(void)
{
  test_sanity_odd();
}


int main(int argc, char** argv)
{
  //scan(); return 0;
  test_sanity();

  return test_run(argc, argv);
}



