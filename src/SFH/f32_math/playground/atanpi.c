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


// atanpi(x) for x on [-1,1]
//   |1 | 1025226194 | 40127023 | 96.233454 | 3.766546 | 1.490116e-08|
double f32_atanpi_ue_ex(double x)
{
  static const double N[] = {0xc.21f873a99eep-8,-0x1.5ba1886c85f3p-8,-0x9.24fd56d356cp-8,-0x1.a1599469c291cp-8 };
  static const double D[] = {0xb.19cd4548071dp-4,0x6.23ec81fac27b8p-4,0x8.5a5211bbc10b8p-8};

  double x2 = x*x;

  double n = N[3];
  double d = D[2];

  n = fma(n, x2, N[2]);
  n = fma(n, x2, N[1]);
  n = fma(n, x2, N[0]);

  d = fma(d, x2, D[1]);
  d = fma(d, x2, 1.f);
  d = fma(d, x2, D[0]);

  return fma(x,(n/d), 0.25*x);
}


// |1 | 976353883 | 88999334 | 91.646026 | 8.353974 | 1.490116e-08|

static inline float f32_atanpi_rk_44(float x2)
{
#if 1  
// |1 | 189278571 | 18129837 | 0 | 0 | 0|91.258871 | 8.741129 | 0.000000 | 0.000000 | 0.000000 | 1.490116e-08| (temp)
  static const float N[] = {0xc.26c30p-8f,-0x1.63129p-8f,-0x9.248c0p-8f,-0x1.9f247p-8f};
  static const float D[] = {0xb.1e2f8p-4f, 0x6.203fep-4f, 0x8.4d802p-8f};
#else
  // ratapprox --function="atan(x)/pi-x/4" --dom=[-0x1.000000001,0x1.000000001] --denF=[SG] --numF=[SG] --num=[x,x^3,x^5,x^7] --den=[1,x^2,x^4,x^6]
  // |1 | 189378392 | 18030016 | 0 | 0 | 0|91.306999 | 8.693001 | 0.000000 | 0.000000 | 0.000000 | 1.490116e-08|
  static const float N[] = {0xc.21bdp-8f,-0x1.5b4534p-8f,-0x9.25028p-8f,-0x1.a1754cp-8f};
  static const float D[] = {0xb.1996ep-4f,0x6.241a6p-4f,0x8.5af3cp-8f};
#endif  

  float n  = N[3];
  float d  = D[2];
  
  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}

static inline float f32_atanpi_rk_34(float x2)
{
  static const float N[] = {0xf.c87a6p-8f,-0x7.07068p-8,-0x8.c173ep-8f};
  static const float D[] = {0xe.70d46p-4f,0x3.63be64p-4f,-0x1.0d5b14p-8f};
    
  float n  = N[2];
  float d  = D[2];
  
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return n/d;
}




float f32_atanpi_ue(float x)
{
  return fmaf(x, f32_atanpi_rk_34(x*x), 0.25f*x);
}


// max rel error = 31 ulp (19 correct digits)
// max abs error = 1.937151e-07
float f32_atanpi_ue_a(float x)
{
  static const float N[] = {0x1.17cb8p-4f,-0xa.0fe9p-8f,-0x7.6ccfp-8f};
  static const float D[] = {0xf.a3ba5p-4f,0x2.a398cp-4f};

  float x2 = x*x;
  float n  = N[2];
  float d  = D[1];

  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[0]);
  d = fmaf(d, x2, 1.f);

  return fmaf(x,(n/d), 0.25f*x);
}


// max abs error = 1.83832e-2  (1/256) = 3.90625e-3
float f32_atanpi_ue_0(float x) { return fmaf(0x1.2d30b2p-6f*x, fabsf(x), 0.25f*x); }

// max abs error = 1.937300e-04
float f32_atanpi_ue_1(float x)
{
  static const float C[] = {0x1.11beb8p-4f,-0x1.7864bcp-4f,0x1.9dc496p-6f};

  float x2 = x*x;
  float r  = C[2];
  
  r = fmaf(r, x2, C[1]);
  r = fmaf(r, x2, C[0]);
  r = fmaf(r, x,  0.25f*x);
  
  return r;
}

// max abs error = 2.591312e-05
// 10.97 ns
float f32_atanpi_ue_2(float x)
{
  static const float C[] = {0x1.16c5b4p-4f, -0x1.a2bf44p-4f, 0x1.7d661p-5, -0x1.96a506p-7f};
  
  float x2 = x*x;
  float r  = C[3];
  
  r = fmaf(r, x2, C[2]);
  r = fmaf(r, x2, C[1]);
  r = fmaf(r, x2, C[0]);
  r = fmaf(r, x,  0.25f*x);

  return r;
}

// max abs error = 3.650784e-06
float f32_atanpi_ue_3(float x)
{
  static const float C[] = { 0x1.179f7ep-4f,-0x1.aea688p-4f, 0x1.d5c858p-5f,
                            -0x1.bc1b62p-6f, 0x1.b2d87ep-8f};
  
  float x2 = x*x;
  float r  = C[4];
  
  r = fmaf(r, x2, C[3]);
  r = fmaf(r, x2, C[2]);
  r = fmaf(r, x2, C[1]);
  r = fmaf(r, x2, C[0]);
  r = fmaf(r, x,  0.25f*x);

  return r;
}

// max rel error = 383 ulp
// max abs error = 5.364418e-07
float f32_atanpi_ue_4(float x)
{
  static const float C[] = { 0x1.17c480p-4f,-0x1.b1ac2cp-4f, 0x1.f8ac4cp-5f,
                            -0x1.2f96e6p-5f, 0x1.128f2ep-6f,-0x1.e8ec10p-9f};
                            
  float x2 = x*x;
  float r  = C[5];
  
  r = fmaf(r, x2, C[4]);
  r = fmaf(r, x2, C[3]);
  r = fmaf(r, x2, C[2]);
  r = fmaf(r, x2, C[1]);
  r = fmaf(r, x2, C[0]);
  r = fmaf(r, x,  0.25f*x);

  return r;
}

// max rel error = 66 ulp
// max abs error = 8.940697e-08
float f32_atanpi_ue_7(float x)
{
  static const float C[] = {0x1.17cad0p-4f, -0x1.b26424p-4f,
                            0x1.024156p-4f, -0x1.591492p-5f,
                            0x1.9f46fep-6f, -0x1.5e8a66p-7f,
                            0x1.1c3d2cp-9f};
  float x2 = x*x;
  float r  = C[6];
  
  r = fmaf(r, x2, C[5]);
  r = fmaf(r, x2, C[4]);
  r = fmaf(r, x2, C[3]);
  r = fmaf(r, x2, C[2]);
  r = fmaf(r, x2, C[1]);
  r = fmaf(r, x2, C[0]);
  r = fmaf(r, x,  0.25f*x);

  return r;
}



//Numerator = [|0x6.85b64p-4,0x7.5d712p-4,0x1.ddf2cp-4,0xe.dce7p-12|];
//Denominator = [|0x1.47d8ep+0,0x1.df7ced916872bp+0,0xb.c1b0fp-4,0x1.08ce52p-4|];

// |3 | 569823266 | 493958357 | 1567093 | 4501 | 0|53.486793 | 46.365689 | 0.147096 | 0.000422 | 0.000000 | 4.470348e-08|
// last at max: 0x1.fff522p-1 (0.999917089939117431640625)
float f32_atanpi_ue_(float x)
{
#if 1
  static const float N[] = {0x6.85b64p-4f,0x7.5d712p-4f,0x1.ddf2cp-4f,0xe.dce7p-12f};
  static const float D[] = {0x1.47d8ep+0f,0x1.df7ceep0f,0xb.c1b0fp-4f,0x1.08ce52p-4f};
//static const float L   = -0x1.ba5e36p-26f;  // low part of D[1]

  float x2 = x*x;

  float n = N[3];
  float d = D[3];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[2]);
  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, D[0]);

  return x*(n/d);
#else
  static const float N[] = {0x6.85b64p-4f,0x7.5d712p-4f,0x1.ddf2cp-4f,0xe.dce7p-12f};
  static const float D[] = {0x1.47d8ep+0f,0x1.df7ceep0f,0xb.c1b0fp-4f,0x1.08ce52p-4f};
//static const float L   = -0x1.ba5e36p-26f;  // low part of D[1]

  float x2 = x*x;
  float l2 = fma(x,x,-x2);

  float n = N[3];
  float d = D[3];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[2]);
  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, D[0]);

  return x*(n/d);
#endif  
}


// |3 | 559596847 | 505282064 | 474234 | 72 | 0|52.526884 | 47.428595 | 0.044514 | 0.000007 | 0.000000 | 4.470348e-08|
float f32_atanpi_ue_yy(float x)
{
  static const float N[] = {0xf.48f054p-48f,-0x1.24a3p-4f,-0x1.03047ap-4f,-0x2.70dcd8p-8f};
  static const float D[] = {0xa.c6094p-4f,0x6.6bbap-4f,0x9.5b6b7p-8f};

  float x2 = x*x;

  float ah = f32_up_pi_i.h * x;
  float al = fmaf(f32_up_pi_i.h,x,-ah);
  
  float n = N[3];
  float d = D[2];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  return fmaf(x,(n/d),ah)+al;
}

//|2 | 1029316271 | 36036239 | 707 |96.617371 | 3.382563 | 0.000066 |2.980232e-08|
float f32_atanpi_ue_xx(float x)
{
  static const float N[] = {-0xd.459ab8p-44f,-0x3.97c714p-4f,-0x3.2d77ep-4f,-0x7.a811dp-8f};
  static const float D[] = {0xa.c7554p-4f,0x6.6a9d1p-4f,0x9.576e8p-8f};

  float x2 = x*x;

  float n = N[3];
  float d = D[2];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  d = fmaf(x,(n/d),x);

  return f32_up_mul(f32_up_pi_i, d);
}

#if 0
float f32_atanpi_ue_zz(float x)
{
  // atan core
  static const float N[] = {-0xd.459ab8p-44f,-0x3.97c714p-4f,-0x3.2d77ep-4f,-0x7.a811dp-8f};
  static const float D[] = {0xa.c7554p-4f,0x6.6a9d1p-4f,0x9.576e8p-8f};

  float x2 = x*x;
  float xh = f32_up_pi_i.h*x;
  float xl = fmaf(f32_up_pi_i.h,x,-xh);

  float n = N[3];
  float d = D[2];

  n = fmaf(n, x2, N[2]);
  n = fmaf(n, x2, N[1]);
  n = fmaf(n, x2, N[0]);

  d = fmaf(d, x2, D[1]);
  d = fmaf(d, x2, 1.f);
  d = fmaf(d, x2, D[0]);

  //d = fmaf(x,(n/d),x);
  d = n/d;

  f32_pair_t r = f32_2mul(xh,d);
  float      t = fmaf(xl,d,r.l);

  r.l = 
  
  // (xh+xl)d
  float rh = xh*d;
  float t  = fmaf(xh,d,-rh);
  float rl = fmaf(xl,d, t);

  return xh+(rl+xl);
}
#endif

#if 1
#define test     f32_atanpi_ue
#define test_end 1.0f
#else
#define test     asin_dr_4_4
#define test_end 0.9125f
//#define test_end 0.75f
#endif

#define test_full


//──────────────────────────────────────────────────────────────────────────────────

#include "../tests/core_math_expand.h"
#include <fenv.h>

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;

static inline double muldd(double xh, double xl, double ch, double cl, double *l){
  double ahlh = ch*xl, alhh = cl*xh, ahhh = ch*xh, ahhl = __builtin_fma(ch, xh, -ahhh);
  ahhl += alhh + ahlh;
  ch = ahhh + ahhl;
  *l = (ahhh - ch) + ahhl;
  return ch;
}

static double polydd(double xh, double xl, int n, const double c[][2], double *l){
  int i = n-1;
  double ch = c[i][0], cl = c[i][1];
  while(--i>=0){
    ch = muldd(xh,xl,ch,cl,&cl);
    double th = ch + c[i][0], tl = (c[i][0] - th) + ch;
    ch = th;
    cl += tl + c[i][1];
  }
  *l = cl;
  return ch;
}

float cr_atan2pif(float y, float x){
  static const double cn[] =
    {0x1.45f306dc9c883p-2, 0x1.988d83a142adap-1, 0x1.747bebf492057p-1, 0x1.2cc5645094ff3p-2,
     0x1.a0521c711ab66p-5, 0x1.881b8058b9a0dp-9, 0x1.b16ff514a0afp-16};
  static const double cd[] =
    {0x1p+0, 0x1.6b8b143a3f6dap+1, 0x1.8421201d18ed5p+1, 0x1.8221d086914ebp+0, 0x1.670657e3a07bap-2,
     0x1.0f4951fd1e72dp-5, 0x1.b3874b8798286p-11};
  static const double m[] = {0, 1};
  static const double off[] = {0.0f, 0.5f, 1.0f, 0.5f, -0.0f, -0.5f, -1.0f, -0.5f};
  static const float sgnf[] = {1,-1};
  static const double sgn[] = {1,-1};
  b32u32_u tx = {.f = x}, ty = {.f = y};
  uint32_t ux = tx.u, uy = ty.u, ax = ux&(~0u>>1), ay = uy&(~0u>>1);
  if(__builtin_expect(ay >= (0xff<<23)||ax >= (0xff<<23), 0)){
    if(ay > (0xff<<23)) return x + y; // nan
    if(ax > (0xff<<23)) return x + y; // nan
    uint32_t yinf = ay==(0xff<<23), xinf = ax==(0xff<<23);
    if(yinf&xinf){
      if(ux>>31)
	return 0.75f*sgnf[uy>>31];
      else
	return 0.25f*sgnf[uy>>31];
    }
    if(xinf){
      if(ux>>31)
	return sgnf[uy>>31];
      else
	return 0.0f*sgnf[uy>>31];
    }
    if(yinf){
      return 0.5f*sgnf[uy>>31];
    }
  }
  if(__builtin_expect(ay==0, 0)){
    if(__builtin_expect(!(ay|ax),0)){
      uint32_t i = (uy>>31)*4 + (ux>>31)*2;
      return off[i];
    }
    if(!(ux>>31)) return 0.0f*sgnf[uy>>31];
  }
  if(__builtin_expect(ax==ay, 0)){
    static const float s[] = {0.25,0.75,-0.25,-0.75};
    uint32_t i = (uy>>31)*2 + (ux>>31);
    return s[i];
  }
  uint32_t gt = ay>ax, i = (uy>>31)*4 + (ux>>31)*2 + gt;
  
  double zx = x, zy = y;
  double z = (m[gt]*zx + m[1-gt]*zy)/(m[gt]*zy + m[1-gt]*zx);
  double r = cn[0], z2 = z*z;
  z *= sgn[gt];
  // avoid spurious underflow in the polynomial evaluation excluding extremely small arguments
  if(__builtin_expect(z2>0x1p-54, 1)){
    double z4 = z2*z2, z8 = z4*z4;
    double cn0 =     r + z2*cn[1];
    double cn2 = cn[2] + z2*cn[3];
    double cn4 = cn[4] + z2*cn[5];
    double cn6 = cn[6];
    cn0 += z4*cn2;
    cn4 += z4*cn6;
    cn0 += z8*cn4;
    double cd0 = cd[0] + z2*cd[1];
    double cd2 = cd[2] + z2*cd[3];
    double cd4 = cd[4] + z2*cd[5];
    double cd6 = cd[6];
    cd0 += z4*cd2;
    cd4 += z4*cd6;
    cd0 += z8*cd4;
    r = cn0/cd0;
  }
  r = z*r + off[i];
  b64u64_u res = {.f = r};
  if(__builtin_expect((res.u<<1) > 0x6d40000000000000 && ((res.u + 8)&0xfffffff) <= 16, 0)){
    // |res| > 0x1p-149
    if(ax==ay) {
      static const double off2[] = {0.25, 0.75, -0.25, -0.75};
      r = off2[(uy>>31)*2 + (ux>>31)];
    } else {
      double zh,zl;
      if(!gt){
	zh = zy/zx;
	zl = __builtin_fma(zh,-zx,zy)/zx;
      } else {
	zh = zx/zy;
	zl = __builtin_fma(zh,-zy,zx)/zy;
      }
      double z2l, z2h = muldd(zh,zl,zh,zl,&z2l);
      static const double c[][2] =
	{{0x1.45f306dc9c883p-2, -0x1.6b01ec5513324p-56}, {-0x1.b2995e7b7b604p-4, 0x1.e402b0c13eedcp-58},
	 {0x1.04c26be3b06cfp-4, -0x1.571d178a53efp-60}, {-0x1.7483758e69c03p-5, 0x1.819a6ed7aaf38p-63},
	 {0x1.21bb9452523ffp-5, -0x1.234d866fb9807p-60}, {-0x1.da1bace3cc54ep-6, -0x1.c84f6ada49294p-64},
	 {0x1.912b1c23345ddp-6, -0x1.534890fbc165p-60}, {-0x1.5bade52f5f52ap-6, 0x1.f783bafc832f6p-60},
	 {0x1.32c69d084c5cp-6, 0x1.042d155953025p-60}, {-0x1.127bcfb3e8c7dp-6, -0x1.85aae199a7b6bp-60},
	 {0x1.f0af43b11a731p-7, 0x1.8f0356356663p-61}, {-0x1.c57e86801029ep-7, 0x1.dcdf3e3b38eb4p-61},
	 {0x1.a136408617ea1p-7, 0x1.a71affb36c6c4p-63}, {-0x1.824ac7814ba37p-7, 0x1.8928b295c0898p-61},
	 {0x1.6794e32ea5471p-7, 0x1.0b4334fb41e63p-61}, {-0x1.501d57f643d97p-7, 0x1.516785bf1376ep-61},
	 {0x1.3adf02ff2400ap-7, -0x1.b0e30bb8c8076p-62}, {-0x1.267702f94faap-7, -0x1.7a4d3a1850cc6p-62},
	 {0x1.10dce97099686p-7, 0x1.fcc208eee2571p-61}, {-0x1.eee49cdad8002p-8, -0x1.9109b3f1bab82p-64},
	 {0x1.af93bc191a929p-8, 0x1.069fd3b47d7bp-62}, {-0x1.6240751b54675p-8, -0x1.72dc8cfd03b6fp-62},
	 {0x1.0b61e84080884p-8, 0x1.825824c80941bp-63}, {-0x1.6a72a8a74e3a5p-9, 0x1.8786a82fd117ep-63},
	 {0x1.aede3217d939dp-10, -0x1.93b626982e1fep-68}, {-0x1.b66568f09ebeep-11, -0x1.704a39121d0a5p-66},
	 {0x1.73af3977fa973p-12, -0x1.aa050e2244ea3p-68}, {-0x1.fc69d85ed28c9p-14, 0x1.867f17b764cap-68},
	 {0x1.0c883a9270162p-15, -0x1.6842833896dd9p-70}, {-0x1.9a0b27b6dfe15p-18, 0x1.427fc2f4e1327p-73},
	 {0x1.91e15e7ab5bdcp-21, -0x1.730dbc6279d0dp-77}, {-0x1.7b1119c1ff867p-25, 0x1.145f9980759c4p-79}};
      double pl, ph = polydd(z2h, z2l, 32, c, &pl);
      zh *= sgn[gt];
      zl *= sgn[gt];
      ph = muldd(zh,zl,ph,pl,&pl);
      double sh = ph + off[i], sl = ((off[i] - sh) + ph) + pl;
      float rf = sh;
      double th = rf, dh = sh - th, tm = dh + sl;
      r = th + tm;
      b64u64_u d = {.f = r - th};
      if( !(d.u<<12) ) {
	double ad = __builtin_fabs(d.f), am = __builtin_fabs(tm);
	if(ad > am) r -= d.f*0x1p-10;
	if(ad < am) r += d.f*0x1p-10;
      }
    }
  }
  float rf = r;
#ifdef CORE_MATH_SUPPORT_ERRNO
  if (__builtin_expect (__builtin_fabsf (rf) < 0x1p-126f, 0))
    errno = ERANGE;
#endif
  return rf;
}

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


//──────────────────────────────────────────────────────────────────────────────────

int main(void)
{
  printf("atanpi test\n");

  printf("%08x %08x %08x\n", f32_to_bits(1.f), f32_to_bits(0.5), f32_to_bits(1.0)-f32_to_bits(0.5f));
  //printf("%f\n", f32_from_bits(1.f));

  
  uint32_t max = 0;
  uint32_t cnt = 0;
  uint32_t hist[5] = {0};
  float    aerror = 0;

  uint32_t u = 0;

  // inital range is x/pi
  for(; u <= 0x332332e8 ; u++) {
#if defined(test_full)
    float    x  = f32_from_bits((uint32_t)u);
    float    r0 = f32_up_mul(f32_up_pi_i,x);
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
    float    r0 = cr_atanpif(x);
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
