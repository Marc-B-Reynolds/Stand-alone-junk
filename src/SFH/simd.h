// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once

// WARNING: This is a "you might puke in your mouth" macro crimes
//
// Single header file "library" of generic SIMD & SIMD/scalar
// operations (float-point and integer). Attempt at less eye
// sore for the combinatorial number of explicit functions
// than is otherwise required.
//
// Requires C11 and common extensions supported by GCC & clang
// including but not limited to:
// • vector_size attribute (makes type defines as ext_vector_type
//   instead if available but doesn't internally use any of the
//   superset functionality)
//   SEE: 1) 
// • expression-statements
// • typeof     (standardized in C23)
// • __VA_OPT__ (standardized in C23)
// • __builtin_{constant_p, types_compatible_p, classify_type}
// • __builtin_choose_expression
//
// Assumptions:
// • target has hardware FMA
// • user has disabled: math-errno and trapping-math (if GCC
//   it will nag the user and set them via pragmas)
//
// Usage: include this header (+options):
// • define `SIMD_SPECIALIZE` 
//
// Credits:
// • David Mazières: SIMD_MAP

#define SIMD_H

#if defined(__GNUC__)

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#if !defined(__clang__)
#pragma GCC push_options

// passive agressive compiler options checking/setting

// clang doesn't do the macro
#if !defined(__NO_MATH_ERRNO__)
#warning "setting `-fno-math-errno` for you.  You're welcome."
#pragma GCC optimize ("no-math-errno")
#endif

// disabled by default in clang and it doesn't do the macro
#if !defined(__NO_TRAPPING_MATH__)
#warning "setting `-no-trapping-math` for you. (stern eyes)"
#pragma GCC optimize ("no-trapping-math")
#endif

#endif


// UB free type punning
#ifndef type_pun
#define type_pun(X,TYPE) ({			    \
 static_assert(sizeof(X) == sizeof(TYPE),"size mismatch"); \
  typeof(X) __x = (X);                              \
  TYPE __d;                                         \
  memcpy(&__d, &__x, sizeof(TYPE));                 \
  __d;                                              \
})
#endif


// returns the i^th element of X (vector/scalar). UB
// if it doesn't exist.
#define simd_elem(X,I) (simd_select_vs(X,X,&X)[I])

// number of elements in X (including scalars)
#define simd_dim(X)    (sizeof(X)/sizeof(simd_elem(X,0)))


// returns a vector of same type as input 'X' where
// each component is: r_i = f(x_i, ...)
#define simd_component_map(f,X,...) ({          \
  typeof(X) _x = X;                             \
  for(size_t i=0; i<simd_dim(_x); i++)          \
    _x[i] = f(_x[i] __VA_OPT__(,) __VA_ARGS__); \
  _x;                                           \
})

// previous specialized to binary32/binary64
#define simd_fp_component_map(F,D,X,...) ({   \
  typeof(X) _x = X;                           \
  for(size_t i=0; i<simd_dim(_x); i++) {      \
    _x[i] = _Generic((_x[i]),                 \
      float:   F,                             \
      default: D)                             \
    (_x[i]  __VA_OPT__(,) __VA_ARGS__);       \
  }                                           \
  _x;                                         \
})

// for generic 'libm' style float unary & binary function defs:
// binary64 is NAME and binary32 is NAME suffixed with 'f'
#define simd_fp_std_unary(NAME,X)    simd_fp_component_map(NAME ## f, NAME, X)
#define simd_fp_std_binary(NAME,X,Y) simd_fp_component_map(NAME ## f, NAME, X,Y[i])


// David Mazières' modernized X macro (SIMD_MAP)
// https://www.scs.stanford.edu/~dm/blog/va-opt.html
#define SIMD_PARENS ()

// increased the number of rescans from 256 to 1024
#define SIMD_EXPAND(arg)  SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(arg))))
#define SIMD_EXPAND1(arg) SIMD_EXPAND2(SIMD_EXPAND2(SIMD_EXPAND2(SIMD_EXPAND2(arg))))
#define SIMD_EXPAND2(arg) SIMD_EXPAND3(SIMD_EXPAND3(SIMD_EXPAND3(SIMD_EXPAND3(arg))))
#define SIMD_EXPAND3(arg) SIMD_EXPAND4(SIMD_EXPAND4(SIMD_EXPAND4(SIMD_EXPAND4(arg))))
#define SIMD_EXPAND4(arg) SIMD_EXPAND5(SIMD_EXPAND5(SIMD_EXPAND5(SIMD_EXPAND5(arg))))
#define SIMD_EXPAND5(arg) arg

// map `macro` across all listed varargs (...)
// SIMD_MAP(F,0,1,2,...,N) -> F(0) F(1) F(2) .. F(N)
#define SIMD_MAP(macro, ...) \
  __VA_OPT__(SIMD_EXPAND(SIMD_MAP_HELPER(macro, __VA_ARGS__)))

#define SIMD_MAP_HELPER(macro, a1, ...)                    \
  macro(a1)                                                \
  __VA_OPT__(SIMD_MAP_AGAIN SIMD_PARENS (macro, __VA_ARGS__))

#define SIMD_MAP_AGAIN() SIMD_MAP_HELPER


// map `macro` across all listed varargs (...)
// SIMD_MAP_PEAL(F,P,0,1,2,...,N) -> F(P,0) F(P,1) F(P,2) .. F(P,N)
#define SIMD_MAP_PEAL(macro, P0, ...)                                   \
  __VA_OPT__(SIMD_EXPAND(SIMD_MAP_PEAL_HELPER(macro,P0,__VA_ARGS__)))

#define SIMD_MAP_PEAL_HELPER(macro,P0,P1, ...)                          \
  macro(P0,P1)                                                          \
  __VA_OPT__(SIMD_MAP_PEAL_AGAIN SIMD_PARENS (macro,P0,__VA_ARGS__))

#define SIMD_MAP_PEAL_AGAIN() SIMD_MAP_PEAL_HELPER

// expands to: (A)+(B)+...+(N)
#define SIMD_SUM(A,...) (A)__VA_OPT__(SIMD_MAP(+,__VA_ARGS__))


// if `ext_vector_type` is available use that instead of `vector_size`
// not used internal but allows users to access the added functionality
// such as shader "dot" notation.
#if __has_attribute(ext_vector_type)

// 64-bit packages
typedef float    f32x2_t  __attribute__ ((ext_vector_type(2)));
typedef int8_t   i8x8_t   __attribute__ ((ext_vector_type(8)));
typedef int16_t  i16x4_t  __attribute__ ((ext_vector_type(4)));
typedef int32_t  i32x2_t  __attribute__ ((ext_vector_type(2)));
typedef uint8_t  u8x8_t   __attribute__ ((ext_vector_type(8)));
typedef uint16_t u16x4_t  __attribute__ ((ext_vector_type(4)));
typedef uint32_t u32x2_t  __attribute__ ((ext_vector_type(2)));

// 128-bit packages
typedef float    f32x4_t  __attribute__ ((ext_vector_type(4)));
typedef double   f64x2_t  __attribute__ ((ext_vector_type(2)));
typedef int8_t   i8x16_t  __attribute__ ((ext_vector_type(16)));
typedef int16_t  i16x8_t  __attribute__ ((ext_vector_type(8)));
typedef int32_t  i32x4_t  __attribute__ ((ext_vector_type(4)));
typedef int64_t  i64x2_t  __attribute__ ((ext_vector_type(2)));
typedef uint8_t  u8x16_t  __attribute__ ((ext_vector_type(16)));
typedef uint16_t u16x8_t  __attribute__ ((ext_vector_type(8)));
typedef uint32_t u32x4_t  __attribute__ ((ext_vector_type(4)));
typedef uint64_t u64x2_t  __attribute__ ((ext_vector_type(2)));

// 256-bit packages
typedef float    f32x8_t  __attribute__ ((ext_vector_type(8)));
typedef double   f64x4_t  __attribute__ ((ext_vector_type(4)));
typedef int8_t   i8x32_t  __attribute__ ((ext_vector_type(32)));
typedef int16_t  i16x16_t __attribute__ ((ext_vector_type(16)));
typedef int32_t  i32x8_t  __attribute__ ((ext_vector_type(8)));
typedef int64_t  i64x4_t  __attribute__ ((ext_vector_type(4)));
typedef uint8_t  u8x32_t  __attribute__ ((ext_vector_type(32)));
typedef uint16_t u16x16_t __attribute__ ((ext_vector_type(16)));
typedef uint32_t u32x8_t  __attribute__ ((ext_vector_type(8)));
typedef uint64_t u64x4_t  __attribute__ ((ext_vector_type(4)));

// 512-bit packages (makes the defines even if 'disabled'.)
typedef float    f32x16_t __attribute__ ((ext_vector_type(16)));
typedef double   f64x8_t  __attribute__ ((ext_vector_type(8)));
typedef int8_t   i8x64_t  __attribute__ ((ext_vector_type(64)));
typedef int16_t  i16x32_t __attribute__ ((ext_vector_type(32)));
typedef int32_t  i32x16_t __attribute__ ((ext_vector_type(16)));
typedef int64_t  i64x8_t  __attribute__ ((ext_vector_type(8)));
typedef uint8_t  u8x64_t  __attribute__ ((ext_vector_type(64)));
typedef uint16_t u16x32_t __attribute__ ((ext_vector_type(32)));
typedef uint32_t u32x16_t __attribute__ ((ext_vector_type(16)));
typedef uint64_t u64x8_t  __attribute__ ((ext_vector_type(8)));

#else

// 64-bit packages
typedef float    f32x2_t  __attribute__ ((vector_size(8)));
typedef int8_t   i8x8_t   __attribute__ ((vector_size(8)));
typedef int16_t  i16x4_t  __attribute__ ((vector_size(8)));
typedef int32_t  i32x2_t  __attribute__ ((vector_size(8)));
typedef uint8_t  u8x8_t   __attribute__ ((vector_size(8)));
typedef uint16_t u16x4_t  __attribute__ ((vector_size(8)));
typedef uint32_t u32x2_t  __attribute__ ((vector_size(8)));

// 128-bit packages
typedef float    f32x4_t  __attribute__ ((vector_size(16)));
typedef double   f64x2_t  __attribute__ ((vector_size(16)));
typedef int8_t   i8x16_t  __attribute__ ((vector_size(16)));
typedef int16_t  i16x8_t  __attribute__ ((vector_size(16)));
typedef int32_t  i32x4_t  __attribute__ ((vector_size(16)));
typedef int64_t  i64x2_t  __attribute__ ((vector_size(16)));
typedef uint8_t  u8x16_t  __attribute__ ((vector_size(16)));
typedef uint16_t u16x8_t  __attribute__ ((vector_size(16)));
typedef uint32_t u32x4_t  __attribute__ ((vector_size(16)));
typedef uint64_t u64x2_t  __attribute__ ((vector_size(16)));

// 256-bit packages
typedef float    f32x8_t  __attribute__ ((vector_size(32)));
typedef double   f64x4_t  __attribute__ ((vector_size(32)));
typedef int8_t   i8x32_t  __attribute__ ((vector_size(32)));
typedef int16_t  i16x16_t __attribute__ ((vector_size(32)));
typedef int32_t  i32x8_t  __attribute__ ((vector_size(32)));
typedef int64_t  i64x4_t  __attribute__ ((vector_size(32)));
typedef uint8_t  u8x32_t  __attribute__ ((vector_size(32)));
typedef uint16_t u16x16_t __attribute__ ((vector_size(32)));
typedef uint32_t u32x8_t  __attribute__ ((vector_size(32)));
typedef uint64_t u64x4_t  __attribute__ ((vector_size(32)));

// 512-bit packages (make the defines even if 'disabled')
typedef float    f32x16_t __attribute__ ((vector_size(64)));
typedef double   f64x8_t  __attribute__ ((vector_size(64)));
typedef int8_t   i8x64_t  __attribute__ ((vector_size(64)));
typedef int16_t  i16x32_t __attribute__ ((vector_size(64)));
typedef int32_t  i32x16_t __attribute__ ((vector_size(64)));
typedef int64_t  i64x8_t  __attribute__ ((vector_size(64)));
typedef uint8_t  u8x64_t  __attribute__ ((vector_size(64)));
typedef uint16_t u16x32_t __attribute__ ((vector_size(64)));
typedef uint32_t u32x16_t __attribute__ ((vector_size(64)));
typedef uint64_t u64x8_t  __attribute__ ((vector_size(64)));

#endif

// defs for SIMD_MAP et al.
#if defined(SIMD_ENABLE_512)
#define SIMD_U8_X   u8x8,u8x16,u8x32,u8x64
#define SIMD_I8_X   i8x8,i8x16,i8x32,i8x64
#define SIMD_U16_X u16x4,u16x8,u16x16,u16x32
#define SIMD_I16_X i16x4,i16x8,i16x16,i16x32
#define SIMD_U32_X u32x2,u32x4,u32x8,u32x16
#define SIMD_I32_X i32x2,i32x4,i32x8,i32x16
#define SIMD_F32_X f32x2,f32x4,f32x8,f32x16
#define SIMD_U64_X u64x2,u64x4,u64x8
#define SIMD_I64_X i64x2,i64x4,i64x8
#define SIMD_F64_X f64x2,f64x4,f64x8
#else

#define SIMD_U8_X   u8x8,u8x16,u8x32
#define SIMD_I8_X   i8x8,i8x16,i8x32
#define SIMD_U16_X u16x4,u16x8,u16x16
#define SIMD_I16_X i16x4,i16x8,i16x16
#define SIMD_U32_X u32x2,u32x4,u32x8
#define SIMD_I32_X i32x2,i32x4,i32x8
#define SIMD_F32_X f32x2,f32x4,f32x8
#define SIMD_U64_X u64x2,u64x4
#define SIMD_I64_X i64x2,i64x4
#define SIMD_F64_X f64x2,f64x4
#endif

// floating point collection
#define SIMD_FP_X  SIMD_F32_X,SIMD_F64_X
#define SIMD_UI_X  SIMD_U32_X,SIMD_U64_X,
#define SIMD_SI_X  SIMD_I32_X,SIMD_I64_X,


//*******************************************************
// identical register width type-puns. It'd be nice
// to have a cleaner (unknown width) versions...
// but anyway this has to jump through hoops since
// all the expressions must be syntactically legal
// for all inputs. Hopefully it's not the "worst" NOP
// you've ever seen.

// self note: doesn't these require adding may_alias
// to the types? thinky-thinky

#define simd_fp_to_ui(X) ({         \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    f32x2_t: ((u32x2_t*) (&_x))[0], \
    f32x4_t: ((u32x4_t*) (&_x))[0], \
    f32x8_t: ((u32x8_t*) (&_x))[0], \
    f32x16_t:((u32x16_t*)(&_x))[0], \
    f64x2_t: ((u64x2_t*) (&_x))[0], \
    f64x4_t: ((u64x4_t*) (&_x))[0], \
    f64x8_t: ((u64x8_t*) (&_x))[0], \
    default: (void*)0);             \
  })

#define simd_ui_to_fp(X) ({         \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    u32x2_t: ((f32x2_t*) (&_x))[0], \
    u32x4_t: ((f32x4_t*) (&_x))[0], \
    u32x8_t: ((f32x8_t*) (&_x))[0], \
    u32x16_t:((f32x16_t*)(&_x))[0], \
    u64x2_t: ((f64x2_t*) (&_x))[0], \
    u64x4_t: ((f64x4_t*) (&_x))[0], \
    u64x8_t: ((f64x8_t*) (&_x))[0], \
    default: (void*)0);             \
  })

#define simd_si_to_ui(X) ({         \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    u32x2_t: ((i32x2_t*) (&_x))[0], \
    u32x4_t: ((i32x4_t*) (&_x))[0], \
    u32x8_t: ((i32x8_t*) (&_x))[0], \
    u32x16_t:((i32x16_t*)(&_x))[0], \
    u64x2_t: ((i64x2_t*) (&_x))[0], \
    u64x4_t: ((i64x4_t*) (&_x))[0], \
    u64x8_t: ((i64x8_t*) (&_x))[0], \
    default: (void*)0);             \
  })

#define simd_ui_to_si(X) ({         \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    i32x2_t: ((u32x2_t*) (&_x))[0], \
    i32x4_t: ((u32x4_t*) (&_x))[0], \
    i32x8_t: ((u32x8_t*) (&_x))[0], \
    i32x16_t:((u32x16_t*)(&_x))[0], \
    i64x2_t: ((u64x2_t*) (&_x))[0], \
    i64x4_t: ((u64x4_t*) (&_x))[0], \
    i64x8_t: ((u64x8_t*) (&_x))[0], \
    default: (void*)0);             \
  })


#define simd_fxor(A,B) simd_ui_to_fp(simd_fp_to_ui(A) ^ simd_fp_to_ui(B))
#define simd_fand(A,B) simd_ui_to_fp(simd_fp_to_ui(A) & simd_fp_to_ui(B))

//*******************************************************


// workers for macro expansions
#define simd_is_const(a)  __builtin_constant_p(a)

// `true` if a is a `vector_size` or `ext_vector_type`
#define simd_is_simd_type(a)  (__builtin_classify_type(a) == 19)

#define simd_is_type(a,T) __builtin_types_compatible_p(typeof(a), T)
#define simd_is_i32(a)    simd_is_type(a, int32_t)
#define simd_is_i64(a)    simd_is_type(a, int64_t)
#define simd_is_u32(a)    simd_is_type(a, uint32_t)
#define simd_is_u64(a)    simd_is_type(a, uint64_t)
#define simd_is_f32(a)    simd_is_type(a, float)
#define simd_is_f64(a)    simd_is_type(a, double)

#define simd_is_scalar_fp(a)  (simd_is_f32(a) || simd_is_f64(a))
#define simd_is_scalar_int(a) (simd_is_i32(a) || simd_is_i64(a) || simd_is_u32(a) || simd_is_u64(a))

#define simd_is_same_type(a,b) __builtin_types_compatible_p(typeof(a), typeof(b))

// attempt to make macro barf errors a bit easier to read
#define simd_assert_const(a)       static_assert(simd_is_const(a), "must be constant")
#define simd_assert_same_type(a,b) static_assert(simd_is_same_type(a,b), "must be same type")


// all listed must be scalar types (floating-point or integer)
#define simd_assert_scalar_fp(A,...)  static_assert(simd_is_scalar_fp(A) __VA_OPT__(SIMD_MAP(&& simd_is_scalar_fp,__VA_ARGS__)), "must be same scalar float type")
#define simd_assert_scalar_int(A,...) static_assert(simd_is_scalar_int(A) __VA_OPT__(SIMD_MAP(&& simd_is_scalar_int,__VA_ARGS__)), "must be same scalar integer type")
#define simd_assert_vec(A,...)        static_assert(simd_is_simd_type(A) __VA_OPT__(SIMD_MAP(&& simd_is_simd_type,__VA_ARGS__)), "must be vector type")


// if variable 'X' is a vector type yield 'V' and otherwise 'S'
#define simd_select_vs(X,V,S) __builtin_choose_expr(simd_is_simd_type(X), V, S)

// return the base type of 'A'
#define simd_base_type(A) simd_select_vs(A,A[0],A)

// choose (scalar) fp constant constant when 'N' cast to the given
// precision is correct.
#define simd_fp_const(T,N)               \
     _Generic((simd_base_type(T)),       \
      float:   (float)(N),               \
      default: (double)(N))

// choose (scalar) fp constant constant when they need to be unique
#define simd_fp_const_select(T,F32,F64)  \
     _Generic((simd_base_type(T)),       \
      float:   (F32),                    \
      default: (F64))

// V is:
//   scalar : broadcast 'V' to all elements of type 'T'
//   vector : returns 'V' if it's type is 'T'
// NOTE: subtracting zero is identity function for floating point
#define simd_splat_i(T,V) V-(T){0}


//*******************************************************
// generic to specialized expansion macros (expand function or prototype)
// the expanded name do *NOT* have a `simd_` prefix. They are `name`_`type`

#if   defined(SIMD_INLINE_SPECIALIZE)
#define SIMD_MAKE_UFUN(name,T) static inline T ##_t name ## _ ## T(T ##_t a) { return simd_ ## name(a); }
#define SIMD_MAKE_BFUN(name,T) static inline T ##_t name ## _ ## T(T ##_t a, T ##_t b) { return simd_ ## name(a,b); }
#elif defined(SIMD_IMPLEMENTATION)
#define SIMD_MAKE_UFUN(name,T) T ##_t name ## _ ## T(T ##_t a) { return simd_ ## name(a); }
#define SIMD_MAKE_BFUN(name,T) T ##_t name ## _ ## T(T ##_t a, T ##_t b) { return simd_ ## name(a,b); }
#else
#define SIMD_MAKE_UFUN(name,T) extern T ##_t name ## _ ## T(T ##_t a);
#define SIMD_MAKE_BFUN(name,T) extern T ##_t name ## _ ## T(T ##_t a, T ##_t b);
#endif

//*******************************************************
// generic SIMD libm functionality


#define simd_floor(x) simd_fp_std_unary(floor,x)
#define simd_ceil(x)  simd_fp_std_unary(ceil,x)
#define simd_fabs(x)  simd_fp_std_unary(fabs,x)
#define simd_trunc(x) simd_fp_std_unary(trunc,x)
#define simd_round(x) simd_fp_std_unary(round,x)
#define simd_sqrt(x)  simd_fp_std_unary(sqrt,x)


#if defined(SIMD_SPECIALIZE)
// expand specialized: (example: floor_f32x4)
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, floor, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, ceil,  SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, fabs,  SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, trunc, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, round, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, sqrt,  SIMD_FP_X)
#endif

//*******************************************************
// min/max
//
// the comparision is written this way for floating point which
// will cause the first parameter to be returned if the inputs
// are unordered (one or both are NaN). Doesn't matter for integers.
//
// this differs from the libm functions in C which will return
// the ordered value if only one is NaN.

// scalar macros
#define simd_min_s(A,B) ({ simd_param_2(A,B); !(_a > _b) ? _a : _b; })
#define simd_max_s(A,B) ({ simd_param_2(A,B); !(_a < _b) ? _a : _b; })

// vector macros (B element is awkward because it's directly expanded.
// could make a binary function expander...but meh ATM)

// integer & floating point (with above listed behavior)
#define simd_min(A,B) simd_component_map(simd_min_s,A,simd_elem(B,i));
#define simd_max(A,B) simd_component_map(simd_max_s,A,simd_elem(B,i));

// floating-point C library behavior
// self note: GCC expansions of this explode w/o weaking FP behavior
// (think about this at some point)
#define simd_fmin(a,b) simd_fp_std_binary(fmin,a,b)
#define simd_fmax(a,b) simd_fp_std_binary(fmax,a,b)

#if defined(SIMD_SPECIALIZE)
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, min, SIMD_FP_X);
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, max, SIMD_FP_X);
#endif

//*******************************************************
//

// yields the first parameter that's a vector type (if there is one)
#define simd_first_vt2(A,B)   __builtin_choose_expr(simd_is_simd_type(A),A,B)
#define simd_first_vt3(A,B,C) simd_first_vt2(simd_first_vt2(A,B),C)


// simd_param_{n} helper:
// • `name` is the variable name of the captured 
// • `orig` is the macro parameter
// uses helper `simd_splat_i` which logically passes
// through a vector type and broadcasts a scalar
#define simd_capture_i(name,orig)  typeof(_type) name = simd_splat_i(typeof(_type),orig);


// mixed vector/scalar macro paramenter helper:
// all parameters must be a vector type (T) or
// a scalar of the element type of T. All scalars
// are broadcast to type T (if there are any)
//
// • determine the vector type T (two methods):
//   • `typeof` of the expression of the sum of parameters
//   • `simd_first_vt{n}`: walks the parameters and yields
//      the first vector type (or final if none)
//   (first is nicer to look at, but wavering
//    about which is a "better" tool)
// • `simd_capture_i` uses typeof(_type) is used to pass through vector
//   and parameters, broadcast any scalars, and 
//   capture macro input (prevent side-effects)
// • {_a,_b,...} are the original values with any
//   scalars broadcast to T. (fixed naming scheme)
//
// TODO:
// • add type checking asserts. 

#if 0
#define simd_param_2(A,B)     \
  typeof(A+B) _type = {};     \
  simd_capture_i(_a,A);       \
  simd_capture_i(_b,B);

#define simd_param_3(A,B,C)    \
  typeof(A+B+C) _type = {};    \
  simd_capture_i(_a,A);        \
  simd_capture_i(_b,B);        \
  simd_capture_i(_c,C);
#else
#define simd_param_2(A,B)      \
  typeof(simd_first_vt2(A,B))  \
    _type = {};                \
  simd_capture_i(_a,A);        \
  simd_capture_i(_b,B);

#define simd_param_3(A,B,C)    \
  typeof(simd_first_vt3(A,B,C))\
    _type = {};                \
  simd_capture_i(_a,A);        \
  simd_capture_i(_b,B);        \
  simd_capture_i(_c,C);
#endif


// scalar FMA (generic binary32/binary64)
#define simd_fma_s(A,B,C)            \
  _Generic((A),                      \
    float:   fmaf,                   \
    default: fma)(A,B,C)


// SIMD FMA helper: expand to element count
// and base element type.
// NO SIDE-EFFECT PROTECTION
#define simd_fma_i(A,B,C) ({              \
  for(size_t i=0; i<simd_dim(A); i++) {   \
    simd_elem(A,i) = simd_fma_s(          \
    simd_elem(A,i),                       \
    simd_elem(B,i),                       \
    simd_elem(C,i));                      \
  }                                       \
  A;                                      \
})

// SIMD FMA : R_i = RN(A_i•B_i + C_i)
#define simd_fma(A,B,C) ({     \
  simd_param_3(A,B,C);         \
  simd_fma_i(_a,_b,_c);        \
})


// SIMD LERP: R_i = A_i(1-T_i) + B_i•T_i
// • end-points are exact (2 FMA formulation)
#define simd_lerp(A,B,T) ({                \
  simd_param_3(A,B,T);                     \
  typeof(_type) _r = simd_fms_v(_a,_c,_a); \
  typeof(_type) _f = simd_fma_v(_b,_c,_r); \
  _f;                                      \
})


#if !defined(__clang__)
#pragma GCC reset_options
#endif

#else
#error "requires extended C features"
#endif
