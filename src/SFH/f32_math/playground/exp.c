// -*- coding: utf-8 -*-

#if 0
#include <math.h>
#include <stdint.h>
#include <string.h>

#include <stdio.h>

#include "SFH/f32_util.h"
#include "SFH/f64_util.h"
#else
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
#endif


static inline float asfloat(int32_t x)
{
  float f; memcpy(&f, &x, 4); return f;
}

static inline double asdouble(int64_t x)
{
  double f; memcpy(&f, &x, 8); return f;
}


// change names
static const int32_t exp_bits_k = INT32_C(0x7F800000);
static const int32_t mag_bits_k = INT32_C(0x007fffff);

static inline int32_t f32_linear_bithack(float x, float m, float a)
{
  // using FMA doesn't matter: solely instruction issue choice
  return (int32_t)fmaf(m,x,a);
}

static inline int32_t exp_initial_bithack(float x, float k)
{
  return f32_linear_bithack(x, k, (float)f32_one_bits_k);
}


// "Simple multiple precision algorithms for exponential functions", 2022
// Moroz, Samotyy, Kokosiński & Gepner
// (paper source)  https://github.com/pawelgepner/Simple-exponential-functions

static inline float exp_base_g0(float x, float k)
{
  int32_t u = f32_linear_bithack(x,k,(float)0x3f7a68c7);
  return asfloat(u);
}

static inline float exp_base_g1(float x, float k)
{
  int32_t u  = exp_initial_bithack(x,k);
  int32_t e  = u & exp_bits_k;
  int32_t l  = u & mag_bits_k;
  float   d2 = (float)(0x00f94ee7 + l) * 0.40196114e-7f;
  float   d3 = (float)(0x0000560e + l);

  e  |= (int32_t)(d2*d3);

  return asfloat(e);
}


static inline float exp_base_g2(float x, float k)
{
  const float a[] = {0.31214472e-7f, 0.37120473e-7f, 0.4414393e-7f, 0.52496276e-7f};
  const float b[] = {(float)0x151d842, (float)0x1113a74, (float)0xcdf4b4, (float)0x81354a};
  const float c[] = {328.83582f, (float)0x9f16, (float)0x3e4d6, (float)0x10a440};
  
  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k;
  int32_t l = u & mag_bits_k;

  float d1,d2,d3;

  int32_t i = l >> 21;
  
  d1 = a[i];
  d2 = b[i];
  d3 = c[i];
  
  d2 += (float)l;
  d3 += (float)l;

  d2=d1*d2;

  h |= (int32_t)(d2*d3);

  return asfloat(h);
}



// "Power Function Algorithms Implemented in Microcontrollers and FPGAs"
//  2023, Moroz, Samotyy, Gepner, Węgrzyn, Nowakowski 
//  (PDF)          https://www.mdpi.com/2079-9292/12/16/3399
//  (paper source) https://github.com/pawelgepner/Power_Function
//  
// • As the title states: these were not designed for mainstream
//   CPUs. I've made some tweaks but really they'd need to
//   be re-designed targeting hardware with fast floating point.
// • this use the top bits of the significand to break into PoT regions
//   sized {2,4,8,16,32} for l0,l1,.. respectively

static inline float exp_base_l0(float x, float k) 
{
  const float a[] = { 0.8284271247f, 1.171572875f};
  const float b[] = { 0.f, -1439258.f};
  
  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k; 
  int32_t l = u & mag_bits_k;
  int32_t i = l >> 22;

  h |= f32_linear_bithack((float)l, a[i], b[i]);

  return 0x1.fc29c0p-1f * asfloat(h);
}

static inline float exp_base_l1(float x, float k) 
{ 
  const float a[] = { 0.75682846f, 0.90002579f, 1.0703171f, 1.272828678f};
  const float b[] = { 0.f, -300307.f, -1014560.f, -2288653.f};

  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k; 
  int32_t l = u & mag_bits_k;
  int32_t i = l >> 21;

  h |= f32_linear_bithack((float)l, a[i], b[i]);

  // float point constant could be tweaked
  return 0x1.ff0a92p-1f * asfloat(h); 
}


static inline float exp_base_l2(float x, float k) 
{
  static const int a[] = { 6027202, 6620496,7219702 ,7873141,
                           8585721, 9362795,10210201,11134303};
  static const int b[] = { 0,-72620,-222421,-467461,
                           -823751,-1309422,-1944976,-2753566};

  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k; 
  int32_t l = u & mag_bits_k;
  int32_t i = l >> 20;

  h |= (int32_t)(((int64_t)l*(int64_t)a[i])>>23)+b[i];

  return asfloat(h); 
}

static inline float exp_base_l3(float x, float k) 
{
  const int a[] = { 5919872, 6204688, 6479393, 6766260,
                    7065828, 7378659, 7705340, 8046485,
                    8402733, 8774754, 9163245, 9568937,
                    9992590, 10435000, 10896997, 11379448 };
  const int b[] = { 0,-17425,-51763,-105551,
                    -180443, -278202, -400708, -549959,
                    -728083, -937344, -1180152, -1459065,
                    -1776804, -2136262, -2540510, -2992808};

  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k; 
  int32_t l = u & mag_bits_k;
  int32_t i = l >> 19;

  h |= (int32_t)(((int64_t)l*(int64_t)a[i])>>23)+b[i];

  return asfloat(h); 
}

static inline float exp_base_l4(float x, float k) 
{
  const int a[] = { 5866957, 6006506,6138031, 6272436,
                    6409785, 6550141, 6693570, 6840140,
                    6989920, 7142979, 7299390 ,7459226,
                    7622562, 7789474, 7960042, 8134344,
                    8312463, 8494482, 8680487, 8870565,
                    9064805, 9263298,9466138, 9673419,
                    9885240, 10101698, 10322897, 10548939,
                    10779930, 11015980, 11257199, 11503699 };
  const int b[] = { 0,-4268,-12488,-25089,
                    -42257, -64188,-91081, -123143,
                    -160588, -203636, -252515,-307458,
                    -368709, -436517, -511140, -592845,
                    -681904, -778602, -883230, -996088,
                    -1117488, -1247750, -1387202, -1536185,
                    -1695051, -1864159, -2043883, -2234606,
                    -2436723, -2650643, -2876786, -3115583};

  int32_t u = exp_initial_bithack(x,k);
  int32_t h = u & exp_bits_k; 
  int32_t l = u & mag_bits_k;
  int32_t i = l >> 18;

  h |= (int32_t)(((int64_t)l*(int64_t)a[i])>>23)+b[i];

  return asfloat(h); 
}

#if 0
float exp_x(float x)
{
  int64_t k1 = 0x171547652b82fe;
  int64_t k2 = 0x3fef4d18e0162e5d;
  int64_t z  = (int64_t)(x * k1 + k2);

  return (float)asdouble(z);
}
#endif

// exp & exp2 expansions
// • 

float exp2_g0(float x) { return exp_base_g0(x, (float)0x00800000); } // peek = (-0.029826,0.029827): example x = (1.030413,1.416575)  (0x1.07c92cp+0,0x1.6aa4aap+0)
float exp2_g1(float x) { return exp_base_g1(x, (float)0x00800000); } // peek = (-0.001730,0.001730): example x = (1.188845,1.538645)  (0x1.30582cp+0,0x1.89e4ap+0)
float exp2_g2(float x) { return exp_base_g2(x, (float)0x00800000); } // peek = (-0.000032,0.000033): example x = (1.514615,1.600359)  (0x1.83bdccp+0,0x1.99b124p+0)

float exp_g0(float x)  { return exp_base_g0(x, (float)0x00b8aa3b); } // peek = (-0.029826,0.029827): example x = (1.486328,1.043686)  (0x1.7c8p+0,  0x1.0b2efep+0)  
float exp_g1(float x)  { return exp_base_g1(x, (float)0x00b8aa3b); } // peek = (-0.001730,0.001730): example x = (1.714989,1.219337)  (0x1.b7098p+0,0x1.38268p+0) 
float exp_g2(float x)  { return exp_base_g2(x, (float)0x00b8aa3b); } // peek = (-0.000032,0.000033): example x = (1.184410,1.308815)  (0x1.2f358p+0,0x1.4f0e8p+0)

// (hacks of) FPGA & lower-powered devices designs
float exp2_l0(float x) { return exp_base_l0(x, (float)0x00800000); } // peek = (-0.007500,0.007499): example x = (1.203071,1.386300)  (0x1.33fc74p+0,0x1.62e488p+0)
float exp2_l1(float x) { return exp_base_l1(x, (float)0x00800000); } // peek = (-0.001885,0.001878): example x = (1.470426,1.559586)  (0x1.786dd4p+0,0x1.8f410ep+0)
float exp2_l2(float x) { return exp_base_l2(x, (float)0x00800000); } // peek = (-0.000643,0.000642): example x = (1.421700,1.472932)  (0x1.6bf48p+0, 0x1.79121ap+0)
float exp2_l3(float x) { return exp_base_l3(x, (float)0x00800000); } // peek = (-0.000165,0.000165): example x = (1.404111,1.429611)  (0x1.6773ccp+0,0x1.6dfaf8p+0)
float exp2_l4(float x) { return exp_base_l4(x, (float)0x00800000); } // peek = (-0.000045,0.000045): example x = (1.394951,1.407950)  (0x1.651b88p+0,0x1.686f68p+0)

float exp_l0(float x)  { return exp_base_l0(x, (float)0x00b8aa3b); } //
float exp_l1(float x)  { return exp_base_l1(x, (float)0x00b8aa3b); } // peek = (-0.001885,0.001878): example x = (1.121361,1.250008)  (0x1.1f118p+0,0x1.40008p+0)  
float exp_l2(float x)  { return exp_base_l2(x, (float)0x00b8aa3b); } // peek = (-0.000643,0.000642): example x = (1.050682,1.124992)  (0x1.0cf98p+0,0x1.1fff7ep+0)
float exp_l3(float x)  { return exp_base_l3(x, (float)0x00b8aa3b); } // peek = (-0.000166,0.000165): example x = (1.025383,1.062492)  (0x1.067f8p+0,0x1.0fff7cp+0)
float exp_l4(float x)  { return exp_base_l4(x, (float)0x00b8aa3b); } // peek = (-0.000045,0.000045): example x = (1.012810,1.031242)  (0x1.03478p+0,0x1.07ff7ep+0)


//────────────────────────────────────────────────────────────────────────────────────
// "Innovative simple yet efficient algorithms of logarithms with adjustable accuracy"
//  2023, Gepner, Moroz, Samotyy, Kokosiński, Gavkalova, Kocot
//  (PDF)          ??? can't find (do I have title wrong?)
//  (paper source) https://github.com/pawelgepner/Power_Function
//  
// • As the title states: these were not designed for mainstream
//   CPUs. I've made some tweaks but really they'd need to
//   be re-designed targeting hardware with fast floating point.
// • this use the top bits of the significand to break into PoT regions
//   sized {2,4,8,16,32} for l0,l1,.. respectively


float log2_m(float x)
{
  const float k = 1.0298f;

  uint32_t u = f32_to_bits(x);
  float    n = f32_zero_exponent(x);
  float    e = (float)((int32_t)(u >> 23)-128);

  return k*(n+e);
}

//────────────────────────────────────────────────────────────────────────────────────
// not bit hacking but still "gross" approximations
// 
// Hastings (1955) builds logb approximations on [1,b) by:
//   k = sqrt(b)
//   t = (x-k)/(x+k)
//   P = min-max optimized polynomial
//
// logb(x) = t P(t²) + 1/2  { x on [1,b) }
//
// form from atanh:
//   log2(x) = 2/log(2) atanh((x-k)/(x+k)) + 1/2
//
// atanh((x-k)/(x+k)) = 1/2 log(x/k) = 1/2(log(x)-log(k))


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


//────────────────────────────────────────────────────────────────────────────────────

void foo(void) {
  //  printf("{");  for(int i=0; i<16; i++)  printf("%af, ",ec1[i]);  printf("};\n");
}


//────────────────────────────────────────────────────────────────────────────────────
// "Innovative, simple yet efficient algorithms of logarithms with adjustable accuracy"
//  2023, Gepner, Moroz, Samotyy, Kokosiński, Gavkalova, Kocot
//  (PDF)          https://assets-eu.researchsquare.com/files/rs-3563916/v1_covered_efc44903-f29b-411e-80ef-15e4dc9c019c.pdf?c=1704220645
//  (paper source) https://github.com/pawelgepner/Power_Function
//  
// • As the title states: these were not designed for mainstream
//   CPUs. I've made some tweaks but really they'd need to
//   be re-designed targeting hardware with fast floating point.
// • this use the top bits of the significand to break into PoT regions
//   sized {2,4,8,16,32} for l0,l1,.. respectively

#if 0
float log2_gx(float x)
{
  int i =  *(int*) & x;
  int k = i & 0x007fffff;
  i = i >> 23;
  i = i - 128;
  float l = (float)k;
  //float l = f32_zero_exponent(x);
  k = (int)(l*(1.334968914f - 4.11091368e-8f*l) + 41437.706985f);
  k = f32_one_bits_k|k;
  float y = *(float*) & k;
  y = y + (float)i;
  return y;
}
#else
float log2_gx(float x)
{
  int32_t i = (int32_t)f32_to_bits(x);
  int32_t k = i & (int32_t)f32_mag_bits_k;
  i = (i >> 23) - 128;
  float l = (float)k;
  //float l = f32_zero_exponent(x);
  k = (int32_t)(l*(1.334968914f - 4.11091368e-8f*l) + 41437.706985f);
  k = (int32_t)f32_one_bits_k|k;
  float y = *(float*) & k;
  y = y + (float)i;
  return y;
}
#endif



float log2_g2(float x)
{
  const float a[] = { 0x1.5b7b00p+11f, 0x1.5b07a6p+15f, 0x1.d0c444p+16f, 0x1.aca78cp+17f,
                      0x1.4c79ccp+18f, 0x1.d27722p+18f, 0x1.327e98p+19f, 0x1.80b3cap+19f,
                      0x1.d2cb20p+19f, 0x1.13f436p+20f, 0x1.3fac46p+20f, 0x1.6c448ep+20f,
                      0x1.998010p+20f, 0x1.c73028p+20f, 0x1.f5293ap+20f, 0x1.1397b0p+21f };
  const float b[] = { 0x1.663f70p+0f,  0x1.51c3d8p+0f,  0x1.3f7f90p+0f,  0x1.2f1b3cp+0f,
                      0x1.205090p+0f,  0x1.12e656p+0f,  0x1.06ad88p+0f,  0x1.f6fe46p-1f,
                      0x1.e27504p-1f,  0x1.cf8842p-1f,  0x1.be0948p-1f,  0x1.add028p-1f,
                      0x1.9eba92p-1f,  0x1.90aadep-1f,  0x1.838746p-1f,  0x1.763428p-1f };

  
  int32_t i =  *(int32_t*) & x;
  int32_t k = i & 0x007fffff;
  int32_t j = k >> 19;
  float   l = (float)k;

  i = i >> 23;
  i = i - 128;
  k = f32_linear_bithack(l,b[j],a[j]);
  k = 0x3f800000|k;

  float y = asfloat(k);

  return y + (float)i;
}




float log2_g0(float x)
{
  const float c0[] = { 0x1.c1620ep+4f,  0x1.363992p+11f, 0x1.4f921ap+13f, 0x1.9eff9cp+14f,
                       0x1.8c1e18p+15f, 0x1.45724ep+16f, 0x1.e511cep+16f, 0x1.51a338p+17f,
                       0x1.befdc8p+17f, 0x1.1cbb14p+18f, 0x1.5ff26ap+18f, 0x1.a8945cp+18f,
                       0x1.f61704p+18f, 0x1.23fc72p+19f, 0x1.4ee0e6p+19f, 0x1.7b8182p+19f };
  const float c1[] = { 0x1.71144cp+0f,  0x1.6eb340p+0f,  0x1.6aa9dap+0f,  0x1.658100p+0f,
                       0x1.5f9998p+0f,  0x1.5938bep+0f,  0x1.52900cp+0f,  0x1.4bc33cp+0f,
                       0x1.44ec00p+0f,  0x1.3e1cd6p+0f,  0x1.3762e8p+0f,  0x1.30c780p+0f,
                       0x1.2a5108p+0f,  0x1.2403c6p+0f,  0x1.1de26ep+0f,  0x1.17ee86p+0f };
  const float c2[] = { 0x1.5b7c22p-24f, 0x1.34e324p-24f, 0x1.1461bap-24f, 0x1.f18032p-25f,
                       0x1.c2215cp-25f, 0x1.9937bap-25f, 0x1.75a3dep-25f, 0x1.56823cp-25f,
                       0x1.3b1cc0p-25f, 0x1.22e05cp-25f, 0x1.0d5538p-25f, 0x1.f431c4p-26f,
                       0x1.d1b3c4p-26f, 0x1.b2a8a4p-26f, 0x1.969e7cp-26f, 0x1.7d353p-26f };
  
  int32_t i =  *(int*) & x;
  int32_t k = i & 0x007fffff;
  float   l = (float)k;
  int32_t j = k>>19;
  
  k  = (int32_t)(c0[j] + l*(c1[j] - c2[j]*l));
  k |= 0x3f800000;

  float y = asfloat(k);
  
  y = y + (float)(logbf(x)-1);

  return y;
}





float log2_g1(float x)
{
  const float m[]  = { 0.60124843e-7f, 0.30062421e-7f };
  const float a0[] = { 1.4130103f,     1.2080635f };
  const float a1[] = { 5230.6446f,     364861.f  };
  
  int32_t i = *(int*) & x;
  int32_t k = i & 0x007fffff;
  float   l = (float)k;

  i = (i >> 23) - 128;

  int id = k > 0x003504f3;

  k  = f32_linear_bithack(l, -fmaf(l,m[id],-a0[id]), a1[id]);
  k |= 0x3f800000;

  return asfloat(k) + (float)i;
}







// the way the code is structured these cannot have opposite signs AND
// 
#if 1
const float start_x = 1.0f;    // inclusive
const float stop_x  = 2.0f;    // exclusive
#else
const float start_x = 0.0f;     // inclusive
const float stop_x  = 100.0f;   // exclusive
#endif

void test_exp(char* name, float (*f)(float), char* note)
{
  float    x     = start_x;
  uint32_t u     = f32_to_bits(x);
  float    min_e = 0;
  float    max_e = 0;
  float    min_x = x;
  float    max_x = x;

  do {
    x = f32_from_bits(u++);

    float r0 = expf(x);
    float r1 = f(x);
    float t  = (r0-r1)/r0;

    // formulations aren't balanced so track both postive and negative
    if (t > max_e)      { max_e = t; max_x = x; }
    else if (t < min_e) { min_e = t; min_x = x; }
    
  } while(x < stop_x);


  printf("%-10s // peek = (% f,%f): example x = (%f,%f)  (%a,%a) : %s\n", name,
         min_e,max_e,min_x,max_x,min_x,max_x, note);
}

void test_exp2(char* name, float (*f)(float), char* note)
{
  float    x     = start_x;
  uint32_t u     = f32_to_bits(x);
  float    min_e = 0;
  float    max_e = 0;
  float    min_x = x;
  float    max_x = x;

  do {
    x = f32_from_bits(u++);

    float r0 = exp2f(x);
    float r1 = f(x);
    float t  = (r0-r1)/r0;

    // formulations aren't balanced so track both postive and negative
    if (t > max_e)      { max_e = t; max_x = x; }
    else if (t < min_e) { min_e = t; min_x = x; }
    
  } while(x < stop_x);


  printf("%-10s // peek = (% f,%f): example x = (%f,%f)  (%a,%a) : %s\n", name,
         min_e,max_e,min_x,max_x,min_x,max_x,note);
}



void test_log2(char* name, float (*f)(float), char* note)
{
  float    start = 0.125f;
  float    x     = start;
  uint32_t u     = f32_to_bits(x);
  float    min_e =  10.f;
  float    max_e = -10.f;
  float    min_x = x;
  float    max_x = x;

  do {
    x = f32_from_bits(u++);

    float r0 = log2f(x);
    float r1 = f(x);
    float t  = (r0-r1)/r0;

    // formulations aren't balanced so track both postive and negative
    if (t > max_e)      { max_e = t; max_x = x; }
    else if (t < min_e) { min_e = t; min_x = x; }
    
  } while(x < 2.f*start);


  printf("%-10s // peek = (% f,%f): example x = (%f,%f)  (%a,%a) : %s\n", name,
         min_e,max_e,min_x,max_x,min_x,max_x, note);
}


void find(void)
{
}

int main(void)
{
  foo();
  find();

  double x = f64_nan;
  printf("%a\n", f64_zero_exponent(x));
  
#if 1  
  printf("\nexp(x)\n");
  test_exp("exp_g0", exp_g0, "");
  test_exp("exp_g1", exp_g1, "");
  test_exp("exp_g2", exp_g2, "");
  
  printf("\nexp2(x)\n");
  test_exp2("exp2_g0", exp2_g0, "");
  test_exp2("exp2_g1", exp2_g1, "");
  test_exp2("exp2_g2", exp2_g2, "");
#endif  

#if 1
  printf("\nexp(x) : FPGA/low powered designed\n");
  test_exp("exp_l0", exp_l0, "");
  test_exp("exp_l1", exp_l1, "");
  test_exp("exp_l2", exp_l2, "");
  test_exp("exp_l3", exp_l3, "");
  test_exp("exp_l4", exp_l4, "");

  printf("\nexp2(x) : FPGA/low powered designed\n");
  test_exp2("exp2_l0", exp2_l0, "");
  test_exp2("exp2_l1", exp2_l1, "");
  test_exp2("exp2_l2", exp2_l2, "");
  test_exp2("exp2_l3", exp2_l3, "");
  test_exp2("exp2_l4", exp2_l4, "");
#endif

  printf("\nlog2(x)\n");
  test_log2("log2_m",  log2_m,  "");
  test_log2("log2_gx", log2_gx, "");
  test_log2("log2_g0", log2_g0, "");
  test_log2("log2_g2", log2_g2, "");
  test_log2("log2_g1", log2_g1, "");
  test_log2("log2_h2", log2_h2, "");
  test_log2("log2_h3", log2_h3, "");
  test_log2("log2_h4", log2_h4, "");
  test_log2("log2_h5", log2_h5, "");

  
  return 0;
}


