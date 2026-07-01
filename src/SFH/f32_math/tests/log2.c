// Public Domain under http://unlicense.org, see link for details.
//
// nothing here yet.
//

// *****EXCEPT:************************
// 1) reference (cr_log2f) version:
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

#define F32_UTIL_IMPLEMENTATION

#include "internal/f32_math_common.h"
#include "util.h"

//**********************************************************************

float libm(float x) { return log2f(x); }

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

// Warning: clang also defines __GNUC__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#pragma STDC FENV_ACCESS ON

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;

static __attribute__((noinline)) float as_special(float x){
  b32u32_u t = {.f = x};
  uint32_t ux = t.u, ax = ux<<1;
  if(ax == 0u){ // +/-0.0
#ifdef CORE_MATH_SUPPORT_ERRNO
    errno = ERANGE;
#endif
    return -1.0f/0.0f; // to raise FE_DIVBYZERO
  }
  if(ux == 0x7f800000u) return x; // +inf
  if(ax > 0xff000000u) return x + x; // nan
#ifdef CORE_MATH_SUPPORT_ERRNO
  errno = EDOM;
#endif
  return 0.0f/0.0f; // to raise FE_INVALID and return nan
}

float cr_log2f(float x) {
  static const double ix[] = {
    0x1p+0, 0x1.f81f82p-1, 0x1.f07c1fp-1, 0x1.e9131acp-1,
    0x1.e1e1e1ep-1, 0x1.dae6077p-1, 0x1.d41d41dp-1, 0x1.cd85689p-1,
    0x1.c71c71cp-1, 0x1.c0e0704p-1, 0x1.bacf915p-1, 0x1.b4e81b5p-1,
    0x1.af286bdp-1, 0x1.a98ef6p-1, 0x1.a41a41ap-1, 0x1.9ec8e95p-1,
    0x1.999999ap-1, 0x1.948b0fdp-1, 0x1.8f9c19p-1, 0x1.8acb90fp-1,
    0x1.8618618p-1, 0x1.8181818p-1, 0x1.7d05f41p-1, 0x1.78a4c81p-1,
    0x1.745d174p-1, 0x1.702e05cp-1, 0x1.6c16c17p-1, 0x1.6816817p-1,
    0x1.642c859p-1, 0x1.605816p-1, 0x1.5c9882cp-1, 0x1.58ed231p-1,
    0x1.5555555p-1, 0x1.51d07ebp-1, 0x1.4e5e0a7p-1, 0x1.4afd6ap-1,
    0x1.47ae148p-1, 0x1.446f865p-1, 0x1.4141414p-1, 0x1.3e22cbdp-1,
    0x1.3b13b14p-1, 0x1.3813814p-1, 0x1.3521cfbp-1, 0x1.323e34ap-1,
    0x1.2f684bep-1, 0x1.2c9fb4ep-1, 0x1.29e412ap-1, 0x1.27350b9p-1,
    0x1.2492492p-1, 0x1.21fb781p-1, 0x1.1f7047ep-1, 0x1.1cf06aep-1,
    0x1.1a7b961p-1, 0x1.1811812p-1, 0x1.15b1e5fp-1, 0x1.135c811p-1,
    0x1.1111111p-1, 0x1.0ecf56cp-1, 0x1.0c9715p-1, 0x1.0a6810ap-1,
    0x1.0842108p-1, 0x1.0624dd3p-1, 0x1.041041p-1, 0x1.0204081p-1, 0.5};

  static const double lix[] = {
    0x0p+0, -0x1.6e7966ead8ac5p-6, -0x1.6bad38119a13ap-5, -0x1.0eb389ee9f56p-4,
    -0x1.663f6fc3a678dp-4, -0x1.bc8423d408321p-4, -0x1.08c588e79f8e9p-3, -0x1.32ae9e28fc362p-3,
    -0x1.5c01a3cde7f74p-3, -0x1.84c2bccf005a9p-3, -0x1.acf5e2c156d8fp-3, -0x1.d49ee4b90c47cp-3,
    -0x1.fbc16b67c143bp-3, -0x1.11307dc445fecp-2, -0x1.24407abf4dc03p-2, -0x1.37124cede831fp-2,
    -0x1.49a784a5bc715p-2, -0x1.5c01a3965cc38p-2, -0x1.6e221cc2bb868p-2, -0x1.800a564aa10b6p-2,
    -0x1.91bba8a906b7fp-2, -0x1.a33760adbb56ep-2, -0x1.b47ebf91d417cp-2, -0x1.c592faf028f8ep-2,
    -0x1.d6753e1a43e85p-2, -0x1.e726aa2157f61p-2, -0x1.f7a8567cd1cbdp-2, -0x1.03fda8a93ea1cp-1,
    -0x1.0c10500ed4feep-1, -0x1.140c9fb5a8f7fp-1, -0x1.1bf311daefb44p-1, -0x1.23c41d317edc1p-1,
    -0x1.2b80347f8250cp-1, -0x1.3327c6a752222p-1, -0x1.3abb3fb080128p-1, -0x1.423b07f5114e5p-1,
    -0x1.49a784b14715p-1, -0x1.5101187e9b871p-1, -0x1.5848226c6c7c3p-1, -0x1.5f7cff3de8f2ap-1,
    -0x1.66a008d8ede91p-1, -0x1.6db19694a04aap-1, -0x1.74b1fd6b5e715p-1, -0x1.7ba18f99ce2a5p-1,
    -0x1.82809d4d79ba9p-1, -0x1.894f749cf5047p-1, -0x1.900e615bac2f7p-1, -0x1.96bdad16f516p-1,
    -0x1.9d5d9fe08baefp-1, -0x1.a3ee7f3e4a7ebp-1, -0x1.aa708f4de7fep-1, -0x1.b0e4125ca68fep-1,
    -0x1.b74948f9a72bp-1, -0x1.bda071b77c9e2p-1, -0x1.c3e9ca4193f99p-1, -0x1.ca258dd397814p-1,
    -0x1.d053f6d543325p-1, -0x1.d6753dfedaa39p-1, -0x1.dc899aa874b31p-1, -0x1.e29142f2e8b3dp-1,
    -0x1.e88c6b41b14aep-1, -0x1.ee7b4718b4414p-1, -0x1.f45e08c87b09p-1, -0x1.fa34e117d8785p-1,
    -0x1p+0
  };
  static const double b[] = {
    0x1.7154765bab2b5p+0, -0x1.71574d6939f6cp-1, 0x1.ec60b584ca47fp-2};
  static const double c[] = {
    0x1.71547652b8314p+0, -0x1.71547652b7f67p-1, 0x1.ec709db872c63p-2,
    -0x1.715476b064cdfp-2, 0x1.277c72c15441fp-2, -0x1.ec4ff35b64c31p-3};

  b32u32_u t = {.f = x};
  uint32_t ux = t.u;
  if(__builtin_expect(ux >= 0x7f800000u, 0)) return as_special(x); // <=-0, nan, inf
  if(__builtin_expect(ux<(1u<<23), 0)){ // subnormal
    if(__builtin_expect(ux == 0u, 0)) return as_special(x); // +0
    int n = __builtin_clz(ux) - 8;
    ux <<= n;
    ux -= n<<23;
  }
  int e = ((int32_t)ux>>23) - 0x7f;
  uint32_t m = ux&(~0u>>9);
  if(__builtin_expect(!m, 0)) return e;
  int32_t j = (m + (1<<(23-7)))>>(23-6);
  b64u64_u xd = {.u = (uint64_t)m<<(52-23) | 0x3ffull<<52};
  double z = xd.f*ix[j] - 1.0, z2 = z*z, el = e - lix[j];
  double f = (el + z*b[0]) + z2*(b[1] + z*b[2]);
  float lb = f , ub = f + 0x1.661p-32;
  if(__builtin_expect(lb==ub, 1)) return lb;
  double c0 = c[0] + z*c[1];
  double c2 = c[2] + z*c[3];
  double c4 = c[4] + z*c[5];
  c0 += z2*(c2 + z2*c4);
  return el + z*c0;
}


#pragma GCC diagnostic pop

//********************************************************
// Copyright (c) 2017-2025, Arm Limited.
// SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception



//********************************************************

//   Cephes Math Library Release 2.2:  June, 1992
//   Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
//   Direct inquiries to 30 Frost Street, Cambridge, MA 02140

#define LOG2EA 0.44269504088896340735992f     // 1/log(2)-1
#define SQRTH  0.70710678118654752440f        // sqrt(2)/2

float polevlf(float x, const float coef[], int N)
{
  float ans;
  const float *p;
  int i;
  
  p = coef;
  ans = *p++;
  
  i = N;
  do
    ans = ans * x  +  *p++;
  while(--i);
  
  return ans;
}

float log2_cephes(float xx)
{
  static float P[] = {
     7.0376836292E-2f,
    -1.1514610310E-1f,
     1.1676998740E-1f,
    -1.2420140846E-1f,
     1.4249322787E-1f,
    -1.6668057665E-1f,
     2.0000714765E-1f,
    -2.4999993993E-1f,
     3.3333331174E-1f
  };
  
  
  float x, y, z;
  int   e;
  
  x = xx;
  x = frexpf(x, &e);
  
  /* logarithm using log(1+x) = x - .5x**2 + x**3 P(x)/Q(x) */

  if (xx == 2.f) { printf("-----{%f %i}----\n",x,e); }
  
  if (x < SQRTH) {
    e -= 1;
    x  = 2.f*x - 1.f;
  }	
  else {
    x = x - 1.f;
  }

  
  z = x*x;
  y = x * ( z * polevlf( x, P, 8 ) );
  y = y - 0.5f * z;   /*  y - 0.5 * x**2  */
  
  
  /* Multiply log of fraction by log2(e)
   * and base 2 exponent by 1
   *
   * ***CAUTION***
   *
   * This sequence of operations is critical and it may
   * be horribly defeated by some compiler optimizers.
   */
  z  = y * LOG2EA;
  z += x * LOG2EA;
  z += y;
  z += x;
  z += (float)e;

  return z;
}

//********************************************************


// x on [1,2]
// |    func|   max ULP|        CR|        FR|     2 ULP|   > 2 ULP|       CR%|       FR%|    2 ULP%|  > 2 ULP%|         abs|
// |    ---:|      ---:|      ---:|      ---:|      ---:|      ---:|      ---:|      ---:|      ---:|      ---:|        ---:|
// | log2_h2|      5753|       609|      1206|      1232|   8385561|  0.007260|  0.014377|  0.014687| 99.963677|3.429055e-04|
// | log2_h3|       123|     32866|     62333|     60612|   8232797|  0.391793|  0.743067|  0.722551| 98.142588|7.331371e-06|
// | log2_h4|         6|   1487483|   3204626|   2635574|   1060925| 17.732179| 38.202119| 31.418490| 12.647212|2.980232e-07|
// | log2_h5|         3|   4307807|   3759956|    319421|      1424| 51.353061| 44.822168|  3.807795|  0.016975|1.788139e-07|

float log2_h2(float x)
{
  static const float c[] = { 0x1.07f88p0f, 0x1.71340cp1f };

  float e  = (float)((int32_t)((f32_to_bits(x) >> 23)-127));
  float n  = f32_zero_exponent(x);
  float t  = (n-1.f)/(n+1.f);
  float t2 = t*t;
  float r  = c[0];

  r = fmaf(r,t2,c[1]);
  r = fmaf(r,t ,e);

  return r;
}

float log2_h3(float x)
{
  static const float c[] = {0x1.4e953ep-1f, 0x1.eac06ep-1f, 0x1.715524p1f };

  float e  = (float)((int32_t)((f32_to_bits(x) >> 23)-127));
  float n  = f32_zero_exponent(x);
  float t  = (n-1.f)/(n+1.f);
  float t2 = t*t;
  float r  = c[0];

  r = fmaf(r,t2,c[1]);
  r = fmaf(r,t2,c[2]);
  r = fmaf(r,t ,e);
  
  return r;
}

float log2_h4(float x)
{
  static const float c[] = { 0x1.f9e8b4p-2, 0x1.24677cp-1f, 0x1.ec82a8p-1f, 0x1.715472p1f };

  float e  = (float)((int32_t)((f32_to_bits(x) >> 23)-127));
  float n  = f32_zero_exponent(x);
  float t  = (n-1.f)/(n+1.f);
  float t2 = t*t;
  float r  = c[0];

  r = fmaf(r,t2,c[1]);
  r = fmaf(r,t2,c[2]);
  r = fmaf(r,t2,c[3]);
  r = fmaf(r,t ,e);
  
  return r;
}

float log2_h5(float x)
{
  static const float c[] = { 0x1.96937p-2f, 0x1.9f7d46p-2f, 0x1.278c1ep-1f, 0x1.ec70bep-1f, 0x1.715476p1f };

  float e  = (float)((int32_t)((f32_to_bits(x) >> 23)-127));
  float n  = f32_zero_exponent(x);
  float t  = (n-1.f)/(n+1.f);
  float t2 = t*t;
  float r  = c[0];

  r = fmaf(r,t2,c[1]);
  r = fmaf(r,t2,c[2]);
  r = fmaf(r,t2,c[3]);
  r = fmaf(r,t2,c[4]);
  r = fmaf(r,t ,e);
  
  return r;
}


func_entry_t func_table[] =
{
  //ENTRY(libm),

  // these are temp hacks for testing them..don't belong
  ENTRY(log2_cephes),
  ENTRY(log2_h2),
  ENTRY(log2_h3),
  ENTRY(log2_h4),
  ENTRY(log2_h5),
};

const char* func_name = "log2f";

float cr_func(float x) { return cr_log2f(x); }

#include "common.h"

//********************************************************

// ALL JUNK BELOW HERE

void scan_limits(void)
{
  uint32_t ix = 0;
  float    r,x;

  // -inf (just 0 on positive)
  do {
    x = f32_from_bits(ix++);
    r = cr_func(x);
  } while(r == -f32_inf);

  printf("f(x) = -inf for x < %a (%08x)\n", x,ix-1);

  // normal negative range
  do {
    x = f32_from_bits(ix++);
    r = cr_func(x);
  } while(r <= -f32_min_denormal);
  
  printf("f(x) = normal for x < %a (%08x)\n", x,ix-1);

#if 0
  
  // check denormal range (obvious answer here)
  do {
    x = f32_from_bits(ix--);
    r = cr_func(x);
  } while(r < f32_min_normal);

  printf("f(x) denormal until %f (%08x)\n", x,ix+1);

  ix = f32_to_bits(125.f);

  // x >= 128 is inf (obvious again)
  do {
    x = f32_from_bits(ix++);
    r = cr_func(x);
  } while(r < f32_inf);

  printf("f(x) finite until %f (%08x)\n", x,ix-1);
#endif  
}


void test_sanity(void)
{
}

void test_ge_1(void)
{
  // break down into log log ranges: [1,2), [2,
  
  uint32_t x0 = 0x3f800000;  // 1
  uint32_t x1 = 0x3fbfffff;  // 1.5 - ulp
  
  test_force(x0,x1);

  
}

void test_spot(void)
{
#if 0  
  uint32_t x0 = 0x3f800000;  // 1
  uint32_t x1 = 0x3fbfffff;  // 1.5 - ulp

  x1 = f32_to_bits(1.125f)-1;
  
  test_force(x0,x1);

  x0 = x1+1;
  x1 = f32_to_bits(2.f)-1;
  test_force(x0,x1);
#else
  float x = f32_min_normal;

  x = 0.5f;

  do {
    test_1pot(x);
    x *= 2.f;
  } while(x < f32_inf);
#endif  
}




void test_all(void)
{
  uint32_t x0 = 0;
  uint32_t x1 = f32_to_bits(f32_min_normal)-1;

  test_force(x0,x1);

  x0 = x1+1;
  x1 = f32_to_bits(f32_inf);

  test_force(x0,x1);
}

int main(int argc, char** argv)
{
  //scan_limits(); return 0;

  printf("%a %a\n", log2f(0.5f), log2_h2(0.5f));
  
  return test_run(argc, argv);
}
