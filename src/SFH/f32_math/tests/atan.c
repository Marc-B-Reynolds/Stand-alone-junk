// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_atanf) version:
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
// fdlibm
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

float fdlibm_atan(float x)
{
  static const float atanhi[] = {
    4.6364760399e-01f, /* atan(0.5)hi 0x3eed6338 */
    7.8539812565e-01f, /* atan(1.0)hi 0x3f490fda */
    9.8279368877e-01f, /* atan(1.5)hi 0x3f7b985e */
    1.5707962513e+00f, /* atan(inf)hi 0x3fc90fda */
  };
  
  static const float atanlo[] = {
    5.0121582440e-09f, /* atan(0.5)lo 0x31ac3769 */
    3.7748947079e-08f, /* atan(1.0)lo 0x33222168 */
    3.4473217170e-08f, /* atan(1.5)lo 0x33140fb4 */
    7.5497894159e-08f, /* atan(inf)lo 0x33a22168 */
  };
  
  static const float aT[] = {
     3.3333328366e-01f,
    -1.9999158382e-01f,
     1.4253635705e-01f,
    -1.0648017377e-01f,
     6.1687607318e-02f,
  };
  
  float_t w,s1,s2,z;
  uint32_t ix,sign;
  int id;
  
  //GET_FLOAT_WORD(ix, x);
  ix = f32_to_bits(x);
  sign = ix>>31;
  ix &= 0x7fffffff;
  if (ix >= 0x4c800000) {  /* if |x| >= 2**26 */
    if (isnan(x))
      return x;
    z = atanhi[3] + 0x1p-120f;
    return sign ? -z : z;
  }
  if (ix < 0x3ee00000) {   /* |x| < 0.4375 */
    if (ix < 0x39800000) {  /* |x| < 2**-12 */
      if (ix < 0x00800000)
	/* raise underflow for subnormal x */
	//FORCE_EVAL(x*x);
      return x;
    }
    id = -1;
  } else {
    x = fabsf(x);
    if (ix < 0x3f980000) {  /* |x| < 1.1875 */
      if (ix < 0x3f300000) {  /*  7/16 <= |x| < 11/16 */
	id = 0;
	x = (2.0f*x - 1.0f)/(2.0f + x);
      } else {                /* 11/16 <= |x| < 19/16 */
	id = 1;
	x = (x - 1.0f)/(x + 1.0f);
      }
    } else {
      if (ix < 0x401c0000) {  /* |x| < 2.4375 */
	id = 2;
	x = (x - 1.5f)/(1.0f + 1.5f*x);
      } else {                /* 2.4375 <= |x| < 2**26 */
	id = 3;
	x = -1.0f/x;
      }
    }
  }
  /* end of argument reduction */
  z = x*x;
  w = z*z;
  /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
  s1 = z*(aT[0]+w*(aT[2]+w*aT[4]));
  s2 = w*(aT[1]+w*aT[3]);
  if (id < 0)
    return x - x*(s1+s2);
  z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
  return sign ? -z : z;
}



//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
  ENTRY(fdlibm_atan),
};

const char* func_name = "atan";

float cr_func(float x) { return cr_atanf(x); }

#include "common.h"

//********************************************************

void test_sample_positive_finite(uint32_t n)
{
  printf("\nchecking: %s on LDS sampling of positive finite (n=%08x)\n", func_name, n);

  func_error_t error[LENGTHOF(func_table)] = {{0}};

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {

    // use an additive recurrence to cover the range cheaply
    static const uint32_t A = 2654435769;
    uint32_t u = A;

    while (n > 0) {
      // skip infinites and nans
      if ((u & 0x7f80000) != 0x7f80000) {
	uint32_t ix = u;
	float    x  = f32_from_bits(ix);
	float    r  = func_table[fi].f(x);
	float    cr = cr_func(x);
	test_error_add(error+fi, cr,r);
	n--;

	if (x !=   x)    { printf("what!\n"); }
	if (x == 0.5f*x) { printf("huh? %f\n",x); }
	
      }
      u += A;
    }
  }

  error_to_totals(error);
  error_dump_i(error);
}

// f(x) = x on [-0x1.713744p-12,0x1.713744p-12] [b9b89ba2,39b89ba2]
void scan_linear() {
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

void test_spot()
{
  test_1pot(.25f);
  test_sample_positive_finite(256);
}

void test_all()
{
  test_spot();
}

void test_sanity()
{
  test_sanity_odd();
}

int main(int argc, char** argv)
{
  printf("%a\n", cr_func(0x1.0p128f));
  //scan_linear();
  return test_run(argc, argv);
}
