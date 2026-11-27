// compact junk for testing ARM versions

// Copyright (c) 2017-2024, Arm Limited.
// SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception

#pragma once

#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"

#ifdef __GNUC__
# if !defined (_WIN32)
#   define HIDDEN __attribute__ ((__visibility__ ("hidden")))
# else
#   define HIDDEN
# endif
# define NOINLINE __attribute__ ((noinline))
# define UNUSED __attribute__ ((unused))
# define ALIGN(X) __attribute__ ((__aligned__(X)))
# define likely(x) __builtin_expect (!!(x), 1)
# define unlikely(x) __builtin_expect (x, 0)
# if __GNUC__ >= 9
#   define attribute_copy(f) __attribute__ ((copy (f)))
# else
#   define attribute_copy(f)
# endif
# define strong_alias(f, a) \
  extern __typeof (f) a __attribute__ ((alias (#f))) attribute_copy (f);
# define hidden_alias(f, a) \
  extern __typeof (f) a __attribute__ ((alias (#f), visibility ("hidden"))) \
  attribute_copy (f);
#else
# define HIDDEN
# define NOINLINE
# define UNUSED
# define ALIGN(X)
# define likely(x) (x)
# define unlikely(x) (x)
#endif

#define with_errnof(x, e) (x)

#if __aarch64__ && __GNUC__
/* Prevent the optimization of a floating-point expression.  */
inline float
opt_barrier_float (float x)
{
  __asm__ __volatile__ ("" : "+w" (x));
  return x;
}
#else
inline float
opt_barrier_float (float x)
{
  volatile float y = x;
  return y;
}
#endif

static inline uint32_t asuint(float x)     { return f32_to_bits(x);   }
static inline float    asfloat(uint32_t x) { return f32_from_bits(x); }

static inline float
eval_as_float(double x)
{
  return (float)x;
}

static inline double
eval_as_double(double x)
{
  return x;
}

inline float
__math_divzerof (uint32_t sign)
{
  float y = opt_barrier_float (sign ? -1.0f : 1.0f) / 0.0f;
  return with_errnof (y, ERANGE);
}

inline float
__math_invalidf(float x)
{
  float y = (x - x) / (x - x);
  return isnan (x) ? y : with_errnof (y, EDOM);
}
