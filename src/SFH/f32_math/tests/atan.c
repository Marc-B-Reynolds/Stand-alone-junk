// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_atanf) version:
// 3) fdlibm (see below)
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

// core identity for range reduction:
//   atan(u) +/- atan(v) = atan( (u +/- v)/(1 -/+ uv)  ) 
//
// plus version on LHS: atan(u)+atan(v)     = atan( (u+v)/(1-uv)  )
// divide through by u: atan( (u/u+v/u)/(1/u-uv/u) ) = atan( (1+v/u)/(1/u-v) )
// limit u->inf:        atan(-(1/v)) = -atan(1/v)
// so:                  atan(inf)+atan(x) = pi/2 - atan(1/x)

//**********************************************************************

float libm(float x) { return atanf(x); }


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
typedef union {double f; uint64_t u;} b64u64_u;
typedef uint64_t u64;

float cr_atanf(float x){
  const double pi2 = 0x1.921fb54442d18p+0;
  b32u32_u t = {.f = x};
  int e = (t.u>>23)&0xff, gt = e>=127;
  if(__builtin_expect(e==0xff, 0)) {
    if(t.u<<9) return x; // nan
    return __builtin_copysign(pi2,(double)x); // inf
  }
  if (__builtin_expect(e<127-13, 0)){
    if (__builtin_expect(e<127-25, 0))
      return __builtin_fmaf(-x, __builtin_fabsf(x), x);
    return __builtin_fmaf(-0x1.5555555555555p-2f*x, x*x, x);
  }
  /* now |x| >= 0x1p-13 */
  double z = x;
  if (gt) z = 1/z; /* gt is non-zero for |x| >= 1 */
  double z2 = z*z, z4 = z2*z2, z8 = z4*z4;
  /* polynomials generated using rminimax
     (https://gitlab.inria.fr/sfilip/rminimax) with the following command:
     ./ratapprox --function="atan(x)" --dom=[0.000122070,1] --num=[x,x^3,x^5,x^7,x^9,x^11,x^13] --den=[1,x^2,x^4,x^6,x^8,x^10,x^12] --output=atanf.sollya --log
     (see output atanf.sollya)
     The coefficient cd[0] was slightly reduced from the original value
     0x1.51eccde075d67p-2 to avoid an exceptional case for |x| = 0x1.1ad646p-4
     and rounding to nearest.
  */
  static const double cn[] =
    {0x1.51eccde075d67p-2, 0x1.a76bb5637f2f2p-1, 0x1.81e0eed20de88p-1,
     0x1.376c8ca67d11dp-2, 0x1.aec7b69202ac6p-5, 0x1.9561899acc73ep-9,
     0x1.bf9fa5b67e6p-16};
  static const double cd[] =
    {0x1.51eccde075d66p-2, 0x1.dfbdd7b392d28p-1, 0x1p+0,
     0x1.fd22bf0e89b54p-2, 0x1.d91ff8b576282p-4, 0x1.653ea99fc9bbp-7,
     0x1.1e7fcc202340ap-12};
  double cn0 = cn[0] + z2*cn[1];
  double cn2 = cn[2] + z2*cn[3];
  double cn4 = cn[4] + z2*cn[5];
  double cn6 = cn[6];
  cn0 += z4*cn2;
  cn4 += z4*cn6;
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
  if (!gt) return r; /* for |x| < 1, (float) r is correctly rounded */

  /* now |x| >= 1 */
  r = __builtin_copysign(0x1.0fdaa22168c23p-7, z) - r + __builtin_copysign(0x1.9p0, z);
  return r;
}

#pragma GCC diagnostic pop

//********************************************************

// s : only the sign bit might be set
// non-zero normal number : 2x
// zero                   : 2^-126
static inline float sgn_mul_2(float x, uint32_t s)
{
  return f32_from_bits(f32_to_bits(x) + (s >> 8));
}

// s : only the sign bit might be set
// non-zero normal number : x/2
// zero-                  : -inf
static inline float sgn_div_2(float x, uint32_t s)
{
  return f32_from_bits(f32_to_bits(x) - (s >> 8));
}


// ~abs error = 8.932298611790113845054565576528975431746079315093e-12
// ~rel error = 2.2745911802486380712014925119468805142178116732138e-11
static inline double f32_atan_d5(double x)
{
  static const double C[] = {0x1.84a1eeff076c7p-5, -0x1.5b30200c976b6p-4, 0x1.c463fc0b3460dp-4, -0x1.247eda4dcaa0cp-3, 0x1.999918a05d2dap-3, -0x1.555554c604107p-2};

  return f64_horner_5(x,C);
}

#include "SFH/f64_pair.h"

float atan_hack(float x)
{
  static const float r0 = 0x1.a8279ap-2f;  // RN( tan( pi/8) )
  static const float r1 = 0x1.3504f4p+1f;  // RN( tan(3pi/8) )
  
  double   t  = (double)x;
  double   a  = fabs(t);
  double   sx = f64_xor(t,a);
   
  uint32_t b0 = a > r0;
  uint32_t b1 = a > r1;
  uint32_t q  = b0 + b1;
  double   d  = (q == 0) ?  1.0 : a;
  double   o  = (q == 1) ?  1.0 : 0.0;
  double   n  = (q == 2) ? -1.0 : a;
  double   hi = 0.25*f64_pi * (double)q;
  double   lo = 0.25*0x1.1a62633145c07p-53 * (double)q;

  t  = (n-o)/(d+o);
  
  double x2 = t*t;

  static const double C[] = { 0x1.999918a05d2ep-3, -0x1.247eda4dcaa97p-3,
                              0x1.c463fc0b3517dp-4, -0x1.5b30200c9aecep-4,
                              0x1.84a1eeff146fcp-5};

  static const double L[] = {-0x1.555554c604107p-2,-0x1.72c8d62f5affp-59};

  double r;
  
  r = C[4];
  r = fma(r,  x2, C[3]);
  r = fma(r,  x2, C[2]);
  r = fma(r,  x2, C[1]);
  r = fma(r,  x2, C[0]);

  r = fma(r,  x2, L[1]) + L[0];
  r = fma(x2,r, 1.f);
  r = hi+fma(t,r,lo);


  //fe_pair R = fe_two_mul(p,x2);
  
  return (float)f64_xor(r,sx);
}




static inline float f32_atan_dk(double x, double a)
{
  // atan(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const double C[] = {-0x1.55554928193eep-2, 0x1.9991961ac575p-3,
                             -0x1.23b522de969c6p-3, 0x1.b1ec3088526bcp-4,
                             -0x1.f1ed8c3145b3p-5};
  static const double L[] = {-0x1.555554c604107p-2,-0x1.72c8d62f5affp-59};  

  double x2 = x*x;
  double r;
  
  r = C[4];
  r = fma(r,  x2, C[3]);
  r = fma(r,  x2, C[2]);
  r = fma(r,  x2, C[1]);
  r = fma(r,  x2, C[0]);
  r = fma(r,  x2, 1.f);
  r = fma(r,  x,  a);
  
  return (float)r;
}

static inline float f32_atan_x(float x)
{
  static const double pi_hi     = 0x1.921fb54442d18p1;
  static const double pi_lo     = 0x1.1a62633145c07p-53;
  static const float  tan_3pi_8 = 2.414213657379150390625f;
  static const float  tan_pi_8  = 0.4142135623730950f;

  uint32_t sx = f32_sign_bit(x);
  
  x = fabsf(x);

  double y  = (double)x;
  double lo = 0.f;
  double hi = 0.f;

  // x > tan(3π/8)
  if (x > tan_3pi_8) {
    hi = pi_hi/2.0;
    lo = pi_lo/2.0;
    y  = -(1.0/y);
  }
  // x > tan(π/8)
  else if (x > tan_pi_8) {
    hi = pi_hi/4.0;
    lo = pi_lo/4.0;
    y  = (y-1.0)/(y+1.0);
  }

  double r = hi + (f32_atan_dk(x,0) + lo);
  
  return f32_mulsign((float)r,sx);
}




// atan(x) : x on [-tan(π/8),tan(π/8)]
static inline float f32_atan_k(float x)
{
  // atan(x) ≈ x³ P(x²) + x
  static const float C[] = {-0x1.555454p-2f, 0x1.9924dap-3f, -0x1.1c380cp-3f, 0x1.49e74cp-4f};

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0]);  // P(x²)
  r = fmaf(r*x,x2, x);     // x P(x²) x² + x
  
  return r;
}

// faithfully rounded: max error = 1 ulp
static inline float f32_atan(float x)
{
  static const float pi_hi     =  3.1415927410125732421875f;
  static const float pi_lo     = -8.74227765734758577309548854827880859375e-8f;
  static const float tan_3pi_8 =  2.414213657379150390625f;
  static const float tan_pi_8  =  0.4142135623730950f;
  
  uint32_t sx = f32_sign_bit(x);
  float    lo = 0.f;
  float    hi = 0.f;
  
  x = fabsf(x);

  // x > tan(3π/8)
  if (x > tan_3pi_8) {
    hi = pi_hi/2.f;
    lo = pi_lo/2.f;
    x  = -(1.f/x);
  }
  // x > tan(π/8)
  else if (x > tan_pi_8) {
    hi = pi_hi/4.f;
    lo = pi_lo/4.f;
    x  = (float)(((double)x-1.0)/((double)x+1.0));
  }

  float r = hi + (f32_atan_k(x) + lo);
  
  return f32_mulsign(r,sx);
}


//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
//ENTRY(atan_hack),
  ENTRY(f32_atan),
//ENTRY(f32_atan_x)  
};

const char* func_name = "atan";

float cr_func(float x) { return cr_atanf(x); }

#include "common.h"

//********************************************************

// f(x) = pi/2 for x > 0x1.e00a3p+25 (4c700518)
void scan_constant(void)
{
  uint32_t ix = f32_to_bits(0x1.0p50f);
  float r;

  do {
    float x = f32_from_bits(--ix);
    r = cr_func(x);
  } while(r == 0.5f*f32_pi);

  ix++;
  
  printf("f(x) = pi/2 for x > %a (%08x)\n", f32_from_bits(ix),ix);
}


// f(x) = x on [-0x1.713744p-12,0x1.713744p-12] [b9b89ba2,39b89ba2]
void scan_linear(void)
{
  uint32_t ix = 0;
  float    x,cr;

  do {
    x = f32_from_bits(++ix);
    cr = cr_func(x);
  } while(x == cr);

  uint32_t hx = ix-1;

  ix = 0x80000000;
  do {
    x = f32_from_bits(++ix);
    cr = cr_func(x);
  } while(x == cr);

  uint32_t lx = ix-1;

  printf("f(x) = x on [%a,%a] [%08x,%08x]\n",
	 f32_from_bits(lx), f32_from_bits(hx),lx,hx);
}


void test_spot(void)
{
  test_sample_positive_finite(0x000fffff);
  test_1pot(1.f/ 4.f);
  test_1pot(1.f/ 2.f);
}

void test_all(void)
{
  uint32_t x0 = 0;
  uint32_t x1 = 0x39b89ba2;
  
  test_linear_range(x0, x1, 1.f);

  x0 = x1+1; x1=f32_to_bits(1.f/64.f);
  test_force(x0,x1);

  // break-down the interior a bit. probably overkill WRT breakdown
  test_1pot(1.f/64.f);
  test_1pot(1.f/32.f);
  test_1pot(1.f/16.f);
  test_1pot(1.f/ 8.f);
  test_1pot(1.f/ 4.f);
  test_1pot(1.f/ 2.f);
  test_1pot(1.f);
  test_1pot(2.f);
  test_1pot(4.f);
  test_1pot(8.f);
  test_1pot(16.f);
  test_1pot(32.f);
  test_1pot(64.f);

  // remaining up until hitting constant output range
  x0 = 0x43000000; x1= 0x4c700518;
  test_force(x0,x1-1);
  
  test_const_range(x1, 0x7f7fffff, 0.5f*f32_pi);
}


void test_sanity(void)
{
  //test_sanity_odd();
}

int main(int argc, char** argv)
{
  float a = sgn_div_2(1.f, 0x80000000);
  float b = sgn_div_2(0.f, 0x80000000);
  printf("%f %f (%a %a)\n", a,b,a,b);

  //scan_constant();
  //scan_linear();
  return test_run(argc, argv);
}
