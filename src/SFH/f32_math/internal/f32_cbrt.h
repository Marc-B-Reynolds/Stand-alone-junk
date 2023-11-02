// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

#ifndef F32_CBRT_H
#define F32_CBRT_H

// THESE ARE ONLY TARGETING NORMAL INPUTS. should add an
// expansion option for denormal support. (temp hack one
// in place in test/cbrt.c. total garbage performace
// though)

//********************************************************
// "Fast Calculation of Cube and Inverse Cube Roots Using
// a Magic Constant and Its Implementation on Microcontrollers"
// Moroz, Samotyy, Walczyk, Cieslinski, 2021
// (PDF: https://www.mdpi.com/1996-1073/14/4/1058)
//
// NOTE: algorithm 6 in print is missing a term

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
  static const float c0 =  0x1.c09806p0f;
  static const float c1 = -0x1.403e6cp0f;
  static const float c2 =  0x1.04cdb2p-1f;

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
  static const float c0 =  0x1.c09806p0f;
  static const float c1 = -0x1.403e6cp0f;
  static const float c2 =  0x1.04cdb2p-1f;

  uint32_t xi = f32_to_bits(x);

  xi = 0x548c2b4b - xi/3;

  float y = f32_from_bits(xi);
  float c = x*(y*y*y);                  // (*)
  
  y = y*fmaf(c, fmaf(c2,c,c1), c0);

  return y;
}

//********************************************************

// "Generalising the Fast Reciprocal Square Root Algorithm", Mike Day, 2023
// (PDF: https://arxiv.org/abs/2307.15600)

// listing 9
static inline float FRCR_Deg1(float x)
{
  static const float c0 =  1.8696972f;
  static const float c1 = -1.2857759f;
  
  uint32_t xi = f32_to_bits(x);
  uint32_t Y  = 0x54638AFE - xi/3;
  float    y  = f32_from_bits(Y);

  return y * fmaf(x*y, c1*(y*y),   c0);      //  37|   1796014|   3024359|   1649561|  18695891
  return y * fmaf(x*c1, y*(y*y),   c0);      //  37|   1796284|   3023066|   1650161|  18696314
  return y * fmaf(x, (c1*y)*(y*y), c0);      //  37|   1796498|   3023261|   1649724|  18696342
  return y * fmaf(c1, (x*y)*(y*y), c0);      //  37|   1796751|   3023133|   1650162|  18695779
  return y * (c0 + (x*y)*(y*y)*c1);          //  37|   1796874|   3023724|   1649076|  18696151
}

// listing 10
// (1) breaks dep-chain & minor improvment
static inline float FRCR_Deg2(float x)
{
  static const float c0 =  1.3739948f;   //  0x1.5fbe2p0
  static const float c1 = -0.47285829f;  // -0x1.e434f6p-2
  static const float c2 =  0.092823250f; //  0x1.7c343cp-4
  
  uint32_t xi = f32_to_bits(x);
  uint32_t Y  = 0x54B8E38E - xi/3;
  float    y  = f32_from_bits(Y);
  float    z  = (x*y)*(y*y);            // (1)

  // changed to explict FMA formulation
  return y * fmaf(z,fmaf(z,c2,c1),c0);
}

#if 0
// listing 11: computes x^(-2/3)
static inline float FRCR2_Deg1(float x)
{
  uint32_t xi = f32_to_bits(x);
  uint32_t Y  = 0x69BC56FC - 2*xi/3;
  float    y  = f32_from_bits(Y);
  float    w  = 0.8152238f * y;
  float    v  = x * w;
  return w * (1.7563311f - v*v*w);
}
#endif


//********************************************************
// range expansions

// expand a cbrt/rcbrt core method for positive only input
static inline float f32_cbrt_px(float x, float (*f)(float))
{
  return f(x);                   
}

// expand a cbrt/rcbrt core method for full range: cbrt(-x) = -cbrt(x)
// * good for signed zeroes, normal numbers, NaNs and both infinites
//   BUT is garbage for denormals.
static inline float f32_cbrt_fx(float x, float (*f)(float))
{
  return f32_odd_reduce(x, f);
}

//********************************************************
// root finding steps

// SEE: https://marc-b-reynolds.github.io/math/2019/03/12/FpDiv.html
// RN(1/3) = RU(1/3)
static const float  f32_cbrt_third    =  0x1.555556p-2f;
static const float  f32_cbrt_third_l  = -0x1.555556p-27f;
static const double f64_cbrt_third    =  0x1.5555555555555p-2;
static const double f64_cbrt_third_l  =  0x1.5555555555555p-56;

// 1/cbrt(x) newton step with correctly rounded div by 3
// y = y_n (estimate in)

static inline float f32_rcbrt_newton(float x, float y)
{
  float c = fmaf(x,y*y*y, -1.0f);                   // xy^3-1
  float t = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));  // -RN(c/3)
  
  return fmaf(y,t,y);
}

static inline float f32_rcbrt_newton_m(float x, float y)
{
  float c = fmaf(x*y,y*y, -1.0f);
  float t = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));
  
  return fmaf(y,t,y);
}

// 1/cbrt(x) newton step with div by 3 as mul by recip
// y = y_n (estimate in)
static inline float f32_rcbrt_newton_fast(float x, float y)
{
  float c = fmaf(x,y*y*y, -1.0f);

  y = y*fmaf(-f32_cbrt_third,c,1.f);

  return y;
}

// dep-chain breakup: slightly hampers results
static inline float f32_rcbrt_newton_fast_m(float x, float y)
{
  float c = fmaf(x*y,y*y, -1.0f);

  y = y*fmaf(-f32_cbrt_third,c,1.f);

  return y;
}

// 1/cbrt(x) newton step with correctly rounded div by 3
// y = y_n (estimate in)
static inline double f64_rcbrt_newton(double x, double y)
{
  double c = fma(x,y*y*y, -1.0);
  double t = fma(c, -f64_cbrt_third, c*(-f64_cbrt_third_l));

  y = fma(y,t,y);
  
  return y;
}

static inline double f64_rcbrt_newton_m(double x, double y)
{
  double c = fma(x*y,y*y, -1.0);
  double t = fma(c, -f64_cbrt_third, c*(-f64_cbrt_third_l));

  y = fma(y,t,y);
  
  return y;
}

// 1/cbrt(x) newton step with div by 3 as mul by recip
// y = y_n (estimate in)
static inline double f64_rcbrt_newton_fast(double x, double y)
{
  double c = fma(x,y*y*y, -1.0);

  y = y*fma(-f64_cbrt_third,c,1.0);

  return y;
}

static inline double f64_rcbrt_newton_fast_m(double x, double y)
{
  double c = fma(x*y,y*y, -1.0);

  y = y*fma(-f64_cbrt_third,c,1.0);

  return y;
}

// Newton step:
//   y       = cbrt(x)
//   f(y)    = y^3 - x
//   f'(y)   = 3y^2
//   y_{n+1} = y - f(y)/f'(y)
//           = y - (y^3 - x)/3y^2


// cbrt(x) newton step. y = ~1/cbrt(x)
static inline float f32_cbrt_newton_fast_(float x, float y)
{
  float d = x*(y*y);
  float c = fmaf(d,y,-1.f);
  float t = fmaf(c, -f32_cbrt_third, 1.f);
  y = d*(t*t);

  return y;
}

static inline float f32_cbrt_newton_fast(float x, float y)
{
  float a = x*y;
  float d = a*y;
  float c = fmaf(d,y,-1.f);
  float t = fmaf(c, -f32_cbrt_third, 1.f);
  y = d*(t*t);

  return y;
}

static inline double f64_cbrt_newton_fast(double x, double y)
{
  double a = x*y;
  double d = a*y;
  double c = fma(d,y,-1.0);
  double t = fma(c, -f64_cbrt_third, 1.0);
  y = d*(t*t);

  return y;
}

// temp hack (not reworked yet)
static inline float f32_cbrt_newton(float x, float y)
{
#if 0
  float d = (x*y)*y;
  float c = fmaf(d,y,-1.f);
  float s = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));  // -RN(c/3)
  float t = s + 1.f;
  y = d*(t*t);
#else
  float d = (x*y)*y;
  float c = fmaf(d,y,-1.f);
  float s = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));  // -RN(c/3)
  //y = d*((s+1.f)*(s+1.f));
  //y = d*(s*(s+1.f)+(s+1.f));

  y = d*( fmaf(s,(s+1.f),(s+1.f)));

#endif  

  return y;
}

// ( currently unused )
// 1/cbrt(x) Halley step:
//   y_{n+1} = y (1 + c(1/3 + 2/9 c)) , c = 1-xy^3
//           = y (1 + (1/3c+ 2/9 c^2)), 
//           = y (1 + (t + 2t^2)),   t = c/3
//           = y (1 + a),   t = c/3

static inline float f32_rcbrt_halley(float x, float y)
{
  float c = fmaf(x,y*y*y, -1.0f);                   // xy^3-1
  float t = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));  // -RN(c/3)
  float a = fmaf(2.f*t,t,t);
  return fmaf(y,a,y);
}

static inline float f32_rcbrt_halley_m(float x, float y)
{
  float c = fmaf(x*y,y*y, -1.0f);                   // xy^3-1
  float t = fmaf(c, -f32_cbrt_third, c*(-f32_cbrt_third_l));  // -RN(c/3)
  float a = fmaf(2.f*t,t,t);
  return fmaf(y,a,y);
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

#if 0
// For fail reference:
//   cbrt(x) = x * f(x*x)  { where f is ~1/cbrt }
// this style of computation narrows usable domain (minor issue)
// and only performs slightly better that 'f32_cbrt_k1' on
// average error and same for error bound so the two
// extra products don't seem to justify themselves.
// compare to:
//   'f32_cbrt_k2' which uses +1 product & +1 sqrt
//   'f32_cbrt_k4' which uses double promote/demote
// ulp   3
// correctly:   62.181562
// faithfully:  37.071056
//   2 ulp:      0.747307 
//   3 ulp:      0.000075 
static inline float f32_cbrt_nope(float x)
{
  float s = x*x;
  float y = f32_cbrt_ki(s);

  return x * f32_rcbrt_newton_fast_m(s,y);
}
#endif

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


static inline float f32_cbrt_d1(float x)
{
  float y = FRCR_Deg1(x);

  return f32_cbrt_newton_fast(x,y);
}

static inline float f32_cbrt_d2(float x)
{
  float y = FRCR_Deg2(x);

  return f32_cbrt_newton_fast(x,y);
}



#endif

