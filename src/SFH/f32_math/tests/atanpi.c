// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_atanpif) version:
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
#include "util.h"

//**********************************************************************

// |         func|max ULP|        CR|        FR|  2 ULP|> 2 ULP|       CR%|      FR%|  2 ULP%|         abs|
// |         ---:|   ---:|      ---:|      ---:|   ---:|   ---:|      ---:|     ---:|    ---:|        ---:|
// |         libm|      2|1689096635| 448701781|1296625|      0| 78.963141|20.976243|0.060616|5.960464e-08|
// |   f32_atanpi|      2|2099117328|  39939254|  38459|      0| 98.131092| 1.867110|0.001798|2.980232e-08|
// |f32_atanpi_fr|      1|2132579049|   6515992|      0|      0| 99.695386| 0.304614|0.000000|2.980232e-08|


float libm(float x) { return atanf(x)/((float)M_PI); }


// atanpi(x) + a : x on [-tan(π/8),tan(π/8)]
static inline float f32_atanpi_k4(float x, float a)
{
  // atanpi(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const float C[] = {-0x1.55544cp-2f, 0x1.9922fp-3f, -0x1.1c267p-3f, 0x1.4982b4p-4f};
  static const float ipi = 0x1.45f306p-2f;  // 1/π

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0]);
  r = fmaf(r*x,x2, x);
  r = fmaf(r, ipi, a);
  //r = f32_up_madd(f32_mul_k_pi_i, r,a); // r/π + a
  
  return r;
}

// atan(x)/π : max error = 2 ulp
static inline float f32_atanpi(float x)
{
  uint32_t sx = f32_sign_bit(x);
  float    a  = 0.f;
  
  x = fabsf(x);

  // x > tan(3π/8)
  if (x > 2.414213657379150390625f) {
    a = 0.5f;
    x = -(1.f/x);
  }
  // x > tan(π/8)
  else if (x > 0.4142135623730950f) {
    a = 0.25f;
    x = (x-1.f)/(x+1.f);
  }
  
  return f32_mulsign(f32_atanpi_k4(x,a),sx);
}


// for binary32 but internal computation in binary64
static inline float f32_atanpi_k4_fr(double x, double a)
{
  // atanpi(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const double C[] = {-0x1.555453812e9b7p-2, 0x1.9924bc804e959p-3,
                             -0x1.1c3701132244fp-3, 0x1.49e167123486cp-4};

  static const double ipi = 0x1.45f306dc9c883p-2;  // 1/π

  double x2 = x*x;
  double r;
  
  r = C[3];
  r = fma(r,  x2, C[2]);
  r = fma(r,  x2, C[1]);
  r = fma(r,  x2, C[0]);
  r = fma(r*x,x2, x);
  r = fma(ipi, r, a);
  
  return (float)r;
}

// atan(x)/π  (faithfully rounded)
static inline float f32_atanpi_fr(float X)
{
  uint32_t sx = f32_sign_bit(X);
  double   x  = (double)fabsf(X);
  double   a  = 0.f;
  
  // x > tan(3π/8)
  if (x > 2.414213657379150390625f) {
    a = 0.5;
    x = -(1.0/x);
  }
  // x > tan(π/8)
  else if (x > 0.4142135623730950f) {
    a = 0.25;
    x = (x-1.0)/(x+1.0);
  }

  x = f32_atanpi_k4_fr(x,a);
  
  return f32_mulsign((float)x,sx);
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

typedef union {float f; uint32_t u;} b32u32_u;

float cr_atanpif(float x){
  b32u32_u t = {.f = x};
  int32_t e = (t.u>>23)&0xff, gt = e>=127;
  if(__builtin_expect(e>127+24, 0)) {
    float f = __builtin_copysignf(0.5f, x);
    if(__builtin_expect(e==0xff, 0)) {
      if(t.u<<9) return x; // nan
      return f; // inf
    }
    return f - 0x1.45f306p-2f/x;
  }
  double z = x;
  if (__builtin_expect(e<127-13, 0)){
    double sx = z*0x1.45f306dc9c883p-2;
    if (__builtin_expect(e<127-25, 0)) return sx;
    return sx - (0x1.5555555555555p-2*sx)*(x*x);
  }
  unsigned ax = t.u&(~0u>>1);
  if(__builtin_expect(ax == 0x3fa267ddu, 0)) return __builtin_copysignf(0x1.267004p-2f,x) - __builtin_copysignf(0x1p-55f,x);
  if(__builtin_expect(ax == 0x3f693531u, 0)) return __builtin_copysignf(0x1.e1a662p-3f,x) + __builtin_copysignf(0x1p-28f,x);
  if(__builtin_expect(ax == 0x3f800000u, 0)) return __builtin_copysignf(0x1p-2f,x);
  if(gt) z = 1/z;
  double z2 = z*z, z4 = z2*z2, z8 = z4*z4;
  static const double cn[] =
    {0x1.45f306dc9c882p-2, 0x1.733b561bc23d5p-1, 0x1.28d9805bdfbf2p-1,
     0x1.8c3ba966ae287p-3, 0x1.94a7f81ee634bp-6, 0x1.a6bbf6127a6dfp-11};
  static const double cd[] =
    {0x1p+0, 0x1.4e3b3ecc2518fp+1, 0x1.3ef4a360ff063p+1, 0x1.0f1dc55bad551p+0,
     0x1.8da0fecc018a4p-3, 0x1.8fa87803776bfp-7, 0x1.dadf2ca0acb43p-14};
  double cn0 = cn[0] + z2*cn[1];
  double cn2 = cn[2] + z2*cn[3];
  double cn4 = cn[4] + z2*cn[5];
  cn0 += z4*cn2;
  cn0 += z8*cn4;
  cn0 *= z;
  double cd0 = cd[0] + z2*cd[1];
  double cd2 = cd[2] + z2*cd[3];
  double cd4 = cd[4] + z2*cd[5];
  double cd6 = cd[6];
  cd0 += z4*cd2;
  cd4 += z4*cd6;
  cd0 += z8*cd4;
  double r = cn0/cd0;
  if (gt) r = __builtin_copysign(0.5, z) - r;
  return r;
}

#pragma GCC diagnostic pop

//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
  ENTRY(f32_atanpi),
  ENTRY(f32_atanpi_fr),
};

const char* func_name = "atanpi";

float cr_func(float x) { return cr_atanpif(x); }

#include "common.h"

//********************************************************

// f(x) = 1/2 for x > 0x1.45f308p+24 (4ba2f984)
void scan_constant(void)
{
  uint32_t ix = f32_to_bits(0x1.0p25f);
  float r;

  do {
    float x = f32_from_bits(--ix);
    r = cr_func(x);
  } while(r == 0.5f);

  ix++;
  
  printf("f(x) = 1/2 for x > %a (%08x)\n", f32_from_bits(ix),ix);
}

// f(x) = x/pi up to 0x1.4665dp-25 332332e8
void scan_linear(void) {
  uint32_t ix = 0;
  float    r,cr;

  do {
    float x = f32_from_bits(++ix);
    r = (float)f64_up_mul(f64_mul_k_pi_i, (double)x);
    cr = cr_func(x);
  } while(r == cr);

  ix--;

  printf("f(x) = x/pi up to %a %08x\n", f32_from_bits(ix),ix);
}


void test_spot(void)
{
}

void test_all(void)
{
  uint32_t x0 = 0;
  uint32_t x1 = 0x332332e8;

#if 1
  test_linear_range_dp_up(x0, x1, f64_mul_k_pi_i);

  x0 = x1+1; x1=f32_to_bits(1.f/64.f);
  test_force(x0,x1);
#endif  

  // break-down the interior a bit. probably overkill WRT breakdown
  test_1pot(1.f/64.f);
  test_1pot(1.f/32.f);
  test_1pot(1.f/16.f);
  test_1pot(1.f/ 8.f);
  test_1pot(1.f/ 4.f);
#if 1
  test_1pot(1.f/ 2.f);
  test_1pot(1.f);
  test_1pot(2.f);
  test_1pot(4.f);
  test_1pot(8.f);
  test_1pot(16.f);
  test_1pot(32.f);
  test_1pot(64.f);

  // remaining up until hitting constant output range
  x0 = 0x43000000; x1= 0x4ba2f983;
  test_force(x0,x1);
  
  test_const_range(0x4ba2f984, 0x7f7fffff, 0.5f);
#endif  
}

void test_sanity(void)
{
  test_sanity_odd();
}

int main(int argc, char** argv)
{
  //scan_constant();
  //scan_linear();
  
  return test_run(argc, argv);
}
