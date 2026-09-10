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
#include "SFH/f32_math/internal/f32_asincos.h"


// 3 ulp:  
static inline float asin_hi_0(float x)
{
  static const float N[] =  {
    -4.7128088772296905517578125e-3f,
    2.559317089617252349853515625e-2f,
    -7.52884447574615478515625e-2f,
    0.21047453582286834716796875f,
    -1.57028067111968994140625f
  };
  
  float s = sqrtf(1.f-x);
  float p = N[0];

  p = fmaf(p,x,N[1]);
  p = fmaf(p,x,N[2]);
  p = fmaf(p,x,N[3]);
  p = fmaf(p,x,N[4]);
#if 1
  p = fmaf(p,s, 0.5f*f32_pi);
#elif 0
  float h = p*s;
  float l = fmaf(p,s,-h);

  p = 0.5f*f32_up_pi.h+(h + (l + 0.5f*f32_up_pi.l));
#else
  p = fmaf(p,s, 0.5f*f32_up_pi.l) + 0.5f*f32_up_pi.h;
#endif  

  return p;
}

// also 3 ulp (say)
static inline float asin_hi_1(float x)
{
  static const float N[] =  {
    1.99121725745499134063720703125e-3f,
    -1.1917327530682086944580078125e-2f,
    3.58834005892276763916015625e-2f,
    -8.253896236419677734375e-2f,
    0.21299441158771514892578125f,
    -1.57062625885009765625f };
  
  float s = sqrtf(1.f-x);
  float p = N[0];

  p = fmaf(p,x,N[1]);
  p = fmaf(p,x,N[2]);
  p = fmaf(p,x,N[3]);
  p = fmaf(p,x,N[4]);
  p = fmaf(p,x,N[5]);
#if 1
  p = fmaf(p,s, 0.5f*f32_pi);
#elif 0
  float h = p*s;
  float l = fmaf(p,s,-h);

  p = 0.5f*f32_up_pi.h+(h + (l + 0.5f*f32_up_pi.l));
#else
  p = fmaf(p,s, 0.5f*f32_up_pi.l) + 0.5f*f32_up_pi.h;
#endif  

  return p;
}

static inline float asin_bf(float a)
{
  static const double N[] =  {
    // polynomial for |x| on [0, 1/2]
    0x1.596d288dc7987p-5f,
    0x1.8c283c3a5a46ap-6f,
    0x1.747e4a3065be5p-5f,
    0x1.3301e4689933p-4f,
    0x1.5555c88340c2cp-3f,

    // polynomial for |x| on [1/2, 1]
    -0x1.34df4625198ddp-8,
     0x1.a354224d7d72p-6,
    -0x1.34625edcedc4ap-4,
     0x1.af0d71c21deefp-3,
    -0x1.91fdeaf5921aap0
  };

  double   x  = (double)a;
  uint64_t sx = f64_sign_bit(x);

  x = f64_mulsign(x,sx);

#if 1
  int            i  = (x <= 0.5);
  const double*  c  = hint_select(i, N, N+5);
  double         v  = hint_select(i, x*x, x);
  double         n  = hint_select(i, v,   1.0);
  double         b  = hint_select(i, x,   0.5*f64_pi);
  double         s  = hint_select(i, x,   sqrt(1.0-x));
#elif 0
  uint64_t m  = f64_sign_mask(x-0.5);
  const double*  c  = N + ((~m) & 5);
  double   v  = f64_mask_select(m, x*x, x);
  double   n  = f64_mask_select(m, v,   1.0);
  double   b  = f64_mask_select(m, x,   0.5*f64_pi);
  double   s  = f64_mask_select(m, x,   sqrt(1.0-x));
#else
  const double*  c  = (x <= 0.5) ? N   : N+5;
  double   v  = (x <= 0.5) ? x*x : x;
  double   n  = (x <= 0.5) ? v   : 1.0;
  double   b  = (x <= 0.5) ? x   : 0.5*f64_pi;
  double   s  = (x <= 0.5) ? x   : sqrt(1.0-x);
#endif  
  
  double   r  = c[0];

  r = fma(r, v, c[1]);
  r = fma(r, v, c[2]);
  r = fma(r, v, c[3]);
  r = fma(r, v, c[4]) * n;
  r = fma(r, s, b);

  return (float)f64_mulsign(r,sx);
}


static inline float asin_d_hi_fr(float a)
{
  static const double N[] =  {
    // polynomial on [1/2, 1]
    -0x1.34df4625198ddp-8,
     0x1.a354224d7d72p-6,
    -0x1.34625edcedc4ap-4,
     0x1.af0d71c21deefp-3,
    -0x1.91fdeaf5921aap0
  };
  
  double   x  = (double)a;
  uint64_t sx = f64_sign_bit(x);

  x = f64_mulsign(x,sx);

  double s = sqrt(1.0-x);
  double r = N[0];

  double v = x;

  r = fma(r, v, N[1]);
  r = fma(r, v, N[2]);
  r = fma(r, v, N[3]);
  r = fma(r, v, N[4]);

  r = fma(r,s, 0.5*f64_pi);

  return (float)f64_mulsign(r,sx);
}


static inline float asin_d_lo_fr(float a)
{
  static const double N[] =  {
    // polynomial for |x| on [0, 1/2]    
    0x1.596d288dc7987p-5f,
    0x1.8c283c3a5a46ap-6f,
    0x1.747e4a3065be5p-5f,
    0x1.3301e4689933p-4f,
    0x1.5555c88340c2cp-3f
  };
  
  double x  = (double)a;

  int    i = 0;
  double v  = x*x;
  double r = N[i+0];

  r = fma(r, v, N[i+1]);
  r = fma(r, v, N[i+2]);
  r = fma(r, v, N[i+3]);
  r = fma(r, v, N[i+4]);

  r = v*r;
  r = fma(r,x,x);

  return (float)r;
}


#if 0
static inline float asin_hi_d2(float a)
{
  static const double N[] =  {
     0x1.0505e20d9741ep-9,
    -0x1.8688c7d81861bp-7,
     0x1.25f774eccc6e7p-5,
    -0x1.521544d236df5p-4,
     0x1.b436924e44e6p-3,
    -0x1.921490b3aca56p0
  };
  
  double x = (double)a;
  double s = sqrt(1.0-x);
  double p = N[0];

  p = fma(p,x,N[1]);
  p = fma(p,x,N[2]);
  p = fma(p,x,N[3]);
  p = fma(p,x,N[4]);
  p = fma(p,x,N[5]);
  p = fma(p,s, 0.5*f64_pi);

  return (float)p;
}
#endif






static inline float asin_hi_d2(float a)
{
  static const double N[] =  {
     0x1.0505e20d9741ep-9,
    -0x1.8688c7d81861bp-7,
     0x1.25f774eccc6e7p-5,
    -0x1.521544d236df5p-4,
     0x1.b436924e44e6p-3,
    -0x1.921490b3aca56p0
  };
  
  double x = (double)a;
  double s = sqrt(1.0-x);
  double p = N[0];

  p = fma(p,x,N[1]);
  p = fma(p,x,N[2]);
  p = fma(p,x,N[3]);
  p = fma(p,x,N[4]);
  p = fma(p,x,N[5]);
#if 1
  p = fma(p,s, 0.5*f64_pi);
#elif 0
  double h = p*s;
  double l = fma(p,s,-h);

  p = 0.5f*f32_up_pi.h+(h + (l + 0.5f*f32_up_pi.l));
#else
  p = fmaf(p,s, 0.5f*f32_up_pi.l) + 0.5f*f32_up_pi.h;
#endif  

  return (float)p;
}



//3.57823558151721954345703125e-2 * sqrt(1 + x * (-1)) * x^2 + -0.18667827546596527099609375 * sqrt(1 + x * (-1)) * x + 1.5653545856475830078125 * sqrt(1 + x * (-1)) + (-1.50338973980979062616825103759765625e-5) }


#if 0
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
#endif


static inline float asin_kd_1(float a)
{
#if 1
  // |x| <= 1/2
  // |1 | 84154520 | 1265664 ||98.518308 | 1.481692 || 5.960464e-08|
  static const double N[] = {0x5.1f66e8p-4,-0x1.4a8a16p-4,-0x4.07e6fp-8};
  static const double D[] = {0x1.ebc6a7ef9db23p+0,-0x1.594808p+0};
#elif 0
  // |x| <= .9  11 ulp
  static const double N[] = {0x2.aaaa932578292p-4,-0xa.c1102152e33cp-8,-0x2.192ba3f20cfe8p-8};
  static const double D[] = {0x1p+0,-0xb.3bd900ba4c018p-4};
#endif
  
  double x  = (double)a;
  double x2 = x*x;

  double n = N[2];
  double d = D[1];
  
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0])*x2;
  d = fma(d, x2, D[0]);
  
  double r = n/d;
  
  return (float)fma(x,r,x);
}


static inline float asin_kd_2(float a)
{
#if 0  
  // |x| <= 3/4
  // |1 | 89264667 | 349821 || 99.609638 | 0.390362 || 5.960464e-08|
  static const double N[] = {0x1.f0ab5e7487373p-4,-0x1.cb29810baac8p-4,0x2.b7cdd84be2178p-8,0x5.e97a5889dadf8p-12};
  static const double D[] = {0xb.a40444af6d8dp-4,-0x1p+0,0x5.1a3539faf067cp-4};
#else
  // |x| <= 0.9
  static const double N[] = {0x1.cc157dd0fe8d6p-4,-0x1.db89e47e23868p-4,0x3.807d104166dd2p-8,0xb.58c3cd556d928p-12};
  static const double D[] = {0xa.c883c04023b8p-4,-0x1p+0,0x5.a458b8819128p-4};
#endif
    
  double x  = (double)a;
  double x2 = x*x;

  double n = N[3];
  double d = D[2];
  
  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0])*x2;

  d = fma(d, x2, D[1]);
  d = fma(d, x2, D[0]);
  
  double r = n/d;
  
  return (float)fma(x,r,x);
}


static inline float asin_dr_4_4(float a)
{
#if 0
  // |x| <= .8
  // |1 | 90395159 | 58190 || 0|99.935668 | 0.064332 || 5.960464e-08|
  static const double N[] = { 0x1.7120cbdc08476p-4,-0x2.048f33b212f82p-4,0xa.f12b838df9c98p-8,-0x5.0f532ccbc982p-12};
  static const double D[] = { 0x8.a6c4c5b6099cp-4,-0x1p+0,0x8.fc5c63d861758p-4,-0x1.73e4ec326680ep-4};
#elif 0
  // |x| <= 0.875
  // |1 | 90913747 | 797893 || 99.129998 | 0.870002 || 1.192093e-07|
  static const double N[] = { 0x1.616b5d62e6c04p-4,-0x2.0ba170e12d48p-4,0xc.0be18a42b9b78p-8,-0x6.d691c6ca0a38p-12};
  static const double D[] = { 0x8.48842169d608p-4,-0x1p+0,0x9.7f77c8f61f7p-4,-0x1.a7c59a800a12p-4};
#elif 0
  // |x| <= 0.9
  // |1| 90138943 | 1992127 ||97.837725 | 2.162275 || 1.192093e-07|
  static const double N[] = { 0x1.5a81cbce07d5dp-4,-0x2.0ebe84acdc2d4p-4,0xc.92441ff4fd23p-8, -0x7.d58fa05e82a2p-12};
  static const double D[] = { 0x8.1f0aa2de1c7fp-4,-0x1p+0,0x9.bccdc01c18c2p-4,-0x1.c14036ada6a3cp-4};
#elif 1
  // |x| <= 0.9125
  // |1 | 88982279 | 3358507 || 96.362921 | 3.637079 || 1.192093e-07|
  static const double N[] = {0x1.568ea823602ep-4,-0x2.10864244669ep-4,0xc.e20fa48bd64dp-8,-0x8.79eebc66cd138p-12};
  static const double D[] = {0x8.0757b70625818p-4,-0x1p+0,0x9.e0ee184fecb98p-4,-0x1.d09e3295fa8ccp-4};
#elif 0  
  // |x| <= 0.925
  // |2 | 87212818 | 4863452 | 474231 || 94.232681 | 5.254917 | 0.512402 || 2.384186e-07|
  static const double N[] = {0x1.522c3d0e6ab0dp-4,-0x2.128086bfc4cf8p-4,0xd.3d5b53174b9p-8,-0x9.428d9babb2938p-12};
  static const double D[] = {0x7.ed090e6aa639p-4,-0x1p+0,0xa.09fe6bc8ea64p-4,-0x1.e268cd1ccebep-4};
#else
  // |x| <= 0.95
  // |6| 81789322 | 6849314 | 1708653 | 1169274 | 1453368|87.973951 | 7.367236 | 1.837856 | 1.257691 | 1.563267 | 7.152557e-07|
  static const double N[] = {0x1.478cfbffa99d6p-4,-0x2.174dcd1ef2308p-4,0xe.276bbbcd3f078p-8,-0xb.86e2b9310a9bp-12};
  static const double D[] = {0x7.ad4cb79a75dbp-4,-0x1p+0,0xa.71ec8e8eedecp-4,-0x2.10fa03480a008p-4};
#endif  
    
  double x  = (double)a;
  double x2 = x*x;

  double n = N[3];
  double d = D[3];
  
  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0])*x2;

  d = fma(d, x2, D[2]);
  d = fma(d, x2, D[1]);
  d = fma(d, x2, D[0]);
  
  double r = n/d;
  
  return (float)fma(x,r,x);
}

float f32_r_wrap(float x)
{
  float (*P)(float) = f32_asincos_r2;
  
  float x2 = x*x;
  float r  = x2*P(x2);
  return fmaf(r,x,x);
}

float f32_p_wrap(float x)
{
  float (*P)(float) = f32_asincos_r2;
  
  float x2 = x*x;
  float r  = x2*P(x2);

  return fmaf(r,x,x);
}

//f32_p_wrap
#if 0
#define test       f32_r_wrap
#define test_start 0.0f
#define test_end   0.5f
#elif 1
#define test       asin_bf
#define test_start 0.0f
//#define test_end   0x1.fffffep-2f
#define test_end   1.0f
#else
#define test       asin_hi_d1
#define test_start 0.5f
#define test_end   1.0f
#endif

#define test_all


//──────────────────────────────────────────────────────────────────────────────────

#include "../tests/core_math_expand.h"
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

//──────────────────────────────────────────────────────────────────────────────────



int main(void)
{
  printf("asin test on [%f,%f]\n", test_start, test_end);

  printf("");
  
  uint32_t max = 0;
  uint32_t cnt = 0;
  uint32_t hist[5] = {0};
  float    aerror = 0;

  uint32_t u = f32_to_bits(test_start);

  // identity range
  for(; u <= 0x39e89768 ; u++) {
#if defined(test_all)
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
    float    r0 = cr_asinf(x);
    float    r1 = test(x);
    float    ae = fabsf(r0-r1);
    uint32_t d  = f32_ulp_dist_ss(r0,r1);
    
    cnt++;
    
    hist[d <=4 ? d : 4]++;
    
    if (ae > aerror) aerror = ae;
    if (d <= max) continue;
    
    max = d;
    
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
