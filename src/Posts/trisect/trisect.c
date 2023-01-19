// Public Domain under http://unlicense.org, see link for details.
//   additionally:

/*
  If USE_CORE_MATH is defined then cr_acosf, cr_cosf and their helper
  functions are built. These are only needed to measure the error for
  an implementation in 3 correctly rounded steps.
  
 The CORE-MATH routines fall under:
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// compile with (or equiv).
// clang -O3 -march=native -Wall -Wextra -Wconversion -Wpedantic -Wno-unused-function -fno-math-errno -ffp-contract=off trisect.c -o trisect -lm

// add -lmpfr (MPFR installed) if building a new test vector

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

//********************************************************
// CONFIG STUFF HERE

// choose to run test (defined) or build a test vector (undefined)
#define RUN_VECTOR_TEST

// length of test vector to build
const uint32_t vector_len = 4096;

// for 3 correctly rounded steps. spews a fair number of warnings.
#define USE_CORE_MATH


float naive(float x)
{
  return cosf(acosf(x)/3.f);
}

float naive_r(float x)
{
  return cosf(acosf(x)*(1.f/3.f));
}


// IQ's approximations
float ha(float x)  { return sqrtf(0.5f+0.5f*x); }

float g2(float x) { x = ha(x); return x*0.5f+0.5f; }
float g3(float x) { x = ha(x); return x*(-0.064913f*x+0.564913f)+0.5f; }
float g4(float x) { x = ha(x); return x*(x*(x*0.021338f-0.096562f)+0.575223f)+0.5f; }

float g5(float x)
{
  x = ha(x);
  return x*(x*(x*(x*-0.008978f+0.039075f)-0.107071f)+0.576974f)+0.5f; 
}

// IQ's approximations (w FMA)
float haf(float x) { return sqrtf(fmaf(0.5f,x,0.5f)); }
    
float g2f(float x) { x = haf(x); return fmaf(0.5f,x,0.5f); }
float g3f(float x) { x = haf(x); return fmaf(x, (-0.064913f*x+0.564913f), 0.5f); }
float g4f(float x) { x = haf(x); return fmaf(x, (x*(x*0.021338f-0.096562f)+0.575223f), 0.5f); }

float g5f(float x)
{
  x = haf(x);
  return fmaf(x,(x*(x*(x*-0.008978f+0.039075f)-0.107071f)+0.576974f),0.5f); 
}


// yeah, yeah. it shouldn't work
#define result_barrier(X) __asm__ __volatile__("" : "+r"(X) : "r"(X));


// Full range weighted function approximations

// approx abs error: 0x1.253ff2p-14 (~6.9916e-5)
float a3(float x)
{
  static const float A[] = { 0x1.ec4dc6p-8f, 0x1.a7e32p-2f};
  static const float B[] = {-0x1.8961dp-5f,  0x1.cee61ap-2f};
  
  float t = sqrtf(1.f+x);
  float a = fmaf(x, A[0], A[1]);
  float b = fmaf(x, B[0], B[1]);
  
  return fmaf(t,a,b);
}

// approx abs error: 0x1.00315ap-16 (~1.5270e-5)
float a4(float x)
{  
  static const float A[] = {-0x1.4c1aaap-11f, 0x1.3e560ap-7f, 0x1.ac0d48p-2f};
  static const float B[] = {-0x1.aa284p-5f,   0x1.cabef8p-2f };
  
  float t = sqrtf(1.f+x);
  float a = fmaf(x, fmaf(x, A[0], A[1]), A[2]);
  float b = fmaf(x, B[0], B[1]);
  
  return fmaf(t,a,b);
}    

// approx abs error: 0x1.1e6202p-20 (~1.0669e-6)
float a5(float x)
{
  static const float A[] = { 0x1.8b53dcp-11f, 0x1.2e7fep-6f,  0x1.b41e68p-2f};
  static const float B[] = {-0x1.3f41fap-8f, -0x1.09305ep-4f, 0x1.c2b138p-2f};
  
  float t = sqrtf(1.f+x);
  float a = fmaf(x, fmaf(x, A[0], A[1]), A[2]);
  float b = fmaf(x, fmaf(x, B[0], B[1]), B[2]);
  
  return fmaf(t,a,b);
}

// approx abs error: 0x1.e67234p-23 (~2.2652e-7)
float a6(float x)
{
  static const float A[] = {-0x1.cd2fe2p-15f, 0x1.3b19fap-10, 0x1.50eb3ep-6f, 0x1.b5cccap-2};
  static const float B[] = {-0x1.935072p-8f, -0x1.152af2p-4, 0x1.c10294p-2};
  
  float t = sqrtf(1.f+x);
  float a = fmaf(x, fmaf(x, fmaf(x, A[0], A[1]), A[2]), A[3]);
  float b = fmaf(x, fmaf(x, B[0], B[1]), B[2]);

  return fmaf(t,a,b);
}    


// cut range weighted function approximations (all nope! stubs);

float c3(float x)
{
  const float cut  = -2674669.f/4194304.f;

  const float A2[2][2] = {{ 11019427.f/2147483648.f, 6809093.f/16777216.f}, { 12876879.f/1073741824.f,  3523121.f/8388608.f}};
  const float B2[2][2] = {{-11111149.f/ 268435456.f, 7720477.f/16777216.f}, {-14304849.f/ 268435456.f, 14989259.f/33554432.f}};
  
  float t = sqrtf(1.f+x);

  result_barrier(t);

  const int    i = (x >= cut) ? 0 : 1;
  const float* A = A2[i];
  const float* B = B2[i];
  
  float a = fmaf(x, A[0], A[1]);
  float b = fmaf(x, B[0], B[1]);
  
  return fmaf(t,a,b);
}

float c4(float x)
{  
  static const float cut  = -712791.f/1048576.f;

  static const float A2[2][3] = {
    {-291589.f/1073741824.f, 15888243.f/2147483648.f, 6917253.f/16777216.f},
    {-13609969/4294967296.f,  1176223.f/134217728.f,  7048603.f/16777216.f}};
  
  static const float B2[2][2] = {
    {-12585691.f/268435456.f, 15224481.f/33554432.f},
    {  -919635.f/ 16777216.f, 14937965.f/33554432.f}};  

  float t = sqrtf(1.f+x);

  result_barrier(t);

  const int    i = (x >= cut) ? 0 : 1;
  const float* A = A2[i];
  const float* B = B2[i];
  
  float a = fmaf(x, fmaf(x, A[0], A[1]), A[2]);
  float b = fmaf(x, B[0], B[1]);
  
  return fmaf(t,a,b);
}

float c5(float x)
{
  const float cut  = -674283.f/1048576.f;

  static const float A2[2][3] = {
    {12985339.f/34359738368.f, 15982721.f/1073741824.f, 7076693.f/16777216.f},
    { 7632049.f/ 4294967296.f, 11869557.f/ 536870912.f,  898793.f/2097152.f}};

  static const float B2[2][3] = {
    { -6887113.f/2147483648.f, -3967495.f/67108864.f, 3726401.f/ 8388608.f},
    {-15007067.f/2147483648.f,  -291449.f/4194304.f,  7340055.f/16777216.f}};

  float t = sqrtf(1.f+x);
  
  result_barrier(t);

  const int    i = (x >= cut) ? 0 : 1;
  const float* A = A2[i];
  const float* B = B2[i];

  float a = fmaf(x, fmaf(x, A[0], A[1]), A[2]);
  float b = fmaf(x, fmaf(x, B[0], B[1]), B[2]);
  
  return fmaf(t,a,b);
}

// looks faithfully rounded. just a hack though: c5 promoted to doubles only
float fr(float v)
{
  const float cut  = -674283.f/1048576.f;

  static const double A2[2][3] = {
    {12985339.f/34359738368.f, 15982721.f/1073741824.f, 7076693.f/16777216.f},
    { 7632049.f/ 4294967296.f, 11869557.f/ 536870912.f,  898793.f/2097152.f}};

  static const double B2[2][3] = {
    { -6887113.f/2147483648.f, -3967495.f/67108864.f, 3726401.f/ 8388608.f},
    {-15007067.f/2147483648.f,  -291449.f/4194304.f,  7340055.f/16777216.f}};

  double x = (double)v;
  double t = sqrt(1.f+x);
  
  result_barrier(t);

  const int    i = (v >= cut) ? 0 : 1;
  const double* A = A2[i];
  const double* B = B2[i];

  double a = fma(x, fma(x, A[0], A[1]), A[2]);
  double b = fma(x, fma(x, B[0], B[1]), B[2]);
  
  return (float)fma(t,a,b);
}

  
//**********************************************************************

#if defined(USE_CORE_MATH)

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
//#include <errno.h>  // MBR: commented out errno 

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
    //errno = EDOM;
    feraiseexcept(FE_INVALID);
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


//typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;
typedef unsigned __int128 u128;
typedef uint64_t u64;

/* __builtin_roundeven was introduced in gcc 10 */
#if defined(__GNUC__) && __GNUC__ >= 10
#define HAS_BUILTIN_ROUNDEVEN
#endif

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__AVX__) || defined(__SSE4_1__))
inline double __builtin_roundeven(double x){
   double ix;
#if defined __AVX__
   __asm__("vroundsd $0x8,%1,%1,%0":"=x"(ix):"x"(x));
#else /* __SSE4_1__ */
   __asm__("roundsd $0x8,%1,%0":"=x"(ix):"x"(x));
#endif
   return ix;
}
#define HAS_BUILTIN_ROUNDEVEN
#endif

#ifndef HAS_BUILTIN_ROUNDEVEN
#include <math.h>
/* round x to nearest integer, breaking ties to even */
static double
__builtin_roundeven (double x)
{
  double y = round (x); /* nearest, away from 0 */
  if (fabs (y - x) == 0.5)
  {
    /* if y is odd, we should return y-1 if x>0, and y+1 if x<0 */
    union { double f; uint64_t n; } u, v;
    u.f = y;
    v.f = (x > 0) ? y - 1.0 : y + 1.0;
    if (__builtin_ctz (v.n) > __builtin_ctz (u.n))
      y = v.f;
  }
  return y;
}
#endif

static inline double rltl(float z, int *q){
  double x = z;
  double idl = -0x1.b1bbead603d8bp-32*x, idh = 0x1.45f306ep-1*x, id = __builtin_roundeven(idh);
  *q = (long)id;
  return (idh - id) + idl;
}

static double __attribute__((noinline)) rbig(uint32_t u, int *q){
  static const u64 ipi[] = {0xfe5163abdebbc562, 0xdb6295993c439041, 0xfc2757d1f534ddc0, 0xa2f9836e4e441529};
  int e = (u>>23)&0xff, i;
  u64 m = (u&(~0u>>9))|1<<23;
  u128 p0 = (u128)m*ipi[0];
  u128 p1 = (u128)m*ipi[1]; p1 += p0>>64;
  u128 p2 = (u128)m*ipi[2]; p2 += p1>>64;
  u128 p3 = (u128)m*ipi[3]; p3 += p2>>64;
  u64 p3h = p3>>64, p3l = p3, p2l = p2, p1l = p1;
  long a;
  int k = e-127, s = k-23;
  if(s<0){
    /* Negative shifts are undefined behaviour: p3l>>-s seems to work
       with gcc, but does not with clang. */
    i =        p3h>>(64-s);
    a = p3h<<s|p3l>>(64-s);
  } else if(s==0) {
    i = p3h;
    a = p3l;
  } else if(s<64) {
    i = p3h<<s|p3l>>(64-s);
    a = p3l<<s|p2l>>(64-s);
  } else if(s==64) {
    i = p3l;
    a = p2l;
  } else { /* s > 64 */
    i = p3l<<s|p2l>>(128-s);
    a = p2l<<s|p1l>>(128-s);
  }
  int sgn = u; sgn >>= 31;
  long sm = a>>63;
  i -= sm;
  double z = (a^sgn)*0x1p-64;
  i = (i^sgn) - sgn;
  *q = i;
  return z;
}

float cr_cosf(float x){
  static const double
    cs[] = {
    -0x1.a51a6625307bdp-2, 0x1.9f9cb402b97ffp-5, -0x1.86a8e46de2fc4p-9,
    0x1.ac67ffda7836cp-14, -0x1.337d0b88f8cb7p-19, 0x1.3417d9749e139p-25},
    cc[] = {
      -0x1.3bd3cc9be458bp+0, 0x1.03c1f081b078ep-2, -0x1.55d3c7dbfe042p-6,
      0x1.e1f4fb610f151p-11, -0x1.a6c9c224d18abp-16, 0x1.f3dbf0909677fp-22},
    q[] = {1.0, 0, -1.0, 0};

  b32u32_u t = {.f = x};
  int e = (t.u>>23)&0xff, i;
  double z;
  if (__builtin_expect(e<127+28, 1)){
    if (__builtin_expect(e<114, 0))
      return __builtin_fmaf(-x, x, 1.0f);
    z = rltl(x, &i);
  } else if (e<0xff) {
    z = rbig(t.u, &i);
  } else {
    if(t.u<<9) return x; // nan
    //errno = EDOM;
    feraiseexcept(FE_INVALID);
    return __builtin_nanf("cinf"); // inf
  }
  double z2 = z*z, z4 = z2*z2;
  double ms = q[(i+1)&3], mc = q[i&3];
  z *= 0x1.921fb54442d18p+0*ms;
  if(__builtin_expect(z2<0x1p-25, 0)){
    if(!(i&1)){
      float a = z2;
      int j = (i>>1)&1;
      static const float tb[] = {1, -1};
      return __builtin_fmaf(((float)cc[0])*tb[j], a, tb[j]);
    }
  }
  double s0 = cs[0] + z2*cs[1];
  double s2 = cs[2] + z2*cs[3];
  double s4 = cs[4] + z2*cs[5];
  double rs = (z*z2)*(s0 + z4*(s2 + z4*s4)) + z;
  double c0 = cc[0] + z2*cc[1];
  double c2 = cc[2] + z2*cc[3];
  double c4 = cc[4] + z2*cc[5];
  double rc = (z2*mc)*(c0 + z4*(c2 + z4*c4)) + mc;
  double r = rs + rc;
  float or = r;
  b64u64_u tr = {.f = r}; u64 tail = (tr.u + 6)&(~0ul>>36);
  if(__builtin_expect(tail<=12, 0)){
    static const struct {union{float arg; uint32_t uarg;}; float rh, rl;} st[] = {
      {{0x1.4555p+51f}, 0x1.115d7ep-1f, -0x1.fffffep-26f},
      {{0x1.3170fp+63f}, 0x1.fe2976p-1f, 0x1.fffffep-26f},
      {{0x1.119ae6p+115f}, 0x1.f3176ap-1f, 0x1.fffffep-26f},
      {{0x1.96344ep+117f}, 0x1.f8a9b4p-1f, 0x1.fffffep-26f},
      {{0x1.24f2eep+85f}, 0x1.af5c6ap-2f, -0x1.91995ap-54f},
      {{0x1.ddebdep+120f}, 0x1.114438p-1f, 0x1.0b776ep-53f},
      {{0x1.ba5a98p+95f}, -0x1.ac093cp-1f, -0x1.7b13cep-53f},
      {{0x1.a13cb2p+1f}, -0x1.fc6f6cp-1f, 0x1.28b55ap-53f},
      {{0x1.417a5cp+8f}, 0x1.052988p-1f, 0x1.2f43fp-53f},
      {{0x1.34486p+19f}, -0x1.edfe3p-1f, 0x1.4ff1aep-55f},
      {{0x1.48a858p+54f}, 0x1.f48148p-2f, 0x1.e09b5p-56f},
      {{0x1.f4a7f4p+106f}, -0x1.b503dap-1f, 0x1.0c60e4p-53f},
      {{0x1.0a1f74p+58f}, -0x1.404ecep-2f, 0x1.58808ap-54f},
      {{0x1.ea56e2p+73f}, -0x1.d74dfap-3f, 0x1.7fa9c2p-55f},
    };
    uint32_t ax = t.u&(~0u>>1);
    for(int i=0;i<14;i++) {
      if(__builtin_expect(st[i].uarg == ax, 0))
	return st[i].rh + st[i].rl;
    }
  }
  return or;
}

#pragma GCC diagnostic pop

// perform each of the three logical steps correctly rounded
// (for error measurement)
float cr3(float x)
{
  return cr_cosf(cr_acosf(x)/3.f);
}

#endif

//**********************************************************************

static inline uint32_t f32_to_bits(float x)
{
  uint32_t u; memcpy(&u,&x,4); return u;
}

static inline float f32_from_bits(uint32_t x)
{
  float u; memcpy(&u,&x,4); return u;
}

static inline uint32_t u32_abs(uint32_t x)
{
  return (int32_t)x >= 0 ? x : -x;
}

// ulp distance provided a & b are finite
// and have the same sign
static inline uint32_t f32_ulp_dist_ss(float a, float b)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);
  return u32_abs(ua-ub);
}


//**********************************************************************

#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))

typedef struct {
  float (*f)(float);
  char*   name;
} func_entry_t;

#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S
#define ENTRY(X) { .f=&X, .name=STRINGIFY(X) }

func_entry_t func_table[] =
{
  ENTRY(naive),
  ENTRY(naive_r),
#if defined(USE_CORE_MATH)
  ENTRY(cr3),
#endif  
  ENTRY(g2),
  ENTRY(g3),
  ENTRY(g4),
  ENTRY(g5),
  ENTRY(g2f),
  ENTRY(g3f),
  ENTRY(g4f),
  ENTRY(g5f),
  ENTRY(a3),
  ENTRY(a4),
  ENTRY(a5),
  ENTRY(a6),
  ENTRY(c3),
  ENTRY(c4),
  ENTRY(c5),
  ENTRY(fr),
};

//********************************************************

typedef struct { float x,y; } f32_pair_t;
typedef struct { f32_pair_t range; float r; } range_result_t;


#if defined(RUN_VECTOR_TEST)
// built by 'vector_build'
#include "test_vector_4096.h"
//#include "test_vector_2048.h"
//#include "test_vector_1024.h"
//#include "test_vector_512.h"
//#include "test_vector_256.h"

void vector_test_func(func_entry_t* entry)
{
  uint32_t max    = 0;
  float    max_x  = 0;
  float    max_cr = 0;
  float    max_r  = 0;

  for(uint32_t i=0; i<LENGTHOF(test_vector); i++) {
    float    x0 = test_vector[i].range.x;
    float    x1 = test_vector[i].range.y;
    float    cr = test_vector[i].r;
    float    r0 = entry->f(x0);
    float    r1 = entry->f(x1);
    uint32_t u0 = f32_ulp_dist_ss(cr,r0);
    uint32_t u1 = f32_ulp_dist_ss(cr,r1);

    if (u0 > max) { max = u0; max_x = x0; max_cr = cr; max_r = r0; }
    if (u1 > max) { max = u1; max_x = x1; max_cr = cr; max_r = r1; }
  }

  float ae = fabsf(max_cr-max_r);
  
  printf("%10s: %8d  %14.6a %14.6a %14.6a %14.6a %e\n", entry->name, max, max_x, max_cr, max_r, ae, ae);
}

void vector_test()
{
  printf("%10s: %8s  %14s %14s %14s %14s\n", "func", "ULP", "x   ", "cr   ", "f   ", "|f-cr|   ");

  for(uint32_t i=0; i<LENGTHOF(func_table); i++) {
    vector_test_func(func_table+i);
  }
}
#elif !defined(NEEDS_MPFR)
#define NEEDS_MPFR
#endif

#if defined(NEEDS_MPFR)
// 40 isn't enough around -1. 50 seems fine. 64 to be safer
const uint32_t mp_precision = 64;

#include <mpfr.h>

mpfr_t mp_three;

void mp_init()
{
  mpfr_init2(mp_three, 2);
  mpfr_set_flt(mp_three, 3.f, MPFR_RNDN);
  mpfr_set_emin(-148);
  mpfr_set_emax( 128);
}

// current overkill version for testing
typedef struct {
  mpfr_t x;

  mpfr_t acos_u;
  mpfr_t third_u;
  mpfr_t r_u;

  mpfr_t acos_d;
  mpfr_t third_d;
  mpfr_t r_d;

  float  ru;
  float  rd;
} box_t;

void box_init(box_t* b, int precision)
{
  mpfr_init2(b->x,  24);
  mpfr_init2(b->acos_u,  precision);
  mpfr_init2(b->acos_d,  precision);
  mpfr_init2(b->third_u, precision);
  mpfr_init2(b->third_d, precision);
  mpfr_init2(b->r_u,     precision);
  mpfr_init2(b->r_d,     precision);
}

// computes (in MP)
//   ru = RU( cos( RU( RU(acos(x))/3 ) ) )
//   rd = RD( cos( RD( RD(acos(x))/3 ) ) )
// then each RN to binary32. working assumption
// is mp_precision is set high enough that the
// two results agree. (this is sanity checking that)
float box_eval(box_t* b, float x)
{
  mpfr_set_flt(b->x, x,      MPFR_RNDN);
  mpfr_acos(b->acos_u, b->x, MPFR_RNDU);
  mpfr_acos(b->acos_d, b->x, MPFR_RNDD);
  mpfr_div(b->third_u, b->acos_u, mp_three, MPFR_RNDU);
  mpfr_div(b->third_d, b->acos_d, mp_three, MPFR_RNDD);
  mpfr_cos(b->r_u,     b->third_u, MPFR_RNDU);
  mpfr_cos(b->r_d,     b->third_d, MPFR_RNDD);

  b->ru = mpfr_get_flt(b->r_u, MPFR_RNDN);
  b->rd = mpfr_get_flt(b->r_d, MPFR_RNDN);

  // temp hack below here
  float ru = b->ru;
  float rd = b->rd;

  if (ru != rd)
    printf("box diff! : f(%f) = %f %f (%u)\n", x,rd,ru, f32_ulp_dist_ss(ru,rd));

  return ru;
}

// 
float mp_eval(box_t* b, float x)
{
#if !defined(BOX_ALWAYS)
  mpfr_set_flt(b->x, x,      MPFR_RNDN);
  mpfr_acos(b->acos_u, b->x, MPFR_RNDN);
  mpfr_div(b->third_u, b->acos_u, mp_three, MPFR_RNDN);
  mpfr_cos(b->r_u,     b->third_u, MPFR_RNDN);
  
  return mpfr_get_flt(b->r_u, MPFR_RNDN);
#else
  return box_eval(b,x);
#endif  
}


// find max 'x' such that f(x) == r
//   assumes f(x0) == r
//           f(x1) != r
//           x0 < x1  (AS IEEE BITS!!)
float mp_find_u_k(box_t* b, float x0, float x1, float r)
{
  uint32_t lo  = f32_to_bits(x0);
  uint32_t hi  = f32_to_bits(x1);
  uint32_t mid = 0;
  float    x=0.f,v;

  if (lo > hi) printf("U");

  while(lo < hi) {
    mid = lo + ((hi-lo)>>1);
    x   = f32_from_bits(mid);
    v   = box_eval(b,x);
    
    if (v == r) {
      lo = mid;
      if (lo+1 == hi) return x;
    }
    else
      hi = mid;
    
    //printf("[%08x,%08x] : %a %a\n", lo,hi,r,v);
  }

  return x;
}

float mp_find_d_k(box_t* b, float x0, float x1, float r)
{
  uint32_t lo  = f32_to_bits(x0);
  uint32_t hi  = f32_to_bits(x1);
  uint32_t mid = 0;
  float    x=0.f,v;

  if (lo > hi) printf("D");

  while(lo < hi) {
    mid = lo + ((hi-lo)>>1);
    x   = f32_from_bits(mid);
    v   = box_eval(b,x);
    
    if (v == r) {
      hi = mid;
      if (hi-1 == lo) return x;
    }
    else {
      lo = mid;
      if (hi-1 == lo) return f32_from_bits(hi);
    }
    //printf("[%08x,%08x] : %a %a\n", lo,hi,r,v);
  }

  return x;
}

float mp_find_up(box_t* b, float x0, float x1)
{
  return mp_find_u_k(b,x0,x1,box_eval(b,x0));
}


// given x:
// * CR compute: r=f(x);
// * find internal [x0,x1] f(x0..x1) = r 
void find_range(box_t* b, range_result_t* d, float x)
{
  float r = box_eval(b,x);

  d->r = r;

  // start of hack: being lazy
  float xu = f32_from_bits(f32_to_bits(x) + 0xFFFF);
  float xd = f32_from_bits(f32_to_bits(x) - 0xFFFF);

  if (fabs(xu) > 1) xu = copysignf(1.f,x);
  if (fabs(xd) > 1) xd = copysignf(1.f,x);
  // end of hack

  if (x > 0) {
    d->range.x = mp_find_u_k(b, x, xu, r);
    d->range.y = mp_find_d_k(b, xd, x, r);
  }
  else {
    d->range.x = mp_find_u_k(b, x, xu, r);
    d->range.y = mp_find_d_k(b, xd, x, r);
  }
}

//#define SAMPLE_DEBUG

// * uniformly sample the interior of the codomain (exclude end points)
// * at each sample (y):
//   1) compute x (via inverse of f)
//   2) find [x0,x1] such that f(x0)=f(x1)=y (CR y is found from x in find_range)
//   3) pass off {{x0,x1},y} to f
void sample_codomain(box_t* b, void (*f)(range_result_t*), uint32_t len)
{
  float d = (float)(len+1);

  range_result_t r;
  
  for(uint32_t i=0; i<len; i++) {
    float y = 0.5f*(((float)(i+1))/d)+0.5f;
    float x = y*fmaf(4.f*y,y,-3.f);
    find_range(b,&r, x);
    f(&r);

#if defined(SAMPLE_DEBUG)    
    float r0 = mp_eval(b,r.range.x);
    float r1 = mp_eval(b,r.range.y);
    int   b0 = r0 != r.r;
    int   b1 = r1 != r.r;

    if (b0 || b1) {
      printf("x in [%14.06a %14.06a] r in [%14.06a %14.06a] stored r = %14.06a : %c %c\n",
	     r.range.x, r.range.y, r0, r1, r.r, (b0 ? '-' : ' '), (b1 ? '+' : ' '));
    }
#endif    
  }
}

void range_result_print(range_result_t* r)
{
  float x0 = r->range.x;
  float x1 = r->range.y;
  
#if !defined(SAMPLE_DEBUG)
  printf("  {{%14.6af,%14.6af},%14.6af}, // %5d  \n", x0, x1, r->r, f32_ulp_dist_ss(x0,x1));
#endif  
}


void vector_build(box_t* b)
{
#if !defined(SAMPLE_DEBUG)
  printf("const range_result_t test_vector[] =\n{\n");
  printf("  {{-0x1.242396p-22f,0x1.6f71a0p-24f},0x1.bb67aep-1f}, // ULPs of range\n");
  sample_codomain(b, &range_result_print, vector_len);
  printf("};\n");
#else
  sample_codomain(b, &range_result_print, vector_len);
#endif  
}

#endif

int main(void)
{
#if defined(RUN_VECTOR_TEST)
  vector_test();
#else
  box_t box;
  mp_init();
  box_init(&box, mp_precision);
  vector_build(&box);
#endif
  
  return 0;
}





