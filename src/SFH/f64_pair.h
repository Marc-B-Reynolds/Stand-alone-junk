// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

/// This header implements two base methods of extended precision:
///
/// * fe_{op}: standard *"floating point expansion"* of two elements (aka *double-doubles* or *unevaluated pair*)
/// * fr_{op}: relaxed version of *double-doubles* by Lange & Rump (CPair)
/// <br>
/// Data structures for representing the order pairs `(hi,lo)`.
/// * `fe_pair_t`
/// * `fr_pair_t`
/// <br>
///
/// * should note when identical to UP & see how wrappered
///   forwarding works out WRT producing same code.

/// add notes about how compiler options are modified by including this file and why:
/// * disables all *fast-math* options
/// * disables floating-point contractions (break some of the methods)
/// * disables *math errno* (you almost certainly never want that enabled)
/// * disables *trapping math*

// NOTES: GCC (14.2) & clang (19.1.0)
// ATM: GCC & clang successfully transform:
// {fe,fr}_result(op) - where result pair of 'op'
//   fast_sum,fast_diff
// uiCA comments are skylake throughput estimates thereof (probably a bunch of copy-paste mistakes
// and out-of-date)
// `~ulp:` are sloppy measures not worth much (from running test/fp64_pair_test)
// AVX-512 has a fair number of ops that are useful (none used here)

#pragma once

#include <stdint.h>
#include <string.h>
#include <math.h>

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
#pragma GCC optimize ("no-trapping-math")
#else
#warning "this is sad"
#endif

#if defined(__GNUC__)
  #define fe_noinline    __attribute__((noinline))
  #define fe_likely(x)   __builtin_expect(!!(x), 1)
  #define fe_unlikely(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
  #define fe_noinline    __declspec(noinline)
  #define fe_likely(x)   (x)
  #define fe_unlikely(x) (x)
#else
  #define fe_noinline
  #define fe_likely(x)
  #define fe_unlikely(x)
#endif

/*

[^1]: *Formalization of double-word arithmetic, and comments on [^2]*, Muller & Rideau, 2021 [link](https://hal.science/hal-02972245)
[^2]: *Tight and rigorous error bounds for basic building blocks of double-word arithmetic*, Joldes, Muller, Popescu, 2017 [link](https://hal.science/hal-01351529)
[^3]: *Faithfully rounded floating-point operations*, M. Lange and S.M. Rump, 2019 [link](https://www.tuhh.de/ti3/paper/rump/LaRu2017b.pdf)
[^4]: *Error bounds for computer arithmetics*, S.M. Rump, 2019, [PDF](https://www.tuhh.de/ti3/paper/rump/Ru19.pdf)
[^5]: *On the robustness of double-word addition algorithms*, 2024, [link](https://arxiv.org/abs/2404.05948)
[^6]: *FastTwoSum revisited*, Jeannerod & Zimmermann, 2025, [link](https://inria.hal.science/hal-04875749)
[^7]: *Exact and Approximated error of the FMA*, Boldo & Muller, 2011, [link](https://inria.hal.science/inria-00429617)
[^8]: *Emulation of the FMA and the correctly-rounded sum of three numbers in rounding-to-nearest floating-point arithmetic*, Graillat & Muller, 2025
       [link](https://inria.hal.science/hal-04575249v2)
[^9]: *Emulation of 3Sum, 4Sum, the FMA and the FD2 instructions in rounded-to-nearest floating-point arithmetic*, Graillat & Muller, 2024 [link](https://hal.science/hal-04624238)
 */

static inline uint64_t fe_to_bits(double x)     { uint64_t u; memcpy(&u, &x, 8); return u; }
static inline double   fe_from_bits(uint64_t x) { double   f; memcpy(&f, &x, 8); return f; }

typedef struct { double hi,lo; } fr_pair_t;
typedef struct { double hi,lo; } fe_pair_t;

typedef struct { double h,m,l; } fe_triple_t;

// generally not a great idea IMHO but they are ordered pairs
static inline fe_pair_t fe_pair(double hi, double lo)
{
  return (fe_pair_t){.hi=hi, .lo=lo};
}

static inline fr_pair_t fr_pair(double hi, double lo)
{
  return (fr_pair_t){.hi=hi, .lo=lo};
}

static inline fe_triple_t fe_triple(double h, double m, double l)
{
  return (fe_triple_t){.h=h, .m=m, .l=l};
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

// slow-path functions (non inlines)
extern fe_noinline double add3_slowpath_f64(fe_pair_t, fe_pair_t);
extern fe_noinline fe_pair_t   fe_add_d_cr_slowpath(fe_pair_t, fe_pair_t, fe_pair_t);
extern fe_noinline fe_triple_t fe_triple_add_pd_slowpath(fe_pair_t, fe_pair_t, fe_pair_t);
extern fe_noinline int64_t fe_to_i64_slowpath(fe_pair_t x, double a);

extern fe_pair_t fe_pow_pn_d(double x, uint64_t n);
extern fe_pair_t fe_pow_n_d(double x, int64_t n);
extern fe_pair_t fe_pow_pn(fe_pair_t x, uint64_t n);


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

static inline fe_pair_t fe_copysign_d(fe_pair_t x, double    n) { return fe_pair(copysign(x.hi,n),    copysign(x.lo,n)); }
static inline fr_pair_t fr_copysign_d(fr_pair_t x, double    n) { return fr_pair(copysign(x.hi,n),    copysign(x.lo,n)); }
static inline fe_pair_t fe_copysign  (fe_pair_t x, fe_pair_t n) { return fe_pair(copysign(x.hi,n.hi), copysign(x.lo,n.hi)); }
static inline fr_pair_t fr_copysign  (fr_pair_t x, fr_pair_t n) { return fr_pair(copysign(x.hi,n.hi), copysign(x.lo,n.hi)); }


// x times the sign of 'n' (no flush of -0 to +0)
static inline fe_pair_t fe_mulsign_d(fe_pair_t x, double n)
{
  uint64_t sn = fe_to_bits(n) & UINT64_C(1) << 63;
  double   h  = fe_from_bits(fe_to_bits(x.hi) ^ sn);
  double   l  = fe_from_bits(fe_to_bits(x.lo) ^ sn);
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_mulsign(fe_pair_t x, fe_pair_t n)
{
  return fe_mulsign_d(x,n.hi);
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
static inline fe_pair_t fe_mul_pot(double s, fe_pair_t x)
{
  return fe_pair(x.hi*s,x.lo*s);
}

static inline fr_pair_t fr_mul_pot(double s, fr_pair_t x)  { return fe2fr(fe_mul_pot(s,fr2fe(x)));  }


/// returns: $(hi,lo) = x+y$. requires |x| >= |y| (or x = 0)
///
/// * in absence of underflow and overflow the result is exact
/// * if the inputs are reversed: |x| < |y| then the error bound
///    is u|hi|. "Note on FastTwoSum with Directed Roundings",
///    2024, Corbineau & Zimmermann, theorem 3.
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
static inline fe_pair_t fe_add_dd(double x, double y)  { return fe_two_sum(x,y);  }
static inline fr_pair_t fr_oadd_dd(double x, double y) { return fr_fast_sum(x,y); }
static inline fe_pair_t fe_oadd_dd(double x, double y) { return fe_fast_sum(x,y); }


static inline fe_pair_t fe_add_d(fe_pair_t x, double y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_sum(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_add(double x, fe_pair_t y) { return fe_add_d(y,x); }


// returns 0 if x=0 or is a power of two & non-zero otherwise
// (discard sign and exponent bits)
static inline uint64_t fe_not_pot(double x) { return fe_to_bits(x) << 12; }


// returns 0 if x=0, x=2^n or x=3 2^n & non-zero otherwise
// (discard sign, exponent and first explict signficand bits)
// computes [8] algorithm 5 (IsNot1or3TimesPowerOf2)
static inline uint64_t fe_not_13xpot(double x) { return fe_to_bits(x) << 13; }


static inline fe_pair_t fe_add_d_cr(fe_pair_t x, double c)
{
  // modified version of [8] algorithm 7.
  // first bullet point after theorem 3.11
  // uiCA: 37.00  (following fast-path)
  // 16 adds (for fast-path)
  fe_pair_t s = fe_two_sum(x.hi,c);     // 6 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds
  fe_pair_t w = fe_fast_sum(s.hi,v.hi); // 3 adds

  // statistically always taken
  if (fe_likely(fe_not_13xpot(v.hi)))
    return fe_pair(w.hi,w.lo+v.lo);

  return fe_add_d_cr_slowpath(s,v,w);
}


// |x| > |c|
static inline fe_pair_t fe_oadd_d_cr(fe_pair_t x, double c)
{
  // fe_add_d_cr: modified for known ordering
  // 13 adds (for fast-path)
  // uiCA: 24.00  (following fast-path)
  fe_pair_t s = fe_fast_sum(x.hi,c);    // 3 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds
  fe_pair_t w = fe_fast_sum(s.hi,v.hi); // 3 adds

  // statistically always taken
  if (fe_likely(fe_not_13xpot(v.hi)))
    return fe_pair(w.hi,w.lo+v.lo);

  return fe_add_d_cr_slowpath(s,v,w);
}


// |x| < |c|
static inline fe_pair_t fe_roadd_d_cr(fe_pair_t x, double c)
{
  // fe_add_d_cr: modified for known ordering
  // 13 adds (for fast-path)
  // uiCA: 24.00  (following fast-path)
  fe_pair_t s = fe_fast_sum(c,x.hi);    // 3 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds
  fe_pair_t w = fe_fast_sum(s.hi,v.hi); // 3 adds

  // statistically always taken
  if (fe_likely(fe_not_13xpot(v.hi)))
    return fe_pair(w.hi,w.lo+v.lo);

  return fe_add_d_cr_slowpath(s,v,w);
}



// 
static inline fe_triple_t fe_triple_add_pd(fe_pair_t x, double c)
{
  // modified version of [8] algorithm 7.
  // second bullet point after theorem 3.11
  // uiCA: 53.00  (following fast-path)
  // 18 adds (for fast-path)
  fe_pair_t s = fe_two_sum(x.hi,c);     // 6
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6
  fe_pair_t w = fe_fast_sum(s.hi,v.hi); // 3

  // statistically always taken
  if (fe_likely(fe_not_13xpot(v.hi))) {
    fe_pair_t e = fe_fast_sum(w.lo,v.lo); // 3
    
    return (fe_triple_t){.h=w.hi, .m=e.hi, .l=e.lo};
  }

  return fe_triple_add_pd_slowpath(s,v,w);
}

// |x| >= |c|
static inline fe_triple_t fe_triple_oadd_pd(fe_pair_t x, double c)
{
  // uiCA: 36.40  (following fast-path)
  // 15 adds (for fast-path)
  fe_pair_t s = fe_fast_sum(x.hi,c);    // 3
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6
  fe_pair_t w = fe_fast_sum(s.hi,v.hi); // 3

  // statistically always taken
  if (fe_likely(fe_not_13xpot(v.hi))) {
    fe_pair_t e = fe_fast_sum(w.lo,v.lo); // 3
    
    return (fe_triple_t){.h=w.hi, .m=e.hi, .l=e.lo};
  }

  return fe_triple_add_pd_slowpath(s,v,w);
}



static inline fe_pair_t fe_oadd_d(fe_pair_t x, double y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 4.50 
  fe_pair_t t = fe_fast_sum(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_oadd(double x, fe_pair_t y) { return fe_oadd_d(y,x); } 


static inline fe_pair_t fe_sub_dd(double x, double y) { return fe_two_diff(x,y); }
static inline fr_pair_t fr_sub_dd(double x, double y) { return fr_two_diff(x,y); }

static inline fe_pair_t fe_sub_d(fe_pair_t x, double y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_diff(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_d_sub(double x, fe_pair_t y)
{
  // DWPlusFP: 10 adds
  // uiCA: 33.00 
  fe_pair_t t = fe_two_diff(x, y.hi);
  double    l = t.lo - y.lo;
  
  return fe_fast_sum(t.hi,l);
}

static inline fe_pair_t fe_osub_d(fe_pair_t x, double y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 24.50 
  fe_pair_t t = fe_fast_diff(x.hi, y);
  double    l = x.lo + t.lo;
  
  return fe_fast_sum(t.hi,l);
}


static inline fe_pair_t fe_d_osub(double x, fe_pair_t y)
{
  // DWPlusFP (mod): 7 adds
  // uiCA: 24.50 
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
  // uiCA: 33.00
  fe_pair_t s = fe_two_sum(x.hi,y.hi);
  double    v = x.lo + y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
}

static inline fe_pair_t fe_oadd_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW (mod): 8 adds
  // uiCA: 24.88
  fe_pair_t s = fe_fast_sum(x.hi,y.hi);
  double    v = x.lo + y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
}

static inline fe_pair_t fe_sub_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW: 11 adds
  // uiCA: 33.00
  fe_pair_t s = fe_two_diff(x.hi,y.hi);
  double    v = x.lo - y.lo;
  double    w = s.lo + v;

  return fe_fast_sum(s.hi,w);
}

static inline fe_pair_t fe_osub_s(fe_pair_t x, fe_pair_t y)
{
  // SloppyDWPlusDW (mod): 8 adds
  // uiCA: 24.88
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
  // uiCA: 33.00 
  fr_pair_t t = fr_two_sum(a.hi, b);
  return fr_pair(t.hi, t.lo+a.lo);
}

static inline fr_pair_t fr_oadd_d(fr_pair_t a, double b)
{
  // CPairSum (mod): 4 adds
  // uiCA: 24.50 
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

static inline fr_pair_t fr_sub_d(fr_pair_t x, double y)
{
  // CPairSum (mod): 7 adds
  // uiCA: 13.00
  fr_pair_t t = fr_two_diff(x.hi,y);    // 6 adds
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_osub_d(fr_pair_t x, double y)
{
  // CPairSum (mod): 4 adds
  // uiCA: 4.50 
  fr_pair_t t = fr_fast_diff(x.hi,y);
  
  return fr_pair(t.hi, t.lo+x.lo);
}

static inline fr_pair_t fr_d_sub(double x, fr_pair_t y)
{
  // CPairSum (mod): 7 adds
  // uiCA: 13.00
  fr_pair_t t = fr_two_diff(x,y.hi);    // 6 adds
  
  return fr_pair(t.hi, t.lo-y.lo);
}

static inline fr_pair_t fr_d_osub(double x, fr_pair_t y)
{
  // CPairSum (mod): 4 adds
  // uiCA: 4.50 
  fr_pair_t t = fr_fast_diff(x,y.hi);
  
  return fr_pair(t.hi, t.lo-y.lo);
}

static inline fe_pair_t fe_mul_d(fe_pair_t x, double y)
{
  // DWTimesFP3: 2 fma, 1 mul, 3 add
  // uiCA: 4.94 
  fe_pair_t c = fe_two_mul(x.hi, y);    // 1 fma, 1 mul
  double    t = fma(x.lo,y,c.lo);
  
  return fe_fast_sum(c.hi,t);           // 3 adds
}

static inline fe_pair_t fe_d_mul(double x, fe_pair_t y) { return fe_mul_d(y,x); }


// identical result to: fe_result(fe_mul_d(x,y)) but
// eliminates useless ops that compilers aren't seeing.
static inline double fe_result_mul_d(fe_pair_t x, double y)
{
  return fma(x.hi,y,x.lo*y);
}

// as above but for 'fr'. need to normalize x first.
static inline double fr_result_mul_d(fr_pair_t x, double y)
{
  x = fr_normalize(x);
  return fma(x.hi,y,x.lo*y);
}


static inline fe_pair_t fe_mul_dd(double x, double y)
{
  return fe_two_mul(x,y);               // 1 fma, 1 mul
}


static inline fe_pair_t fe_mul_da(fe_pair_t x, double y)
{
  // DWTimesFP1: 1 fma, 2 mul, 7 add
  // uiCA: 32.00
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
  // 2 fma, 1 mul
  // (a,e)*y = ay+ey
  // uiCA: 4.94 
  double hi = x.hi*y;                   // RN(ay)
  double t  = fma(x.hi,y,-hi);          // RN(ay-RN(ay) 
  double lo = fma(x.lo,y, t);           // RN(ey+t);
  
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
  double    a = x.lo * y.lo;
  double    b = fma(x.hi,y.lo,a);
  double    c = fma(x.lo,y.hi,b);
  double    d = p.lo + c;
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
  double h = x.hi*y.hi;                 // ab
  double s = x.hi*y.lo + y.hi*x.lo;     // af+be (1)
  double g = fma(x.hi,y.hi,-h)+s;       // ab-RN(ab)+af+be
  
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

static inline fe_pair_t fe_sq_hq(fe_pair_t x)
{
  // 3 fma, 2 mul, 10 adds
  // uiCA: 36.40
  // ~1.5 ulp
  fe_pair_t a2 = fe_two_mul(x.hi,x.hi); // a² : 1 fma, 1 mul
  fe_pair_t ab = fe_two_mul(x.hi,x.lo); // ab : 1 fma, 1 mal

  ab.hi += ab.hi;                       // 2ab    (high)
  ab.lo += fma(x.lo,x.lo,ab.lo);        // 2ab+b² 

  // add in the a² term
  return fe_oadd_s(a2,ab);              // 8 adds
}

// double input is error free TwoMulFMA
static inline fe_pair_t fe_sq_d(double x) { return fe_two_mul(x,x); }
static inline fr_pair_t fr_sq_d(double x) { return fr_two_mul(x,x); }


#if 0
// higher op count and error. (which is why it's commented out)
static inline fe_pair_t fe_mul_a(fe_pair_t x, fe_pair_t y)
{
  // DWTimeDW2: 2 fma, 2 mul, 7 add
  fe_pair_t p = fe_two_mul(x.hi,y.hi);     // 1 fma, 1 mul
  double    a = x.hi * y.lo;
  double    m = fma(x.lo,y.hi,a);
  double    b = p.lo + m;

  return fe_two_sum(p.hi,b);               // 6 adds
}
#endif


static inline fe_pair_t fe_fma_ddd(double a, double b, double c)
{
  // Boldo & Muller 2011, algorithm 5 (ErrFmaNearest)
  // 2 fma, 1 mul, 15 add
  double    r1 = fma(a,b,c);
  fe_pair_t u  = fe_mul_dd(a,b);           // 1 fma, 1 mul
  fe_pair_t al = fe_two_sum(u.lo,c);       // 6 add
  fe_pair_t be = fe_two_sum(u.hi,al.hi);   // 6 add
  double    g  = (be.hi-r1)+be.lo;
  double    r2 = g+al.lo;
  
  return fe_pair(r1,r2);
}


static inline fe_pair_t fe_fma_ddd_a(double a, double b, double c)
{
  // Boldo & Muller 2011, algorithm 6 (ErrFmaApprox)
  // 2 fma, 1 mul, 9 add
  double    z = fma(a,b,c);
  fe_pair_t p = fe_mul_dd(a,b);            // 1 fma, 1 mul
  fe_pair_t u = fe_two_sum(c,p.hi);        // 6 add
  double    t = u.hi - z;
  double    l = t + (p.lo+u.lo);

  return fe_pair(z,l);
}



static inline fe_pair_t fe_div_d(fe_pair_t x, double y)
{
  // DWDivFP3: 2 div, 1 fma, 1 mul, 6 add
  // uiCA: 50.00 
  double    h = x.hi/y;
  fe_pair_t e = fe_two_mul(h,y);           // 1 fma, 1 mul
  double    a = x.hi - e.hi;               // (exact operation)
  double    b = a - e.lo;                  // (exact operation)
  double    c = b + x.lo;
  double    l = c / y;

  return fe_fast_sum(h,l);                 // 3 add
}

static inline fe_pair_t fe_inv_d(double x)
{
  // 2 div, 1 fma
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


static inline fr_pair_t fr_inv_d(double x)   { return fe2fr(fe_inv_d(x)); }
static inline fr_pair_t fr_inv_dh(double x)  { return fe2fr(fe_inv_dh(x)); }
static inline fr_pair_t fr_inv_dn(double x)  { return fe2fr(fe_inv_dn(x)); }



// add to doc/table
static inline fe_pair_t fe_inv(fe_pair_t x)
{
  // DWDivDW2 (mod): 2 div, 2 fma, 1 mul, 8 add
  // uiCA: 68.00 
  // ulp: ~6.69
  double    h = 1.0/x.hi;
  fe_pair_t r = fe_mul_d(x,h);         // DWTimesFP3: 2 fma, 1 mul, 3 add
  double    a = 1.0 - r.hi;            // (exact operation)
  double    c = a   - r.lo;
  double    l = c / x.hi;

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
  double    h = 1.0 / x.hi;             // initial approximation
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
  double    h = 1.0 / x.hi;             // initial approximation
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
  // uiCA: 66.25

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

static inline fe_pair_t fe_d_div(double x, fe_pair_t y)
{
  // DWDivDW2 (mod): 2 div, 2 fma, 1 mul, 8 adds
  // uiCA: 66.00
  double    h = x/y.hi;
  fe_pair_t r = fe_mul_d(y,h);          // DWTimesFP3: 2 fma, 1 mul, 3 add
  double    t = (x - r.hi) - r.lo;
  double    l = t / y.hi;

  return fe_fast_sum(h,l);              // 3 add
}


static inline fe_pair_t fe_div_dd(double x, double y)
{
  // 2 div, 1 fma, 1 mul, 5 add
  //uiCA: 50.25 
  double    h = x / y;
  fe_pair_t r = fe_mul_dd(y,h);         // 1 fma, 1 mul
  double    t = (x - r.hi) - r.lo;      // 2 add
  double    l = t / y;

  return fe_fast_sum(h,l);              // 3 add
}


#if 0
// not CPU useful
static inline fe_pair_t fe_div_a(fe_pair_t x, fe_pair_t y)
{
  // DWDivDW3: 1 div, 6 fma, 4 mul, 20 adds
  // uiCA: 99.50 
  double    i = 1.0 / y.hi;
  double    a = -fma(y.hi,i, -1.0);     // (exact)
  double    b = y.lo * i;
  fe_pair_t e = fe_fast_diff(a,b);      // 3 adds
  fe_pair_t d = fe_mul_d(e, i);         // DWTimesFP3: 2 fma, 1 mul, 3 add
  fe_pair_t m = fe_add_d(d, i);         // DWPlusFP: 10 adds

  return fe_mul(x, m);                  // DWTimeDW3: 3 fma, 2 mul, 4 add
}
#endif


static inline fr_pair_t fr_div(fr_pair_t x, fr_pair_t y)
{
  // CPairDiv: 2 div, 1 fma, 1 mul, 3 add
  // uiCA: 21.60
  double c = x.hi/y.hi;
  double t = fma(y.hi,c,-x.hi);      // (exact)
  double p = x.lo-t;
  double q = c*y.lo;
  double r = p-q;
  double s = y.hi+y.lo;              // s = y.hi if UP
  double g = r/s;

  return fr_pair(c,g);
}

static inline fr_pair_t fr_div_a(fr_pair_t x, fr_pair_t y)
{
  // CPairDiv: weakened by using fma (the second one)
  // 2 div, 2 fma, 2 add
  // uiCA: 21.10
  double h = x.hi/y.hi;
  double t =  fma(y.hi,h,  -x.hi);
  double n = -fma(y.lo,h, t-x.lo);
  double l = n/(y.hi+y.lo);
  return fr_pair(h,l);
}


static inline fr_pair_t fr_d_div(double x, fr_pair_t y)
{
  // 2 div, 1 fma, 1 mul, 2 add
  // uiCA: 34.33 
  double h = x/y.hi;
  double t = -fma(y.hi,h,-x);
  double r = t-h*y.lo;
  double s = y.hi+y.lo;              // s = y.hi if UP
  double l = r/s;

  return fr_pair(h,l);
}


/// returns $\frac{x}{y}$ as a `fr_pair_t`
static inline fr_pair_t fr_div_dd(double x, double y)
{
  // 2 div, 1 fma
  // uiCA: 30.00 
  double h = x/y;
  double l = -fma(y,h,-x)/y;

  return fr_pair(h,l);
}


/// returns $\frac{1}{x}$ as a `fr_pair_t`
static inline fr_pair_t fr_inv(fr_pair_t x)
{
  // 2 div, 1 fma, 1 mul, 2 add
  // uiCA: 21.40
  // ~7.78 ulp
  double c = 1.0/x.hi;
  double t = fma(x.hi,c,-1.0);
  double r = t + c*x.lo;
  double s = x.hi+x.lo;              // s = x.hi if UP
  double g = -r/s;

  return fr_pair(c,g);
}


static inline fr_pair_t fr_inv_a(fr_pair_t x)
{
  // 2 div, 2 fma, 1 add
  // uiCA: 
  double c = 1.0/x.hi;
  double t =  fma(x.hi,c,-1.0);
  double r = -fma(x.lo,c,   t);
  double s = x.hi+x.lo;              // s = x.hi if UP
  double g = r/s;

  return fr_pair(c,g);
}


// 1) "Accelerating Correctly Rounded Floating-Point Division when the Divisor Is Known in Advance"


// Newton-Raphson step for 1/sqrt(x)
static inline double fe_rsqrt_nr_(double y, double a, double r)
{
  // 3 fma, 1 mul
  double s = fma(a,r,0.5);
  double u = fma(y,y, -r);
  double v = fma(a,u,  s);

  return y*v;
}

// Halley method step for 1/sqrt(x)
static inline double fe_rsqrt_hm_(double h, double a, double r)
{
  // 4 fma, 2 mul
  double s = fma(r,a,0.5);
  double t = fma(h,h,-r);
  double v = fma(a,t, s);
  double w = fma(1.5*v,v,v);

  return h*w;
}

static inline fe_pair_t fe_rsqrt_d(double x)
{
  // NR: 1 sqrt, 1 div, 3 fma, 2 mul
  // uiCA: 26.86
  // ~5.24 ulp
  double r = 1.f/x;
  double a = -0.5*x;
  double h = sqrt(r);
  double l = fe_rsqrt_nr_(h,a,r);       // 3 fma, 1 mul
  
  return fe_pair(h,l);
}

static inline fe_pair_t fe_rsqrt_dh(double x)
{
  // Halley method: 1 sqrt, 1 div, 4 fma, 3 mul
  // uiCA: 26.00
  // ~3.22 ulp
  double r = 1.f/x;
  double t = -0.5*x;
  double h = sqrt(r);
  double l = fe_rsqrt_hm_(h,t,r);       // 4 fma, 2 mul
  
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
  // uiCA: 73.50 
  // ~6.97 ulp
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


static inline fe_pair_t fe_sqrt_d(double x)
{
  // CPairSqrt (mod): 1 sqrt, 1 div, 1 fma, 1 add
  // uiCA: 13.44
  // ~0.995 ulp
  double h = sqrt(x);
  double d = x != 0 ? h+h : 1.0;
  double t = -fma(h,h,-x);
  double l = t/d;

  return fe_pair(h,l);
}

static inline fr_pair_t fr_sqrt_d(double x)
{
  // CPairSqrt (mod): 1 sqrt, 1 div, 1 fma, 1 add
  // uiCA: 13.44
  // ~0.995 ulp
  double h = sqrt(x);
  double d = x != 0 ? h+h : 1.0;
  double t = -fma(h,h,-x);
  double l = t/d;

  return fr_pair(h,l);
}


// Iterative methods could be used instead
// but blows up the operation count per
// divide removed.
//
// This routine blows up if 'x' is denormal.
// 
// 1 sqrt, 3 div, 3 fma, 1 mul, 10 add
//   fe_inv:  2 div, 2 fma, 1 mul, 8 add
//   fe_sqrt: 1 sqrt, 1 div, 1 fma, 2 add
static inline fe_pair_t fe_rsqrt(fe_pair_t x)
{
  //  ~4.894 ulp
  return fe_sqrt(fe_inv(x));
}

// just reverse order evaluation: increases error but
// doesn't spurious overflow on denormal input.
// Denormal Safe 
static inline fe_pair_t fe_rsqrt_s(fe_pair_t x)
{
  // ~14.4531 ulp
  return fe_inv(fe_sqrt(x));
}


static inline fr_pair_t fr_rsqrt(fr_pair_t x)   { return fr2fe_uo_wrap(fe_rsqrt, x); }
static inline fr_pair_t fr_rsqrt_s(fr_pair_t x) { return fr2fe_uo_wrap(fe_rsqrt_s,x); }


static inline double fe_result_ro(fe_pair_t x)
{
  // bit-manipulation version of Boldo & Melquiond (2008)
  // SEE: reference.h for paper version.
  uint64_t  xh = fe_to_bits(x.hi);
  uint64_t  xl = fe_to_bits(x.lo);

  // thinking cap is in order for below here.
  uint64_t  o  = xl != 0;                   // if (x+y) is exact (low result zero) then there's no rounding
  uint64_t  s  = (xh & 1)-1;                // -1 if even, 0 if already odd (selector)
  uint64_t  d  = (-(xl >> 63))|o;           // ±1 to round in opposite direction that RN performed (if even)

  xh += s & d;                              // perform any rounding correction

  return fe_from_bits(xh);
}


// double round-to-odd
// (AVX-512 has some support for up/down/truncate)
// (TODO: need to note w/reference when Fast2Sum has the wrong sign on the low result)
static inline double add_ro_f64(double x, double y) { return fe_result_ro(fe_two_sum(x,y));  }
static inline double sub_ro_f64(double x, double y) { return fe_result_ro(fe_two_diff(x,y)); }

// below here slower than needed. lo word only needs to be accurate for: 0 and sign
static inline double mul_ro_f64(double x, double y) { return fe_result_ro(fe_two_mul(x,y));  }


// RN(a+b+c)  a.k.a correctly rounded.
// seem very unlikely to outperform the fast-path/slow-path
static inline double add3_bf_f64(double a, double b, double c)
{
  // modified version of Boldo & Melquiond (2008) which
  // uses a branch-free bit manipulation version of
  // their round to odd addition.
  // uiCA: 60.25
  fe_pair_t x = fe_two_sum(a,b);
  fe_pair_t s = fe_two_sum(c,x.hi);
  double    v = add_ro_f64(x.lo,s.lo);

  return(s.hi+v);
}


#if !defined(FE_PAIR_IMPLEMENTATION)

extern fe_pair_t fe_pow_n_d(double x, int64_t n);
extern fr_pair_t fr_pow_n_d(double x, int64_t n);

extern fe_pair_t fe_pow_n(fe_pair_t x, int64_t n);
extern fr_pair_t fr_pow_n(fr_pair_t x, int64_t n);

extern fe_pair_t fe_pow_pn_d(double x, uint64_t n);
extern fr_pair_t fr_pow_pn_d(double x, uint64_t n);
extern fe_pair_t fe_pow_pn(fe_pair_t x, uint64_t n);
extern fr_pair_t fr_pow_pn(fr_pair_t x, uint64_t n);

extern fe_triple_t fe_triple_add3_ddd(double a, double b, double c);
extern fe_pair_t   fe_add3_ddd(double a, double b, double c);
extern double      fe_result_add(fe_pair_t x, fe_pair_t y);

#else

// integer powers support

// * core squaring loop
static inline fe_pair_t fe_pow_pn_i(fe_pair_t r, fe_pair_t b, uint64_t i)
{
  i >>= 1;                              // caller handles low bit
  
  while(i > 1) {
    if (i & 1) r = fe_mul(r,b);
    b = fe_sq_hq(b);
    i >>= 1;
  }

  return fe_mul(r,b);
}

static inline fr_pair_t fr_pow_pn_i(fr_pair_t r, fr_pair_t b, uint64_t i)
{
  i >>= 1;                              // caller handles low bit
  
  while(i > 1) {
    if (i & 1) r = fr_mul(r,b);
    b = fr_sq(b);
    i >>= 1;
  }

  return fr_mul(r,b);
}


// * positive (n > 0) wrappers for doubles & pairs
// requires n > 0
fe_pair_t fe_pow_pn_d(double x, uint64_t n)
{
  fe_pair_t r  = fe_pair(1,0);
  fe_pair_t b  = fe_sq_d(x);
  uint64_t  i  = n;

  if (i & 1) r.hi = x;

  return fe_pow_pn_i(r,b,i);
}

// requires n > 0
fe_pair_t fe_pow_pn(fe_pair_t x, uint64_t n)
{
  fe_pair_t r  = fe_pair(1,0);
  fe_pair_t b  = fe_sq_hq(x);
  uint64_t  i  = n;

  if (i & 1) r = x;
  
  return fe_pow_pn_i(r,b,i);
}

// requires n > 0
fr_pair_t fr_pow_pn_d(double x, uint64_t n)
{
  fr_pair_t r  = fr_pair(1,0);
  fr_pair_t b  = fr_sq_d(x);
  uint64_t  i  = n;

  if (i & 1) r.hi = x;

  return fr_pow_pn_i(r,b,i);
}

// requires n > 0
fr_pair_t fr_pow_pn(fr_pair_t x, uint64_t n)
{
  fr_pair_t r  = fr_pair(1,0);
  fr_pair_t b  = fr_sq(x);
  uint64_t  i  = n;

  if (i & 1) r = x;
  
  return fr_pow_pn_i(r,b,i);
}


// * complete versions:
//   overall structure assumes that n=0 is unlikely

fe_pair_t fe_pow_n_d(double x, int64_t n)
{
  if (n > 0)
    return fe_pow_pn_d(x,(uint64_t)n);

  if (n < 0) {
    fe_pair_t r = fe_pow_pn_d(x,(uint64_t)(-n));
    
    return fe_inv(r);
  }

  return fe_pair(1.0,0.0);
}


fr_pair_t fr_pow_n_d(double x, int64_t n)
{
  if (n > 0)
    return fr_pow_pn_d(x,(uint64_t)n);

  if (n < 0) {
    fr_pair_t r = fr_pow_pn_d(x,(uint64_t)(-n));
    
    return fr_inv(r);
  }

  return fr_pair(1.0,0.0);
}


fe_pair_t fe_pow_n(fe_pair_t x, int64_t n)
{
  if (n > 0)
    return fe_pow_pn(x,(uint64_t)n);

  if (n < 0) {
    fe_pair_t r = fe_pow_pn(x,(uint64_t)(-n));
    
    return fe_inv(r);
  }

  return fe_pair(1.0,0.0);
}


fr_pair_t fr_pow_n(fr_pair_t x, int64_t n)
{
  if (n > 0)
    return fr_pow_pn(x,(uint64_t)n);

  if (n < 0) {
    fr_pair_t r = fr_pow_pn(x,(uint64_t)(-n));
    
    return fr_inv(r);
  }

  return fr_pair(1.0,0.0);
}


// slow-path routines

static inline double add3_slowpath_core(fe_pair_t s, fe_pair_t v)
{
  uint64_t ss = fe_to_bits(v.hi) ^ fe_to_bits(v.hi);

  // should change this to bit manipulation
  if (((int64_t)ss) > 0)
    return s.hi + (1.125 * v.hi);
  else
    return s.hi + (0.875 * v.hi);
}

fe_noinline double add3_slowpath_f64(fe_pair_t s, fe_pair_t v)
{
  // this is just about zero
  if (fe_unlikely(v.lo == 0))
    return s.hi+v.hi;

  return add3_slowpath_core(s,v);
}

fe_noinline fe_pair_t fe_add_d_cr_slowpath(fe_pair_t s, fe_pair_t v, fe_pair_t w)
{
  if (fe_unlikely(v.lo==0.0))
    return fe_pair(w.hi,w.lo+v.lo);

  double z     = add3_slowpath_core(s,v);
  double alpha = z - w.hi;
  double delta = w.lo - alpha;

  return fe_pair(z,delta+v.lo);
}

fe_noinline fe_triple_t fe_triple_add_pd_slowpath(fe_pair_t s, fe_pair_t v, fe_pair_t w)
{
  if (fe_unlikely(v.lo==0.0)) {
    fe_pair_t e = fe_fast_sum(w.lo,v.lo);
    
    return (fe_triple_t){.h=w.hi, .m=e.hi, .l=e.lo};
  }
  
  double    z     = add3_slowpath_core(s,v);
  double    alpha = z - w.hi;
  double    delta = w.lo - alpha;
  fe_pair_t e     = fe_fast_sum(delta,v.lo);
  
  return (fe_triple_t){.h=z, .m=e.hi, .l=e.lo};
}


fe_noinline fe_triple_t fe_triple_add3_ddd_slowpath(fe_pair_t z, double w, double σ, double t)
{
  // [9] algorithm 9 (Sum3-with-error) : slow-path part
  double wp = 1.5*w;
  double σ2 = z.hi+wp;
  double Σ;

  // restructure with: Σ = z.hi above. slow-path is almost
  // never so doesn't really matter. 
  if (fe_unlikely(σ2 == z.hi)) {
    Σ = z.hi;
  }
  else {
    if (fe_unlikely(t==0.0)) {
      Σ = σ;
    }
    else {
      double g = t*w;
      if (g < 0)
        Σ = z.hi;
      else
        Σ = σ2;
    }
  }

  double    α = Σ - z.hi;
  double    η = w - α;
  fe_pair_t r = fe_fast_sum(η,t);
  
  return fe_triple(Σ,r.hi,r.lo);
}


fe_noinline fe_pair_t fe_add3_ddd_slowpath(fe_pair_t z, double w, double σ, double t)
{
  // [9] algorithm 9 (Sum3-with-error) : slow-path part (commented replacement)
  double wp = 1.5*w;
  double σ2 = z.hi+wp;
  double Σ;

  // restructure with: Σ = z.hi above. slow-path is almost
  // never so doesn't really matter. 

  if (fe_unlikely(σ2 == z.hi)) {
    Σ = z.hi;
  }
  else {
    if (fe_unlikely(t==0.0)) {
      Σ = σ;
    }
    else {
      double g = t*w;
      if (g < 0)
        Σ = z.hi;
      else
        Σ = σ2;
    }
  }

  double α = Σ - z.hi;
  double η = w - α;
  
  return fe_pair(Σ, η+t);
}


fe_triple_t fe_triple_add3_ddd(double a, double b, double c)
{
  // [9] algorithm 9 (Sum3-with-error)
  // 30 adds along fast-path
  fe_pair_t x = fe_two_sum(a,b);        // 6 adds
  fe_pair_t s = fe_two_sum(x.hi,c);     // 6 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds
  fe_pair_t z = fe_fast_sum(s.hi,v.hi); // 3 adds
  double    w = v.lo + z.lo;
  double    σ = z.hi + w;
  double    δ = w - z.lo;
  double    t = v.lo - δ;
  uint64_t  T = fe_not_pot(v.hi);       // !IsPowerOf2 as bit manipulation (discard sign & exp)

  if (fe_likely(T != 0)) {
    double    α = σ - z.hi;
    double    η = w - α;
    fe_pair_t r = fe_fast_sum(η,t);
    return fe_triple(σ,r.hi,r.lo);
  }

  return fe_triple_add3_ddd_slowpath(z,w,σ,t);
}


fe_pair_t fe_add3_ddd(double a, double b, double c)
{
  // [9] algorithm 9 (Sum3-with-error) : commented replacement
  // 28 adds along fast-path
  fe_pair_t x = fe_two_sum(a,b);        // 6 adds
  fe_pair_t s = fe_two_sum(x.hi,c);     // 6 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds
  fe_pair_t z = fe_fast_sum(s.hi,v.hi); // 3 adds
  double    w = v.lo + z.lo;
  double    σ = z.hi + w;
  double    δ = w - z.lo;
  double    t = v.lo - δ;
  uint64_t  T = fe_not_pot(v.hi);       // !IsPowerOf2 as bit manipulation (discard sign & exp)

  if (fe_likely(T != 0)) {
    double α = σ - z.hi;
    double η = w - α;
    return fe_pair(σ, η+t);
  }

  return fe_add3_ddd_slowpath(z,w,σ,t);
}


// RN(x+y) : error bound = u (correctly rounded)
double fe_result_add(fe_pair_t x, fe_pair_t y)
{
  // [9] algorithm 11 (FPNearestSumDW)
  // 55 adds (following fast-path)
  fe_pair_t s = fe_two_sum(x.hi, y.hi);        // 6 adds
  fe_pair_t t = fe_two_sum(x.lo, y.lo);        // 6 adds
  fe_pair_t γ = fe_two_sum(s.lo, t.hi);        // 6 adds
  fe_pair_t v = fe_fast_sum(s.hi, γ.hi);       // 3 adds
  fe_pair_t w = fe_fast_sum(v.lo, t.lo);       // 3 adds
  fe_pair_t z = fe_fast_sum(v.hi, w.hi);       // 3 adds
  fe_pair_t q = fe_add3_ddd(z.lo, w.lo, γ.lo); // 28 adds (along fast-path)
  uint64_t  T = fe_not_pot(q.hi);
  
  if (fe_likely(T != 0)) return z.hi;

  // slow-path: 
  double ρ = 1.5*q.hi;
  double σ = z.hi + ρ;

  if (fe_unlikely(σ    == z.hi)) return z.hi;
  if (fe_unlikely(q.lo == 0.0))  return z.hi + q.hi;
  if (q.hi * q.lo <= 0)          return z.hi;

  return σ;
}

// |x| >= 2^63 or NaN
fe_noinline int64_t fe_to_i64_slowpath(fe_pair_t x, double a)
{
  int64_t r = INT64_MIN;
  
  if (x.hi > 0.0) {
    r -= 1;
    
    if (a == 0x1.0p63) {
      if (x.lo < 0.0 && x.lo > -0x1.0p63)
        return r+(int64_t)x.lo+1;
    }
    
    return r;
  }
  
  if (a == 0x1.0p63 && x.lo < 0x1.0p63)
    r += (int64_t)x.lo;
  
  return r;
}

#endif


// RN(x+c)  a.k.a correctly rounded.
static inline double fe_result_add_d(fe_pair_t x, double c)
{
  // modified version of [8] algorithm 6 (CR-DWPlusFP)
  fe_pair_t s = fe_two_sum(x.hi,c);     // 6 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds

  if (fe_likely(fe_not_13xpot(x.hi)))
    return s.hi+v.hi;

  // expected rate to reach here: 2^-51
  return add3_slowpath_f64(s,v);
}


// RN(x+c) : given |x| > |c|
static inline double fe_result_oadd_d(fe_pair_t x, double c)
{
  // modified version of 'fe_result_add_d'
  fe_pair_t s = fe_fast_sum(x.hi,c);    // 3 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds

  if (fe_likely(fe_not_13xpot(x.hi)))
    return s.hi+v.hi;

  return add3_slowpath_f64(s,v);
}

// RN(x+c) : given |x| < |c|
static inline double fe_result_roadd_d(fe_pair_t x, double c)
{
  // modified version of 'fe_result_add_d'
  fe_pair_t s = fe_fast_sum(c,x.hi);    // 3 adds
  fe_pair_t v = fe_two_sum(x.lo,s.lo);  // 6 adds

  if (fe_likely(fe_not_13xpot(x.hi)))
    return s.hi+v.hi;

  return add3_slowpath_f64(s,v);
}

// RN(a+b+c)  a.k.a correctly rounded.
static inline double add3_f64(double a, double b, double c)
{
  // modified version of [8] algorithm 8 (EmulADD3)
  // [9] has a different method (algorithm 8)
  return fe_result_add_d(fe_two_sum(a,b),c);
}


// 3 word expansion of: ab+c
static inline fe_triple_t fe_triple_fma_ddd(double a, double b, double c)
{
  // both versions:  2 fma, 1 mul, 18 adds
#if 0
  // Boldo & Muller (2011)
  // algorithm xx ()
  // uiCA: 57.25
  fe_pair_t   x,s,v;
  fe_triple_t z;
  double      d;

  z.h  = fma(a,b,c);
  x.hi = a*b;
  x.lo = fma(a,b,-x.hi);
  s    = fe_two_sum(x.hi,c);            // 6 adds
  v    = fe_two_sum(x.lo,s.lo);         // 6 adds
  d    = v.hi - (z.h - s.hi);           // 2 adds
  v    = fe_fast_sum(d,v.lo);           // 3 adds
  z.m  = v.hi;
  z.l  = v.lo;
#else
  // [8] algorithm 12 (Faster-FMA-with-error)
  // should have higher throughput.
  // uiCA: 50.25
  fe_triple_t z;
  fe_pair_t   x, alpha, beta, r;
  double      gamma;

  z.h   = fma(a,b,c);
  x.hi  = a*b;
  x.lo  = fma(a,b,-x.hi);
  alpha = fe_two_sum(x.lo, c);          // 6 adds
  beta  = fe_two_sum(x.hi, alpha.hi);   // 6 adds
  gamma = (beta.hi-z.h)+beta.lo;        // 2 adds
  r     = fe_fast_sum(gamma,alpha.lo);  // 3 adds
  z.m = r.hi;
  z.l = r.lo;
#endif  

  return z;
}


// RN(a+b+c+d) : error bound = u (correctly rounded)
static inline double sum4_cr_f64(double a, double b, double c, double d)
{
  // [9] algorithm 12 (Sum4)
  // 67 adds (following fast-path)
  fe_pair_t x = fe_two_sum(a,b);
  fe_pair_t y = fe_two_sum(c,d);

  return fe_result_add(x,y);
}

// ab+cd
// error bound = 3u & 2u if sign(ab)==sign(cd)  (Kahan)
static inline double mma_f64(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b, t);
  return f+e;
}

// RN(ab+cd) : error bound = u (correctly rounded)
static inline double mma_cr_f64(double a, double b, double c, double d)
{
  // [9] algorithm 13 (FD2)
  fe_pair_t x = fe_two_mul(a,b);
  fe_pair_t y = fe_two_mul(c,d);

  return fe_result_add(x,y);
}

// ab-cd
// error bound = 3u & 2u if sign(ab)!=sign(cd)  (Kahan)
static inline double mms_f64(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b,-t);
  return f-e;
}

// RN(ab-cd) : error bound = u (correctly rounded)
static inline double mms_cr_f64(double a, double b, double c, double d)
{
  // [9] algorithm 13 (FD2)
  fe_pair_t x = fe_two_mul(a,b);
  fe_pair_t y = fe_two_mul(c,d);

  return fe_result_add(x,fe_neg(y));
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


static inline fe_pair_t fe_from_i64(int64_t x)
{
  // split the hi word so no rounding occurs
  double a = (double)(x & (int64_t)UINT64_C(0xffffffff00000000));
  double b = (double)(x-((int64_t)a));

  // fast-sum to canonicalize: note legal when a=0
  return fe_fast_sum(a,b);
}


// if conversion of hi of 'x' cannot integer overflow
static inline int64_t fe_to_i64_i(fe_pair_t x)
{
  int64_t h = (int64_t)x.hi;
  int64_t l = (int64_t)x.lo;
  return h+l;
}

// convert to integer: values that overflow are saturated. NaN
// maps to min-integer
static inline int64_t fe_to_i64(fe_pair_t x)
{
  double a = fabs(x.hi);
  
  if (a < 0x1.0p63) return fe_to_i64_i(x);

  return fe_to_i64_slowpath(x,a);
}


static inline fe_pair_t fe_one(void)       { return fe_set_d( 1.0);  }
static inline fr_pair_t fr_one(void)       { return fr_set_d( 1.0);  }
static inline fe_pair_t fe_zero(void)      { return fe_set_d( 0.0);  }
static inline fr_pair_t fr_zero(void)      { return fr_set_d( 0.0);  }
static inline fe_pair_t fe_neg_zero(void)  { return fe_set_d(-0.0);  }
static inline fr_pair_t fr_neg_zero(void)  { return fr_set_d(-0.0);  }

static inline _Bool fe_eq_zero(fe_pair_t x) { return x.hi      == 0.0; }   // if hi is zero then illegal for lo to be non-zero
static inline _Bool fr_eq_zero(fr_pair_t x) { return x.hi+x.lo == 0.0; }
static inline _Bool fe_gt_zero(fe_pair_t x) { return x.hi      >  0.0; }
static inline _Bool fr_gt_zero(fr_pair_t x) { return x.hi+x.lo >  0.0; }


static inline _Bool fe_eq(fe_pair_t x, fe_pair_t y)
{
  return (x.hi == y.hi) && (x.lo == y.lo);
}


// generics versions (dd = double-double). Nothing really here yet because
// I'm not so sure it's an interesting thing to do. Treating double-doubles
// like builtins types (hardware supported) doesn't seem very useful.
// Much better suited to temporarily bumping up precision using know ranges
// (etc) to minimize performance impact and working with known targets
// of error at each step.  IMHO black-box "increased precision" is better
// handled by some multi-precision library (like MPFR).
// Anyway half thinking there might be the narrow case of not wanting to
// pull in a MP lib and/or pay the RT cost of the evalulation for some
// empirical testing and calling some pair implementation good enough as
// the "reference".

#define dd_def_uop(x,OP)                   \
    double: _Generic((x),                  \
        fe_pair_t: fe_##OP,                \
        double:    fe_##OP##_d             \
    )(x)

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



