
#ifndef F32_CBRT_H
#define F32_CBRT_H

// populate this dude

// core approximations based on:
// "Fast Calculation of Cube and Inverse Cube Roots Using
// a Magic Constant and Its Implementation on Microcontrollers"
// Moroz, Samotyy, Walczyk, Cieslinski, 2021
// (PDF: https://www.mdpi.com/1996-1073/14/4/1058)
//
// NOTE: algorithm 6 in print is missing a term

//********************************************************
// paper's inital "guess" method

// common computation for x^(1/3) and x^(-1/3)
//   x  is positive
//
// (reworked core portion of A5/A6 by Moroz,
//  Samotyy, Walczyk, Cieslinski)
//
// (1) unsigned div by constant faster than signed
// (2) breaks dependences up (y*x) & (y*y) can be
//     'in-flight' at the same time. The other
//     two choices the product chain is serial. 
static inline float f32_cbrt_ki(float x)
{
  // authors optimized Halley's method first step
  // given their "magic" constant (in 1)
  const float c0 =  0x1.c09806p0f;
  const float c1 = -0x1.403e6cp0f;
  const float c2 =  0x1.04cdb2p-1f;

  uint32_t xi = f32_to_bits(x);
  
  // initial guess
  xi = 0x548c2b4b - xi/3;               // (1)

  // modified Halley's method
  float y = f32_from_bits(xi);
  float c = (x*y)*(y*y);                // (2)
  
  y = y*fmaf(c, fmaf(c2,c,c1), c0);

  return y;
}


//********************************************************




//********************************************************

// Author's corrected version (algorithm 6.1).
// (modified)
// ulp   3
// correctly:   52.266572% (13153314)
// faithfully:  45.591750% (11473540)
//   2 ulp:      2.141619% (538956)
//   3 ulp:      0.000060% (15)
static inline float f32_cbrt_k_1(float x)
{
  float y = f32_cbrt_ki(x);

  return f32_cbrt_newton_fast(x,y);
}





#endif
