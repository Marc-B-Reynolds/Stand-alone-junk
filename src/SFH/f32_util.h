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

#if defined(__clang__) && (__clang_major__ >= 13)
// future me note: GCC 12 is adding: __builtin_assoc_barrier

#define FP_REASSOCIATE_ON  F32_PRAGMA("clang fp reassociate(on)")
#define FP_REASSOCIATE_OFF F32_PRAGMA("clang fp reassociate(off)")
#else
#define FP_REASSOCIATE_ON
#define FP_REASSOCIATE_OFF
#endif

static const float f32_ulp1 = (1.f/16777216.f); // 0x1.0p-24f


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

// converts input to IEEE bit pattern and XOR's them
static inline uint32_t f32_xor(float a, float b)
{
  return f32_to_bits(a)^f32_to_bits(b);
}

static inline uint32_t f32_sign_bit(float a)
{
  return f32_to_bits(a) & 0x80000000;
}

// to cut some of the pain stupid math errno not being disabled (groan..please do)
static inline float f32_sqrt(float a)
{
  return __builtin_sqrtf(a);
}

// x >=0 ? 1.0 : -1.f
static inline float f32_sgn(float x) { return copysignf(1.f,x); }

/** exact transform functions **/


// (1-t)a + tb
// should be two fma ops
static inline float f32_lerp(float t, float a, float b)
{
  return fmaf(t,b,-fmaf(t,a,-a));
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

// next representable FP away from zero
// * a is normal and not zero
static inline float f32_nnz_succ(float a)
{
  const float s = 0x1.000002p-24f;
  return fmaf(a,s,a);
}

// next representable FP toward zero
// * a is normal and not zero
static inline float f32_nnz_pred(float a)
{
  const float s = -0x1.000002p-24f;
  return fmaf(a,s,a);
}

// next representable FP toward +inf
// * a is normal and not zero
static inline float f32_nnz_next(float a)
{
  const float s = 0x1.000002p-24f;
  return fmaf(fabsf(a),s,a);
}

// next representable FP toward -inf
// * a is normal and not zero
static inline float f32_nnz_prev(float a)
{
  const float s = -0x1.000002p-24f;
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
FP_REASSOCIATE_OFF
static inline float f32_mma_c(float a, float b, float c, float d)
{
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
  
  return ua+ub+0x80000000;
}

// a & b are within 'd' ulp of each other
uint32_t f32_within_ulp(float a, float b, uint32_t d)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);

  if ((int32_t)(ub^ua) >= 0)
    return ua-ub+d <= d+d;
  
  return ua+ub+0x80000000+d <= d+d;
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
FP_REASSOCIATE_OFF
static inline bool f32_ordered_ge(float a, float b) { return !(a < b); }

// returns true if a > b and neither are NaN
FP_REASSOCIATE_OFF
static inline bool f32_ordered_gt(float a, float b) { return !(a <= b); }

// returns true if neither are NaN
static inline bool f32_are_ordered(float a, float b) { return fpclassify(a+b) != FP_NAN; }



// (a+b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
FP_REASSOCIATE_OFF
static inline void f32_2sum(f32_pair_t* p, float a, float b)
{
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
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
FP_REASSOCIATE_OFF
static inline void f32_fast2sum(f32_pair_t* p, float a, float b)
{
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

