// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2016-2022

#ifndef F32_UTIL_H
#define F32_UTIL_H

#ifdef __cplusplus
extern "C" {
#ifdef __emacs_hack
}  
#endif  
#endif

#include <stdbool.h>

#define F32_PRAGMA(X) _Pragma(X)

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define F32_INTEL
#elif defined(__ARM_ARCH)
#define F32_ARM
#endif

// MSC: expects /Gv
// https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros
// _M_FP_FAST
// _M_FP_CONTRACT
// _M_FP_PRECISE
// _M_FP_STRICT 
// GCC macros:
// __NO_MATH_ERRNO__
// __RECIPROCAL_MATH__
// __ASSOCIATIVE_MATH__

#if defined(__clang__) && (__clang_major__ >= 13)
// future me note: GCC 12 is adding: __builtin_assoc_barrier

#define FP32_REASSOCIATE_ON()  F32_PRAGMA("clang fp reassociate(on)")
#define FP32_REASSOCIATE_OFF() F32_PRAGMA("clang fp reassociate(off)")
#else
#define FP32_REASSOCIATE_ON()
#define FP32_REASSOCIATE_OFF()
#endif

// wrap an intel intrinsic
#define F32_SSE_WRAP(F,X) _mm_cvtss_f32(F(_mm_set_ss(X)))

// ulp(1.f)
static const float f32_ulp1 = 0x1.0p-23f;

static const uint32_t f32_sign_bit_k = 0x80000000;

// NOTES:
// u = round-off unit (2^-24) = ulp(1)/2

typedef struct { float h,l; } f32_pair_t;

static inline uint32_t f32_to_bits(float x)
{
  uint32_t u; memcpy(&u, &x, 4); return u;
}

static inline float f32_from_bits(uint32_t x)
{
  float f; memcpy(&f, &x, 4); return f;
}

// convert inputs to IEEE bit pattern and XOR's them
static inline uint32_t f32_xor(float a, float b)
{
  return f32_to_bits(a)^f32_to_bits(b);
}

// if 'v' is float and 's' is all clear (except sign bit)
static inline float f32_mulsign(float v, uint32_t s)
{
  return f32_from_bits(f32_to_bits(v)^s);
}

static inline uint32_t f32_sign_bit(float a)
{
  return f32_to_bits(a) & f32_sign_bit_k;
}

// to cut some of the pain of math errno not being disabled
// (-fno-math-errno). But you really should do that. Unless
// your using visual-c. Then: I feel your pain.
static inline float f32_sqrt(float a)
{
#if defined(__NO_MATH_ERRNO__) || defined(_M_FP_FAST)
  return sqrtf(a);
#elif defined(F32_INTEL)
  return F32_SSE_WRAP(_mm_sqrt_ss, a);
#elif defined(F32_ARM)
  return __sqrtf(a);
#else
  // do nothing for warning
#endif  
}


// round to nearest (ties to even) (doesn't modify sticky flags)
static inline float f32_round(float x)
{
#if defined(F32_INTEL)
  __m128 v = _mm_set_ss(x);
  __m128 r = _mm_round_ss(v,v, _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC);
  
  return _mm_cvtss_f32(r);
#elif defined(F32_ARM)
  // sticky flags modified?
  return __rintnf(x);
#else
  // do nothing for warning
#endif  
}


#if defined(F32_INTEL)

// round toward zero (doesn't modify sticky flags)
static inline float f32_round_tz(float x)
{
  __m128 v = _mm_set_ss(x);
  __m128 r = _mm_round_ss(v,v, _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC);
  
  return _mm_cvtss_f32(r);
}

#else
// error ATM
#endif

// x >=0 ? 1.0 : -1.f
static inline float f32_sgn(float x) { return copysignf(1.f,x); }

/** exact transform functions **/


// (1-t)a + tb
// should be two fma ops
static inline float f32_lerp(float t, float a, float b)
{
  return fmaf(t,b,-fmaf(t,a,-a));
}

static const f32_pair_t f32_up_pi = {.h=0x1.921fb6p1, .l=-0x1.777a5cp-24};

// return RN(x*p) where p is unevaluated pair
static inline float f32_up_mul(const f32_pair_t* const p, float x) 
{
  return fmaf(x, p->h, x*p->l);
}

// compute a*b+c
static inline float f32_up_madd(const f32_pair_t* const a, float b, float c)
{
  return fmaf(a->h, b, fmaf(a->l, b, c));
}

static inline float f32_mul_pi(float x)
{
  return f32_up_mul(&f32_up_pi,x);
}


// (a*b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow. 
static inline void f32_2mul(f32_pair_t* p, float a, float b)
{
  float x = a*b;
  float y = fmaf(a,b,-x);
  p->h = x;
  p->l = y;
}

// checks if LSB is clear.
static inline uint32_t f32_is_even(float a)
{
  uint32_t u = f32_to_bits(a);
  return (u & 1);
}

// next representable FP away from zero
// * blindly walks past +/-inf to NaN
static inline float f32_succ(float a)
{
  return f32_from_bits(f32_to_bits(a)+1);
}

// next representable FP toward zero
// * blindly walks past +/-zero to NaN
static inline float f32_pred(float a)
{
  return f32_from_bits(f32_to_bits(a)-1);
}

// walks 'd' ULP away from zero
// * blindly walks past +/-zero to NaN
static inline float f32_walk(float a, int32_t d)
{
  return f32_from_bits(f32_to_bits(a)+d);
}


// rounding unit + lowest bit set
static const float f32_succ_pred_k = 0x1.000002p-24f;

// next representable FP away from zero
// * a is normal and not zero
static inline float f32_nnz_succ(float a)
{
  const float s = f32_succ_pred_k;
  return fmaf(a,s,a);
}

// next representable FP toward zero
// * a is normal and not zero
static inline float f32_nnz_pred(float a)
{
  const float s = -f32_succ_pred_k;
  return fmaf(a,s,a);
}

// next representable FP toward +inf
// * a is normal and not zero
static inline float f32_nnz_next(float a)
{
  const float s = f32_succ_pred_k;
  return fmaf(fabsf(a),s,a);
}

// next representable FP toward -inf
// * a is normal and not zero
static inline float f32_nnz_prev(float a)
{
  const float s = -f32_succ_pred_k;
  return fmaf(fabsf(a),s,a);
}

// ulp(a)
// * |a| != flt_max
static inline float f32_ulp(float a)
{
  float t = fabsf(a);
  return f32_succ(t)-t;
}

// ab+cd
// * r within +/- 3/2 ulp
// * r within +/-   1 ulp, if sign(ab) == sign(cd)
static inline float f32_mma(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b, t);
  return f+e;
}

// ab+cd
// for cascading: result = returned + e
static inline float f32_mma_e(float a, float b, float c, float d, float* e)
{
  float t = c*d;
  float f = fmaf(a,b, t);
  *e = fmaf(c,d,-t);
  return f;
}

// ab-cd
// SEE: f32_mma for error bounds
static inline float f32_mms(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b,-t);
  return f-e;
}

// ab-cd
// for cascading: result = returned + e
static inline float f32_mms_e(float a, float b, float c, float d, float* e)
{
  float t = c*d;
  float f = fmaf(a,b,-t);
  *e = -fmaf(c,d,-t);
  return f;
}

// ab+cd: this version returns same results if {a,b} and {c,d} are swapped. It
// does not improve the error bound.
static inline float f32_mma_c(float a, float b, float c, float d)
{
  FP32_REASSOCIATE_OFF()
  float p0 = a*b;
  float p1 = c*d;
  float e0 = fmaf(a,b,-p0);
  float e1 = fmaf(c,d,-p1);
  float r0 = p0+p1;
  float r1 = e0+e1;
  return r0+r1;
}

//
static inline uint32_t u32_abs(uint32_t x)
{
  return (int32_t)x >= 0 ? x : -x;
}


// ulp distance provided a & b are finite
// and have the same sign
static inline uint32_t f32_ulp_dist_ss(float a, float b)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);
  return u32_abs(ua-ub);
}

// ulp distance for finite a & b
static inline uint32_t f32_ulp_dist(float a, float b)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);

  if ((int32_t)(ub^ua) >= 0)
    return u32_abs(ua-ub);
  
  return ua+ub+f32_sign_bit_k;
}

// a & b are within 'd' ulp of each other
uint32_t f32_within_ulp(float a, float b, uint32_t d)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);

  if ((int32_t)(ub^ua) >= 0)
    return ua-ub+d <= d+d;
  
  return ua+ub+f32_sign_bit_k+d <= d+d;
}

// filtered approximately equal:
uint32_t f32_approx_eq(float a, float b, float absD, float relD)
{
  float d = fabsf(a-b);
  
  if (d <= absD)
    return 1;
  
  a = fabsf(a);
  b = fabsf(b);
  
  float m = (b>a) ? b : a;
  
  if (d <= m * relD)
    return 1;
  
  return 0;
}


// returns true if a >= b and neither are NaN

static inline bool f32_ordered_ge(float a, float b) { FP32_REASSOCIATE_OFF(); return !(a < b); }

// returns true if a > b and neither are NaN
static inline bool f32_ordered_gt(float a, float b) { FP32_REASSOCIATE_OFF(); return !(a <= b); }

// returns true if neither are NaN
static inline bool f32_are_ordered(float a, float b) { return fpclassify(a+b) != FP_NAN; }



// (a+b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline void f32_2sum(f32_pair_t* p, float a, float b)
{
  FP32_REASSOCIATE_OFF();
  float x  = a+b;
  float bp = x-a;
  float ap = x-bp;
  float eb = b-bp;
  float ea = a-ap;
  float y  = ea+eb;
  p->h = x;
  p->l = y;
}

// (a+b) exactly represented by unevaluated pair (h+l)
// * requires |a| >= |b|
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline void f32_fast2sum(f32_pair_t* p, float a, float b)
{
  FP32_REASSOCIATE_OFF();
  float x  = a+b;
  float bp = x-a;
  float y  = b-bp;
  p->h = x;
  p->l = y;
}

#ifdef __cplusplus
extern "C" {
#endif
#endif

