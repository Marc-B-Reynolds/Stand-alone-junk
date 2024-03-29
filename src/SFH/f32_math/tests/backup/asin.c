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


// compile with (or equiv).
// clang -O3 -march=native -Wall -Wextra -Wconversion -Wpedantic -Wno-unused-function -fno-math-errno -ffp-contract=off asinf.c -o asinf -lm

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

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
inline float fdlibm_asincos_k(float x2)
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

//*********************************************

inline float f32_asincos_k3(float x2)
{
  static const float C[] = {
    0x1.993a88p-5f,
    0x1.47f5c4p-5f,
    0x1.34dc56p-4f,
    0x1.554fa2p-3f,
  };

  return f32_horner_3(x2,C);
}

inline float f32_asincos_k4(float x2)
{
  static const float C[] = {
    0x1.5959aap-5f,
    0x1.8c415ap-6f,
    0x1.747b72p-5f,
    0x1.330206p-4f,
    0x1.5555c8p-3f };

  return f32_horner_4(x2,C);
}

inline float f32_asincos_k5(float x2)
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


inline double f32_asincos_k5d(double x2)
{
  // temp hack
  static const double C[] = {
    0x1.33d432p-5f,
    0x1.d7924cp-7f,
    0x1.04af34p-5f,
    0x1.6caab6p-5f,
    0x1.33389ep-4f,
    0x1.55554cp-3f
  };

  return f64_horner_5(x2,C);
}


float asinf_x_fff(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;

  // |x| <= 0.5
  if (ax <= 0x3f000000) {
    double t  = (double)x;
    double t2 = t*t;
    double r  = t2*f32_asincos_k5d(t2);
    return (float)fma(r,x,x);
  }

  // |x| > 0.5
  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  double d2 = (double)t2;
  double t  = -2.0*sqrt(d2); 
  double r  = d2*(double)f32_asincos_k5(t2);
  
  r = fma(r,t,t);
  r = f64_half_pi + r;
  
  return f32_mulsign((float)r,sx);
}


float asinf_x(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;

  // |x| <= 0.5
  if (ax <= 0x3f000000) {
    float x2 = x*x;
    float r  = x2*f32_asincos_k5(x2);
    return fmaf(r,x,x);
  }

  // |x| > 0.5
  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  double d2 = (double)t2;
  double t  = -2.0*sqrt(d2); 
  double r  = d2*(double)f32_asincos_k5(t2);
  
  r = fma(r,t,t);
  r = f64_half_pi + r;
  
  return f32_mulsign((float)r,sx);
}


float asinf_x_(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;
  
  // |x| <= 0.5
  if (ax <= 0x3f000000) {
    float x2 = x*x;
    float r  = x2*f32_asincos_k4(x2);
    return fmaf(r,x,x);
  }

  // |x| > 0.5
#if 0
  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  double d2 = (double)t2;
  double t  = -2.0*sqrt(d2); 
  double r  = d2*(double)f32_asincos_k4(t2);
  
  r = fma(r,t,t);
  r = f64_half_pi + r;
#else
  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  double d2 = (double)t2;
  double t  = -2.0*sqrt(d2); 
  double r  = d2*(double)f32_asincos_k4(t2);
  
  r = fma(r,t,t+f64_half_pi);
#endif  
  
  return f32_mulsign((float)r,sx);
}


float asinf_0(float x)
{
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;

  // |x| < 0.5
  if (ax <= 0x3f000000) {
    float x2 = x*x;
    float r  = x2*f32_asincos_k4(x2);
    return fmaf(r,x,x);
  }

  float  a  = f32_from_bits(ax);
  float  t2 = 0.5f * (1.f-a);
  float  t  = -2.f*sqrtf(t2); 
  float  r  = t2*f32_asincos_k5(t2);
  
  r = fmaf(r,t,t);
  r = f32_add_half_pi(r);

  return f32_mulsign(r,sx);
}


float asinf_1(float x)
{
  static const float C[] = {
    4.2219109833240509033203125e-2f,
    2.414443902671337127685546875e-2f,
    4.547862708568572998046875e-2f,
    7.4952177703380584716796875e-2f,
    0.1666675508022308349609375f
  };
  
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
    t = f32_add_half_pi(-2.f*t);
  }
  
  return f32_mulsign(t,sx);
}


float asinf_2(float x)
{

#if 0
  static const float C[] = {
    3.7575043737888336181640625e-2f,
    1.43922157585620880126953125e-2f,
    3.18216383457183837890625e-2f,
    4.4515006244182586669921875e-2f,
    7.5005166232585906982421875e-2f,
    0.1666665971279144287109375f
  };
#else
  // asin_3(epbasis5);
  static const float C[] = {
    3.4441731870174407958984375e-2f,
    1.664256118237972259521484375e-2f,
    3.12243886291980743408203125e-2f,
    4.45862747728824615478515625e-2f,
    7.50015079975128173828125e-2f,
    0.1666666567325592041015625f
  };
#endif  
  
  uint32_t ix = f32_to_bits(x);
  uint32_t ax = ix & 0x7fffffff;
  uint32_t sx = ix ^ ax;
  float    a  = f32_from_bits(ax);
  float    s;
  int flag;
  
  if (a > 0.5f) {
    s = 0.5f * (1.f-a);
    x = sqrtf(s);
    flag = 1;
  }
  else {
    x = a;
    s = x*x;
    flag = 0;
  }

  float r = f32_horner_5(s,C);
  r = fmaf(r,x*s,x);

  if (flag != 0) {
    r = f32_add_half_pi(-2.f*r);
  }
  
  return f32_mulsign(r,sx);
}




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
    //errno = EDOM;
    feraiseexcept(FE_INVALID);
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
  ENTRY(libm),
  ENTRY(fdlibm_asinf),
  //ENTRY(cephes_asinf),
  //ENTRY(sleef_asinf),
  ENTRY(asinf_x),
  //ENTRY(asinf_xd),
  ENTRY(asinf_0),
  //ENTRY(asinf_1),
  //ENTRY(asinf_2),
};

const char* func_name = "asin";

float cr_func(float x) { return cr_asinf(x); }

#include "common.h"

//********************************************************

// asinf(x) = x on |x| < 0x1.d12edp-12
void brute_simple()
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

void brute_hi()
{
  uint32_t x0 = f32_to_bits(0.5f);
  uint32_t x1 = f32_to_bits(1.0f);
  brute_force(x0,x1);
}


void brute_lo()
{
  uint32_t x0 = f32_to_bits(0.0f);
  uint32_t x1 = f32_to_bits(0.5f);
  brute_force(x0,x1);
}

void brute_all()
{
  uint32_t x0 = f32_to_bits(0.0f);
  uint32_t x1 = f32_to_bits(1.0f);
  brute_force(x0,x1);
}

void odd_sanity()
{
  printf("\nrunning: minimal sanity check\n");
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {

    for(float x=0.f; x <= 1.f; x += (1.f/1024.f)) {
      float r0 = func_table[fi].f( x);
      float r1 = func_table[fi].f(-x);

      if (r0+r1 != 0) {
	printf("  %s : FAILED -> f(%a) = %a, f(-%a)=%a\n",
	       func_table[fi].name, x,r0,x,r1);
	break;
      }
    }
  }
}


int main(void)
{
  //scan(); return 0;
  odd_sanity();

#if 1
  //brute_lo();
  //brute_hi();
  brute_hi();
  brute_1pot(0.25f);
  brute_1pot(0.125f);
  //brute_1pot(0x1.0p-100f);
  //brute_di();
#else  
  brute_all();
#endif  

  error_dump();

  return 0;
}



