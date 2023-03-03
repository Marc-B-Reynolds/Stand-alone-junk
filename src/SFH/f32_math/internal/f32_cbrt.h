
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


// (*) evaluation order flipped. it's possible
// for some combos this is a slight win.
static inline float f32_cbrt_ki_m(float x)
{
  const float c0 =  0x1.c09806p0f;
  const float c1 = -0x1.403e6cp0f;
  const float c2 =  0x1.04cdb2p-1f;

  uint32_t xi = f32_to_bits(x);

  xi = 0x548c2b4b - xi/3;

  float y = f32_from_bits(xi);
  float c = x*(y*y*y);                  // (*)
  
  y = y*fmaf(c, fmaf(c2,c,c1), c0);

  return y;
}


//********************************************************
// range expansions

// expand a cbrt/rcbrt core method for positive only input
static inline float f32_cbrt_px(float x, float (*f)(float))
{
  return f(x);                   
}

// expand a cbrt/rcbrt core method for full range: cbrt(-x) = -cbrt(x)
static inline float f32_cbrt_fx(float x, float (*f)(float))
{
  // TODO: flip to just using f32_odd_reduce
  
#if defined(F32_ODD_REDUCE_BITOPS)  
  // purely bit manipulation.
  
  uint32_t ix = f32_to_bits(x);     // bit pattern of x
  uint32_t sx = ix & 0x80000000;    // sign(x) {bit }
  uint32_t ax = ix ^ sx;            // |x|     {bits}
  float    a  = f32_from_bits(ax);  // |x|
  float    r  = f(a);               // core approximation
  
  return f32_mulsign(r, sx);        // restore sign

#elif defined(F32_ODD_REDUCE_STDFUNCS)
  // standard functions only
  float a     = fabsf(x);
  float r     = f(a);               // core approximation

  return copysignf(r, x);           // restore sign

#else
  // default: hybrid
  float    a  = fabsf(x); 
  float    sx = f32_xor(x,a);       // isolated sign bit of 'x'
  float    r  = f(a);               // core approximation

  return f32_xor(r, sx);            // restore sign
#endif  
}

//********************************************************

// Author's corrected version (algorithm 6.1).
// (modified)
// ulp   3
// correctly:   52.266572% (13153314)
// faithfully:  45.591750% (11473540)
//   2 ulp:      2.141619% (538956)
//   3 ulp:      0.000060% (15)
static inline float f32_cbrt_k1(float x)
{
  float y = f32_cbrt_ki(x);

  return f32_cbrt_newton_fast(x,y);
}

// sqrt(x*x^(-1/3)) = sqrt(x^(2/3)) = x^(1/3)
// ulp   2
// correctly:   77.553369% (19516945)
// faithfully:  22.445829% (5648678)
//   2 ulp:      0.000803% (202)
static inline float f32_cbrt_k2(float x)
{
  float y = f32_cbrt_ki(x);

  return sqrtf(x * f32_rcbrt_newton_fast_m(x,y));
}

// ulp   1
// correctly:   80.880794% (20354319)
// faithfully:  19.119206% (4811506)
static inline float f32_cbrt_k3(float x)
{
  float y = f32_cbrt_ki(x);

  return sqrtf(x * f32_rcbrt_newton_m(x,y));
}

// correctly:   98.239267% (24722722)
// faithfully:   1.760733% (443103)
static inline float f32_cbrt_k4(float x)
{
  double y = (double)f32_cbrt_ki(x);
  double d = (double)x;

  y = f64_cbrt_newton_fast(d,y);

  return (float)y;
}

// correctly rounded : but questionable perf vs. fdlibm
static inline float f32_cbrt_k5(float x)
{
  double y = (double)f32_cbrt_ki(x);
  double d = (double)x;

  y = f64_rcbrt_newton_fast(d,y);
  y = f64_cbrt_newton_fast(d,y);

  return (float)y;
}

#endif
