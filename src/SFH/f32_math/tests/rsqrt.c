// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "internal/f32_rsqrt.h"

#include "util.h"


//********************************************************
// helpers: mostly to make the code more readable than
// the original versions & easier to experiment.

// y = y(1/2 - 1/2 xy^2)
//   = y(3/2 - 1/2 xy^2)
//   = y + y(1/2 - 1/2 xy^2)
//
// h = 1/2x
// v = 1/2 - 1/2 xy^2
// r = y^2
//
// y = y + y(1/2 - h(r+t))
//   = y + y(1/2 - hr - ht)



// Newton-Raphson step for 1/sqrt(x) using FMA when
// we haven't computed (or approximated) 1/x
//   y = estimated in (y_n)
//   h = -x/2

#if 0
static inline float f32_rsqrt_nr_nodiv_step(float y, float h)
{
  return y*fmaf(h,y*y, 1.5f);
}
#else
static inline float f32_rsqrt_nr_nodiv_step(float y, float h)
//static inline float f32_rsqrt_nr_step_v0(float y, float h)
{
  float r = y*y;
  float v = fmaf(h,r, 0.5f);
  return fmaf(y,v,y);
}
#endif

static inline float f32_rsqrt_nr_foo(float x, float y)
{
  float h = -0.5f*x;
  float r = y*y;
  float v = fmaf(h,r, 0.5f);
  return fmaf(y,v,y);
}

// humm...don't think this is interesting
static inline float f32_rsqrt_nr_bar(float x, float y)
{
  float h = -0.5f*x;
  float r = y*y;
  float t = fmaf(y,y,-r);
  float v = fmaf(h,r, 0.5f);
  return fmaf(y,fmaf(h,t,v),y);
}

//**********************************************************************

static inline float f32_rsqrt_quake_step(float x, uint32_t K)
{
  return f32_from_bits(K - (f32_to_bits(x) >> 1));
}

static inline float f32_rsqrt_initial(float x, uint32_t K)
{
  return f32_from_bits(K - (f32_to_bits(x) >> 1));
}

static inline float f32_rsqrt_initial_md(float x, uint32_t K)
{
  return f32_from_bits((K-f32_to_bits(x)) >> 1);
}

//*********************************************************************


// Mike Day core routines as in paper. Without star is paper
// and contractions are disabled. Starred versions are explict FMAs
// where version choosen uses the greedy measure to minimize
// the counts of >2 ulp results. 
// |         func |max ULP|    CR|     FR|  2 ULP|  > 2 ULP|
// |         ---: |   ---:|  ---:|   ---:|   ---:|     ---:|
// |    FRSR_Mon0 | 564177|    12|     28|     23| 25165761|
// |    FRSR_Deg0 | 403258|    11|     26|     24| 25165763|
// |    FRSR_Mon1 |  14751|   557|   1070|   1063| 25163134|
// |    FRSR_Mon1*|  14751|   545|   1057|   1099| 25163123|
// |    FRSR_Deg1 |  10215|   941|   1802|   1939| 25161142|
// |    FRSR_Deg1*|  10215|  1051|   1848|   1963| 25160962|
// | FRSR_Deg1Alt |  10219|   874|   1858|   2033| 25161059|
// | FRSR_Deg1Alt*|  10219|   972|   1920|   1901| 25161031|
// |    FRSR_Mon2 |    317|105658| 214279| 222937| 24622950|
// |    FRSR_Mon2*|    317|105658| 214279| 222937| 24622950|

// listing 2 (FRSR_Mon0)
// max-ulp: 564177 
float FRSR_Mon0(float x)
{
  return f32_rsqrt_initial(x, 0x5F37642F);
}

// listing 3 (FRSR_Deg0)
// max-ulp: 403258
float FRSR_Deg0(float x)
{
  return 0.79247999f * f32_rsqrt_initial_md(x, 0xBEBFFDAA);
}

// listing 4 (FRSR_Mon1)
// max-ulp: 14751
// |     CR|    FR |  2 ULP| > 2 ULP |
// |    557|   1070|   1063| 25163134| 1
// |    497|   1094|   1097| 25163136| 2
// |    545|   1057|   1099| 25163123| 3
// 3 fma/mul
float FRSR_Mon1(float x)
{
  float y = f32_rsqrt_initial_md(x, 0xBE167122);

//return y * (1.8909901f - x*y*y);       // 1
//return y * fmaf(-x, y*y, 1.8909901f);  // 2
  return y * fmaf(-y, x*y, 1.8909901f);  // 3
}

// listing 5 (FRSR_Deg1)
// max-ulp: 10215   
// |     CR|    FR |  2 ULP| > 2 ULP |
// |    941|   1802|   1939| 25161142| 1
// |    992|   1907|   1882| 25161043| 2
// |   1051|   1848|   1963| 25160962| 3
// |    976|   1907|   1894| 25161047| 4
// 4 fma/mul
float FRSR_Deg1(float x)
{
  static const float c0 =  1.1893165f;
  static const float c1 = -0.24889956f;
  
  float y = f32_rsqrt_initial(x, 0x5F5FFF00);
  
//return y * (c0 + x*y*y*c1);            // 1
//return y * fmaf(x, y*y*c1, c0);        // 2
  return y * fmaf(x*y, y*c1, c0);        // 3
//return y * fmaf(x*c1,y*y,  c0);        // 4
}

// listing 6 (FRSR_Deg1Alt)
// max-ulp: 10219    
// |     CR|    FR |  2 ULP| > 2 ULP |
// |    874|   1858|   2033| 25161059| 1
// |    972|   1920|   1901| 25161031| 2
// |    950|   1991|   1908| 25160975| 3
// |    972|   1920|   1901| 25161031| 4
// 4 fma/mul
float FRSR_Deg1Alt(float x)
{
  static const float c0 =  1.1891762f;
  static const float c1 = -0.24881148f;

  float y = f32_rsqrt_initial(x, 0x5F6004CC);

//return y * (c0 + y*y*x*c1);            // 1
//return y * fmaf(y*y, x*c1, c0);        // 2
//return y * fmaf(x*y, y*c1, c0);        // 3
  return y * fmaf(x*c1,y*y,  c0);        // 4
}

// listing 7 (FRSR_Mon2)
// max-ulp: 317    
// |     CR|    FR |  2 ULP| > 2 ULP |
// | 105658| 214279| 222937| 24622950| 1
// | 105658| 214279| 222937| 24622950| 2
// | 106447| 213978| 221946| 24623453| 3
// 4 fma/mul, 1 add
float rsqrt_day(float x)
{
  static const float c0 =  2.2825186f;
  static const float c1 = -2.253305f;
  
  float y = f32_rsqrt_initial(x, 0x5F11107D);
  float z = x*y*y;

//return y * (c0 + z *(z+c1));           // 1
  return y * fmaf(z, z+c1, c0);          // 2
//return y * fmaf(z,z,fmaf(z,c1,c0));    // 3
}

// max-ulp:       2
// correctly:    70.604507% (17768206)
// faithfully:   29.345020% (7384916)
// 2 ulp:         0.050473% (12702)
// 8 fma/mul, 1 add
float rsqrt_day_r1(float x)
{
  float y = rsqrt_day(x);
  //return f32_rsqrt_nr_nodiv_step(y, -0.5f*x);
  return f32_rsqrt_nr_bar(x,y);
}

// max-ulp:      1
// correctly:   99.667398% (25082122)
// faithfully:   0.332602% (83702)
// 10 fma/mul, 1 add
float rsqrt_day_r2(float x)
{
  float y = rsqrt_day(x);
  return f32_rsqrt_nr_step(y, -0.5f*x, y*y);
}

// max-ulp:      1
// correctly:   99.999559% (25165713)
// faithfully:   0.000441% (111)
// 12 fma/mul, 1 add
float rsqrt_day_hm(float x)
{
  float y = rsqrt_day(x);
  return f32_rsqrt_hm_step(y, -0.5f*x, y*y);
}

float rsqrt_day_cr(float x)
{
  float y = rsqrt_day(x);
  float h = -0.5f*x;
  y = f32_rsqrt_hm_step(y, h, y*y);
  y = f32_rsqrt_nr_step(y, h, y*y);
  return y;
}

//*********************************************************************

// Walczyk, Moroz, Cieslinski, Cezary:
// "InvSqrt2" - modified to use FMA
//    no constant refinement
//  max-ulp:      11
//  correctly:    5.120015% (858996)
//  faithfully:  11.176844% (1814391)
//   2 ulp:      12.521724% (3151195)
//   >2 ulp:     71.061440% (17883197)
// 7 fma/mul
float rsqrt_wmc(float x)
{
  float h = -0.5f*x;
  
  x = f32_rsqrt_initial(x, 0x5F376908);
  x = x*fmaf(h,x*x, 1.5008789f);
  x = x*fmaf(h,x*x, 1.5000006f);
  return x;
}

//*********************************************************************
// "Modified Fast Inverse Square Root and Square Root Approximation
//  Algorithms: The Method of Switching Magic Constants"
//  Moroz, Volodymyr, Samotyy and Horyachyy, 2021

float rsqrt_msh(float x)
{
  static const uint32_t R[2] = {0x5f19e8b7, 0x5ed9e8b7};
  static const float    A[2] = {2.1499474f, 1.0749737f};
  static const float    M[2] = {0.824218631f, 2.33124256f};

  uint32_t id = (f32_to_bits(x) & 0x00800000) == 0 ? 0 : 1;  

  float y;

  y = f32_rsqrt_initial(x, R[id]);
  y = M[id]*y*fmaf(-x, y*y, A[id]);
  
  return y;
}


//*********************************************************************

#include <x86intrin.h>

// return hardware 1/sqrt(x) approx
static inline float rsqrt_sse(float x)
{
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

// 4 fma/mul
float rsqrt_sse_r1(float x)
{
  float y = rsqrt_sse(x);
  return f32_rsqrt_nr_nodiv_step(y, -0.5f*x);
}

// 8 fma/mul
float rsqrt_sse_r2(float x)
{
  float y = rsqrt_sse(x);
  return f32_rsqrt_nr_step(y, -0.5f*x, y*y);
}

// 8 fma/mul
float rsqrt_sse_hm(float x)
{
  float y = rsqrt_sse(x);
  return f32_rsqrt_hm_step(y, -0.5f*x, y*y);
}

// 13 fma/mul
float rsqrt_sse_cr(float x)
{
  float y = rsqrt_sse(x);
  float h = -0.5f*x;
  y = f32_rsqrt_hm_step(y, h, y*y);
  y = f32_rsqrt_nr_step(y, h, y*y);
  return y;
}

float rsqrt_nope(float x)
{
  return f32_sqrt(x)*(1.f/x);
}

//**********************************************************************

func_entry_t func_table[] =
{
#if 0
  ENTRY(FRSR_Mon0),
  ENTRY(FRSR_Deg0),
  ENTRY(FRSR_Mon1),
  ENTRY(FRSR_Deg1),
  ENTRY(FRSR_Deg1Alt),
  ENTRY(rsqrt_sse),
  ENTRY(rsqrt_msh),
  ENTRY(rsqrt_day),
  ENTRY(rsqrt_wmc),
  ENTRY(rsqrt_sse_r1),
  ENTRY(rsqrt_sse_r2),
  ENTRY(rsqrt_day_r1),
  ENTRY(f32_rsqrt),
  ENTRY(rsqrt_day_r2),
  ENTRY(rsqrt_sse_hm),
  ENTRY(rsqrt_day_hm),
  //ENTRY(f32_rsqrt_bar),
  // find ranges of these
  ENTRY(f32_rsqrt_cb),
  ENTRY(rsqrt_day_cr),
  ENTRY(rsqrt_sse_cr),
#else
  //ENTRY(f32_rsqrt_pd),
  ENTRY(rsqrt_nope),
#endif  
};

const char* func_name = "rsqrt";

float cr_func(float x) { return f32_rsqrt_cr(x); }

// just annoying here IMHO
#define NO_TEST_SPECIALS

#include "common.h"

//********************************************************

void test_spot(void)
{
  // [1,4) -> [1,2)
  test_1pot(1.f);
  test_1pot(2.f);
}

void test_all(void)
{
  float    x0;
  uint32_t xs,xe;

  test_spot();

  // narrowed domain to not pollute numbers for
  // the correctly rounded functions which are only
  // CR on a specified domain.
  x0 = 0x1.0p-125f;
  xs = f32_to_bits(x0);
  xe = f32_to_bits(4.f*x0);
  test_force(xs,xe);

  x0 = 0x1.0p+120f;
  xs = f32_to_bits(x0);
  xe = f32_to_bits(4.f*x0);
  test_force(xs,xe);
}

void test_sanity(void)
{
}

int main(int argc, char** argv)
{
  return test_run(argc, argv);
}
