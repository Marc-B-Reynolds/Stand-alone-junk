// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "f32_util.h"

// cube root and reciprocal cube root

// SEE:
// * https://gist.github.com/Marc-B-Reynolds/739a46f55c2a9ead54f4d0629ee5e417
// * "Fast Calculation of Cube and Inverse Cube Roots Using a Magic Constant
//   and Its Implementation on Microcontrollers", Moroz, Samotyy, Walczyk,
//   Cieslinski, 2021 (algorithm 5 (A5) and algorithm 6)
//   https://www.mdpi.com/1996-1073/14/4/1058

// RN(1/3) = RU(1/3)
static const float one_third    =  0x1.555556p-2f;

// for correctly rounded x/3
// SEE: https://marc-b-reynolds.github.io/math/2019/03/12/FpDiv.html
static const float one_third_l  = -0x1.555556p-27f;


// core portion of A5/A6 (so paper version)
// common computation for x^(1/3) and x^(-1/3)
static inline float f32_cbrt_k(float x)
{
  const float c0 =  0x1.c09806p0f;
  const float c1 = -0x1.403e6cp0f;
  const float c2 =  0x1.04cdb2p-1f;

  int32_t i = (int32_t)f32_to_bits(x);
  i = 0x548c2b4b - i/3;

  float y = f32_from_bits((uint32_t)i);
  float c = x*y*y*y;
  
  y = y*fmaf(c, fmaf(c2,c,c1), c0);

  return y;
}

// reworked core portion of A5/A6. Changed to only
// positive input because:
//   * the integer division by 3 is cheaper in unsigned
//   * meet expectation that cbrt(-x) = -cbrt(x)
// common computation for x^(1/3) and x^(-1/3)
//   x  is positive input (|x| of original)
//   xi is IEEE bit pattern of x
static inline float f32_cbrt_ki(float x, uint32_t xi)
{
  const float c0 =  0x1.c09806p0f;
  const float c1 = -0x1.403e6cp0f;
  const float c2 =  0x1.04cdb2p-1f;

  xi = 0x548c2b4b - xi/3;

  float y = f32_from_bits(xi);
  float c = (x*y)*(y*y);
  
  y = y*fmaf(c, fmaf(c2,c,c1), c0);

  return y;
}


// A5
// ulp   2
// correctly:   72.750709% (18308316)
// faithfully:  27.207556% (6847006)
//   2 ulp:      0.041735% (10503)
// ~17 cycles
float f32_rcbrt(float x)
{
  float y = f32_cbrt_k(x);
  float t = y*y*y;
  float c = fmaf(x,t, -1.0f);

  y = y*fmaf(-one_third,c,1.f);
  
  return y;
}

// A5: modified by
// * core on positive (as noted in recip versions)
// * correctly rounded divison by 3
// ulp   1
// correctly:   88.337028% (22230742)
// faithfully:  11.662972% (2935083)
float f32_rcbrt_fr(float x)
{
  uint32_t ix = f32_to_bits(x);   // bit pattern of x
  uint32_t px = ix & 0x7fffffff;  // fabs(x)
  uint32_t sx = ix & 0x80000000;  // sign(x)

  x = f32_from_bits(px);          // |x|
  
  float y = f32_cbrt_ki(x,ix);
  float t = y*y*y;
  float c = fmaf(x,t, -1.0f);

  t = fmaf(c, -one_third, c*(-one_third_l));
  y = fmaf(y,t,y);
  
  return f32_mulsign(y,sx);
}


// ulp   2
// correctly:   77.490049% (19501010)
// faithfully:  22.509554% (5664715)
//   2 ulp:      0.000397% (100)
float f32_cbrt(float x)
{
  uint32_t ix = f32_to_bits(x);   // bit pattern of x
  uint32_t px = ix & 0x7fffffff;  // fabs(x)
  uint32_t sx = ix & 0x80000000;  // sign(x)

  x = f32_from_bits(px);          // |x|
  
  float y  = f32_cbrt_ki(x,ix);
  float a = x*y;

  y = a*fmaf(a*y,y,-4.f);
  y = fmaf(y, -one_third, y*(-one_third_l));
  y = f32_sqrt(y);

  return f32_mulsign(y,sx);
}


// correctly:   81.791998% (20583631)
// faithfully:  18.208002% ( 4582194)
// ~20.272688 cycles
float f32_cbrt_fr(float x)
{
  uint32_t ix = f32_to_bits(x);   // bit pattern of x
  uint32_t px = ix & 0x7fffffff;  // fabs(x)
  uint32_t sx = ix & 0x80000000;  // sign(x)

  x = f32_from_bits(px);          // |x|
  
  float    y  = f32_cbrt_ki(x,ix);
  float    t  = y*y*y;
  float    c  = fmaf(x,t, -1.0f);

  // c = RN(c/3) : correctly rounded
  c = fmaf(c, -one_third, c*(-one_third_l));
  y = y*fmaf(x,c,x);  
  y = f32_sqrt(y);

  return f32_mulsign(y,sx);
}
