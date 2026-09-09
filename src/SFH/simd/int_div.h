// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2025-2026
// Public Domain under http://unlicense.org, see link for details.

// from spot checking both GCC & clang are doing well with constant divisor
// this is a quick hack. like 32 bit should (probably) wrapper the
// promote and convert steps.

// change to splitting for all sizes? 

// use workarounds if there's no native FP/unsigned conversions
#if defined(__x86_64__) && !defined(__AVX512F__)
#define SIMD_FP_CONVERT_SIGNED
#endif

// simply using the divide operator is a disaster on both if
// divisor isn't a constant.

// i8x32_t vdiv_i8x32(i8x32_t a, i8x32_t b) { return a/b; }

static inline i32x2_t div_i32x2_v(i32x2_t a, i32x2_t b)
{
  f64x2_t fa = __builtin_convertvector(a,f64x2_t);
  f64x2_t fb = __builtin_convertvector(b,f64x2_t);
  f64x2_t fq = fa/fb;
  return __builtin_convertvector(fq,i32x2_t);
}

static inline u32x2_t div_u32x2_v(u32x2_t a, u32x2_t b)
{
#if !defined(SIMD_FP_CONVERT_SIGNED)  
  f64x2_t fa = __builtin_convertvector(a,f64x2_t);
  f64x2_t fb = __builtin_convertvector(b,f64x2_t);
  f64x2_t fq = fa/fb;
  return __builtin_convertvector(fq,u32x2_t);
#else
  // GCC & clang: both expand very poorly
  i64x2_t ia = __builtin_convertvector(a, i64x2_t);
  i64x2_t ib = __builtin_convertvector(b, i64x2_t);
  f64x2_t fa = __builtin_convertvector(ia,f64x2_t);
  f64x2_t fb = __builtin_convertvector(ib,f64x2_t);
  f64x2_t fq = fa/fb;
  return __builtin_convertvector(fq, u32x2_t);
#endif
}

static inline i32x4_t div_i32x4_v(i32x4_t a, i32x4_t b)
{
  // GCC: ARM is poor (16.1.0) does a split in memory
  f64x4_t fa = __builtin_convertvector(a,f64x4_t);
  f64x4_t fb = __builtin_convertvector(b,f64x4_t);
  f64x4_t fq = fa/fb;
  return __builtin_convertvector(fq,i32x4_t);
}

static inline u32x4_t div_u32x4_v(u32x4_t a, u32x4_t b)
{
#if !defined(SIMD_FP_CONVERT_SIGNED)  
  f64x4_t fa = __builtin_convertvector(a,f64x4_t);
  f64x4_t fb = __builtin_convertvector(b,f64x4_t);
  f64x4_t fq = fa/fb;
  return __builtin_convertvector(fq,u32x4_t);
#else
  // GCC: poor intel & ARM (16.1.0)
  i64x4_t ia = __builtin_convertvector(a, i64x4_t);
  i64x4_t ib = __builtin_convertvector(b, i64x4_t);
  f64x4_t fa = __builtin_convertvector(ia,f64x4_t);
  f64x4_t fb = __builtin_convertvector(ib,f64x4_t);
  f64x4_t fq = fa/fb;
  return __builtin_convertvector(fq, u32x4_t);
#endif
}

static inline i32x8_t div_i32x8_v(i32x8_t a, i32x8_t b)
{
  // GCC: poor ARM (16.1.0)
  f64x8_t fa = __builtin_convertvector(a,f64x8_t);
  f64x8_t fb = __builtin_convertvector(b,f64x8_t);
  f64x8_t fq = fa/fb;
  return __builtin_convertvector(fq,i32x8_t);
}
