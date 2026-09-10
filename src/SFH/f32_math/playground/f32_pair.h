// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2026
// Public Domain under http://unlicense.org, see link for details.

// junk: just stripping down

#pragma once

#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct { float hi,lo; } fr_pair_t;
typedef struct { float hi,lo; } fe_pair_t;

static inline float fe_result(fe_pair_t x) { return x.hi;      }
static inline float fr_result(fr_pair_t x) { return x.hi+x.lo; }

static inline fe_pair_t fe_neg(fe_pair_t x) { return fe_pair(-x.hi, -x.lo); }
static inline fr_pair_t fr_neg(fr_pair_t x) { return fr_pair(-x.hi, -x.lo); }

static inline fe_pair_t fe_abs(fe_pair_t x)
{
  // doesn't flush lo = -0 to +0. don't think that
  // has any value in general
  return fe_pair(fabs(x.hi), x.hi >= 0.0 ? x.lo : -x.lo);
}

static inline fr_pair_t fr_abs(fr_pair_t x) { return fr2fe_uo_wrap(fe_abs,x); }

// $ (hi,lo) = x+y $
// rename variable to x,y
static inline fe_pair_t fe_two_sum(float a, float b)
{
  // 2Sum: 6 adds
  float x = a + b;
  float t = x - a;
  float y = (a - (x - t)) + (b - t);

  return fe_pair(x,y);
}

static inline fe_pair_t fe_two_diff(float a, float b)
{
  // 2Sum: 6 adds
  float x = a - b;
  float t = a - x;
  float y = (a - (x + t)) + (t - b);

  return fe_pair(x,y);
}


static inline fr_pair_t fr_two_sum(float x, float y)  { return fe2fr(fe_two_sum(x,y));  }
static inline fr_pair_t fr_two_diff(float x, float y) { return fe2fr(fe_two_diff(x,y)); }


// return x*y where 'y' is a power-of-two value
static inline fe_pair_t fe_mul_pot(float s, fe_pair_t x)
{
  return fe_pair(x.hi*s,x.lo*s);
}

static inline fr_pair_t fr_mul_pot(float s, fr_pair_t x)  { return fe2fr(fe_mul_pot(s,fr2fe(x)));  }


/// returns: $(hi,lo) = x+y$. requires |x| >= |y| (or x = 0)
///
/// * in absence of underflow and overflow the result is exact
/// * if the inputs are reversed: |x| < |y| then the error bound
///    is u|hi|. "Note on FastTwoSum with Directed Roundings",
///    2024, Corbineau & Zimmermann, theorem 3.
static inline fe_pair_t fe_fast_sum(float x, float y)
{
  // Fast2Sum: 3 adds
  float h = x+y;
  return fe_pair(h, y-(h-x));
}

static inline fr_pair_t fr_fast_sum(float x, float y) { return fe2fr(fe_fast_sum(x,y)); }

/// returns: $(a,b) = x-y$. requires |x| >= |y| (or x = 0)
static inline fe_pair_t fe_fast_diff(float x, float y)
{
  // Fast2Sum: 3 adds
  float h = x-y;
  return fe_pair(h, (x-h)-y);
}

static inline fr_pair_t fr_fast_diff(float x, float y) { return fe2fr(fe_fast_diff(x,y)); }

static inline fe_pair_t fe_two_mul(float x, float y)
{
  // 2Prod
  float hi = x*y;

  return fe_pair(hi, fma(x,y,-hi));
}

static inline fr_pair_t fr_two_mul(float x, float y)
{
  // 2Prod
  float hi = x*y;

  return fr_pair(hi, fma(x,y,-hi));
}

/// returns pair in unevaluated pair form
static inline fr_pair_t fr_normalize(fr_pair_t x)
{
  return fr_fast_sum(x.hi,x.lo);
}

static inline fe_pair_t fe_add_d(fe_pair_t x, float y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_sum(x.hi, y);
  float    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_add(float x, fe_pair_t y) { return fe_add_d(y,x); }


static inline fe_pair_t fe_oadd_d(fe_pair_t x, float y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 4.50 
  fe_pair_t t = fe_fast_sum(x.hi, y);
  float    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_oadd(float x, fe_pair_t y) { return fe_oadd_d(y,x); } 


static inline fe_pair_t fe_sub_dd(float x, float y) { return fe_two_diff(x,y); }
static inline fr_pair_t fr_sub_dd(float x, float y) { return fr_two_diff(x,y); }

static inline fe_pair_t fe_sub_d(fe_pair_t x, float y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_diff(x.hi, y);
  float    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_sub(float x, fe_pair_t y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_diff(x, y.hi);
  float    l = t.lo - y.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_osub_d(fe_pair_t x, float y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 24.50 
  fe_pair_t t = fe_fast_diff(x.hi, y);
  float    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}


static inline fe_pair_t fe_d_osub(float x, fe_pair_t y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 24.50 
  fe_pair_t t = fe_fast_diff(x, y.hi);
  float    l = t.lo - y.lo;
  
  return fe_fast_sum(t.hi,l);
}


static inline fr_pair_t fr_add_s (fr_pair_t x, fr_pair_t y) { return fr2fe_bo_wrap(fe_add_s,  x, y); }
static inline fr_pair_t fr_sub_s (fr_pair_t x, fr_pair_t y) { return fr2fe_bo_wrap(fe_sub_s,  x, y); }
static inline fr_pair_t fr_oadd_s(fr_pair_t x, fr_pair_t y) { return fr2fe_bo_wrap(fe_oadd_s, x, y); }
static inline fr_pair_t fr_osub_s(fr_pair_t x, fr_pair_t y) { return fr2fe_bo_wrap(fe_osub_s, x, y); }


static inline fr_pair_t fr_add(fr_pair_t x, fr_pair_t y)
{
  // CPairSum: 8 adds
  // uiCA: 13
  fr_pair_t t = fr_two_sum(x.hi,y.hi);
  return fr_pair(t.hi,t.lo+(x.lo+y.lo));
}

static inline fr_pair_t fr_oadd(fr_pair_t x, fr_pair_t y)
{
  // CPairSum (mod): 5 adds
  // uiCA: 8
  fr_pair_t t = fr_fast_sum(x.hi,y.hi);
  return fr_pair(t.hi,t.lo+(x.lo+y.lo));
}

static inline fr_pair_t fr_add_d(fr_pair_t a, float b)
{
  // CPairSum (mod): 7 adds
  // uiCA: 33.00 
  fr_pair_t t = fr_two_sum(a.hi, b);
  return fr_pair(t.hi, t.lo+a.lo);
}

static inline fr_pair_t fr_oadd_d(fr_pair_t a, float b)
{
  // CPairSum (mod): 4 adds
  // uiCA: 24.50 
  fr_pair_t t = fr_fast_sum(a.hi, b);
  return fr_pair(t.hi, t.lo+a.lo);
}

static inline fr_pair_t fr_sub(fr_pair_t x, fr_pair_t y)
{
  // CPairSum: 8 adds
  // uiCA: 13.00 
  fr_pair_t t = fr_two_diff(x.hi,y.hi);
  return fr_pair(t.hi,t.lo+(x.lo-y.lo));
}

static inline fr_pair_t fr_osub(fr_pair_t x, fr_pair_t y)
{
  // CPairSum (mod): 5 adds
  // uiCA: 8.00
  fr_pair_t t = fr_fast_diff(x.hi,y.hi);
  
  return fr_pair(t.hi, t.lo+(x.lo-y.lo));
}

static inline fr_pair_t fr_sub_d(fr_pair_t x, float y)
{
  // CPairSum (mod): 7 adds
  // uiCA: 13.00
  fr_pair_t t = fr_two_diff(x.hi,y);    // 6 adds
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_osub_d(fr_pair_t x, float y)
{
  // CPairSum (mod): 4 adds
  // uiCA: 4.50 
  fr_pair_t t = fr_fast_diff(x.hi,y);
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_d_sub(float x, fr_pair_t y)
{
  // CPairSum (mod): 7 adds
  // uiCA: 13.00
  fr_pair_t t = fr_two_diff(x,y.hi);    // 6 adds
  
  return fr_pair(t.hi, t.lo-y.lo);
}

static inline fr_pair_t fr_d_osub(float x, fr_pair_t y)
{
  // CPairSum (mod): 4 adds
  // uiCA: 4.50 
  fr_pair_t t = fr_fast_diff(x,y.hi);
  
  return fr_pair(t.hi, t.lo-y.lo);
}

static inline fe_pair_t fe_mul_d(fe_pair_t x, float y)
{
  // DWTimesFP3: 2 fma, 1 mul, 3 add
  // uiCA: 4.94 
  fe_pair_t c = fe_two_mul(x.hi, y);    // 1 fma, 1 mul
  float    t = fma(x.lo,y,c.lo);
  
  return fe_fast_sum(c.hi,t);           // 3 adds
}

static inline fe_pair_t fe_d_mul(float x, fe_pair_t y) { return fe_mul_d(y,x); }


// identical result to: fe_result(fe_mul_d(x,y)) but
// eliminates useless ops that compilers aren't seeing.
static inline float fe_result_mul_d(fe_pair_t x, float y)
{
  return fma(x.hi,y,x.lo*y);
}

// as above but for 'fr'. need to normalize x first.
static inline float fr_result_mul_d(fr_pair_t x, float y)
{
  x = fr_normalize(x);
  return fma(x.hi,y,x.lo*y);
}


static inline fe_pair_t fe_mul_dd(float x, float y)
{
  return fe_two_mul(x,y);               // 1 fma, 1 mul
}


static inline fe_pair_t fe_mul_da(fe_pair_t x, float y)
{
  // DWTimesFP1: 1 fma, 2 mul, 7 add
  // uiCA: 32.00
  fe_pair_t m = fe_two_mul(x.hi, y);    // 1 mul, 1 fma
  float    a = x.lo * y;
  fe_pair_t s = fe_fast_sum(m.hi,a);    // 3 adds
  float    b = s.lo + m.lo;
  
  return fe_fast_sum(s.hi,b);           // 3 adds
}

static inline fr_pair_t fr_mul_dd(float x, float y)
{
  // 2Prod
  float hi = x*y;                      // RN(xy)
  float lo = fma(x,y, -hi);            // xy-RN(xy)

  return fr_pair(hi,lo);
}

static inline fr_pair_t fr_mul_d(fr_pair_t x, float y)
{
  // 2 fma, 1 mul
  // (a,e)*y = ay+ey
  // uiCA: 4.94 
  float hi = x.hi*y;                   // RN(ay)
  float t  = fma(x.hi,y,-hi);          // RN(ay-RN(ay) 
  float lo = fma(x.lo,y, t);           // RN(ey+t);
  
  return fr_pair(hi,lo);
}


// largest discovered error in :  rel-error=3.936x10^-106
// x = {.hi=4510026974538724, .lo=4232862152422029.0*0x1.0p-53}
// y = {.hi=4511576932111935, .lo=2250098448199619.0*0x1.0p-53}

static inline fe_pair_t fe_mul(fe_pair_t x, fe_pair_t y)
{
  // DWTimeDW3: 3 fma, 2 mul, 4 add
  // uiCA: 28.00
  fe_pair_t p = fe_two_mul(x.hi,y.hi);  // 1 fma, 1 mul
  float    a = x.lo * y.lo;
  float    b = fma(x.hi,y.lo,a);
  float    c = fma(x.lo,y.hi,b);
  float    d = p.lo + c;
  return fe_fast_sum(p.hi,d);           // 3 adds
}

// TODO: fma consideration (different version though)
static inline fr_pair_t fr_mul(fr_pair_t x, fr_pair_t y)
{
  // CPairMul: 2 fma, 3 mul, 2 add
  // (1) what about Kahan's mma here as variant? or use FMA?
  // NOTE: This is the correct implementation of CPairMul.
  // uiCA: 21.60
  // ulp: ~4.85
  float h = x.hi*y.hi;                 // ab
  float s = x.hi*y.lo + y.hi*x.lo;     // af+be (1)
  float g = fma(x.hi,y.hi,-h)+s;       // ab-RN(ab)+af+be
  
  return fr_pair(h,g);
}

// squaring a pair 'x = (a+b)':
// directly using fe_mul(x,x) is 3 fma, 2 mul, 4 add and ~3.4994 ulp
//                fr_mul(x,x) is 2 fma, 2 mul, 2 add and ~3.9964 ulp
//   x²   = a²+2ab+b²
//        = a²+b(2a+b)    : if UP then RN(a+b)=a ⇒ RN(2a+b)=2a
//   x²   = a²+2ab        : if evaluated as previous
//
// this choice drops operations vs mul and tightens the error bound.

static inline fe_pair_t fe_sq(fe_pair_t x)
{
  // 2 fma, 1 mul, 6 add
  // uiCA: 33.00
  // ~2.4072 ulp
  fe_pair_t p = fe_fast_sum(x.hi,x.lo+x.lo);   // a+2b    : 3 add

  return fe_mul_d(p,x.hi);                     // a(a+2b) : 2 fma, 1 mul, 3 add
}

static inline fr_pair_t fr_sq(fr_pair_t x)
{
  // 2 fma, 1 mul, 1 add
  // uiCA: 8.00
  // ulp: ~2.9997
  fr_pair_t a = fr_two_mul(x.hi,x.hi);  //  a²  : 1 fma, 1 mul
  a.lo = fma(x.hi,x.lo+x.lo,a.lo);      // +2ab : 1 fma, 1 add

  return  a;
}

// float input is error free TwoMulFMA
static inline fe_pair_t fe_sq_d(float x) { return fe_two_mul(x,x); }
static inline fr_pair_t fr_sq_d(float x) { return fr_two_mul(x,x); }

static inline fe_pair_t fe_div_d(fe_pair_t x, float y)
{
  // DWDivFP3: 2 div, 1 fma, 1 mul, 6 add
  // uiCA: 50.00 
  float    h = x.hi/y;
  fe_pair_t e = fe_two_mul(h,y);           // 1 fma, 1 mul
  float    a = x.hi - e.hi;               // (exact operation)
  float    b = a - e.lo;                  // (exact operation)
  float    c = b + x.lo;
  float    l = c / y;

  return fe_fast_sum(h,l);                 // 3 add
}

static inline fe_pair_t fe_inv_d(float x)
{
  // 2 div, 1 fma
  // uiCA: 13.00 
  float h = 1.0/x;
  float l = -fma(x,h,-1.0)/x;

  return fe_pair(h,l);
}

static inline fe_pair_t fe_inv_dn(float x)
{
  // 1 div, 1 fma, 1 mul
  // lo is computed with one Newton step
  // uiCA: 13.00 
  float h = 1.0/x;
  float t = fma(x,h,-1.f);
  float l = -t*h;
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_inv_dh(float x)
{
  // 1 div, 2 fma, 1 mul
  // lo is computed with one Halley step
  // uiCA: 13.18
  float h = 1.0/x;
  float t = fma(x,-h,1.f);
  float l = h*fma(t,t,t);
  
  return fe_pair(h,l);
}


static inline fr_pair_t fr_inv_d(float x)   { return fe2fr(fe_inv_d(x)); }
static inline fr_pair_t fr_inv_dh(float x)  { return fe2fr(fe_inv_dh(x)); }
static inline fr_pair_t fr_inv_dn(float x)  { return fe2fr(fe_inv_dn(x)); }



// add to doc/table
static inline fe_pair_t fe_inv(fe_pair_t x)
{
  // DWDivDW2 (mod): 2 div, 2 fma, 1 mul, 8 add
  // uiCA: 68.00 
  // ulp: ~6.69
  float    h = 1.0/x.hi;
  fe_pair_t r = fe_mul_d(x,h);         // DWTimesFP3: 2 fma, 1 mul, 3 add
  float    a = 1.0 - r.hi;            // (exact operation)
  float    c = a   - r.lo;
  float    l = c / x.hi;

  return fe_fast_sum(h,l);              // 3 add
}

// 1/x using a Newton-Raphson step
// both versions appear to have approximately the same
// error bound and performace is close. tough call
// about which should be the default
//
// computation of 's' is ordered since x*RN(1/x) ≈ 1
static inline fe_pair_t fe_inv_n(fe_pair_t x)
{
  // 1 div, 4 fma, 2 mul, 13 adds
  // uiCA: 66.50
  float    h = 1.0 / x.hi;             // initial approximation
  fe_pair_t r = fe_mul_d(x,h);          // hx      (2 fma, 1 mul, 3 add)
  fe_pair_t s = fe_d_osub(2.0,r);       // 2-hx    (7 adds)  {1}

  return fe_mul_d(s,h);                 // h(2-hx) (2 fma, 1 mul 3 add)
}

static inline fr_pair_t fr_inv_n(fr_pair_t x)
{
  // 1 div, 4 fma, 2 mul, 4 adds
  // Newton-Raphson step
  // uiCA: 27.29
  // SEE: notes in `fe_inv_n`
  float    h = 1.0 / x.hi;             // initial approximation
  fr_pair_t r = fr_mul_d(x,h);          // hx      (2 fma, 1 mul)
  fr_pair_t s = fr_d_osub(2.0,r);       // 2-hx    (4 adds)

  return fr_mul_d(s,h);                 // h(2-hx) (2 fma, 1 mul)
}


// add to doc/table
static inline fe_pair_t fe_inv_a(fe_pair_t x)
{
  // DWDivDW3 (mod): 1 div, 3 fma, 2 mul, 16 adds
  // core of DWDivDW3 computes the inverse
  // uiCA: 68.00 
  float    i = 1.0 / x.hi;
  float    a = -fma(x.hi,i, -1.0);     // (exact operation) -> change to FMA
  float    b = x.lo * i;
  fe_pair_t e = fe_fast_diff(a,b);      // 3 adds
  fe_pair_t d = fe_mul_d(e, i);         // DWTimesFP3: 2 fma, 1 mul, 3 add

  return fe_add_d(d, i);                // DWPlusFP: 10 adds
}

// Newton-Raphson step for 1/sqrt(x)
static inline float fe_rsqrt_nr_(float y, float a, float r)
{
  // 3 fma, 1 mul
  float s = fma(a,r,0.5);
  float u = fma(y,y, -r);
  float v = fma(a,u,  s);

  return y*v;
}

// Halley method step for 1/sqrt(x)
static inline float fe_rsqrt_hm_(float h, float a, float r)
{
  // 4 fma, 2 mul
  float s = fma(r,a,0.5);
  float t = fma(h,h,-r);
  float v = fma(a,t, s);
  float w = fma(1.5*v,v,v);

  return h*w;
}

static inline fe_pair_t fe_rsqrt_d(float x)
{
  float r = 1.f/x;
  float a = -0.5*x;
  float h = sqrt(r);
  float l = fe_rsqrt_nr_(h,a,r);       // 3 fma, 1 mul
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_rsqrt_dh(float x)
{
  float r = 1.f/x;
  float t = -0.5*x;
  float h = sqrt(r);
  float l = fe_rsqrt_hm_(h,t,r);       // 4 fma, 2 mul
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_sqrt(fe_pair_t x)
{
  float h = sqrt(x.hi);
  float d = x.hi != 0 ? h+h : 1.0;
  float t = -fma(h,h,-x.hi);
  float l = (t+x.lo)/d;

  return fe_pair(h,l);
}


static inline fe_pair_t fe_sqrt_d(float x)
{
  float h = sqrt(x);
  float d = x != 0 ? h+h : 1.0;
  float t = -fma(h,h,-x);
  float l = t/d;

  return fe_pair(h,l);
}



