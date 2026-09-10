// -*- coding: utf-8 -*-

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "SFH/compiler_hints.h"
#include "SFH/f32_util.h"
#include "SFH/f64_util.h"

#define PRNG_IMPLEMENTATION
#include "prng_small_global.h"

#include "SFH/f32_horner.h"


#define test_start 0.f
#define test_end   1.f


//──────────────────────────────────────────────────────────────────────────────────
// SEE: https://core-math.gitlabpages.inria.fr
// and license info at top of file.

#include "../tests/core_math_expand.h"

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

#pragma GCC diagnostic pop

//──────────────────────────────────────────────────────────────────────────────────



float atan2pi_k4(float y, float x)
{
  static const float C[] = {0x1.45f2fap-2f, -0x1.b27552p-4f, 0x1.0091b4p-4f, -0x1.219412p-5f};
  static const float K   = 0x1.a8279ap-2f; // tan(π/8)

  // range reduction
  uint32_t m  = f32_sign_mask(fmaf(x,K,-y));
  float    n  = f32_mask_select1(m, x);
  float    d  = f32_mask_select1(m, y);
  float    a  = f32_mask_select1(m, 0.25f);

  float t  = (y-n)/(x+d);
  float t2 = t*t;
  float r  = C[3];

  // kernel on [-tan(π/8),tan(π/8)]
  r = fmaf(r,t2,C[2]);
  r = fmaf(r,t2,C[1]);
  r = fmaf(r,t2,C[0]);
  r = fmaf(r,t, a);

  return r;
}

float atan2pi_k5(float y, float x)
{
  static const float C[] = {0x1.45f306p-2f, -0x1.b29702p-4f,0x1.04611ap-4f, -0x1.6878fcp-5f, 0x1.9cba76p-6f};
  static const float K   = 0x1.a8279ap-2f; // tan(π/8)

#if 0
  int   s  = y > x*K;
  float n  = (!s) ? 0.f : x;
  float d  = (!s) ? 0.f : y;
  float a  = (!s) ? 0.f : 0.25f;
  float t  = (y-n)/(x+d);
#else
  float t,a;
  
  if (y <= x*K) {
    t = y/x;
    a = 0.f;
    if (t > K) printf("x");
  }
  else {
    t = (y-x)/(y+x);
    a = 0.25f;
    //if (t > K) printf("{%f}",fabsf(t-K));
  }
#endif  

  //if (t > K) printf("X");
  
  
  float t2 = t*t;
  float r  = C[4];

  // kernel on [-tan(π/8),tan(π/8)]
  r = fmaf(r,t2,C[3]);
  r = fmaf(r,t2,C[2]);
  r = fmaf(r,t2,C[1]);
  r = fmaf(r,t2,C[0]);
  r = fmaf(r,t, a);

  return r;
}

//──────────────────────────────────────────────────────────────────────────────────

// atan on [-1,1] : peak abs error  0.003347723744809627532958984375
//                    note  1/256 = 0.00390625
static inline float atan_u8(float x)
{
  float ax = fabsf(x);

  return x * (fmaf(ax, -0x1.10517p-2, 0x1.0dff9cp0));
}

// asm("" : "+r"(x), "+r"(y)); x = ? x:y; asm("" : "+r"(x));
// asm("" : "+r"(r), "+r"(s)) 

static inline float atan2_a0(float y, float x)
{
  uint32_t sx = f32_sign_bit(x);
  float    ax = f32_mulsign(x,sx);
  float    d  = sqrtf(fmaf(x,x,y*y)) + ax;
  float    a  = (sx) ? 1.f : 0.0;   // not really like this (and not this value for atan)

  d = (d != 0) ? d : 1.f;

  float    r  = f32_mulsign(atan_u8(y/d), sx);

  return r+a;
}



// atan(x) : x on [-tan(π/8),tan(π/8)]
static inline float f32_atan_k4(float x)
{
  // atan(x) ≈ x³ P(x²) + x
  static const float C[] = {-0x1.55544cp-2f, 0x1.9922fp-3f, -0x1.1c267p-3f, 0x1.4982b4p-4f};

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0]);
  r = fmaf(r*x,x2, x);
  
  return r;
}


// atanpi(x) + a : x on [-tan(π/8),tan(π/8)]
static inline float f32_atanpi_k4(float x, float a)
{
  // atanpi(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const float C[] = {-0x1.55544cp-2f, 0x1.9922fp-3f, -0x1.1c267p-3f, 0x1.4982b4p-4f};

  float x2 = x*x;
  float r;
  
  r = C[3];
  r = fmaf(r,  x2, C[2]);
  r = fmaf(r,  x2, C[1]);
  r = fmaf(r,  x2, C[0]);
  r = fmaf(r*x,x2, x);

#if 0  
  static const float ipi = 0x1.45f306p-2f;  // 1/π
  r = fmaf(r, ipi, a);
#else  
  r = f32_up_madd(f32_up_pi_i, r,a);     // r/π + a
#endif  
  
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


// atanpi(x) + a : x on [-tan(π/8),tan(π/8)]
// (for binary32 but internal computation in binary64)
static inline float f32_atanpi_k4_fr(double x, double a)
{
  // atanpi(x) + a ≈ (x³ P(x²) + x)(1/π) + a
  static const double C[] = {-0x1.555453812e9b7p-2, 0x1.9924bc804e959p-3,
                             -0x1.1c3701132244fp-3, 0x1.49e167123486cp-4};

  double x2 = x*x;
  double r;
  
  r = C[3];
  r = fma(r,  x2, C[2]);
  r = fma(r,  x2, C[1]);
  r = fma(r,  x2, C[0]);
  r = fma(r*x,x2, x);

#if 0  
  static const double ipi = 0x1.45f306dc9c883p-2;  // 1/π

  r = fma(ipi, r, a);
#else
  r = f64_up_madd(f64_up_pi_i, r,a); // r/π + a
#endif  
  
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




static inline uint32_t mask_zselect_u32(uint32_t m, uint32_t v) { return m & v; }
static inline uint64_t mask_zselect_u64(uint64_t m, uint64_t v) { return m & v; }
static inline float    mask_zselect_f32(uint32_t m, float    v) { return f32_from_bits(m & f32_to_bits(v)); }
static inline double   mask_zselect_f64(uint64_t m, double   v) { return f64_from_bits(m & f64_to_bits(v)); }

// 
#define mask_zselect(m,v)                             \
  _Generic((void (*)(__typeof(m), __typeof(v)))NULL,  \
    void (*)(uint32_t, uint32_t) : mask_zselect_u32,  \
    void (*)(uint64_t, uint64_t) : mask_zselect_u64,  \
    void (*)(uint32_t, float)    : mask_zselect_f32,  \
    void (*)(uint64_t, double)   : mask_zselect_f64)(m,v)


// about 2 ulp ATM
float f32_atan2pi(float y, float x)
{
  // argument reduction: this looks more complicated than
  // it is.
  //
  //The two goals are to kick the compiler into
  // lowering it branch-free. The four cases are serial and
  // are random for uniform inputs. Also want to kick
  // it into starting the required divide as early as possible.
  // (lesser concern the given the code size).

  static const float cut = 0.4142135623730950f; // tan(π/8)

  uint32_t sx  = f32_sign_bit(x);
  uint32_t sy  = f32_sign_bit(y);
  uint32_t sgn = sx ^ sy;

  // reduce to first quadrant: |x|,|y|
  y   = f32_mulsign(y,sy);
  x   = f32_mulsign(x,sx);

  // reduce to first octant: (y > x)
  uint32_t c0 = f32_sign_mask(x-y);

  hint_cswap(y>x,x,y);
  
  // reduce to polynomial range: y/x > tan(π/8)
  uint32_t c1 = f32_sign_mask(fmaf(cut,x,-y));
  float    n  = f32_mask_select1(c1,x);
  float    d  = f32_mask_select1(c1,y);
  float    r  = (y-n)/(x+d);

  hint_alive(r);

  // complete computing argument reduction related
  // quantities while the divide is in progress.
  float    a;
  a    = sx != 0 ? 1.f : 0.f;
  a    = f32_mulsign(a, sx&sy);
  a   += f32_mask_select1(c0, f32_mulsign(0.5f,sgn));
  sgn ^= c0 & 0x80000000u;
  
  float s = f32_mask_select1(c1, 0.25f);

  s += f32_mulsign(a,sgn);
  r  = f32_mulsign(f32_atanpi_k4(r,s),sgn);

  return r;
}


static inline float f32_atanpi_k4_fr_(double x, double a)
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


float f32_atan2pi_x(float X, float Y)
{
  static const double cut = 0.4142135623730950; // change to proper value
  double   x   = (double)X;
  double   y   = (double)Y;
  uint64_t sx  = f64_sign_bit(x);
  uint64_t sy  = f64_sign_bit(y);
  uint64_t sgn = sx ^ sy;

  x  = f64_mulsign(x,sx);
  y  = f64_mulsign(y,sy);

  uint64_t c0 = f64_sign_mask(x-y);

  hint_cswap(y>x, x,y);

  uint64_t c1 = f64_sign_mask(fma(cut,x,-y));
  double   n  = f64_mask_select1(c1,x);
  double   d  = f64_mask_select1(c1,y);
  double   r  = (y-n)/(x+d);
  hint_alive(r);
  
  double   a;
  a    = sx != 0 ? 1.0 : 0.0;
  a    = f64_mulsign(a, sx&sy);
  a   += f64_mask_select1(c0, f64_mulsign(0.5,sgn));
  sgn ^= c0 & (UINT64_C(1) << 63);
  
  double s = f64_mask_select1(c1, 0.25);

  s += f64_mulsign(a,sgn);
  r  = f64_mulsign(f32_atanpi_k4_fr_(r,s),sgn);

  return (float)r;
}


int main(void)
{
#if 0  
  printf("atan test\n");
  
  uint32_t max = 0;
  uint32_t cnt = 0;
  uint32_t hist[5] = {0};
  float    aerror = 0;

  uint32_t u = 0;

#if 0
  // identity range -- this is wrong for atan. fix
  for(; u <= 0x39e89768 ; u++) {
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
  }
#endif
  
  u = 0x39e89769;

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
#endif
  
  return 0;
}
