// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#define SFH_SIMD_H  // marker define

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
// • WARNING: weaking floating point (other various fast-math
//   optimization will break some contracts). I'll try to
//   remember to note them in the code.
//
// Usage: include this header (+options):
// • define `SIMD_USE_VECTOR_SIZE` to force using vector_size
//   for types.
// • define `SIMD_SPECIALIZE` to expand functions for types.
//   by default this expands non-inline functions which will
//   be compiled in the file that defines `SIMD_IMPLEMENTATION`
//   If additionally `SIMD_INLINE_SPECIALIZE` is defined
//   these function will instead by expanded into static inline.
//   A number of inline functions are always defined since they
//   back the generic macros. (note: the default means you want
//   to LTO). ALSO: some small functions we be defined static
//   inline even if not requested.
//
// Behavior diffrences:
// • Older versions of clang and GCC (prior to 18/14 respectively)
//   don't classify vector types as `vector_type_class` and
//   return `no_type_class` instead. Although I try to work
//   around this there might be some hic-ups related to this.
// • GCC is more strict than clang about types (make notes)
// • For vector comparison results:
//   vector_size: any same sized type allowed
//   ext_vector_type: signed integer with same number elements
//   so: i32x8_t fcmp(T a, T b) { return a>=b; } is valid for
//       T={f32x8_t,i32x8_t,u32x8_t} for both extensions.
// SEE:
//   https://gcc.gnu.org/onlinedocs/gcc/Vector-Extensions.html
//   https://clang.llvm.org/docs/LanguageExtensions.html#vectors-and-extended-vectors
//
// Credits:
// • David Mazières: SIMD_MAP

// TODO: (there's tons of stuff. just starting some self notes) 
// • #define performs no expansions so any exposed functionality defines
//   should be as lightweigh WRT expansion required as possible.
//   a far amount of effory could be made here.
// • most type checking is missing (and as per previous should possible
//   to disable)
// • use small map expansion macro more (compile time improvement) esp for
//   for logical callsite macros. (repeatin first point)
// • generic macros could be simplifed when `SIMD_SPECIALIZE` is set.
//   tricky though (total macros that need to be "call-site" expanded.
//   humm...thinky, thinky)
//   


// marker define
#define SFH_SIMD_H

#if defined(__GNUC__)

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// do what I can to inspect compiler options. 
// need to rethink about the others (WIP)

#if defined(__FINITE_MATH_ONLY__) && (__FINITE_MATH_ONLY__)
#define SIMD_FINITE_MATH_ENABLED
#endif

#if defined(__clang__)

// TODO: version limit these.
// expression statements are how everything "works"
#pragma GCC diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#pragma GCC diagnostic push

// temp disable some that can occur during expansion
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wvariadic-macro-arguments-omitted"

#else

// GCC specific step-up stuff
#pragma GCC push_options
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// passive agressive compiler options checking/setting

// clang doesn't do the macro. 
#if !defined(__NO_MATH_ERRNO__)
// errno is just a pre IEEE 745 feature that should never be used
// and compilers should have disabled by default 20+ years ago.
#if !defined(SIMD_NO_NAG)
#warning "setting `-fno-math-errno` for you.  You're welcome."
#endif
#pragma GCC optimize ("no-math-errno")
#endif

// disabled by default in clang and it doesn't do the macro
// 
#if !defined(__NO_TRAPPING_MATH__) && (__GNUC__ >= 12)
#if !defined(SIMD_NO_NAG)
// do you have some signal handler for floating point? Didn't think so.
#warning "setting `-no-trapping-math` for you. (stern eyes)"
#endif
#pragma GCC optimize ("no-trapping-math")
#endif

#endif

#if !defined(__has_builtin)
#define __has_builtin(X) 0
#endif

// determine if internally to use to C23 features
// that require -std=c23 (currently only `auto`)
#if (__STDC_VERSION__ >= 202311L) && !defined(SIMD_NO_C23)
#define SIMD_USE_C23
#endif

// determine if to define 512 bit package functionality
#if defined(__AVX512F__) && !defined(SIMD_NO_512)
#define SIMD_ENABLE_512
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

// number of elements in X (must be vector)
#if (__has_builtin(__builtin_vectorelements))
#define simd_dim_v(X) __builtin_vectorelements(X)
#else
#define simd_dim_v(X)  (sizeof(X)/sizeof(X[0]))
#endif

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


//*******************************************************
// David Mazières' modernized X macro (SIMD_MAP)
// https://www.scs.stanford.edu/~dm/blog/va-opt.html
#define SIMD_PARENS ()

#define SIMD_EXPAND(...)  SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(__VA_ARGS__))))
#define SIMD_EXPAND4(...) SIMD_EXPAND3(SIMD_EXPAND3(SIMD_EXPAND3(SIMD_EXPAND3(__VA_ARGS__))))
#define SIMD_EXPAND3(...) SIMD_EXPAND2(SIMD_EXPAND2(SIMD_EXPAND2(SIMD_EXPAND2(__VA_ARGS__))))
#define SIMD_EXPAND2(...) SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(SIMD_EXPAND1(__VA_ARGS__))))
#define SIMD_EXPAND1(...) __VA_ARGS__

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
#define SIMD_MAP_PEAL(macro, ...)                                       \
  __VA_OPT__(SIMD_EXPAND(SIMD_MAP_PEAL_HELPER(macro,__VA_ARGS__)))

#define SIMD_MAP_PEAL_HELPER(macro,P0,P1, ...)                          \
  macro(P0,P1)                                                          \
  __VA_OPT__(SIMD_MAP_PEAL_AGAIN SIMD_PARENS (macro,P0,__VA_ARGS__))

#define SIMD_MAP_PEAL_AGAIN() SIMD_MAP_PEAL_HELPER


//*******************************************************
// A non-rescan expansion (argcount based) limited to [1,6]
// varargs. Lighter weight preprocessing time than SIMD_MAP

#define SIMD_SMAP_APPLY(F, X) F X
#define SIMD_SMAP1(F,A)           SIMD_SMAP_APPLY(F,A)
#define SIMD_SMAP2(F,A,B)         SIMD_SMAP_APPLY(F,A) SIMD_SMAP_APPLY(F,B)
#define SIMD_SMAP3(F,A,B,C)       SIMD_SMAP_APPLY(F,A) SIMD_SMAP_APPLY(F,B) SIMD_SMAP_APPLY(F,C)
#define SIMD_SMAP4(F,A,B,C,D)     SIMD_SMAP_APPLY(F,A) SIMD_SMAP_APPLY(F,B) SIMD_SMAP_APPLY(F,C) SIMD_SMAP_APPLY(F,D)
#define SIMD_SMAP5(F,A,B,C,D,E)   SIMD_SMAP_APPLY(F,A) SIMD_SMAP_APPLY(F,B) SIMD_SMAP_APPLY(F,C) SIMD_SMAP_APPLY(F,D) SIMD_SMAP_APPLY(F,E)
#define SIMD_SMAP6(F,A,B,C,D,E,G) SIMD_SMAP_APPLY(F,A) SIMD_SMAP_APPLY(F,B) SIMD_SMAP_APPLY(F,C) SIMD_SMAP_APPLY(F,D) SIMD_SMAP_APPLY(F,E) SIMD_SMAP_APPLY(F,G)
#define SIMD_SMAP_(_1,_2,_3,_4,_5,_6,NAME,...) NAME

#define SIMD_SMAP(F,...) SIMD_SMAP_(__VA_ARGS__,SIMD_SMAP6,SIMD_SMAP5,SIMD_SMAP4,SIMD_SMAP3,SIMD_SMAP2,SIMD_SMAP1)(F, __VA_ARGS__)


//*******************************************************
// argument count based concat (limited to 8 for one, nest for more)

#define CAT0()
#define CAT1(A) A
#define CAT2(A,B) A##B
#define CAT3(A,B,C) A##B##C
#define CAT4(A,B,C,D) A##B##C##D
#define CAT5(A,B,C,D,E) A##B##C##D##E
#define CAT6(A,B,C,D,E,F) A##B##C##D##E##F
#define CAT7(A,B,C,D,E,F,G) A##B##C##D##E##F##G
#define CAT8(A,B,C,D,E,F,G,H) A##B##C##D##E##F##G##H
#define CAT_(_0,_1,_2,_3,_4,_5,_6,_7,M,...) M
#define CAT(...) CAT_(__VA_ARGS__,CAT8,CAT7,CAT6,CAT5,CAT4,CAT3,CAT2,CAT1,CAT0)(__VA_ARGS__)


//*******************************************************

// expands to: (A)+(B)+...+(N)
#define SIMD_SUM(A,...) (A)__VA_OPT__(SIMD_MAP(+,__VA_ARGS__))


//*******************************************************
// Make the type defs and base support
//
// if `ext_vector_type` is available use that instead (1) of `vector_size`
// not used internal but allows users to access the added functionality
// such as shader "dot" notation. (1: unless disabling)
//
// Full fills out logical register sizes of {64,128,256,512} bits.
// 64 is handy for 2D vectors binary32 (as an example) and just
// make the 512 even if not expanding any explict functions for
// that type.

// define (bit-width,num-elemnts) for register sizes of (64,128,256)
// (and optionally 512)
#if defined(SIMD_ENABLE_512)
//                 | 64  |  128  |  256  | 512  |
#define SIMD_S8_X  ( 8,8),( 8,16),( 8,32),( 8,64)
#define SIMD_S16_X (16,4),(16, 8),(16,16),(16,32)
#define SIMD_S32_X (32,2),(32, 4),(32, 8),(32,16)
#define SIMD_S64_X         (64,2),(64, 4),(64, 8)
#else
#define SIMD_S8_X  ( 8,8),( 8,16),( 8,32)
#define SIMD_S16_X (16,4),(16, 8),(16,16)
#define SIMD_S32_X (32,2),(32, 4),(32, 8)
#define SIMD_S64_X        (64, 2),(64, 4)
#endif

// choose vector attribute type
#if __has_attribute(ext_vector_type) && !defined(SIMD_USE_VECTOR_SIZE)
#define SIMD_MAKE_TYPE_EX(B,N) __attribute__((ext_vector_type(N)))
#else
#define SIMD_MAKE_TYPE_EX(B,N) __attribute__((vector_size(B*N/8)))
#endif


// create typedef for: signed int, unsigned int, binary32, binary64 respectively
#define SIMD_MAKE_TYPE_I(B,N) typedef int##B##_t  CAT(i,B,x,N,_t) SIMD_MAKE_TYPE_EX(B,N);
#define SIMD_MAKE_TYPE_U(B,N) typedef uint##B##_t CAT(u,B,x,N,_t) SIMD_MAKE_TYPE_EX(B,N);
#define SIMD_MAKE_TYPE_F(B,N) typedef float       CAT(f,B,x,N,_t) SIMD_MAKE_TYPE_EX(B,N);
#define SIMD_MAKE_TYPE_D(B,N) typedef double      CAT(f,B,x,N,_t) SIMD_MAKE_TYPE_EX(B,N);


// make a pair of type-puns. example (32x4,i,u) expands to:
//   static inline u32x4_t bitcast_iu_32x4(i32x4_t x) { return type_pun(x,u32x4_t); }
//   static inline i32x4_t bitcast_ui_32x4(u32x4_t x) { return type_pun(x,i32x4_t); }
// and likewise for float-point & signed integer. (choosen instead of unsigned because of
// default float compare behavior)
#define SIMD_MAKE_TPUN(base,i,o) \
    static inline CAT(o,base,_t) CAT(bitcast_,i,o,_,base)(CAT(i,base,_t) x) { return type_pun(x,CAT(o,base,_t)); }  \
    static inline CAT(i,base,_t) CAT(bitcast_,o,i,_,base)(CAT(o,base,_t) x) { return type_pun(x,CAT(i,base,_t)); } 

// make a pair of float/int conversions. Example:
//   static inline i32x4_t convert_fi_32x4(f32x4_t x) { ... }
//   static inline f32x4_t convert_if_32x4(i32x4_t x) { ... }
// only with signed integer because it's the best supported in hardware.

#define simd_convert(v,t) __builtin_convertvector(v,t)

#define SIMD_MAKE_TCONV(base)                                           \
    static inline CAT(f,base,_t) CAT(convert_if,_,base)(CAT(i,base,_t) x) {   \
         return __builtin_convertvector(x,CAT(f,base,_t)); }                  \
    static inline CAT(i,base,_t) CAT(convert_fi,_,base)(CAT(f,base,_t) x) {   \
         return __builtin_convertvector(x,CAT(i,base,_t)); } 


// make integer typedefs, uint <-> sint type-pun
// • 
// • 
#define SIMD_BUILD_TYPE_INT(B,N) SIMD_MAKE_TYPE_I(B,N) \
                                 SIMD_MAKE_TYPE_U(B,N) \
                                 SIMD_MAKE_TPUN(CAT(B,x,N),i,u)

// for the moment the following two are identical

// 32-bit elements types: expand int macro and add float typedefs
// and type-pun back and forth signed integers
#define SIMD_BUILD_TYPE_32(B,N) SIMD_BUILD_TYPE_INT(B,N)       \
                                SIMD_MAKE_TYPE_F(B,N)          \
                                SIMD_MAKE_TPUN(CAT(B,x,N),f,i) \
                                SIMD_MAKE_TCONV(CAT(B,x,N))

// 64-bit elements types: expand int macro and add double typedefs,
// type-puns,  back and forth signed integers
#define SIMD_BUILD_TYPE_64(B,N) SIMD_BUILD_TYPE_INT(B,N) \
                                SIMD_MAKE_TYPE_D(B,N)    \
                                SIMD_MAKE_TPUN(CAT(B,x,N),f,i) \
                                SIMD_MAKE_TCONV(CAT(B,x,N))

SIMD_SMAP(SIMD_BUILD_TYPE_INT, SIMD_S8_X);
SIMD_SMAP(SIMD_BUILD_TYPE_INT, SIMD_S16_X);
SIMD_SMAP(SIMD_BUILD_TYPE_32,  SIMD_S32_X);
SIMD_SMAP(SIMD_BUILD_TYPE_64,  SIMD_S64_X);



//---------------------------
// rework all of this (should be expanded)
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
#define SIMD_UI_X  SIMD_U32_X,SIMD_U64_X
#define SIMD_SI_X  SIMD_I32_X,SIMD_I64_X


//*******************************************************
// manually expanded "generic" type puns.
// using a macro to expand it (which would happen
// each time it's used) seems really sucky.

// entries for 512-bit packages
#if defined(SIMD_ENABLE_512)
#define simd_bitcast_fi_x      \
    f32x16_t:bitcast_fi_32x16, \
    f64x8_t: bitcast_fi_64x8,

#define simd_bitcast_if_x      \
    i32x16_t:bitcast_if_32x16, \
    i64x8_t: bitcast_if_64x8,

#define simd_bitcast_iu_x      \
    i8x64_t :bitcast_iu_8x64,  \
    i16x32_t:bitcast_iu_16x32, \
    i32x16_t:bitcast_iu_32x16, \
    i64x8_t: bitcast_iu_64x8,

#define simd_bitcast_ui_x      \
    u8x64_t: bitcast_ui_8x64,  \
    u16x32_t:bitcast_ui_16x32, \
    u32x16_t:bitcast_ui_32x16, \
    u64x8_t: bitcast_ui_64x8,

#else
#define simd_bitcast_fi_x
#define simd_bitcast_if_x
#define simd_bitcast_iu_x
#define simd_bitcast_ui_x
#endif

// type pun: floating point to signed integer
#define simd_bitcast_fi(X) ({ \
  typeof(X) _x = X;           \
  _Generic((_x),              \
    f32x2_t: bitcast_fi_32x2, \
    f32x4_t: bitcast_fi_32x4, \
    f32x8_t: bitcast_fi_32x8, \
    f64x2_t: bitcast_fi_64x2, \
    f64x4_t: bitcast_fi_64x4, \
        simd_bitcast_fi_x     \
    default: (void*)0)(_x);   \
  })

// type pun: signed integer to floating point 
#define simd_bitcast_if(X) ({ \
  typeof(X) _x = X;           \
  _Generic((_x),              \
    i32x2_t: bitcast_if_32x2, \
    i32x4_t: bitcast_if_32x4, \
    i32x8_t: bitcast_if_32x8, \
    i64x2_t: bitcast_if_64x2, \
    i64x4_t: bitcast_if_64x4, \
        simd_bitcast_if_x     \
    default: (void*)0)(_x);   \
  })

#define simd_bitcast_ui(X) ({  \
  typeof(X) _x = X;            \
  _Generic((_x),               \
    u8x8_t:  bitcast_ui_8x8,   \
    u8x16_t: bitcast_ui_8x16,  \
    u8x32_t: bitcast_ui_8x32,  \
    u16x4_t: bitcast_ui_16x4,  \
    u16x8_t: bitcast_ui_16x8,  \
    u16x16_t:bitcast_ui_16x16, \
    u32x2_t: bitcast_ui_32x2,  \
    u32x4_t: bitcast_ui_32x4,  \
    u32x8_t: bitcast_ui_32x8,  \
    u64x2_t: bitcast_ui_64x2,  \
    u64x4_t: bitcast_ui_64x4,  \
        simd_bitcast_ui_x      \
    default: (void*)0)(_x);    \
  })

#define simd_bitcast_iu(X) ({  \
  typeof(X) _x = X;            \
  _Generic((_x),               \
    i8x8_t:  bitcast_iu_8x8,   \
    i8x16_t: bitcast_iu_8x16,  \
    i8x32_t: bitcast_iu_8x32,  \
    i16x4_t: bitcast_iu_16x4,  \
    i16x8_t: bitcast_iu_16x8,  \
    i16x16_t:bitcast_iu_16x16, \
    i32x2_t: bitcast_iu_32x2,  \
    i32x4_t: bitcast_iu_32x4,  \
    i32x8_t: bitcast_iu_32x8,  \
    i64x2_t: bitcast_iu_64x2,  \
        simd_bitcast_iu_x      \
    default: (void*)0)(_x);    \
  })



//*******************************************************
// floating/integer conversion

#if defined(SIMD_ENABLE_512)
#define simd_convert_fi_x      \
    f32x16_t:convert_fi_32x16, \
    f64x8_t: convert_fi_64x8,

#define simd_convert_if_x      \
    i32x16_t:convert_if_32x16, \
    i64x8_t: convert_if_64x8,
#else
#define simd_convert_fi_x
#define simd_convert_if_x
#endif



// floating point to signed integer conversion
#define simd_convert_fi(X) ({  \
  typeof(X) _x = X;            \
  _Generic((_x),               \
    f32x2_t: convert_fi_32x2,  \
    f32x4_t: convert_fi_32x4,  \
    f32x8_t: convert_fi_32x8,  \
    f64x2_t: convert_fi_64x2,  \
    f64x4_t: convert_fi_64x4,  \
        simd_convert_fi_x      \
    default: (void*)0)(_x);    \
  })

// signed integer to floating point conversion
#define simd_convert_if(X) ({  \
  typeof(X) _x = X;            \
  _Generic((_x),               \
    i32x2_t: convert_if_32x2,  \
    i32x4_t: convert_if_32x4,  \
    i32x8_t: convert_if_32x8,  \
    i64x2_t: convert_if_64x2,  \
    i64x4_t: convert_if_64x4,  \
        simd_convert_if_x      \
    default: (void*)0)(_x);    \
  })

//*******************************************************
// macros to get the converted type

// for external use. this allows comma list of variables
// with and without initializers. 
#define simd_fi_typeof(X) typeof(simd_bitcast_fi(X))
#define simd_if_typeof(X) typeof(simd_bitcast_if(X))
#define simd_ui_typeof(X) typeof(simd_bitcast_ui(X))
#define simd_iu_typeof(X) typeof(simd_bitcast_iu(X))


#if defined(SIMD_USE_C23)
// I'm assuming auto is lighter weight to compile than
// the macro expansion of without. Using this has the
// restriction of auto (noted above)
#define simd_fi_typeof_i(X) auto
#define simd_if_typeof_i(X) auto
#define simd_ui_typeof_i(X) auto
#define simd_iu_typeof_i(X) auto
#else
#define simd_fi_typeof_i(X) simd_fi_typeof(X)
#define simd_if_typeof_i(X) simd_if_typeof(X)
#define simd_ui_typeof_i(X) simd_ui_typeof(X)
#define simd_iu_typeof_i(X) simd_iu_typeof(X)
#endif


// not quite. these are the inner expansion.
#define simd_fxor(A,B) simd_bitcast_if(simd_bitcast_fi(A) ^ simd_bitcast_fi(B))
#define simd_fand(A,B) simd_bitcast_if(simd_bitcast_fi(A) & simd_bitcast_fi(B))

//*******************************************************

// workers for macro expansions
#define simd_is_const(a)  __builtin_constant_p(a)

// have the defines even if not present in header. it's not
// nice they didn't make a MAX_TYPE_CLASS kindof define.
enum simd_type_class
{
  no_type_class = -1,
  void_type_class,
  integer_type_class,
  char_type_class,
  enumeral_type_class,
  boolean_type_class,
  pointer_type_class,
  reference_type_class,
  offset_type_class,
  real_type_class,
  complex_type_class,
  function_type_class,
  method_type_class,
  record_type_class,
  union_type_class,
  array_type_class,
  string_type_class,
  lang_type_class,
  opaque_type_class,
  bitint_type_class,
  vector_type_class
};

// determine the value the compiler will return for vectors. (vector_type_class not exisiting yet)
#if (defined(__clang__) && __clang_major__ >= 18) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 14)
#define SIMD_VEC_CLASS_TYPE vector_type_class
#else
#define SIMD_VEC_CLASS_TYPE no_type_class
#endif

// validate we're using the correct value for vector types 
static_assert(__builtin_classify_type((f32x4_t){0})==SIMD_VEC_CLASS_TYPE, "classify_type: wrong value");


// `true` if a is a `vector_size` or `ext_vector_type`
#define simd_is_vec_type(a)  (__builtin_classify_type(a) == SIMD_VEC_CLASS_TYPE)

#define simd_is_same_type(a,b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define simd_is_same_type3(a,b,c) (simd_is_same_type(a,b) & simd_is_same_type(a,c))

#define simd_is_type(a,T) __builtin_types_compatible_p(typeof(a), T)
#define simd_is_i32(a)    simd_is_type(a, int32_t)
#define simd_is_i64(a)    simd_is_type(a, int64_t)
#define simd_is_u32(a)    simd_is_type(a, uint32_t)
#define simd_is_u64(a)    simd_is_type(a, uint64_t)
#define simd_is_f32(a)    simd_is_type(a, float)
#define simd_is_f64(a)    simd_is_type(a, double)

// both arrays and pointers classify as `pointer_type_class` so jump through
// some hoops to be able to differentiate.
#define simd_is_ptr_or_array(a)  (__builtin_classify_type(a) == pointer_type_class)
#define simd_is_ptr_(a)          (&*__builtin_choose_expr(simd_is_ptr_or_array(a), a,0))
#define simd_is_ptr(a)           simd_is_same_type(a,simd_is_ptr_(a))
#define simd_is_array(a)         (simd_is_ptr_or_array(a) && !simd_is_ptr(a))

#define simd_is_scalar_fp(a)     (__builtin_classify_type(a) == real_type_class)
#define simd_is_scalar_int(a)    (__builtin_classify_type(a) == integer_type_class)

// attempt to make macro barf errors a bit easier to read
#define simd_assert_const(a)          static_assert(simd_is_const(a), "must be constant")
#define simd_assert_same_type(a,b)    static_assert(simd_is_same_type(a,b), "must be same type")
#define simd_assert_same_type3(a,b,c) static_assert(simd_is_same_type3(a,b,c),"must be same type")
#define simd_assert_same_size(a,b)    static_assert(sizeof(a)==sizeof(b), "must be same size types")

// all listed must be the specified type
#define simd_assert_scalar_fp(A,...)  static_assert(simd_is_scalar_fp(A) __VA_OPT__(SIMD_MAP(&& simd_is_scalar_fp,__VA_ARGS__)), "must be same scalar float type")
#define simd_assert_scalar_int(A,...) static_assert(simd_is_scalar_int(A) __VA_OPT__(SIMD_MAP(&& simd_is_scalar_int,__VA_ARGS__)), "must be same scalar integer type")
#define simd_assert_vec(A,...)        static_assert(simd_is_vec_type(A) __VA_OPT__(SIMD_MAP(&& simd_is_vec_type,__VA_ARGS__)), "must be vector type")


// if variable 'X' is a vector type yield 'V' and otherwise 'S'
#define simd_select_vs(X,V,S) __builtin_choose_expr(simd_is_vec_type(X), V, S)

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
// type widen/narrow

// clean these with (with other similar)
#define simd_def_promote(x,ti,to) \
  static inline CAT(to,_t) CAT(promote_,ti)(CAT(ti,_t) v)  \
  { return __builtin_convertvector(v,CAT(to,_t)); }

#define simd_def_demote(x,ti,to) \
  static inline CAT(to,_t) CAT(demote_,ti)(CAT(ti,_t) v)  \
  { return __builtin_convertvector(v,CAT(to,_t)); }

#define simd_def_widen_narrow(x,ti,to) \
  simd_def_promote(x,ti,to) \
  simd_def_demote(x,to,ti) 

//  8/16 pairs
simd_def_widen_narrow(x, u8x8,  u16x8);
simd_def_widen_narrow(x, i8x8,  i16x8);
simd_def_widen_narrow(x, u8x16, u16x16);
simd_def_widen_narrow(x, i8x16, i16x16);

// 16/32 pairs
simd_def_widen_narrow(x, u16x4, u32x4);
simd_def_widen_narrow(x, i16x4, i32x4);
simd_def_widen_narrow(x, u16x8, u32x8);
simd_def_widen_narrow(x, i16x8, i32x8);

// 32/64 pairs
simd_def_widen_narrow(x, f32x2, f64x2);
simd_def_widen_narrow(x, u32x2, u64x2);
simd_def_widen_narrow(x, i32x2, i64x2);
simd_def_widen_narrow(x, f32x4, f64x4);
simd_def_widen_narrow(x, u32x4, u64x4);
simd_def_widen_narrow(x, i32x4, i64x4);

// expand to 512 packages if enabled
#if defined(SIMD_ENABLE_512)
simd_def_widen_narrow(x, u8x32, u16x32);
simd_def_widen_narrow(x, i8x32, i16x32);

simd_def_widen_narrow(x, u16x16, u32x16);
simd_def_widen_narrow(x, i16x16, i32x16);

simd_def_widen_narrow(x, f32x8, f64x8);
simd_def_widen_narrow(x, u32x8, u64x8);
simd_def_widen_narrow(x, i32x8, i64x8);

// extra generic entries
#define simd_promote_f_x f32x8_t: promote_f32x8,

#define simd_promote_u_x u8x32_t:  promote_u8x32, \
                         u16x16_t: promote_u8x32, \
                         u32x8_t:  promote_u32x8,

#define simd_promote_i_x i8x32_t:  promote_i8x32, \
                         i16x16_t: promote_i8x32, \
                         i32x8_t:  promote_i32x8,

#define simd_demote_f_x  f64x8_t:  demote_f64x8,

#define simd_demote_u_x  u16x32_t: demote_u16x32, \
                         u32x16_t: demote_u32x16, \
                         u64x8_t:  demote_u64x8,

#define simd_demote_i_x  i16x32_t: demote_i16x32, \
                         i32x16_t: demote_i32x16, \
                         i64x8_t:  demote_i64x8,
#else
#define simd_promote_f_x
#define simd_promote_u_x
#define simd_promote_i_x
#define simd_demote_f_x
#define simd_demote_u_x
#define simd_demote_i_x
#endif

// scalar support
static inline uint16_t promote_u8(uint8_t  x)  { return (uint16_t)x; }
static inline uint32_t promote_u16(uint16_t x) { return (uint32_t)x; }
static inline uint64_t promote_u32(uint32_t x) { return (uint64_t)x; }

static inline uint8_t  demote_u16 (uint16_t x) { return (uint8_t )x; }
static inline uint16_t demote_u32 (uint32_t x) { return (uint16_t)x; }
static inline uint32_t demote_u64 (uint64_t x) { return (uint32_t)x; }

static inline int16_t  promote_i8 (int8_t  x) { return (int16_t)x; }
static inline int32_t  promote_i16(int16_t x) { return (int32_t)x; }
static inline int64_t  promote_i32(int32_t x) { return (int64_t)x; }

static inline int8_t   demote_i16 (int16_t x) { return (int8_t )x; }
static inline int16_t  demote_i32 (int32_t x) { return (int16_t)x; }
static inline int32_t  demote_i64 (int64_t x) { return (int32_t)x; }

static inline double   promote_f32(float  x)  { return (double)x; }
static inline float    demote_f64 (double x)  { return (float) x; }
  
// generic type widen/narrow
// • only same base types (floating point ,signed int, unsigned int).
//   Don't see any value to unknown base type.
// • support scalars (expect small usage so extra expansion is meh)
// number of combos is small enough (and mostly fixed)
// so no strong reason to macro expand these.
  
// binary32 → binary64
#define simd_promote_f(X) ({  \
  typeof(X) _x = X;           \
  _Generic(_x,                \
    float:   promote_f32,     \
    f32x2_t: promote_f32x2,   \
    f32x4_t: promote_f32x4,   \
        simd_promote_f_x      \
    default: (void*)0)(_x);   \
  })

// binary64 → binary32
#define simd_demote_f(X) ({   \
  typeof(X) _x = X;           \
  _Generic(_x,                \
    double:  demote_f64,      \
    f64x2_t: demote_f64x2,    \
    f64x4_t: demote_f64x4,    \
        simd_demote_f_x       \
    default: (void*)0)(_x);   \
  })

// unsigned integers    
#define simd_promote_u(X) ({  \
  typeof(X) _x = X;           \
  _Generic(_x,                \
    uint8_t:  promote_u8,     \
    uint16_t: promote_u16,    \
    uint32_t: promote_u32,    \
    u8x8_t:   promote_u8x8,   \
    u8x16_t:  promote_u8x16,  \
    u16x4_t:  promote_u16x4,  \
    u16x8_t:  promote_u16x8,  \
    u32x2_t:  promote_u32x2,  \
         simd_promote_u_x     \
    default: (void*)0)(_x);   \
  })

// signed integers    
#define simd_promote_i(X) ({  \
  typeof(X) _x = X;           \
  _Generic(_x,                \
    int8_t:  promote_i8,      \
    int16_t: promote_i16,     \
    int32_t: promote_i32,     \
    i8x8_t:  promote_i8x8,    \
    i8x16_t: promote_i8x16,   \
    i16x4_t: promote_i16x4,   \
    i16x8_t: promote_i16x8,   \
    i32x2_t: promote_i32x2,   \
        simd_promote_i_x      \
    default: (void*)0)(_x);   \
  })


//*******************************************************
// generic to specialized expansion macros (expand function or prototype)
// the expanded name do *NOT* have a `simd_` prefix. They are `name`_`type`

// clean these up..this is silly
#if   defined(SIMD_INLINE_SPECIALIZE)
#define SIMD_MAKE_UFUN(name,T) static inline CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a) { return CAT(simd_,name)(a); }
#define SIMD_MAKE_BFUN(name,T) static inline CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b) { return CAT(simd_,name)(a,b); }
#define SIMD_MAKE_3FUN(name,T) static inline CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b, CAT(T,_t) c) { return CAT(simd_,name)(a,b,c); }
#elif defined(SIMD_IMPLEMENTATION)
#define SIMD_MAKE_UFUN(name,T) CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a)                          { return CAT(simd_,name)(a);     }
#define SIMD_MAKE_BFUN(name,T) CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b)             { return CAT(simd_,name)(a,b);   }
#define SIMD_MAKE_3FUN(name,T) CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b,CAT(T,_t) c) { return CAT(simd_,name)(a,b,c); }
#else
#define SIMD_MAKE_UFUN(name,T) extern CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a);
#define SIMD_MAKE_BFUN(name,T) extern CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b);
#define SIMD_MAKE_3FUN(name,T) extern CAT(T,_t) CAT(name,_,T)(CAT(T,_t) a, CAT(T,_t) b, CAT(T,_t) c);
#endif

//*******************************************************
// generic SIMD libm functionality

// don't expect GCC to add and checking individually is a PITA
#if defined(__clang__) && (__clang_major__ >= 14)
#define simd_floor(x) __builtin_elementwise_floor(x)
#define simd_trunc(x) __builtin_elementwise_trunc(x)
#define simd_ceil(x)  __builtin_elementwise_ceil(x)
//#define simd_fabs(x)  __builtin_elementwise_abs(x)
#define simd_abs(x)   __builtin_elementwise_abs(x)
#else
#define simd_floor(x) simd_fp_std_unary(floor,x)
#define simd_trunc(x) simd_fp_std_unary(trunc,x)
#define simd_ceil(x)  simd_fp_std_unary(ceil,x)
//#define simd_fabs(x)  simd_fp_std_unary(fabs,x)
#endif

#define simd_round(x) simd_fp_std_unary(round,x)
#define simd_sqrt(x)  simd_fp_std_unary(sqrt,x)


#if defined(SIMD_SPECIALIZE)
// expand specialized: (example: floor_f32x4)
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, floor, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, ceil,  SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, trunc, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, round, SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, sqrt,  SIMD_FP_X)
//SIMD_MAP_PEAL(SIMD_MAKE_UFUN, fabs,  SIMD_FP_X) 
#endif

// GCC only. break into integer and fp, explict and mask for fp
#if !defined(simd_abs)

// abs stub for GCC floating point
// ({ simd_make_sgnmask_fv(x); simd_bitcast_if(simd_bitcast_fi(x) & _sgnmask); })


#define simd_abs_s(A) ({ typeof(A) _a = A; (_a > 0) ? _a : 0-_a; })
#define simd_abs(A)   simd_component_map(simd_abs_s,A)
#endif

#if defined(SIMD_SPECIALIZE)
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, abs,  SIMD_FP_X) 
SIMD_MAP_PEAL(SIMD_MAKE_UFUN, abs,  SIMD_SI_X) 
#endif


//*******************************************************
// mixed vector/scalar macro paramenter helpers

// yields the first parameter that's a vector type (if there is one)
#define simd_first_vt2(A,B)     __builtin_choose_expr(simd_is_vec_type(A),A,B)
#define simd_first_vt3(A,B,C)   simd_first_vt2(simd_first_vt2(A,B),C)
#define simd_first_vt4(A,B,C,D) simd_first_vt3(A,B,simd_first_vt2(C,D))


// simd_param_{n} helper:
// • `name` is the variable name of the captured 
// • `orig` is the macro parameter
// uses helper `simd_splat_i` which logically passes
// through a vector type and broadcasts a scalar
// and `_type` is defined instead the expression statement
#define simd_capture_i(name,orig)  typeof(_type) name = simd_splat_i(typeof(_type),orig);


// all parameters must be a vector type (T) or
// a scalar of the element type of T. All scalars
// are broadcast to type T (if there are any)
//
// • determine the vector type T (two methods):
//   `simd_first_vt{n}`: walks the parameters and yields
//   the first vector type (or final if none)
// • `simd_capture_i` uses typeof(_type) to pass through
//   vector parameters, broadcast any scalars, and 
//   capture macro input (prevent side-effects)
// • {_a,_b,...} are the original values with any
//   scalars broadcast to T. (fixed naming scheme)
//
// TODO:
// • add type checking asserts. 

// NOTE: using = {0} instead of {} for older compilers
#define simd_param_2(A,B)      \
  typeof(simd_first_vt2(A,B))  \
    _type = {0};               \
  simd_capture_i(_a,A);        \
  simd_capture_i(_b,B);

#define simd_param_3(A,B,C)    \
  typeof(simd_first_vt3(A,B,C))\
    _type = {0};               \
  simd_capture_i(_a,A);        \
  simd_capture_i(_b,B);        \
  simd_capture_i(_c,C);

#define simd_param_4(A,B,C,D)     \
  typeof(simd_first_vt3(A,B,C,D)) \
    _type = {0};                  \
  simd_capture_i(_a,A);           \
  simd_capture_i(_b,B);           \
  simd_capture_i(_c,C);           \
  simd_capture_i(_d,D);           \


// internal helper: given floating-point vector X return same size integer with all bits set
// (could be done smarter)
#define simd_make_ones_fv(X)    simd_fi_typeof(x) _ones = (~(typeof(_ones)){0})

// internal helper: given floating-point vector X return same size integer sign bit set
// (could be done smarter)
#define simd_make_sgnmask_fv(X) simd_fi_typeof(x) _sgnmask = ((~(typeof(_ones)){0}) << (8*sizeof(x[0])-1))



//*******************************************************
// min/max
//

// the comparision is written this way for floating point which
// will cause the first parameter to be returned if the inputs
// are unordered (one or both are NaN). Doesn't matter for integers.
//
// this differs from the libm functions in C which will return
// the ordered value if only one is NaN.

// scalar macros (compare written for NaN)
// WARNING: enabling finite math breaks any promise of NaN behavior)
#define simd_min_s(A,B) ({ simd_param_2(A,B); (_b > _a) ? _a : _b; })
#define simd_max_s(A,B) ({ simd_param_2(A,B); (_b < _a) ? _a : _b; })

// vector macros (B element is awkward because it's directly expanded.
// could make a binary function expander...but meh ATM)

// integer & floating point (with above listed behavior)
#define simd_min(A,B) simd_component_map(simd_min_s,A,simd_elem(B,i))
#define simd_max(A,B) simd_component_map(simd_max_s,A,simd_elem(B,i))

#if defined(SIMD_SPECIALIZE)
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, min, SIMD_UI_X)
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, min, SIMD_SI_X)
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, min, SIMD_FP_X);
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, max, SIMD_FP_X);
#endif

#if 0//defined(SIMD_SPECIALIZE)
SIMD_MAP_PEAL(SIMD_MAKE_3FUN, min, SIMD_UI_X)
SIMD_MAP_PEAL(SIMD_MAKE_3FUN, min, SIMD_SI_X)
SIMD_MAP_PEAL(SIMD_MAKE_3FUN, min, SIMD_FP_X);
SIMD_MAP_PEAL(SIMD_MAKE_3FUN, max, SIMD_FP_X);
#endif


#if 0
// disable for now.
// floating-point C library behavior
// self note: GCC expansions of this explode w/o weaking FP behavior
// (think about this at some point -ffinite-math-only does the trick
// but then single NaN behavior is...whatever dude?)
#define simd_fmin(a,b) simd_fp_std_binary(fmin,a,b)
#define simd_fmax(a,b) simd_fp_std_binary(fmax,a,b)

#if defined(SIMD_SPECIALIZE)
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, fmin, SIMD_FP_X);
SIMD_MAP_PEAL(SIMD_MAKE_BFUN, fmax, SIMD_FP_X);
#endif
#endif

// temp hack as-is
// todo: either 'x' is a vector or all are same scalar type
#define simd_clamp(x,lo,hi) simd_min(simd_max(x,lo),hi)




//*******************************************************
// 

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

// R_i = RN(A_i•B_i - C_i)
// simd_fms_i macro can't take a negated
// input so specialize it
#define simd_fms_i(A,B,C) ({              \
  for(size_t i=0; i<simd_dim(A); i++) {   \
    simd_elem(A,i) = simd_fma_s(          \
    simd_elem(A,i),                       \
    simd_elem(B,i),                       \
    -simd_elem(C,i));                     \
  }                                       \
  A;                                      \
})


// SIMD FMA : R_i = RN(A_i•B_i + C_i)
#define simd_fma(A,B,C) ({     \
  simd_param_3(A,B,C);         \
  simd_fma_i(_a,_b,_c);        \
})


// all parameters must be vectors version
#if (__has_builtin(__builtin_elementwise_fma))
#define simd_fma_v(a,b,c) ({           \
  simd_assert_same_type3(a,b,c);       \
  simd_assert_vec(a);                  \
  typeof(a) _a=a,_b=b,_c=c;            \
  __builtin_elementwise_fma(_a,_b,_c); \
})
#else
#define simd_fma_v(a,b,c) ({     \
  simd_assert_same_type3(a,b,c); \
  simd_assert_vec(a);            \
  typeof(a) _a=a,_b=b,_c=c;      \
  for(size_t i=0; i<simd_dim_v(_a); i++) {  \
    _a[i] = simd_fma_s(_a[i],_b[i],_c[i]);  \
  }                             \
  _a;                           \
})
#endif

// SIMD LERP: R_i = A_i(1-T_i) + B_i•T_i
// • end-points are exact (2 FMA formulation)
#define simd_lerp(A,B,T) ({                \
  simd_param_3(A,B,T);                     \
  typeof(_type) _r = simd_fms_i(_a,_c,_a); \
  typeof(_type) _f = simd_fms_i(_b,_c,_r); \
  _f;                                      \
})

// all vector input version
#if (__has_builtin(__builtin_elementwise_fma))
#define simd_lerp_v(a,b,t) ({                \
  simd_assert_same_type3(a,b,t);             \
  simd_assert_vec(a);                        \
  typeof(a) _a=a,_b=b,_t=t;                  \
  _a = __builtin_elementwise_fma(_a,_t,-_a); \
  _a = __builtin_elementwise_fma(_b,_t,-_a); \
  _a;                                        \
})
#else
#define simd_lerp_v(a,b,t) ({    \
  simd_assert_same_type3(a,b,t); \
  simd_assert_vec(a);            \
  typeof(a) _a=a,_b=b,_t=t;      \
  for(size_t i=0; i<simd_dim_v(_a); i++) {  \
    _a[i] = simd_fma_s(_a[i],_t[i],-_a[i]); \
    _a[i] = simd_fma_s(_b[i],_t[i],-_a[i]); \
  }                             \
  _a;                           \
})
#endif

// TODO: alternating sign version (even = add, odd = sub)
// but neither GCC nor clang match the pattern so it'd
// need to be explicit. (see simd_2d3d.h)


//*******************************************************

// compute: sa•A + sb•B  scalar (sa,sb), vector (A,B)
// todo : add typechecking & use capture macro
#define smind_wsum(a,b,sa,sb) ({      \
  typeof(a) _a  = a;                  \
  typeof(a) _b  = b;                  \
  typeof(a) _sa = sa-(typeof(a)){0};  \
  typeof(a) _sb = sb-(typeof(a)){0};  \
  _a = simd_fma_v(_sa,_a,_sb*_b);     \
  _a;                                 \
})


//*******************************************************
// blend support:  (a & s) | (a & (~s))
// can only be lowered into an actual blend if the
// compiler can see `s` is a selection mask

#if defined(SIMD_USE_C23)
#define simd_blend_i(A,B,S) ({    \
  auto _a = simd_bitcast_fi(A);   \
  auto _b = simd_bitcast_fi(B);   \
  auto _s = S;                    \
  auto _r = (_a&_s)|(_b & (~_s)); \
  simd_bitcast_if(_r);            \
})
#else
#define simd_blend_i(A,B,S) ({          \
  simd_fi_typeof_i(A) _a                \
    = simd_bitcast_fi(A);               \
  typeof(_a) _b = simd_bitcast_fi(B);   \
  typeof(_a) _s = S;                    \
  typeof(_a) _r = (_a&_s)|(_b & (~_s)); \
  simd_bitcast_if(_r);                  \
})
#endif

// should be checking for FP vector types
#define simd_blend_v(A,B,S) ({    \
  simd_assert_same_type(A,B);     \
  simd_assert_vec(A,B);           \
  simd_blend_i(A,B,S);            \
})




// clean-up compiler option mods
#pragma GCC diagnostic pop

#if !defined(__clang__)
#pragma GCC reset_options
#endif

#else
#error "requires extended C features"
#endif
