// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2016-2023

// WIP: loaded with unconverted constants, etc

#ifndef F64_UTIL_H
#define F64_UTIL_H

#ifdef __cplusplus
extern "C" {
#ifdef __emacs_hack
}  
#endif  
#endif

#include <stdbool.h>

#define F64_PRAGMA(X) _Pragma(X)

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define F64_INTEL
#elif defined(__ARM_ARCH)
#define F64_ARM
#endif

#if defined(__clang__) && (__clang_major__ >= 13)
// future me note: GCC 12 is adding: __builtin_assoc_barrier
#define FP64_REASSOCIATE_ON()  F64_PRAGMA("clang fp reassociate(on)")
#define FP64_REASSOCIATE_OFF() F64_PRAGMA("clang fp reassociate(off)")
#define FP64_ASSOC_BARRIER(X)  (X)
#elif defined(__GNUC__) &&  (__GNUC__ >= 12)
#define FP64_REASSOCIATE_ON()
#define FP64_REASSOCIATE_OFF()
#define FP64_ASSOC_BARRIER(X)   __builtin_assoc_barrier(X)
#endif

// just say no
#if !(defined(__FAST_MATH__) || defined(__FINITE_MATH_ONLY__) || defined(__ASSOCIATIVE_MATH__) || defined(_M_FP_FAST))
#define FP64_LOL_NO
#endif

// wrap an intel intrinsic
#define F64_SSE_WRAP(F,X) _mm_cvtsd_f64(F(_mm_set_sd(X)))


// ulp(1.0)
static const double   f64_ulp1 = 0x1.0p-52;

static const uint64_t f64_sign_mask = UINT64_C(1)<<63;


// NOTES:
// u = round-off unit (2^-) = ulp(1)/2

typedef struct { double h,l; } f64_pair_t;

static inline uint64_t f64_to_bits(double x)
{
  uint64_t u; memcpy(&u, &x, 8); return u;
}

static inline double f64_from_bits(uint64_t x)
{
  double f; memcpy(&f, &x, 8); return f;
}

// convert inputs to IEEE bit pattern and XOR's them
static inline uint64_t f64_xor(double a, double b)
{
  return f64_to_bits(a)^f64_to_bits(b);
}

static inline double f64_mulsign(double v, uint64_t s)
{
  return f64_from_bits(f64_to_bits(v)^s);
}

static inline uint64_t f64_sign_bit(float a)
{
  return f64_to_bits(a) & f64_sign_mask;
}

// to cut some of the pain of math errno not being disabled
// (-fno-math-errno). But you really should do that. Unless
// your using visual-c. Then: I feel your pain.
static inline double f64_sqrt(double a)
{
#if defined(__NO_MATH_ERRNO__) || defined(_M_FP_FAST)
  return sqrt(a);
#elif defined(F64_INTEL)
  return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_undefined_pd(),_mm_set_sd(a)));
#elif defined(F64_ARM)
  return __sqrt(a);
#else
  // do nothing for warning ATM
#endif  
}

// 1/sqrt(a) : lowest error "naive" version
static inline double f64_rsqrt(double a)
{
  return f64_sqrt(1.0/a);
}


#if defined(F32_INTEL)
// on Intel minsd/maxsd if either input is NaN then the second source
// is returned so it can be either input.
static inline double f64_fast_min(double a, double b)
{
  return !(a > b) ? a : b;
}

static inline double f64_fast_max(double a, double b)
{
  return !(a < b) ? a : b;
}
#else
static inline double f64_fast_min(double a, double b) { return fmin(a,b); }
static inline double f64_fast_max(double a, double b) { return fmax(a,b); }
#endif


// all seem fine on all modernish targets/compilers
static inline double f64_ceil(double x)     { return ceil(x);  }
static inline double f64_floor(double x)    { return floor(x); }
static inline double f64_trunc(double x)    { return trunc(x); }
static inline double f64_fract(double x)    { return x - f64_trunc(x); }

// intel doesn't have as a op. computed as
//   trunc(x + copysign(0.5-ulp(0.5), x))
static inline double f64_round(double x)    { return round(x); }

// directed rounding aliases
// cm=current mode, tz=toward zero, pi=positive inf, ni=negative inf
static inline double f64_round_cm(double x)  { return nearbyint(x);  }
static inline double f64_round_tz(double x)  { return f64_trunc(x);  }
static inline double f64_round_pi(double x)  { return f64_ceil(x);   }
static inline double f64_round_ni(double x)  { return f64_floor(x);  }
static inline double f64_round_na(double x)  { return f64_round(x);  }

// round to nearest (ties to even) aka banker's rounding. default mode
// so 'f32_round_cm' produces same if mode hasn't been changed.
static inline double f64_round_ne(double x)
{
#if defined(F32_INTEL)
  __m128d v = _mm_set_sd(x);
  __m128d r = _mm_round_sd(v,v, _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC);
  
  return _mm_cvtsd_f64(r);
#elif defined(__ARM_FEATURE_DIRECTED_ROUNDING)
  return __rintn(x);
#else  
  // do nothing for warning ATM
#endif
}



// x >=0 ? 1.0 : -1.f
static inline double f64_sgn(double x) { return copysign(1.0,x); }

/** exact transform functions **/


// (1-t)a + tb
// should be two fma ops
static inline double f64_lerp(double t, double a, double b)
{
  return fma(t,b,-fma(t,a,-a));
}

static const f64_pair_t f64_up_pi = {.h=0x1.921fb54442d18p1, .l= 0x1.1a62633145c07p-53};


// exact product: ab (provided no over/underflow)
// h = RN(ab), l=RN(ab-RN(ab) -> ab = (h+l)
static inline void f64_2mul(f64_pair_t* r, double a, double b)
{
  double t = a*b;
  double e = fma(a,b,-t);
  r->h = t;
  r->l = e;
}

// return RN(x*p) where p is unevaluated pair
static inline double f64_up_mul(const f64_pair_t* const p, double x) 
{
  return fma(x, p->h, x*p->l);
}

// compute a*b+c
static inline double f64_up_madd(const f64_pair_t* const a, double b, double c)
{
  return fma(a->h, b, fma(a->l, b, c));
}

static inline double f64_mul_pi(double x)
{
  return f64_up_mul(&f64_up_pi,x);
}
				

// (a*b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow. 
static inline void f64_2mul(f64_pair_t* p, double a, double b)
{
  double x = a*b;
  double y = fma(a,b,-x);
  p->h = x;
  p->l = y;
}

// checks if LSB is clear.
static inline uint64_t f64_is_even(double a)
{
  uint64_t u = f64_to_bits(a);
  return (u & 1);
}

// next representable FP away from zero
// * blindly walks past +/-inf to NaN
static inline double f64_succ(double a)
{
  return f64_from_bits(f64_to_bits(a)+1);
}

// next representable FP toward zero
// * blindly walks past +/-zero to NaN
static inline double f64_pred(double a)
{
  return f64_from_bits(f64_to_bits(a)-1);
}

// walks 'd' ULP away from zero
// * blindly walks past +/-zero to NaN
static inline double f64_walk(double a, int64_t d)
{
  return f64_from_bits(f64_to_bits(a)+(uint64_t)d);
}

// rounding unit + lowest bit set
static const double f64_succ_pred_k = 0x1.0000000000001p-53;

// next representable FP away from zero
// * a is normal and not zero
static inline double f64_nnz_succ(double a)
{
  const double s = f64_succ_pred_k; 
  return fma(a,s,a);
}

// next representable FP toward zero
// * a is normal and not zero
static inline double f64_nnz_pred(double a)
{
  const double s = -f64_succ_pred_k;
  return fma(a,s,a);
}

// next representable FP toward +inf
// * a is normal and not zero
static inline double f64_nnz_next(double a)
{
  const double s = f64_succ_pred_k; 
  return fma(fabs(a),s,a);
}

// next representable FP toward -inf
// * a is normal and not zero
static inline double f64_nnz_prev(double a)
{
  const double s = -f64_succ_pred_k;
  return fma(fabs(a),s,a);
}

// ulp(a)
// * |a| != flt_max
static inline double f64_ulp(double a)
{
  double t = fabs(a);
  return f64_succ(t)-t;
}

// ab+cd
// * r within +/- 3/2 ulp
// * r within +/-   1 ulp, if sign(ab) == sign(cd)
static inline double f64_mma(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b, t);
  return f+e;
}

// ab+cd
// for cascading: result = returned + e
static inline double f64_mma_e(double a, double b, double c, double d, double* e)
{
  double t = c*d;
  double f = fma(a,b, t);
  *e = fma(c,d,-t);
  return f;
}

// ab-cd
// SEE: f64_mma for error bounds
static inline double f64_mms(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b,-t);
  return f-e;
}

// ab-cd
// for cascading: result = returned + e
static inline double f64_mms_e(double a, double b, double c, double d, double* e)
{
  double t = c*d;
  double f = fma(a,b,-t);
  *e = -fma(c,d,-t);
  return f;
}

// ab+cd: this version returns same results if {a,b} and {c,d} are swapped. It
// does not improve the error bound.
static inline double f64_mma_c(double a, double b, double c, double d)
{
  FP64_REASSOCIATE_OFF();
  double p0 = a*b;
  double p1 = c*d;
  double e0 = fma(a,b,-p0);
  double e1 = fma(c,d,-p1);
  double r0 = p0+p1;
  double r1 = e0+e1;
  return r0+r1;
}

//
static inline uint64_t u64_abs(uint64_t x)
{
  return (int64_t)x >= 0 ? x : -x;
}


// ulp distance provided a & b are finite
// and have the same sign
static inline uint64_t f64_ulp_dist_ss(double a, double b)
{
  uint64_t ua = f64_to_bits(a);
  uint64_t ub = f64_to_bits(b);
  return u64_abs(ua-ub);
}

// ulp distance for finite a & b
static inline uint64_t f64_ulp_dist(double a, double b)
{
  uint64_t ua = f64_to_bits(a);
  uint64_t ub = f64_to_bits(b);

  if ((int64_t)(ub^ua) >= 0)
    return u64_abs(ua-ub);
  
  return ua+ub+f64_sign_mask;
}

// a & b are within 'd' ulp of each other
static inline uint64_t f64_within_ulp(double a, double b, uint64_t d)
{
  uint64_t ua = f64_to_bits(a);
  uint64_t ub = f64_to_bits(b);

  if ((int64_t)(ub^ua) >= 0)
    return ua-ub+d <= d+d;
  
  return ua+ub+f64_sign_mask <= d+d;
}

// filtered approximately equal:
static inline uint64_t f64_approx_eq(double a, double b, double absD, double relD)
{
  double d = fabs(a-b);
  
  if (d <= absD)
    return 1;
  
  a = fabs(a);
  b = fabs(b);
  
  double m = (b>a) ? b : a;
  
  if (d <= m * relD)
    return 1;
  
  return 0;
}

// returns true if neither are NaN
static inline bool f64_are_ordered(double a, double b)
{ 
#if !(defined(__ASSOCIATIVE_MATH__) || defined(_M_FP_FAST))
  return (a==a) && (b==b);
#else
#endif
}

// returns true if a >= b or neither are NaN
static inline bool f64_ge_or_unordered(double a, double b)
{
  FP64_REASSOCIATE_OFF();
  return !(a < b);
}

// returns true if a > b or neither are NaN
static inline bool f64_gt_or_unordered(double a, double b)
{
  FP64_REASSOCIATE_OFF();
  return !(a <= b);
}

// (a+b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline void f64_2sum(f64_pair_t* p, double a, double b)
{
  FP64_REASSOCIATE_OFF();
  double x  = (a+b);
  double bp = (x-a);
  double ap = (x-bp);
  double eb = (b-bp);
  double ea = (a-ap);
  double y  = (ea+eb);
  p->h = x;
  p->l = y;
}

// (a+b) exactly represented by unevaluated pair (h+l)
// * requires |a| >= |b|
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline void f64_fast2sum(f64_pair_t* p, double a, double b)
{
  FP64_REASSOCIATE_OFF();
  double x  = (a+b);
  double bp = (x-a);
  double y  = (b-bp);
  p->h = x;
  p->l = y;
}


#ifdef __cplusplus
extern "C" {
#endif
#endif

