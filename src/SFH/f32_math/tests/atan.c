// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_atanf) version:
// 2) sleef  (see below)
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
    z = f32_pi*0.5f; // atanhi[3] + 0x1p-120f;  // MBR: hack 2 exact for big and poop poop to inexact flag
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
//   Copyright Naoki Shibata and contributors 2010 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//#define MLA mlaf
#define MLA fmaf // mbr hack
#define C2V(x) (x)

#define POLY2(x, c1, c0) MLA(x, C2V(c1), C2V(c0))
#define POLY3(x, x2, c2, c1, c0) MLA(x2, C2V(c2), MLA(x, C2V(c1), C2V(c0)))
#define POLY4(x, x2, c3, c2, c1, c0) MLA(x2, MLA(x, C2V(c3), C2V(c2)), MLA(x, C2V(c1), C2V(c0)))
#define POLY5(x, x2, x4, c4, c3, c2, c1, c0) MLA(x4, C2V(c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY6(x, x2, x4, c5, c4, c3, c2, c1, c0) MLA(x4, POLY2(x, c5, c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY7(x, x2, x4, c6, c5, c4, c3, c2, c1, c0) MLA(x4, POLY3(x, x2, c6, c5, c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0) MLA(x4, POLY4(x, x2, c7, c6, c5, c4), POLY4(x, x2, c3, c2, c1, c0))

// mbr hack
static inline float signf(float x) { return copysignf(1.f,x); }

float sleef_atan(float s) {
  float t, u;
  int q = 0;

  if (signf(s) == -1) { s = -s; q = 2; }
  if (s > 1) { s = 1.0f / s; q |= 1; }

  t = s * s;

  float t2 = t * t, t4 = t2 * t2;
  u = POLY8(t, t2, t4,
	    0.00282363896258175373077393f,
	    -0.0159569028764963150024414f,
	    0.0425049886107444763183594f,
	    -0.0748900920152664184570312f,
	    0.106347933411598205566406f,
	    -0.142027363181114196777344f,
	    0.199926957488059997558594f,
	    -0.333331018686294555664062f);

  t = s + s * (t * u);

  if ((q & 1) != 0) t = 1.570796326794896557998982f - t;
  if ((q & 2) != 0) t = -t;

  return t;
}

//********************************************************
/*
  Cephes Math Library Release 2.2:  June, 1992
  Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
  Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/
// MBR: hacked a bit

float cephes_atan(float xx)
{
  float x, y, z;
  int sign;
  
  x = xx;
  
  /* make argument positive and save the sign */
  if (xx < 0.f) {
    sign = -1;
    x = -xx;
  }
  else {
    sign = 1;
    x = xx;
  }

  /* range reduction */
  if (x > 2.414213562373095f) { /* tan 3pi/8 */
    y = f32_pi/2.f;//PIO2F;
    x = -(1.f/x);
  }
  else
  if (x > 0.4142135623730950f) { /* tan pi/8 */
    y = f32_pi/4.f;//PIO4F;
    x = (x-1.f)/(x+1.f);
  }
  else
    y = 0.f;
  
  z = x * x;
  y +=
    (((8.05374449538e-2f  * z
     - 1.38776856032E-1f) * z
     + 1.99777106478E-1f) * z
     - 3.33329491539E-1f) * z * x
     + x;
  
  if (sign < 0)
    y = -y;
  
  return y;
}

//********************************************************
// hacked version of
/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

/* Contains implementation of float atanf(float x)
 *
 * sign = sign(x)
 * x = abs(x)
 *
 * Argument Reduction for every x into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities
 * atan(x) = pi/2 - atan(1/x)                when x > 1
 *         = pi/6 + atan(f)                  when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * All elements are approximated by using polynomial of degree 7
 */

//#include <libm/alm_special.h>
//#include <libm/typehelper.h>
//#include <libm/amd_funcs_internal.h>
//#include <libm/poly.h>

static struct{
            float sqrt3, range, piby_2, unit;
            unsigned int mask;
            float pi[4], poly_atanf[3];
            } atanf_data = {
                .sqrt3  = 0x1.bb67aep0,
                .range  = 0x1.126146p-2,
                .piby_2 = 0x1.921fb6p0,
                .unit   = 1.0f,
                .mask   = 0x7FFFFFFFU,
                .pi     = {
                            0,
                            0x1.0c1524p-1,
                            0x1.921fb6p0,
                            0x1.0c1524p0,
                            },
                .poly_atanf = {
                            -0x1.5552f2p-2,
                            0x1.9848ap-3,
                            -0x1.066ac8p-3,
                            },
                        };

#define SQRT3  atanf_data.sqrt3
#define RANGE  atanf_data.range
#define PI     atanf_data.pi
#define PIBY_2 atanf_data.piby_2
#define UNIT   atanf_data.unit
#define SIGN_MASK   atanf_data.mask

#define C0 atanf_data.poly_atanf[0]
#define C1 atanf_data.poly_atanf[1]
#define C2 atanf_data.poly_atanf[2]

// map to local funcs/macros
#define asuint32 f32_to_bits
#define asfloat  f32_from_bits


#define POLY_EVAL_ODD_7(r, c0, c1, c2) ({                       \
        __typeof(r) r2, r4;                                     \
        __typeof(r) a1, a2, a3, q;                              \
        r2 = r * r;                                             \
        r4 = r2 * r2;                                           \
        a1 = c1 + c2 * r2;                                      \
        a2 = a1 * r4;                                           \
        a3 = 1 + c0 * r2;                                       \
        q = (a2 + a3) * r;                                      \
        q;                                                      \
    })

/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(xi)
 * xi = |xi|
 *
 * Argument reduction: Use the following identities
 *
 * 1. If xi > 1,
 *      atan(xi) = pi/2 - atan(1/xi)
 *
 * 2. If f > (2-sqrt(3)),
 *      atan(x) = pi/6 + atan(f)
 *      where f = (sqrt(3)*xi-1)/(xi+sqrt(3))
 *
 *      atan(f) is calculated using the polynomial of degree 7,
 *
 */
float
amd_atanf(float x)
{
  float F, poly, result;
  uint32_t n = 0;
  uint32_t ux = asuint32(x);
  /* Get sign of input value */
  uint32_t sign = ux & (~SIGN_MASK);
  ux = ux & SIGN_MASK;
  F = asfloat(ux);
  
  if (F > UNIT) {
    F = UNIT / F;
    n = 2;
  }
  
  if (F > RANGE) {
    F = (F * SQRT3 - UNIT) / (SQRT3 + F);
    n++;
  }
  
  poly = POLY_EVAL_ODD_7(F, C0, C1, C2);

  // THIS DIFFERS. USE FMA.
  //poly = F*f32_horner_2(F*F,atanf_data.poly_atanf);
  
  if (n > 1) poly = -poly;
  result = PI[n] + poly;
  
  return asfloat(asuint32(result) ^ sign);
}


//********************************************************


// kernels on [0,] : error numbers listed are for dev reference (sollya) and not actual

// ~abs error = 8.076679210744672984905982912400153259095662054471e-9
// ~rel error = 2.0567094722521001030964742420885101260294581004079e-8
static inline float f32_atan_k3(float x)
{
  static const float C[] = {0x1.49e74cp-4f, -0x1.1c380cp-3f, 0x1.9924dap-3f, -0x1.555454p-2f};

  return f32_horner_3(x,C);
}

// ~abs error = 2.741736165110236716770575626651440565655612147484e-10
// ~rel error = 6.9846742879580123383384312928692470127659109264337e-10
static inline float f32_atan_k4(float x)
{
  static const float C[] = {-0x1.f2f766p-5f, 0x1.b227bp-4f, -0x1.23b9d6p-3f, 0x1.9991e4p-3f, -0x1.55554ap-2f};

  return f32_horner_4(x,C);
}

// ~abs error = 7.5262882772964624710774484778354340144362370998285e-12
// ~rel error = 7.4052469528746873645517541529861785118441042122558e-11
static inline float f32_atan_k5(float x)
{
  static const float C[] = {0x1.768d06p-5f, -0x1.5767cp-4f, 0x1.c39f28p-4f, -0x1.24758ap-3f, 0x1.9998b2p-3f, -0x1.555554p-2f};

  return f32_horner_5(x,C);
}

// ~abs error = 3.6676695776968535550982088802819023440506848021614e-12
// ~rel error = 3.65170102085744197025823312809289822929990067319e-11
static inline float f32_atan_k6(float x)
{
  static const float C[] = {-0x1.fb9356p-5f, 0x1.5bf91cp-4f, -0x1.7fc7cep-4f, 0x1.c8b72ep-4f, -0x1.24a068p-3f, 0x1.999a0ep-3f, -0x1.555556p-2f};

  return f32_horner_6(x,C);
}

float cephes_atan_x(float x)
{
  float a  = fabsf(x);
  float sx = f32_xor(x,a);
  float ah = 0;
  float al = 0;
  float z;
  //double da=0.0;

  /* range reduction */
  if (a > 2.414213562373095f) { /* tan 3pi/8 */
    ah = f32_pi/2.f;
    al = f32_mul_k_pi.l/2.f;
    //da = f64_pi/2.0;
    a  = -(1.f/a);
  }
  else if (a > 0.4142135623730950f) { /* tan pi/8 */
    ah = f32_pi/4.f;
    al = f32_mul_k_pi.l/4.f;
    //da = f64_pi/4.0;
    a  = (a-1.f)/(a+1.f);
  }
  
  z = a*a;

  float r;

#if 0
  // 2|  13106398|   4637048|     82345|         0| 73.524917| 26.013140|  0.461943|
  r = 
    (((8.05374449538e-2f  * z
       - 1.38776856032E-1f) * z
      + 1.99777106478E-1f) * z
     - 3.33329491539E-1f) * z * a
    + al;

  r += ah + a;
#else
#if 1
  // 3|  11619430|   5268786|    906139|     31436|
  //r = a*fmaf(z,f32_atan_k6(z),al); r += ah + a;

  // 2|  12410923|   4993598|    421270|
  //r = z*fmaf(a,f32_atan_k6(z),al); r += ah + a;

  // 2|  13798456|   3967946|     59389|
  //r = a*z*f32_atan_k6(z)+al; r += ah + a;

  // 2|  13802093|   3964565|     59133|
  r = fmaf(z,a*f32_atan_k6(z),al); r += ah + a;

  //
  //double t = fma((double)z,(double)a*(double)f32_atan_k4(z),da)+(double)a; r =(float)t;


#else  
  //r = a*z*f32_atan_k5(z)+al;

  float p = f32_atan_k6(z);

  r  = fmaf(z,p,al);
  r  = fmaf(a,r,a);
  r += ah;
#endif  
#endif  


  
  return f32_xor(r,sx);
}

// kernels on [0,] : error numbers listed are for dev reference (sollya) and not actual

// ~abs error = 8.064294902448633802328275450934537763902190604841e-9
// ~rel error = 2.05355587104842068064381844868281860709965642641417e-8
static inline double f32_atan_d3(double x)
{
  static const double C[] = {0x1.49e1673feaeb1p-4, -0x1.1c370119bfaa1p-3, 0x1.9924bc80dfb1ep-3, -0x1.555453813053p-2};

  return f64_horner_3(x,C);
}

// ~abs error = 2.6439710754096367399898726267505237083406045059501e-10
// ~rel error = 6.7328404456209885195014351908340644339366945112152e-10
static inline double f32_atan_d4(double x)
{
  static const double C[] = {-0x1.f1ed8c3145b3p-5, 0x1.b1ec3088526bcp-4, -0x1.23b522de969c6p-3, 0x1.9991961ac575p-3, -0x1.55554928193eep-2};

  return f64_horner_4(x,C);
}

// ~abs error = 8.932298611790113845054565576528975431746079315093e-12
// ~rel error = 2.2745911802486380712014925119468805142178116732138e-11
static inline double f32_atan_d5(double x)
{
  static const double C[] = {0x1.84a1eeff076c7p-5, -0x1.5b30200c976b6p-4, 0x1.c463fc0b3460dp-4, -0x1.247eda4dcaa0cp-3, 0x1.999918a05d2dap-3, -0x1.555554c604107p-2};

  return f64_horner_5(x,C);
}

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


float atan_hack_(float x)
{
#if 0  
  float  a  = fabsf(x);
  float  sx = f32_xor(x,a);
  double t  = (double)a;
  double c  = f64_pi;
  double o  = 0.0;
  double m  = 0.0;
  double n;
  double d;

#if 1
  o = (a > 0x1.a8279ap-2f) ? 1.0 : 0.0;
#else
#endif  

  n = t;
  d = 1.0;
  
  // range reduction
  if (a > 0x1.3504f4p1f) { /* tan 3pi/8 */
    m = 0.5;
    n = -1.0;
    d = t;
  }
  else if (a > 0x1.a8279ap-2f) { /* tan pi/8 */
    m = 0.25;
    n = t-o;
    d = t+o;
  }

  t  = n/d;  // temp hack
  c *= m;
  
  double t2 = t*t;
  double p  = f32_atan_d5(t2);
  double r;

  r = fma(t2,p, 1.f);
  r = fma(t,r,c);

  return f32_xor((float)r,sx);
#else
  static const float r0 = 0x1.a8279ap-2f;
  //static const float r1 = 0x1.3504f4p+1f;
  
  float  a  = fabsf(x);
  float  sx = f32_xor(x,a);
  double t  = (double)a;
  double c  = f64_pi;
  double o  = 0.0;
  double m  = 0.0;
  double n;
  double d;

  //uint64_t m0 = f32_sign_mask_u64(a-0x1.a8279ap-2f);
  //uint64_t m1 = f32_sign_mask_u64(a-0x1.3504f4p+1f);
  //double c = f64_from_bits(f64_to_bits(f64_pi) & m0);
  
#if 1
  o = (a > r0) ? 1.0 : 0.0;
#else
#endif  

  n = t;
  d = 1.0;
  
  // range reduction
  if (a > 0x1.3504f4p1f) { /* tan 3pi/8 */
    m = 0.5;
    n = -1.0;
    d = t;
  }
  else if (a > 0x1.a8279ap-2f) { /* tan pi/8 */
    m = 0.25;
    n = t-o;
    d = t+o;
  }

  t  = n/d;  // temp hack
  c *= m;
  
  double t2 = t*t;
  double p  = f32_atan_d5(t2);
  double r;

  r = fma(t2,p, 1.f);
  r = fma(t,r,c);

  return f32_xor((float)r,sx);
#endif  
}

float atan_hack_prev(float x)
{
  static const float r0 = 0x1.a8279ap-2f;  // RN( tan( pi/8) )
  static const float r1 = 0x1.3504f4p+1f;  // RN( tan(3pi/8) )
  
  float    a  = fabsf(x);
  float    sx = f32_xor(x,a);
  double   t  = (double)a;
  uint32_t b0 = a > r0;
  uint32_t b1 = a > r1;
  uint32_t q  = b0 + b1;
  double   d  = (q == 0) ?  1.0 : t;
  double   n  = (q == 2) ? -1.0 : t;
  double   o  = (q == 1) ?  1.0 : 0.0;
  double   c  = 0.25*f64_pi * (double)q;

  t  = (n-o)/(d+o);
  
  double t2 = t*t;
  double p  = f32_atan_d5(t2);
  double r;

  r = fma(t2,p, 1.f);
  r = fma(t,r,c);

  return f32_xor((float)r,sx);
}


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
  double   c  = 0.25*f64_pi * (double)q;

  t  = (n-o)/(d+o);
  
  double t2 = t*t;
  double p  = f32_atan_d5(t2);
  double r;

  r = fma(t2,p, 1.f);
  r = fma(t,r,c);

  return (float)f64_xor(r,sx);
}





//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
  //ENTRY(fdlibm_atan),
  //ENTRY(cephes_atan),
  //ENTRY(cephes_atan_x),
  //ENTRY(sleef_atan),
  //ENTRY(atan_hack),
  ENTRY(amd_atanf),
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
  test_1pot(1.f/ 4.f);
  test_1pot(1.f/ 2.f);
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
