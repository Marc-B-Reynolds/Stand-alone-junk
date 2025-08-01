// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

/// This header implements two base methods of extended precision:
///
/// * fe_{op}: standard *"floating point expansion"* of two elements (aka *double-doubles* or *unevaluated pair*)
/// * fr_{op}: relaxed version of *double-doubles* by Lange & Rump
/// <br>
/// Data structures for representing the order pairs `(hi,lo)`.
/// * `fe_pair_t`
/// * `fr_pair_t`
/// <br>
///
/// * should note when identical to UP & see how wrappered
///   forwarding works out WRT producing same code.

// TODO:
// * add squaring?
// * added ordered variants

/// add notes about how compiler options are modified by including this file and why
/// * disables floating-point contractions
/// * disables all *fast-math* options
/// * disables all *math errno* in GCC (you almost certainly never want that enabled)

// NOTES: GCC (14.2) & clang (19.1.0)
// ATM: GCC & clang successfully transform:
// {fe,fr}_result(op) - where result pair of 'op'
//   fast_sum,fast_diff
// uiCA comments are skylake throughput estimates thereof
// `~ulp:` are sloppy measures not worth much (from running test/fp64_pair_test)

#pragma once

// duct-tape and super-glue. the good answer is properly set compiler options instead
// of attempting to modifiy them in source. it'd be nice if compilers provided the
// features needed to go this route but...
#if defined(__clang__)
#pragma STDC FP_CONTRACT OFF
#pragma clang fp reassociate(off)
//#pragma clang fp fast(off)     // whimper
#elif defined(__GNUC__)
#pragma GCC optimize ("fp-contract=off")
#pragma GCC optimize ("no-fast-math")
#pragma GCC optimize ("no-math-errno")
#else
#warning "this is sad"
#endif


#define F64_PAIR_CPAIR_DIV_FMA     // not certain this is legal

/*

[^1]: *Formalization of double-word arithmetic, and comments on [^2]*, Muller & Rideau, 2021 [link](https://hal.science/hal-02972245)
[^2]: *Tight and rigorous error bounds for basic building blocks of double-word arithmetic*, Joldes, Muller, Popescu, 2017 [link](https://hal.science/hal-01351529)
[^3]: *Faithfully rounded floating-point operations*, M. Lange and S.M. Rump, 2019 [link](https://www.tuhh.de/ti3/paper/rump/LaRu2017b.pdf)
[^4]: *Error bounds for computer arithmetics*, S.M. Rump, 2019, [PDF](https://www.tuhh.de/ti3/paper/rump/Ru19.pdf)
[^5]: *On the robustness of double-word addition algorithms*, 2024, [link](https://arxiv.org/abs/2404.05948)
[^6]: *FastTwoSum revisited*, Jeannerod & Zimmermann, 2025, [link](https://inria.hal.science/hal-04875749)

 */


typedef struct { double hi,lo; } fr_pair_t;
typedef struct { double hi,lo; } fe_pair_t;

// generally not a great idea IMHO but they are ordered pairs
static inline fe_pair_t fe_pair(double hi, double lo)
{
  return (fe_pair_t){.hi=hi, .lo=lo};
}

static inline fr_pair_t fr_pair(double hi, double lo)
{
  return (fr_pair_t){.hi=hi, .lo=lo};
}

static inline fe_pair_t fe_set_d(double x) { return fe_pair(x,0.0); }
static inline fr_pair_t fr_set_d(double x) { return fr_pair(x,0.0); }


// hack type specialization to lower copy-pasta & eye-ball identical functionality
// 1) type pun: the type part
static inline fr_pair_t fe2fr(fe_pair_t x) { return fr_pair(x.hi,x.lo); }
static inline fe_pair_t fr2fe(fr_pair_t x) { return fe_pair(x.hi,x.lo); }

// 2) unary op forward: 
static inline fe_pair_t fe2fr_uo_wrap(fr_pair_t (*op)(fr_pair_t), fe_pair_t x) { return fr2fe(op(fe2fr(x))); }
static inline fr_pair_t fr2fe_uo_wrap(fe_pair_t (*op)(fe_pair_t), fr_pair_t x) { return fe2fr(op(fr2fe(x))); }

// 3) binary op forward:
static inline fe_pair_t fe2fr_bo_wrap(fr_pair_t (*op)(fr_pair_t, fr_pair_t), fe_pair_t x, fe_pair_t y)
{
  return fr2fe(op(fe2fr(x), fe2fr(y)));
}

static inline fr_pair_t fr2fe_bo_wrap(fe_pair_t (*op)(fe_pair_t, fe_pair_t), fr_pair_t x, fr_pair_t y)
{
  return fe2fr(op(fr2fe(x), fr2fe(y)));
}

static inline double fe_result(fe_pair_t x) { return x.hi;      }
static inline double fr_result(fr_pair_t x) { return x.hi+x.lo; }


/// returns: $-x$
static inline fe_pair_t fe_neg(fe_pair_t x) { return fe_pair(-x.hi, -x.lo); }
static inline fr_pair_t fr_neg(fr_pair_t x) { return fr_pair(-x.hi, -x.lo); }

/// returns: $|x|$
static inline fe_pair_t fe_abs(fe_pair_t x)
{
  // doesn't flush lo = -0 to +0. don't think that
  // has any value in general
  return fe_pair(fabs(x.hi), x.hi >= 0.0 ? x.lo : -x.lo);
}

static inline fr_pair_t fr_abs(fr_pair_t x) { return fr2fe_uo_wrap(fe_abs,x); }

// $ (hi,lo) = x+y $
// rename variable to x,y
static inline fe_pair_t fe_two_sum(double a, double b)
{
  // 2Sum: 6 adds
  double x = a + b;
  double t = x - a;
  double y = (a - (x - t)) + (b - t);

  return fe_pair(x,y);
}


// $ (hi,lo) = x-y $
// rename variable to x,y
static inline fe_pair_t fe_two_diff(double a, double b)
{
  // 2Sum: 6 adds
  double x = a - b;
  double t = a - x;
  double y = (a - (x + t)) + (t - b);

  return fe_pair(x,y);
}


static inline fr_pair_t fr_two_sum(double x, double y)  { return fe2fr(fe_two_sum(x,y));  }
static inline fr_pair_t fr_two_diff(double x, double y) { return fe2fr(fe_two_diff(x,y)); }


// return x*y where 'y' is a power-of-two value
static inline fe_pair_t fe_mul_pot(fe_pair_t x, double y)
{
  return fe_pair(x.hi*y,x.lo*y);
}

static inline fr_pair_t fr_mul_pot(fr_pair_t x, double y)  { return fe2fr(fe_mul_pot(fr2fe(x),y));  }


/// returns: $(hi,lo) = x+y$. requires |x| >= |y| (or x = 0)
///
/// * in absence of underflow and overflow the result is exact
/// * constraint violation: (add notes and refrence)
static inline fe_pair_t fe_fast_sum(double x, double y)
{
  // Fast2Sum: 3 adds
  double h = x+y;
  return fe_pair(h, y-(h-x));
}

static inline fr_pair_t fr_fast_sum(double x, double y) { return fe2fr(fe_fast_sum(x,y)); }

/// returns: $(a,b) = x-y$. requires |x| >= |y| (or x = 0)
static inline fe_pair_t fe_fast_diff(double x, double y)
{
  // Fast2Sum: 3 adds
  double h = x-y;
  return fe_pair(h, (x-h)-y);
}

static inline fr_pair_t fr_fast_diff(double x, double y) { return fe2fr(fe_fast_diff(x,y)); }

static inline fe_pair_t fe_two_mul(double x, double y)
{
  // 2Prod
  double hi = x*y;

  return fe_pair(hi, fma(x,y,-hi));
}

static inline fr_pair_t fr_two_mul(double x, double y)
{
  // 2Prod
  double hi = x*y;

  return fr_pair(hi, fma(x,y,-hi));
}

/// returns pair in unevaluated pair form
static inline fr_pair_t fr_normalize(fr_pair_t x)
{
  return fr_fast_sum(x.hi,x.lo);
}

static inline fr_pair_t fr_add_dd(double x, double y)  { return fr_two_sum(x,y);  }
static inline fe_pair_t fe_addo_dd(double x, double y)  { return fe_two_sum(x,y);  }
static inline fr_pair_t fr_oadd_dd(double x, double y) { return fr_fast_sum(x,y); }
static inline fe_pair_t fe_oadd_dd(double x, double y) { return fe_fast_sum(x,y); }


static inline fe_pair_t fe_add_d(fe_pair_t x, double y)
{
  // DWPlusFP: 10 adds
  fe_pair_t t = fe_two_sum(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_add(double x, fe_pair_t y) { return fe_add_d(y,x); }


static inline fe_pair_t fe_oadd_d(fe_pair_t x, double y)
{
  // DWPlusFP (mod): 7 adds
  fe_pair_t t = fe_fast_sum(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_oadd(double x, fe_pair_t y) { return fe_oadd_d(y,x); } 


static inline fe_pair_t fe_sub_dd(double x, double y) { return fe_two_diff(x,y); }

static inline fe_pair_t fe_sub_d(fe_pair_t x, double y)
{
  // DWPlusFP: 10 adds
  fe_pair_t t = fe_two_diff(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_osub_d(fe_pair_t x, double y)
{
  // DWPlusFP (mod): 7 adds
  fe_pair_t t = fe_fast_diff(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_sub(double x, fe_pair_t y)
{
  // DWPlusFP: 10 adds
  fe_pair_t t = fe_two_diff(x, y.hi);
  double    l = t.lo - y.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_osub(double x, fe_pair_t y)
{
  // DWPlusFP (mod): 7 adds
  fe_pair_t t = fe_fast_diff(x, y.hi);
  double    l = t.lo - y.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_add(fe_pair_t x, fe_pair_t y)
{
  // AccurateDWPlusDW: 20 adds
  // uiCA: 49.00
  fe_pair_t s = fe_two_sum(x.hi,y.hi);  // 6 adds
  fe_pair_t t = fe_two_sum(x.lo,y.lo);  // 6 adds
  double    c = s.lo + t.hi;
  fe_pair_t v = fe_fast_sum(s.hi, c);   // 3 adds
  double    w = t.lo + v.lo;

  return fe_fast_sum(v.hi,w);           // 3 adds
}

static inline fe_pair_t fe_oadd(fe_pair_t x, fe_pair_t y)
{
  // AccurateDWPlusDW (mod): 17 adds
  // uiCA: 41.50
  fe_pair_t s = fe_fast_sum(x.hi,y.hi); // 3 adds
  fe_pair_t t = fe_two_sum(x.lo,y.lo);  // 6 adds
  double    c = s.lo + t.hi;
  fe_pair_t v = fe_fast_sum(s.hi, c);   // 3 adds
  double    w = t.lo + v.lo;

  return fe_fast_sum(v.hi,w);           // 3 adds
}

static inline fe_pair_t fe_add_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW: 11 adds
  fe_pair_t s = fe_two_sum(x.hi,y.hi);
  double    v = x.lo + y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
}

static inline fe_pair_t fe_oadd_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW (mod): 8 adds
  fe_pair_t s = fe_fast_sum(x.hi,y.hi);
  double    v = x.lo + y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
}

static inline fe_pair_t fe_sub_s(fe_pair_t x, fe_pair_t y)
{
#if 1
  return fe_add_s(x,fe_neg(y));
#else  
  // SloppyDWPlusDW: 11 adds
  fe_pair_t s = fe_two_diff(x.hi,y.hi);
  double    v = x.lo - y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
#endif  
}

static inline fe_pair_t fe_osub_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW (mod): 8 adds
  fe_pair_t s = fe_fast_diff(x.hi,y.hi);
  double    v = x.lo - y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
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

static inline fr_pair_t fr_add_d(fr_pair_t a, double b)
{
  // CPairSum (mod): 7 adds
  fr_pair_t t = fr_two_sum(a.hi, b);
  return fr_pair(t.hi, t.lo+a.lo);
}

static inline fr_pair_t fr_oadd_d(fr_pair_t a, double b)
{
  // CPairSum (mod): 4 adds
  fr_pair_t t = fr_fast_sum(a.hi, b);
  return fr_pair(t.hi, t.lo+a.lo);
}

/// returns: $x-y$
static inline fe_pair_t fe_sub(fe_pair_t x, fe_pair_t y)
{
  // AccurateDWPlusDW: 20 adds
  // uiCA: 49.00
  fe_pair_t s = fe_two_diff(x.hi,y.hi); // 6 adds
  fe_pair_t t = fe_two_diff(x.lo,y.lo); // 6 adds
  double    c = s.lo + t.hi;
  fe_pair_t v = fe_fast_sum(s.hi, c);   // 3 adds
  double    w = t.lo + v.lo;

  return fe_fast_sum(v.hi,w);           // 3 adds
}


/// returns: $x-y$ when |x| >= |y|
static inline fe_pair_t fe_osub(fe_pair_t x, fe_pair_t y)
{
  // 14 adds
  // uiCA: 40.80 
  fe_pair_t s = fe_fast_diff(x.hi,y.hi); // 3 adds
  fe_pair_t t = fe_fast_diff(x.lo,y.lo); // 3 adds
  double    c = s.lo + t.hi;
  fe_pair_t v = fe_fast_sum(s.hi, c);    // 3 adds
  double    w = t.lo + v.lo;

  return fe_fast_sum(v.hi,w);            // 3 adds
}

static inline fr_pair_t fr_sub(fr_pair_t x, fr_pair_t y)
{
  // CPairSum: 8 adds
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

static inline fr_pair_t fr_sub_d(fr_pair_t x, double y)
{
  // CPairSum (mod): 7 adds
  fr_pair_t t = fr_two_diff(x.hi,y);    // 6 adds
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_osub_d(fr_pair_t x, double y)
{
  // CPairSum (mod): 4 adds
  fr_pair_t t = fr_fast_diff(x.hi,y);
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_d_sub(double x, fr_pair_t y)
{
  // CPairSum (mod): 7 adds
  fr_pair_t t = fr_two_diff(x,y.hi);    // 6 adds
  
  return fr_pair(t.hi, t.lo-y.lo);
}

static inline fr_pair_t fr_d_osub(double x, fr_pair_t y)
{
  // CPairSum (mod): 4 adds
  fr_pair_t t = fr_fast_diff(x,y.hi);
  
  return fr_pair(t.hi, t.lo-y.lo);
}


static inline fe_pair_t fe_mul_d(fe_pair_t x, double y)
{
  // DWTimesFP3: 2 fma, 1 mul, 3 add
  fe_pair_t c = fe_two_mul(x.hi, y);    // 1 fma, 1 mul
  double    t = fma(x.lo,y,c.lo);
  
  return fe_fast_sum(c.hi,t);           // 3 adds
}

static inline fe_pair_t fe_d_mul(double x, fe_pair_t y) { return fe_mul_d(y,x); }


static inline fe_pair_t fe_mul_dd(double x, double y)
{
  return fe_two_mul(x,y);               // 1 fma, 1 mul
}

static inline fe_pair_t fe_sq_d(double x) { return fe_two_mul(x,x); }

static inline fe_pair_t fe_mul_da(fe_pair_t x, double y)
{
  // DWTimesFP1: 1 fma, 2 mul, 7 add
  fe_pair_t m = fe_two_mul(x.hi, y);    // 1 mul, 1 fma
  double    a = x.lo * y;
  fe_pair_t s = fe_fast_sum(m.hi,a);    // 3 adds
  double    b = s.lo + m.lo;
  
  return fe_fast_sum(s.hi,b);           // 3 adds
}


static inline fr_pair_t fr_mul_dd(double x, double y)
{
  // 2Prod
  double hi = x*y;                      // RN(xy)
  double lo = fma(x,y, -hi);            // xy-RN(xy)

  return fr_pair(hi,lo);
}

static inline fr_pair_t fr_mul_d(fr_pair_t x, double y)
{
  // (a,e)*y = ay+ey
  double hi = x.hi*y;                   // RN(ay)
  double t  = fma(x.hi,y,-hi);          // RN(ay-RN(ay) 
  double lo = fma(x.lo,y, t);           // RN(ey+t);
  
  return fr_pair(hi,lo);
}


// pair products:
// (xh,xl)(yh,yl) = xh yh + xl yh + xh yl + xl yl


// largest discovered error in :  rel-error=3.936x10^-106
// x = {.hi=4510026974538724, .lo=4232862152422029.0*0x1.0p-53}
// y = {.hi=4511576932111935, .lo=2250098448199619.0*0x1.0p-53}

static inline fe_pair_t fe_mul(fe_pair_t x, fe_pair_t y)
{
  // DWTimeDW3: 3 fma, 2 mul, 4 add
  // uiCA: 28.00
  fe_pair_t p = fe_two_mul(x.hi,y.hi);  // 1 fma, 1 mul
  double    a = x.lo * y.lo;
  double    b = fma(x.hi,y.lo,a);
  double    c = fma(x.lo,y.hi,b);
  double    d = p.lo + c;
  return fe_fast_sum(p.hi,d);           // 3 adds
}

// homegrown square
static inline fe_pair_t fe_sq(fe_pair_t x)
{
  // uiCA: 20.00
  // ulp:  ~2.998
  fe_pair_t a = fe_two_mul(x.hi,x.hi);     // 1 fma, 1 mul

  a.lo = fma(x.hi,x.lo+x.lo,a.lo);         // 1 fma, 1 add

  // TODO:
  // fix the invariant: (x.hi+x.lo == x.hi)
  // could probably rework and maybe reduce error
  double t = a.hi + a.lo;
  a.lo += a.hi-t;
  a.hi  = t;

  return  a;
}

static inline fr_pair_t fr_sq(fr_pair_t x)
{
  // uiCA: 8.00
  // ulp: ~2.998
  fr_pair_t a = fr_two_mul(x.hi,x.hi);     // 1 fma, 1 mul

  a.lo = fma(x.hi,x.lo+x.lo,a.lo);         // 1 fma, 1 add

  return  a;
}


#if 0
// higher op count and error. (which is why it's commented out)
static inline fe_pair_t fe_mul_a(fe_pair_t x, fe_pair_t y)
{
  // DWTimeDW2: 2 fma, 2 mul, 7 add
  fe_pair_t p = fe_two_mul(x.hi,y.hi);  // 1 fma, 1 mul
  double    a = x.hi * y.lo;
  double    m = fma(x.lo,y.hi,a);
  double    b = p.lo + m;

  return fe_two_sum(p.hi,b);            // 6 adds
}
#endif

// GCC is producing 2 FMAs (w fast contract)..examine
static inline fr_pair_t fr_mul(fr_pair_t x, fr_pair_t y)
{
  // CPairMul:
  // (1) what about Kahn's mma here as variant? or use FMA?
  // uiCA: 21.60
  // ulp: ~4.85
  double h = x.hi*y.hi;              // ab
  double s = x.hi*y.lo + y.hi*x.lo;  // af+be (1)
  double g = fma(x.hi,y.hi,-h)+s;    // ab-RN(ab)+af+be
  
  return fr_pair(h,g);
}

static inline fe_pair_t fe_div_d(fe_pair_t x, double y)
{
  // DWDivFP3: 2 div, 1 fma, 1 mul, 6 add
  double    h = x.hi/y;
  fe_pair_t e = fe_two_mul(h,y);        // 1 fma, 1 mul
  double    a = x.hi - e.hi;            // (exact operation)
  double    b = a - e.lo;               // (exact operation)
  double    c = b + x.lo;
  double    l = c / y;

  return fe_fast_sum(h,l);              // 3 add
}

static inline fe_pair_t fe_inv_d(double x)
{
  // uiCA: 13.00 
  double h = 1.0/x;
  double l = -fma(x,h,-1.0)/x;

  return fe_pair(h,l);
}

static inline fe_pair_t fe_inv_dn(double x)
{
  // 1 div, 1 fma, 1 mul
  // lo is computed with one Newton step
  // uiCA: 13.00 
  double h = 1.0/x;
  double t = fma(x,h,-1.f);
  double l = -t*h;
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_inv_dh(double x)
{
  // 1 div, 2 fma, 1 mul
  // lo is computed with one Halley step
  // uiCA: 13.18
  double h = 1.0/x;
  double t = fma(x,-h,1.f);
  double l = h*fma(t,t,t);
  
  return fe_pair(h,l);
}


// add to doc/table
static inline fe_pair_t fe_inv(fe_pair_t x)
{
  // DWDivDW2 (mod): 2 div, 2 fma, 1 mul, 8 add
  // ulp: ~6.69
  double    h = 1.0/x.hi;
  fe_pair_t r = fe_mul_d(x,h);         // DWTimesFP3: 2 fma, 1 mul, 3 add
  double    a = 1.0 - r.hi;            // (exact operation)
  double    c = a   - r.lo;
  double    l = c / x.hi;

  return fe_fast_sum(h,l);              // 3 add
}

// add to doc/table
static inline fe_pair_t fe_inv_a(fe_pair_t x)
{
  // DWDivDW3 (mod): 1 div, 3 fma, 2 mul, 16 adds
  // core of DWDivDW3 computes the inverse
  double    i = 1.0 / x.hi;
  double    a = -fma(x.hi,i, -1.0);     // (exact operation) -> change to FMA
  double    b = x.lo * i;
  fe_pair_t e = fe_fast_diff(a,b);      // 3 adds
  fe_pair_t d = fe_mul_d(e, i);         // DWTimesFP3: 2 fma, 1 mul, 3 add

  return fe_add_d(d, i);                // DWPlusFP: 10 adds
}

static inline fe_pair_t fe_div(fe_pair_t x, fe_pair_t y)
{
  // DWDivDW2: 2 div, 2 fma, 1 mul, 9 adds

  // 'r' computation is noted to be legal without changing
  // error bound in "Tight and Rigorous". (vs. DWTimesFP1)
  double    h = x.hi/y.hi;
  fe_pair_t r = fe_mul_d(y,h);          // DWTimesFP3: 2 fma, 1 mul, 3 add
  double    a = x.hi - r.hi;            // (exact operation)
  double    b = x.lo - r.lo;
  double    c = a + b;
  double    l = c / y.hi;

  return fe_fast_sum(h,l);              // 3 add
}

static inline fe_pair_t fe_d_div(double x, fe_pair_t y) { return fe_div(fe_set_d(x),y); } 

// local hack: recheck
static inline fe_pair_t fe_div_dd(double x, double y)
{
  double    h = x/y;
  fe_pair_t r = fe_mul_dd(y,h);
  double    a = x - r.hi; 
  double    b =   - r.lo;
  double    c = a + b;
  double    l = c / y;

  return fe_fast_sum(h,l);
}


// todo: think about double return straight after
static inline fe_pair_t fe_div_a(fe_pair_t x, fe_pair_t y)
{
  // DWDivDW3: 1 div, 6 fma, 4 mul, 20 adds
  // rewrite to fe_inv_a which is all the first part?
  double    i = 1.0 / y.hi;
  double    a = -fma(y.hi,i, -1.0);     // (exact)
  double    b = y.lo * i;
  fe_pair_t e = fe_fast_diff(a,b);      // 3 adds
  fe_pair_t d = fe_mul_d(e, i);         // DWTimesFP3: 2 fma, 1 mul, 3 add
  fe_pair_t m = fe_add_d(d, i);         // DWPlusFP: 10 adds

  return fe_mul(x, m);                  // DWTimeDW3: 3 fma, 2 mul, 4 add
}


static inline fr_pair_t fr_div(fr_pair_t x, fr_pair_t y)
{
  // CPairDiv:
#if !defined(F64_PAIR_CPAIR_DIV_FMA)
  // uiCA: 21.60
  double c = x.hi/y.hi;
  double t = fma(y.hi,c,-x.hi);      // (exact)
  double p = x.lo-t;
  double q = c*y.lo;                 // FMA?
  double r = p-q;
  double s = y.hi+y.lo;              // s = y.hi if UP
  double g = r/s;
#else  
  // uiCA: 21.10
  double h = x.hi/y.hi;
  double t = fma(y.hi,h,-x.hi);
  double n = -fma(h,y.lo,t-x.lo);
  double l = n/(y.hi+y.lo);
  return fr_pair(h,l);
#endif  
}

static inline fr_pair_t fr_d_div(double x, fr_pair_t y)
{
  // 2 div, 1 fma, 1 mul, 2 add
  double h = x/y.hi;
  double t = -fma(y.hi,h,-x);
  double r = t-h*y.lo;               // what about FMA here?
  double s = y.hi+y.lo;              // s = y.hi if UP
  double l = r/s;

  return fr_pair(h,l);
}


/// returns $\frac{x}{y}$ as a `fr_pair_t`
static inline fr_pair_t fr_div_dd(double x, double y)
{
  double h = x/y;
  double l = -fma(y,h,-x)/y;

  return fr_pair(h,l);
}


/// returns $\frac{1}{x}$ as a `fr_pair_t`
// todo: think about double return straight afters
static inline fr_pair_t fr_inv(fr_pair_t x)
{
  double c = 1.0/x.hi;
  double t = -fma(x.hi,c,-1.0);
  double q = c*x.lo;                 // FMA? (with next line)
  double r = t-q;
  double s = x.hi+x.lo;              // s = x.hi if UP
  double g = r/s;

  return fr_pair(c,g);
}

// 1) "Accelerating Correctly Rounded Floating-Point Division when the Divisor Is Known in Advance"

static inline fr_pair_t fr_inv_d(double x)
{
  double h = 1.0/x;
  double l = -fma(x,h,-1.0)/x;

  return fr_pair(h,l);
}


// Newton-Raphson step for 1/sqrt(x)
static inline double fe_rsqrt_nr(double y, double h, double r)
{
  double s = fma(h,r,0.5);
  double u = fma(y,y, -r);
  double v = fma(h,u,  s);

  return y*v;
}

// Halley method step for 1/sqrt(x)
static inline double fe_rsqrt_hm(double y, double a, double r)
{
  double s = fma(r,a,0.5);
  double t = fma(y,y,-r);
  double v = fma(a,t, s);
  double w = fma(1.5*v,v,v);

  return y*w;
}

static inline fe_pair_t fe_rsqrt_d(double x)
{
  double r = 1.f/x;
  double a = -0.5*x;
  double h = sqrt(r);
  double l = fe_rsqrt_nr(h,a,r);
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_rsqrt_dh(double x)
{
  double r = 1.f/x;
  double t = -0.5*x;
  double h = sqrt(r);
  double l = fe_rsqrt_hm(h,t,r);
  
  return fe_pair(h,l);
}

static inline fr_pair_t fr_sqrt(fr_pair_t x)
{
  // CPairSqrt: 1 sqrt, 1 div, 1 fma, 2 add  
  // uiCA: 17.00 
  // ~3.03 ulp
  double h = sqrt(x.hi);
  double d = x.hi != 0 ? h+h : 1.0;
  double t = -fma(h,h,-x.hi);
  double l = (t+x.lo)/d;

  return fr_pair(h,l);
}

#if 0
// "High Precision Division and Square Root", Karp & Markstein, 1997
// [link](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=e011110a5c81454b8dd7bf46f6b636ed2b2c6774)
static inline fe_pair_t fe_sqrt(fe_pair_t a)
{
  // ~6.97 ulp
  // uiCA: 73.50 
  double    x   = 1.0 / sqrt(a.hi);
  double    ax  = a.hi * x;
  fe_pair_t ax2 = fe_sq_d(ax);

  return fe_fast_sum(ax, fe_sub(a,ax2).hi * (x * 0.5));
}
#else
static inline fe_pair_t fe_sqrt(fe_pair_t x)
{
  // CPairSqrt: 1 sqrt, 1 div, 1 fma, 2 add
  // uiCA: 17.00 
  // ~3.09 ulp
  double h = sqrt(x.hi);
  double d = x.hi != 0 ? h+h : 1.0;
  double t = -fma(h,h,-x.hi);
  double l = (t+x.lo)/d;

  return fe_pair(h,l);
}
#endif


static inline fr_pair_t fr_sqrt_d(double x)
{
  // CPairSqrt (mod): 1 sqrt, 1 div, 1 fma, 1 add
  double h = sqrt(x);
  double d = x != 0 ? h+h : 1.0;
  double t = -fma(h,h,-x);
  double l = t/d;

  return fr_pair(h,l);
}

/// ## Extended precision constants
///
/// | name        | value |
/// | :---        |:---:  |
/// |fe_k_e       | $e$   |
/// |fe_k_e_i     | $\frac{1}{e}$ |
/// |fe_k_log2    | $\text{log}\left(2\right)$ |
/// |fe_k_log2_i  | $\frac{1}{\text{log}\left(2\right)}$ |
/// |fe_k_log10   | $\text{log}\left(10\right)$ |
/// |fe_k_log10_i | $\frac{1}{\text{log}\left(10\right)}$ |
/// |fe_k_pi      | $\pi$ |
/// |fe_k_pi_i    | $\frac{1}{\pi}$ |
/// |fe_k_sqrt_2  | $\sqrt{2}$ |
/// |fe_k_sqrt_2_i| $\frac{1}{\sqrt{2}}$ |
/// <br>
/// SEE: https://marc-b-reynolds.github.io/math/2020/01/09/ConstAddMul.html (and references)

// meh. rethink conversion things?
static inline fe_pair_t fr_load(fe_pair_t x) { return fe_pair(x.hi,x.lo); }

// extended precision multiplicative constants as unevaluate pairs: {RN(K) + RN(K-RN(K))}
static const fe_pair_t fe_k_pi       = {.hi = 0x1.921fb54442d18p1,  .lo= 0x1.1a62633145c07p-53};
static const fe_pair_t fe_k_pi_i     = {.hi = 0x1.45f306dc9c883p-2, .lo=-0x1.6b01ec5417056p-56};
static const fe_pair_t fe_k_log2     = {.hi = 0x1.62e42fefa39efp-1, .lo= 0x1.abc9e3b39803fp-56};
static const fe_pair_t fe_k_log2_i   = {.hi = 0x1.71547652b82fep0,  .lo= 0x1.777d0ffda0d24p-56};
static const fe_pair_t fe_k_log10    = {.hi = 0x1.26bb1bbb55516p1,  .lo=-0x1.f48ad494ea3e9p-53};
static const fe_pair_t fe_k_log10_i  = {.hi = 0x1.bcb7b1526e50ep-2, .lo= 0x1.95355baaafad3p-57};
static const fe_pair_t fe_k_e        = {.hi = 0x1.5bf0a8b145769p1,  .lo= 0x1.4d57ee2b1013ap-53};
static const fe_pair_t fe_k_e_i      = {.hi = 0x1.78b56362cef38p-2, .lo=-0x1.ca8a4270fadf5p-57};
static const fe_pair_t fe_k_sqrt_2   = {.hi = 0x1.6a09e667f3bcdp0,  .lo=-0x1.bdd3413b26456p-54};
static const fe_pair_t fe_k_sqrt_2_i = {.hi = 0x1.6a09e667f3bcdp-1, .lo=-0x1.bdd3413b26456p-55};



static inline fe_pair_t fe_one(void)       { return fe_set_d( 1.0);  }
static inline fr_pair_t fr_one(void)       { return fr_set_d( 1.0);  }
static inline fe_pair_t fe_zero(void)      { return fe_set_d( 0.0);  }
static inline fr_pair_t fr_zero(void)      { return fr_set_d( 0.0);  }
static inline fe_pair_t fe_neg_zero(void)  { return fe_set_d(-0.0);  }
static inline fr_pair_t fr_neg_zero(void)  { return fr_set_d(-0.0);  }

static inline bool fe_eq_zero(fe_pair_t x) { return x.hi      == 0.0; }
static inline bool fr_eq_zero(fr_pair_t x) { return x.hi+x.lo == 0.0; }
static inline bool fe_gt_zero(fe_pair_t x) { return x.hi      >  0.0; }
static inline bool fr_gt_zero(fr_pair_t x) { return x.hi+x.lo >  0.0; }


static inline bool fe_eq(fe_pair_t x, fe_pair_t y)
{
  return (x.hi == y.hi) && (x.lo == y.lo);
}


#if 0

static inline f64_pair_t f64_pair_recip_f(double x)
{
  const double rh = 1.f/x;
  const double rl = -fmaf(rh, x, -1.f)/x;

  return (f64_pair_t){.h = rh, .l = rl};
}

#endif


// generics versions (dd = double-double)

#define dd_def_bop(x,y,OP) _Generic((x),   \
    double: _Generic((y),                  \
        fe_pair_t: fe_d_##OP,              \
        double:    fe_##OP##_dd            \
    ),                                     \
    fe_pair_t: _Generic((y),               \
        fe_pair_t: fe_##OP,                \
        double:    fe_##OP##_d             \
    )                                      \
)(x,y)

#define dd_add(x,y)  dd_def_bop(x,y,add)
#define dd_sub(x,y)  dd_def_bop(x,y,sub)   
#define dd_mul(x,y)  dd_def_bop(x,y,mul)
#define dd_div(x,y)  dd_def_bop(x,y,div)

// ordered binary ops: |x| >= |y|
#define dd_oadd(x,y) dd_def_bop(x,y,oadd)
#define dd_osub(x,y) dd_def_bop(x,y,osub)   


