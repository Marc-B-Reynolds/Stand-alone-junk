// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2016-2025

#ifndef F32_UTIL_H
#define F32_UTIL_H

#include <stdbool.h>

#ifndef   INTOPS_H
#include "intops.h"
#endif

#define F32_PRAGMA(X) _Pragma(X)

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define F32_INTEL

// evil temp hack
#if !defined(__X86INTRIN_H)
#include <x86intrin.h>
#endif

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
// __FAST_MATH__         // gcc/clang
// __NO_MATH_ERRNO__     // gcc/clang
// __RECIPROCAL_MATH__   // gcc (not clang?)
// __ASSOCIATIVE_MATH__  // gcc (not clang?)
// __FINITE_MATH_ONLY__  // gcc/clang/intel
// __SUPPORT_SNAN__      // don't care atm

// kill usage: pita
#if defined(__clang__) && (__clang_major__ >= 12)
#define FP32_REASSOCIATE_ON()  F32_PRAGMA("clang fp reassociate(on)")
#define FP32_REASSOCIATE_OFF() F32_PRAGMA("clang fp reassociate(off)")
#define FP32_ASSOC_BARRIER(X)  __arithmetic_fence(X)
#define FP32_STRICT_FUNC()     
#elif defined(__GNUC__) &&  (__GNUC__ >= 12)
#define FP32_REASSOCIATE_ON()
#define FP32_REASSOCIATE_OFF()
#define FP32_ASSOC_BARRIER(X)   __builtin_assoc_barrier(X)
#define FP32_STRICT_FUNC()      // nope...think 
#else
#define FP32_REASSOCIATE_ON()
#define FP32_REASSOCIATE_OFF()
#define FP32_ASSOC_BARRIER(X)
#define FP32_STRICT_FUNC()
#endif


// just say no.
#if (defined(__FAST_MATH__)  || (defined(__FINITE_MATH_ONLY__) && (__FINITE_MATH_ONLY__)) || defined(__ASSOCIATIVE_MATH__) || defined(_M_FP_FAST))
#warning "relaxed floating point operations breaks everything"
#define FP32_LOL_NO
#endif

// wrap an intel intrinsic
#define F32_SSE_WRAP(F,X) _mm_cvtss_f32(F(_mm_set_ss(X)))

#define F32_SSE_WRAP_B(F,A,B)  _mm_cvtss_f32(F(_mm_set_ss(A),_mm_set_ss(B)))
#define F32_SSE_WRAP_BI(F,A,B) _mm_cvtsi128_si32(F(_mm_set_ss(A),_mm_set_ss(B)))


typedef union {
  struct {float h,l; };
  struct {float x,y; };
  float f[2];
} f32_pair_t;

static inline f32_pair_t f32_pair(float h, float l)
{
  return (f32_pair_t){.h=h,.l=l};
}


typedef struct { float f; uint32_t u; } f32_u32_tuple_t;

// u = round-off unit = ulp(1)/2
// ulp(1.f)
static const float f32_ulp1         = 0x1.0p-23f;
static const float f32_min_normal   = 0x1.0p-126f;
static const float f32_max_normal   = 0x1.fffffep+127f;
static const float f32_min_denormal = 0x1.0p-149f;
static const float f32_nan          = (0.f/0.f);
static const float f32_inf          = (1.f/0.f);

static const uint32_t f32_sign_bit_k = UINT32_C(0x80000000);
static const uint32_t f32_mag_bits_k = UINT32_C(0x007fffff);
static const uint32_t f32_exp_bits_k = UINT32_C(0x7F800000);
static const uint32_t f32_one_bits_k = UINT32_C(0x3f800000);



// rounding unit + lowest bit set
static const float f32_succ_pred_k = 0x1.000002p-24f;

// extended precision constants section
// SEE: https://marc-b-reynolds.github.io/math/2020/01/09/ConstAddMul.html (and references)

// extended precision multiplicative constants as unevaluate pairs: {RN(K) + RN(K-RN(K))}
// product: Kx = fmaf(H,x,L*x) -> f32_up_mul(K,x)
// K_i = 1/K  (e.g. pi_i = 1/pi)
// sollya script: src/Sollya/mulk.sollya
static const f32_pair_t f32_mul_k_pi      = {.h = 0x1.921fb6p1f,  .l=-0x1.777a5cp-24f};
static const f32_pair_t f32_mul_k_pi_i    = {.h = 0x1.45f306p-2f, .l= 0x1.b9391p-27f};
static const f32_pair_t f32_mul_k_log2    = {.h = 0x1.62e43p-1f,  .l=-0x1.05c61p-29f};
static const f32_pair_t f32_mul_k_log2_i  = {.h = 0x1.715476p0f,  .l= 0x1.4ae0cp-26f};
static const f32_pair_t f32_mul_k_log10   = {.h = 0x1.26bb1cp1f,  .l=-0x1.12aabap-25f};
static const f32_pair_t f32_mul_k_log10_i = {.h = 0x1.bcb7b2p-2f, .l=-0x1.5b235ep-27f};
static const f32_pair_t f32_mul_k_e       = {.h = 0x1.5bf0a8p1f,  .l= 0x1.628aeep-24f};
static const f32_pair_t f32_mul_k_e_i     = {.h = 0x1.78b564p-2f, .l=-0x1.3a621ap-27f};

static const f32_pair_t f32_mul_k_3_i     = {.h = 0x1.555556p-2f, .l=-0x1.555556p-27f};


// single word precision "helpers"
static const float f32_pi      = 0x1.921fb6p1f;
static const float f32_half_pi = 0.5f*0x1.921fb6p1f;


// extended precision additive constants as unevaluate pairs:
// K + x = fma(H,L,x)
static const f32_pair_t f32_mk_pi = {.h = (float)(61*256661), .l= (float)(13*73*14879)*0x1.0p-46f};


static inline uint32_t f32_to_bits(float x)
{
  uint32_t u; memcpy(&u, &x, 4); return u;
}

static inline float f32_from_bits(uint32_t x)
{
  float f; memcpy(&f, &x, 4); return f;
}

// convert inputs to IEEE bit pattern and XOR's them
static inline uint32_t f32_xor_to_bits(float a, float b)
{
  return f32_to_bits(a)^f32_to_bits(b);
}

static inline float f32_xor(float a, float b)
{
  return f32_from_bits(f32_xor_to_bits(a,b));
}


// if 'v' is float and 's' is all clear (except sign bit)
// return copysign(1.f,s)*v
static inline float f32_mulsign(float v, uint32_t s)
{
  return f32_from_bits(f32_to_bits(v)^s);
}

static inline uint32_t f32_sign_bit(float a)
{
  return f32_to_bits(a) & f32_sign_bit_k;
}

// all set bits if x is negative (sign bit set), otherwise zero
static inline uint32_t f32_sign_mask(float x)
{
  return sgn_mask_u32(f32_to_bits(x));
}

// widened to 64-bit version of previous
static inline uint64_t f32_sign_mask_u64(float x)
{
  uint32_t ix = f32_to_bits(x);
  return (uint64_t)(((int64_t)((int32_t)ix))>>31);
}

// returns 'c' if 'cond' is less than zero. otherwise zero
static inline float f32_sign_select1(float c, float cond)
{
#if defined(F64_SIGN_SELECT_VIA_MASK)
  // NOTE: opposite result for sNaN
  uint32_t m = sgn_mask_u32(f32_to_bits(sx));
  return f32_from_bits(m & f32_to_bits(c));
#else
  return (cond < 0.f) ? c : 0.f;
#endif
}

// 'a' if cond is less than zero, otherwise 'b'
static inline float f32_sign_select(float a, float b, float cond)
{
#if defined(F64_SIGN_SELECT_VIA_MASK)
  // NOTE: opposite result for sNaN
  uint32_t m  = f32_sign_mask(cond);
  uint32_t ia = f32_to_bits(a);
  uint32_t ib = f32_to_bits(b);
  uint32_t ir = (ia & m)|(ib & ~m);
  return f32_from_bits(ir);
#else
  return (cond < 0.f) ? a : b;
#endif  
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
  // do nothing for warning ATM
#endif  
}

// 1/sqrt(a) : avoid spurious overflow choice
// SEE: https://gist.github.com/Marc-B-Reynolds/9fb24a7a4ee915e6e973bf9f4d08c404
static inline float f32_rsqrt(float a) { return 1.f/f32_sqrt(a); }

// promote/demote double is correctly rounded (full domain)
static inline float f32_rsqrt_cr(float x)
{
  return (float)sqrt(1.0/(double)x);
}

// min/max : returns non-NaN result if one is NaN
static inline float f32_min(float a, float b)  { return fminf(a,b); }
static inline float f32_max(float a, float b)  { return fmaxf(a,b); }

// Intel min/max ops returns second source if either are NaN so the
// previous defs are 3 op sequences and the following are 1 op:

// min/max : returns first if either are NaN
static inline float f32_min1(float a, float b) { return !(a > b) ? a : b; }
static inline float f32_max1(float a, float b) { return !(a < b) ? a : b; }

// 2 ops on ARM & Intel. Returns x if both are NaN
static inline float f32_clamp(float x, float lo, float hi)
{
#if defined(F32_INTEL)
  return f32_min1(f32_max1(x,lo),hi);
#else
  return f32_min(f32_max(x,lo),hi);
#endif  
}


// all seem fine on all modernish targets/compilers
static inline float f32_ceil(float x)     { return ceilf(x);  }
static inline float f32_floor(float x)    { return floorf(x); }
static inline float f32_trunc(float x)    { return truncf(x); }
static inline float f32_fract(float x)    { return x - f32_trunc(x); }

// intel doesn't have as a op. computed as
//   truncf(x + copysignf(0x1.fffffep-2,x))  : constant = 0.5-ulp(0.5)
static inline float f32_round(float x)    { return roundf(x); }

// directed rounding aliases
// cm=current mode, tz=toward zero, pi=positive inf, ni=negative inf
static inline float f32_round_cm(float x)  { return nearbyintf(x); }
static inline float f32_round_tz(float x)  { return f32_trunc(x);  }
static inline float f32_round_pi(float x)  { return f32_ceil(x);   }
static inline float f32_round_ni(float x)  { return f32_floor(x);  }
static inline float f32_round_na(float x)  { return f32_round(x);  }

// round to nearest (ties to even) aka banker's rounding. default mode
// so 'f32_round_cm' produces same if mode hasn't been changed.
#if defined(F32_INTEL)
static inline float f32_round_ne(float x)
{
#if defined(F32_INTEL)
  __m128 v = _mm_set_ss(x);
  __m128 r = _mm_round_ss(v,v, _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC);
  
  return _mm_cvtss_f32(r);
#elif defined(__ARM_FEATURE_DIRECTED_ROUNDING)
  return __rintnf(x);
#else
  // do nothing for warning ATM
#endif  
}
#endif

// x >=0 ? 1.0 : -1.f
static inline float f32_sgn(float x) { return copysignf(1.f,x); }

/** exact transform functions **/


// (1-t)a + tb
// should be two fma ops
static inline float f32_lerp(float a, float b, float t)
{
  return fmaf(t,b,-fmaf(t,a,-a));
}


// exact product: ab (provided no over/underflow)
// h = RN(ab), l=RN(ab-RN(ab) -> ab = (h+l)
static inline f32_pair_t f32_2mul(float a, float b)
{
  float t = a*b;
  float e = fmaf(a,b,-t);
  return f32_pair(t,e);
}

// return RN(x*p) where p is unevaluated pair
static inline float f32_up_mul(f32_pair_t const p, float x) 
{
  return fmaf(x, p.h, x*p.l);
}

// compute a*b+c
static inline float f32_up_madd(f32_pair_t const a, float b, float c)
{
  return fmaf(a.h, b, fmaf(a.l, b, c));
}

// pi*x (extended precision)
static inline float f32_mul_pi(float x)
{
  return f32_up_mul(f32_mul_k_pi,x);
}

// x + pi  (FMA variant)
static inline float f32_add_pi(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x);
}

// x + pi/2  (FMA variant)
static inline float f32_add_half_pi(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = 0.5f*f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x);
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
  return f32_from_bits(f32_to_bits(a)+(uint32_t)d);
}

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
static inline uint32_t f32_within_ulp(float a, float b, uint32_t d)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);

  if ((int32_t)(ub^ua) >= 0)
    return ua-ub+d <= d+d;
  
  return ua+ub+f32_sign_bit_k+d <= d+d;
}

// filtered approximately equal:
static inline uint32_t f32_approx_eq(float a, float b, float absD, float relD)
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

// returns true if neither are NaN
static inline bool f32_are_ordered(float a, float b)
{ 
  FP32_REASSOCIATE_OFF();
  return (a==a) && (b==b);
}

// returns true if a >= b or neither are NaN
static inline bool f32_ge_or_unordered(float a, float b)
{
  FP32_REASSOCIATE_OFF();
  return !(a < b);
}

// returns true if a > b or neither are NaN
static inline bool f32_gt_or_unordered(float a, float b)
{
  FP32_REASSOCIATE_OFF();
  return !(a <= b);
}

// (a+b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline f32_pair_t f32_2sum(float a, float b)
{
  FP32_REASSOCIATE_OFF();
  float x  = (a+b);
  float bp = (x-a);
  float ap = (x-bp);
  float eb = (b-bp);
  float ea = (a-ap);
  float y  = (ea+eb);
  return f32_pair(x,y);
}

// (a+b) exactly represented by unevaluated pair (h+l)
// * requires |a| >= |b|
// * |l| <= ulp(h)/2
// * provided a+b does not overflow
// * fastmath breaks me
static inline f32_pair_t f32_fast2sum(float a, float b)
{
  FP32_REASSOCIATE_OFF();
  float x  = (a+b);
  float bp = (x-a);
  float y  = (b-bp);
  return f32_pair(x,y);
}

#endif

