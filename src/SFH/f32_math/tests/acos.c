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

// branchfree direction WIP
static inline float f32_acos_x3(float v, float (*P)(float))
{
  double x = (double)v;
  double a = fabs(v);

  // |x| <  0.5 : acos(x) = pi/2 -   asin(x)
  // x   < -0.5 : acos(x) = pi   - 2 asin( sqrt((1+x)/2 )
  // x   >  0.5 : acos(x) =        2 asin( sqrt((1-x)/2 )


#if 0
  uint32_t sx = f32_sign_bit(v);
  uint64_t m  = f64_sign_mask(x);
  double   c  = f64_from_bits(m & f64_to_bits(-f64_pi));
  
  double t, t2;
  
  if (a <= 0.5) {
    t  = v;
    t2 = v*v;
    
    double k  = P(v*v);
    double r  = fma(x,x*x*k,x);        
    return (float)(0.5*M_PI-r);
  }
  else {
    t2 = 0.5*(1.0-a);
    t  = f64_sqrt(t2);
  }

  double z  = 0.5*(1.0-a);         // exact: Sterbenz lemma
  double t2 = z;
  double t  = 2.0*f64_sqrt(t2);
  double p  = (double)P((float)z);
  double r  = fma(t, t2*p, t+c);

  float    f  = (float)r;
  uint32_t ir = f32_to_bits(f);

  return f32_from_bits(f+e);
#else
  uint32_t sx = f32_sign_bit(v);
  uint64_t m  = f64_sign_mask(x);
  double   c  = f64_from_bits(m & f64_to_bits(-f64_pi));
  
  if (a <= 0.5) {
    double k  = P(v*v);
    double r  = fma(x,x*x*k,x);        
    return (float)(0.5*M_PI-r);
  } 

  double z  = 0.5*(1.0-a);         // exact: Sterbenz lemma
  double t2 = z;
  double t  = 2.0*f64_sqrt(t2);
  double p  = (double)P((float)z);
  double r  = fma(t, t2*p, t+c);

  
  return f32_mulsign((float)r, sx);
#endif  
}


float f32_acos_hq(float x) { return f32_acos_x2(x, &f32_asincos_k5); }
float f32_acos(float x)    { return f32_acos_x2(x, &f32_asincos_k4); }


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

#include <fenv.h>
//#include <errno.h>

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

//********************************************************


float f32_acos_x1_3(float x)  { return f32_acos_x1(x, &f32_asincos_k3); }
float f32_acos_x1_4(float x)  { return f32_acos_x1(x, &f32_asincos_k4); }
float f32_acos_x1_5(float x)  { return f32_acos_x1(x, &f32_asincos_k5); }
float f32_acos_x1_6(float x)  { return f32_acos_x1(x, &f32_asincos_k6); }

float f32_acos_x2_3(float x)  { return f32_acos_x2(x, &f32_asincos_k3); }
float f32_acos_x2_4(float x)  { return f32_acos_x2(x, &f32_asincos_k4); }
float f32_acos_x2_5(float x)  { return f32_acos_x2(x, &f32_asincos_k5); }
float f32_acos_x2_6(float x)  { return f32_acos_x2(x, &f32_asincos_k6); }

float f32_acos_x3_6(float x)  { return f32_acos_x3(x, &f32_asincos_k6); }

func_entry_t func_table[] =
{
  //ENTRY(libm),
  //ENTRY(libm_d),
  //ENTRY(fdlibm_acosfu),
  //ENTRY(fdlibm_acosf),
  //ENTRY(sleef_acosf),
  //ENTRY(f32_acos_),
  //ENTRY(f32_acos),
#if 0  
  ENTRY(f32_acos_x3),
  ENTRY(f32_acos_x2_3),
  ENTRY(f32_acos_x2_4),
  ENTRY(f32_acos_x2_5),
#endif  
  ENTRY(f32_acos_x1_6),
  ENTRY(f32_acos_x2_6),
  ENTRY(f32_acos_x3_6),
  //ENTRY(f32_acos_x2_f),
};

const char* func_name = "acos";

float cr_func(float x) { return cr_acosf(x); }

#include "common.h"

//********************************************************

void test_hi()
{
  uint32_t x0 = f32_to_bits(0.5f);
  uint32_t x1 = f32_to_bits(1.0f);
  test_force(x0,x1);
}

void test_hi_neg()
{
  uint32_t x0 = f32_to_bits(-0.5f);
  uint32_t x1 = f32_to_bits(-1.0f);
  test_force(x0,x1);
}

void test_lo()
{
  uint32_t x0 = f32_to_bits(0.0f);
  uint32_t x1 = f32_to_bits(0.5f);
  test_force(x0,x1);
}

void test_spot()
{
}

void test_all()
{
  uint32_t x0 = f32_to_bits( 0.0f);
  uint32_t x1 = f32_to_bits( 1.0f);
  //test_force(x0,x1);
  x0 = f32_to_bits( -0.0f);
  x1 = f32_to_bits( -1.0f);
  test_force(x0,x1);  
}

int main(int argc, char** argv)
{
  return test_run(argc, argv);
}



