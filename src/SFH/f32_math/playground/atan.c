// -*- coding: utf-8 -*-
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "SFH/compiler_hints.h"
#include "SFH/f64_util.h"
#include "SFH/f32_util.h"

#define PRNG_IMPLEMENTATION
#include "prng_small_global.h"

#include "SFH/f32_horner.h"

#include "SFH/f32_math/internal/f32_atan.h"

//────────────────────────────────────────────────────────────────────────────────────
// rational approximation kernels for x on [-1,1]  (in binary32)
// 

// ratapprox --function="(atan(x)-x)/x" --dom=[-1,1] --denF=[SG] --numF=[SG] --num=[x^2,x^4,x^6] --den=[1,x^2,x^4,x^6] --output=atan_3_3.sollya --log

static inline float f32_atan_rd_k(float a)
{
  double x = (double)a;
  double x2 = x*x;

  static const double N[] = {-0x3.97489796d0896p-4,-0x3.2dc3b8351325ap-4,-0x7.ab8ca571d35ep-8};
  static const double D[] = { 0xa.c5d9c94ca163p-4,0x6.6be5a606541ecp-4,0x9.5c13ff8f20cf8p-8};

  double n = N[2];
  double d = D[2];

  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]) * x2;

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.f);
  d = fma(d, x2, D[0]);

  return (float)fma(x, n/d, x);  
}




// atan(x) for x on [-1,1] (unit extent)
float f32_atan_ue(float x)
{
  // x P(x²)/Q(x²) + x
  return fmaf(x, f32_atan_r22_k(x*x), x);
  //return f32_atan_rd_k(x);
}


//────────────────────────────────────────────────────────────────────────────────────


#if 0  
  
// |27 | 1043332699 | 6910022 | 3282392 | 3445571 | 8382533|97.933031 | 0.648613 | 0.308104 | 0.323421 | 0.786831 | 1.609325e-06|
float f32_atan_ue_a(float x)
{
  static const float N[] = {-0x3.ac262cp-36f,-0x5.367ca8p-4f,-0x2.351a5p-4f};
  static const float D[] = {0xf.a37ep-4f,0x2.f0444p-4f};
  float x2 = x*x;

  float n = N[2];
  float d = D[1];

  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x,(n/d),x);
}




// rescaled. perform slightly worse but I'm not handling the multiword term D[1]
// |1 | 1064298241 | 1054976 | 99.900974 | 0.099026 | 5.960464e-08|
float f32_atan_ue_(float x)
{
  static const float N[] = {-0xa.4e28b4p-44f,-0x3.fcfd34p-4f,-0x3.86edap-4f,-0x8.7f648p-8f};
  static const float D[] = {0xb.f6f7ap-4f,0x1.1c28f6p0f,0x7.1f339p-4f,0xa.5e1a5p-8f};
//static const float L   = -0x1.eb851ep-27;

  float x2 = x*x;

  float n = N[3];
  float d = D[3];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[2]);
  d = fmaf(d, x2, D[1]); 
  d = fmaf(d, x2, D[0]);
//d = fmaf(L, x2, d);        // can't contribute

  return fmaf(x,(n/d),x);
}

// |1 | 1064938767 | 414450 | 99.961097 | 0.038903 | 5.960464e-08|
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

// |1 | 1064938767 | 414450 | 99.961097 | 0.038903 | 5.960464e-08|
float f32_atan_ue_d(float a)
{
  double x = (double)a;

  return (float)fma(x,(n/d),x);
}
#endif


#if 1
#define test     f32_atan_ue
#define test_end 1.0f
#else
#define test     f32_atan_sp4
#define test_end 0.4142135623730950f
#endif

#define test_full


//──────────────────────────────────────────────────────────────────────────────────

#include "../tests/core_math_expand.h"

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

//──────────────────────────────────────────────────────────────────────────────────

int main(void)
{
  printf("atan test\n");
  
  uint32_t max = 0;
  uint32_t cnt = 0;
  uint32_t hist[5] = {0};
  float    aerror = 0;

  uint32_t u = 0;

  float x = .2f;
  printf("spot check: %f = %f %f\n", x,cr_atanf(x), test(x));
  
  // inital range is x/pi
  for(; u <= 0x39b89ba2 ; u++) {
#if defined(test_full)
    float    x  = f32_from_bits((uint32_t)u);
    float    r0 = x;
    float    r1 = test(x);
    float    ae = fabsf(r0-r1);
    uint32_t d  = f32_ulp_dist_ss(r0,r1);
    
    cnt++;
    
    hist[d <=4 ? d : 4]++;
    
    if (ae > aerror) aerror = ae;
    if (d <= max) continue;
    
    max = d;
    
    printf("% 10a : % 10a % 10a : %2u\n", x,r0,r1,d);
#endif
  }
  
  uint32_t e = f32_to_bits(test_end);

  for(; u <= e; u++) {
    float    x  = f32_from_bits((uint32_t)u);
    float    r0 = cr_atanf(x);
    float    r1 = test(x);
    float    ae = fabsf(r0-r1);
    uint32_t d  = f32_ulp_dist_ss(r0,r1);
    
    cnt++;
    
    hist[d <=4 ? d : 4]++;
    
    if (ae > aerror) aerror = ae;
    if (d <= max) continue;
    
    max = d;

    // don't spam on poor approximations
    if (max > 100) continue;
    
    printf("% 10a : % 10a % 10a : %2u\n", x,r0,r1,d);
  }

  printf("done: checked %x\n", cnt);
  printf("|%u | %u | %u | %u | %u | %u"
         "|%f | %f | %f | %f | %f | %e|\n"
         ,
         max,
         hist[0],hist[1],hist[2],hist[3],hist[4],
         100.0*(double)hist[0]/(double)cnt,
         100.0*(double)hist[1]/(double)cnt,
         100.0*(double)hist[2]/(double)cnt,
         100.0*(double)hist[3]/(double)cnt,
         100.0*(double)hist[4]/(double)cnt,
         aerror
         );

  return 0;
}
