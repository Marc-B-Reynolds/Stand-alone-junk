// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

// ssimd (2D/3D spatial SIMD)
// Logically extends `simd.h` with some 2D & 3D types. Meaning that
// it can be used either in isolation or with `simd.h` (expand a bit)
// If `simd.h` isn't include then:
// • There's no attempt compiler option inspection. (expand)
// • note other stuff
// The construction methodology (however) is opposite. A smaller set
// of types and expected interactions between scalar/vector types
// because they usually explict with specific operations. So a much higher
// rate of specialization and explict implementations. The reasoning
// is much lighter weight macros expansions for faster compile times.
// A side-effect is logical functionality duplication between the
// two with local versions being more restrictive. 
//
// Usage: include this header (+simd.h):
// • `SIMD_IMPLEMENTATION`
// • make real note

// NOTE:
// • Since both 3D vectors and quaternions are 4 element vectors
//   (C compatible types) the routines can't distinguish between them
//   which (sadly) requires programmer care. (expand)
// • GCC specific:
//   • attempts to eliminate shuffles which can back-fire and blow-up
//     the code. Does it stem from making a "greedy" choice which
//     helps initially but causes explosion latter? humm... If I
//     grokked it maybe there's a work-around.
// • clang specific:
//     
// •

// TODO: (tons)
// • big functions being inlines is temp hack

// temp hack
//#pragma once

#if defined(__GNUC__)

#define SFH_SIMD_2D3D_H

#if !defined(SFH_SIMD_H)
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// this should duplicate how attributes are choosen in simd.h (SIMD_MAKE_TYPE_EX)
#if __has_attribute(ext_vector_type) && !defined(SIMD_USE_VECTOR_SIZE)
#define SSIMD_TYPE_ATTR(B,N) __attribute__((ext_vector_type(N)))
#else
#define SSIMD_TYPE_ATTR(B,N) __attribute__((vector_size(B*N/8)))
#endif

// make the typedefs
typedef float  f32x2_t SSIMD_TYPE_ATTR(32,2);
typedef double f64x2_t SSIMD_TYPE_ATTR(64,2);
typedef float  f32x4_t SSIMD_TYPE_ATTR(32,4);
typedef double f64x4_t SSIMD_TYPE_ATTR(64,4);

// for bit manipulation based functionality
typedef int32_t  i32x2_t SSIMD_TYPE_ATTR(32,2);
typedef int64_t  i64x2_t SSIMD_TYPE_ATTR(64,2);
typedef int32_t  i32x4_t SSIMD_TYPE_ATTR(32,4);
typedef int64_t  i64x4_t SSIMD_TYPE_ATTR(64,4);

typedef uint32_t u32x2_t SSIMD_TYPE_ATTR(32,2);
typedef uint64_t u64x2_t SSIMD_TYPE_ATTR(64,2);
typedef uint32_t u32x4_t SSIMD_TYPE_ATTR(32,4);
typedef uint64_t u64x4_t SSIMD_TYPE_ATTR(64,4);


#ifndef type_pun
#define type_pun(X,TYPE) ({			    \
 static_assert(sizeof(X) == sizeof(TYPE),"size mismatch"); \
  typeof(X) __x = (X);                              \
  TYPE __d;                                         \
  memcpy(&__d, &__x, sizeof(TYPE));                 \
  __d;                                              \
})
#endif


#if defined(__x86_64__)
#include <x86intrin.h>

static inline __m128  f32x4_to_intel(f32x4_t x) { return type_pun(x,__m128);  }
static inline __m128i i32x4_to_intel(i32x4_t x) { return type_pun(x,__m128i); }
static inline __m128d f64x2_to_intel(f64x2_t x) { return type_pun(x,__m128d); }
static inline __m256d f64x4_to_intel(f64x4_t x) { return type_pun(x,__m256d); }

static inline f32x4_t f32x4_from_intel(__m128  x) { return type_pun(x,f32x4_t); }
static inline i32x4_t i32x4_from_intel(__m128i x) { return type_pun(x,i32x4_t); }
static inline f64x2_t f64x2_from_intel(__m128d x) { return type_pun(x,f64x2_t); }
static inline f64x4_t f64x4_from_intel(__m256d x) { return type_pun(x,f64x4_t); }

#endif



#else

// could put some forwarding to simd.h stuff here (esp if complexity
// is about the same as any specialized versions in this file.

#endif

// 3D vector is stored 4 elements
typedef f32x2_t vec2f_t;
typedef f64x2_t vec2d_t;
typedef f32x4_t quatf_t;
typedef f64x4_t quatd_t;
typedef quatf_t vec3f_t;
typedef quatd_t vec3d_t;

// Internal notes:
// • Structured quite differently from the base header since we
//   only have a very small number of types. Much more focus on
//   explicit type implementations. This helps reduce the amount
//   of `callsite` macro expansion for generic functionality macros.
// • On intel f32x2_t is synthesized (uses 128-bit registers).
//   need to figure out a (zero overhead on expansion) way to
//   type pun to __m128 for any work-a-round direct to intrinsic
//   calls. (SEE: vec2f_fmadd_sub)

//*******************************************************
// type pun (bit pattern) float to int (fi) and int to float (if)

static inline int32_t ssimd_bitcast_fi_32  (float   a) { return type_pun(a,int32_t); }
static inline float   ssimd_bitcast_if_32  (int32_t a) { return type_pun(a,float  ); }
static inline int64_t ssimd_bitcast_fi_64  (double  a) { return type_pun(a,int64_t); }
static inline double  ssimd_bitcast_if_64  (int64_t a) { return type_pun(a,double ); }
static inline i32x2_t ssimd_bitcast_fi_32x2(f32x2_t a) { return type_pun(a,i32x2_t); }
static inline f32x2_t ssimd_bitcast_if_32x2(i32x2_t a) { return type_pun(a,f32x2_t); }
static inline i64x2_t ssimd_bitcast_fi_64x2(f64x2_t a) { return type_pun(a,i64x2_t); }
static inline f64x2_t ssimd_bitcast_if_64x2(i64x2_t a) { return type_pun(a,f64x2_t); }
static inline i32x4_t ssimd_bitcast_fi_32x4(f32x4_t a) { return type_pun(a,i32x4_t); }
static inline f32x4_t ssimd_bitcast_if_32x4(i32x4_t a) { return type_pun(a,f32x4_t); }
static inline i64x4_t ssimd_bitcast_fi_64x4(f64x4_t a) { return type_pun(a,i64x4_t); }
static inline f64x4_t ssimd_bitcast_if_64x4(i64x4_t a) { return type_pun(a,f64x4_t); }

#define ssimd_bitcast_fi(X) ({      \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    float:   ssimd_bitcast_fi_32,   \
    double:  ssimd_bitcast_fi_64,   \
    f32x2_t: ssimd_bitcast_fi_32x2, \
    f32x4_t: ssimd_bitcast_fi_32x4, \
    f64x2_t: ssimd_bitcast_fi_64x2, \
    f64x4_t: ssimd_bitcast_fi_64x4, \
    default: (void*)0)(_x);         \
  })

// type pun: signed integer to floating point 
#define ssimd_bitcast_if(X) ({      \
  typeof(X) _x = X;                 \
  _Generic((_x),                    \
    int32_t: ssimd_bitcast_if_32,   \
    int64_t: ssimd_bitcast_if_64,   \
    i32x2_t: ssimd_bitcast_if_32x2, \
    i32x4_t: ssimd_bitcast_if_32x4, \
    i64x2_t: ssimd_bitcast_if_64x2, \
    i64x4_t: ssimd_bitcast_if_64x4, \
    default: (void*)0)(_x);         \
  })

// same number of elements and width type of opposite (int vs. float)
// should probably add (ssimd) base element type versions as well
#define ssimd_itypeof(x) typeof(ssimd_bitcast_fi(x))
#define ssimd_ftypeof(x) typeof(ssimd_bitcast_if(x))

//*******************************************************
// basic support functionality

// "constructor" like functions
static inline vec2f_t vec2f(float  x, float  y)                     { return (vec2f_t){x,y}; }
static inline vec2d_t vec2d(double x, double y)                     { return (vec2d_t){x,y}; }
static inline vec3f_t vec3f(float  x, float  y, float  z)           { return (vec3f_t){x,y,z,0}; }
static inline vec3d_t vec3d(double x, double y, double z)           { return (vec3d_t){x,y,z,0}; }
static inline quatf_t quatf(float  x, float  y, float  z, float  w) { return (quatf_t){x,y,z,w}; }
static inline quatd_t quatd(double x, double y, double z, double w) { return (quatd_t){x,y,z,w}; }

#define vec2(x,y)   ({_Generic(x, float:vec2f, default: vec2d)(x,y);   })
#define vec3(x,y,z) ({_Generic(x, float:vec3f, default: vec3d)(x,y,z); })


// set from bivector + scalar: Q = (b,s)
static inline quatf_t quatf_bs(vec3f_t b, float  s) { return (quatf_t){b[0],b[1],b[2],s}; }
static inline quatd_t quatd_bs(vec3d_t b, double s) { return (quatd_t){b[0],b[1],b[2],s}; }

#define quat_bs(b,s) quat_fwd(bs,b,s)

// nerdy accept either way of setting
#define quatf_set(x,...) \
({_Generic(x, quatf_t:quatf_bs, default: quatf)(x __VA_OPT__(,__VA_ARGS__));})

#define quatd_set(x,...)                                                \
({_Generic(x, quatd_t:quatd_bs, default: quatd)(x __VA_OPT__(,__VA_ARGS__));})

#define quat(x,...) ({_Generic(x, float:quatf, double:quatd)(x __VA_OPT__(,__VA_ARGS__));})

// expand generic expression which forward to a function and first parameter is a scalar
// (no need to perform any parameter capturing)
#define ssimd_fwd_sfunc(type, name,x,...)  \
  ({_Generic(x, float:type##f_##name, default:type##d_##name)(x __VA_OPT__(,__VA_ARGS__));})


// same but first parameter must be vector type
#define vec2_fwd(name,x,...) ({_Generic(x, vec2f_t:vec2f_##name, default:vec2d_##name)(x __VA_OPT__(,__VA_ARGS__));})
#define vec3_fwd(name,x,...) ({_Generic(x, vec3f_t:vec3f_##name, default:vec3d_##name)(x __VA_OPT__(,__VA_ARGS__));})
#define quat_fwd(name,x,...) ({_Generic(x, quatf_t:quatf_##name, default:quatd_##name)(x __VA_OPT__(,__VA_ARGS__));})


// broadcast a scalar to all elements
// • IMPORTANT: vec3_broadcast sends 'v' to the first 3 elements and 4th is
//   set to zero. In many cases setting all four elements to 'v' is equally
//   correct and more efficient.
static inline vec2f_t vec2f_broadcast(float  v) { return vec2f(v,v); }
static inline vec2d_t vec2d_broadcast(double v) { return vec2d(v,v); }
static inline vec3f_t vec3f_broadcast(float  v) { return vec3f(v,v,v); }
static inline vec3d_t vec3d_broadcast(double v) { return vec3d(v,v,v); }
static inline quatf_t quatf_broadcast(float  v) { return quatf(v,v,v,v); }
static inline quatd_t quatd_broadcast(double v) { return quatd(v,v,v,v); }

#define vec2_broadcast(x) ssimd_fwd_sfunc(vec2,broadcast,x)
#define vec3_broadcast(x) ssimd_fwd_sfunc(vec3,broadcast,x)
#define quat_broadcast(x) ssimd_fwd_sfunc(quat,broadcast,x)

// single "register" shuffles
#define vec2_shuffle(V,A,B)     __builtin_shufflevector(V,V,A,B)
#define vec3_shuffle(V,A,B,C)   __builtin_shufflevector(V,V,A,B,C,3)
#define quat_shuffle(Q,A,B,C,D) __builtin_shufflevector(Q,Q,A,B,C,D)

// two "register" shuffles
#define vec2_shuffle2(V0,V1,A,B)     __builtin_shufflevector(V0,V1,A,B)
#define vec3_shuffle2(V0,V1,A,B,C)   __builtin_shufflevector(V0,V1,A,B,C,3)
#define quat_shuffle2(Q0,Q1,A,B,C,D) __builtin_shufflevector(Q0,Q1,A,B,C,D)


// promote/demote (widen/narrow) between binary32/binary64 elements
static inline vec2d_t vec2f_promote(vec2f_t v) { return __builtin_convertvector(v,vec2d_t); }
static inline vec2f_t vec2d_demote (vec2d_t v) { return __builtin_convertvector(v,vec2f_t); }
static inline vec3d_t vec3f_promote(vec3f_t v) { return __builtin_convertvector(v,vec3d_t); }
static inline vec3f_t vec3d_demote (vec3d_t v) { return __builtin_convertvector(v,vec3f_t); }
static inline quatd_t quatf_promote(quatf_t q) { return __builtin_convertvector(q,quatd_t); }
static inline quatf_t quatd_demote (quatd_t q) { return __builtin_convertvector(q,quatf_t); }

// favor avoiding spurious overflow (infinity for denormal input) to improved average error
static inline float  ssimd_rsqrt_f32(float  x) { return 1.f/sqrtf(x); }
static inline double ssimd_rsqrt_f64(double x) { return 1.0/sqrt(x);  }

#define ssimd_rsqrt(x)   (_Generic((x),float:ssimd_rsqrt_f32,default:ssimd_rsqrt_f64)(x))
#define ssimd_sqrt(x)    (_Generic((x),float:sqrtf,default: sqrt)(x))
#define ssimd_abs(x)     (_Generic((x),float:fabsf,default: fabs)(x))
#define ssimd_fma(a,b,c) (_Generic((a),float:fmaf, default: fma)(a,b,c))

// element-wise FMAs (unlike 'simd.h' it's up the the user to perform any broadcasts)
static inline vec2f_t vec2f_fma(vec2f_t a, vec2f_t b, vec2f_t c) { return vec2f(fmaf(a[0],b[0],c[0]), fmaf(a[1],b[1],c[1])); }
static inline vec2d_t vec2d_fma(vec2d_t a, vec2d_t b, vec2d_t c) { return vec2d(fma (a[0],b[0],c[0]), fma (a[1],b[1],c[1])); }
static inline quatf_t quatf_fma(quatf_t a, quatf_t b, quatf_t c) { return quatf(fmaf(a[0],b[0],c[0]), fmaf(a[1],b[1],c[1]), fmaf(a[2],b[2],c[2]), fmaf(a[3],b[3],c[3])); }
static inline quatd_t quatd_fma(quatd_t a, quatd_t b, quatd_t c) { return quatd(fma (a[0],b[0],c[0]), fma (a[1],b[1],c[1]), fma (a[2],b[2],c[2]), fma (a[3],b[3],c[3])); }
static inline vec3f_t vec3f_fma(vec3f_t a, quatf_t b, quatf_t c) { return quatf_fma(a,b,c); }
static inline vec3d_t vec3d_fma(vec3d_t a, quatd_t b, quatd_t c) { return quatd_fma(a,b,c); }

#define vec2_fma(a,b,c) vec2_fwd(fma,a,b,c)
#define vec3_fma(a,b,c) vec3_fwd(fma,a,b,c)
#define quat_fma(a,b,c) quat_fwd(fma,a,b,c)


// float → int (standard truncation)
static inline i32x2_t vec2f_to_int(vec2f_t x) { return __builtin_convertvector(x,i32x2_t); }
static inline i64x2_t vec2d_to_int(vec2d_t x) { return __builtin_convertvector(x,i64x2_t); }
static inline i32x4_t quatf_to_int(quatf_t x) { return __builtin_convertvector(x,i32x4_t); }
static inline i64x4_t quatd_to_int(quatd_t x) { return __builtin_convertvector(x,i64x4_t); }
static inline i32x4_t vec3f_to_int(vec3f_t x) { return __builtin_convertvector(x,i32x4_t); }
static inline i64x4_t vec3d_to_int(vec3d_t x) { return __builtin_convertvector(x,i64x4_t); }

#define vec2_to_int(x) vec2_fwd(to_int,x)
#define vec3_to_int(x) vec3_fwd(to_int,x)
#define quat_to_int(x) quat_fwd(to_int,x)

// int → float
static inline vec2f_t vec2f_from_int(i32x2_t x) { return __builtin_convertvector(x,f32x2_t); }
static inline vec2d_t vec2d_from_int(i64x2_t x) { return __builtin_convertvector(x,f64x2_t); }
static inline quatf_t quatf_from_int(i32x4_t x) { return __builtin_convertvector(x,f32x4_t); }
static inline quatd_t quatd_from_int(i64x4_t x) { return __builtin_convertvector(x,f64x4_t); }
static inline vec3f_t vec3f_from_int(i32x4_t x) { return __builtin_convertvector(x,f32x4_t); }
static inline vec3d_t vec3d_from_int(i64x4_t x) { return __builtin_convertvector(x,f64x4_t); }

#define vec2_from_int(x) ({ _Generic(x, i32x2_t:vec2f_from_int, default:vec2d_from_int)(x); })
#define vec3_from_int(x) ({ _Generic(x, i32x4_t:vec3f_from_int, default:vec3d_from_int)(x); })
#define quat_from_int(x) ({ _Generic(x, i32x4_t:quatf_from_int, default:quatd_from_int)(x); })


// float → int : input is scaled by 's', rounded to nearest and converted to an integer (logically)
static inline i32x2_t vec2f_scale_round_to_int(vec2f_t x, float  s) { return vec2f_to_int(vec2f_fma(x,vec2f_broadcast(s), vec2f_broadcast(0.5f))); }
static inline i64x2_t vec2d_scale_round_to_int(vec2d_t x, double s) { return vec2d_to_int(vec2d_fma(x,vec2d_broadcast(s), vec2d_broadcast(0.5)));  }
static inline i32x4_t quatf_scale_round_to_int(quatf_t x, float  s) { return vec3f_to_int(quatf_fma(x,quatf_broadcast(s), quatf_broadcast(0.5f))); }
static inline i64x4_t quatd_scale_round_to_int(quatd_t x, double s) { return vec3d_to_int(quatd_fma(x,quatd_broadcast(s), quatd_broadcast(0.5)));  }

#define vec2_scale_round_to_int(x,s) vec2_fwd(scale_round_to_int,x,s)
#define quat_scale_round_to_int(x,s) vec2_fwd(scale_round_to_int,x,s)
#define vec3_scale_round_to_int(x,s) quat_scale_round_to_int(x,s)


// elementwise absolute value
static inline vec2f_t vec2f_abs(vec2f_t x) { return vec2f(fabsf(x[0]),fabsf(x[1])) ; }
static inline vec2d_t vec2d_abs(vec2d_t x) { return vec2d(fabs (x[0]),fabs (x[1])) ; }
static inline quatf_t quatf_abs(quatf_t x) { return quatf(fabsf(x[0]),fabsf(x[1]),fabsf(x[2]), fabsf(x[3])) ; }
static inline quatd_t quatd_abs(quatd_t x) { return quatd(fabs (x[0]),fabs (x[1]),fabs (x[2]), fabs (x[3])) ; }

#define vec3f_abs quatf_abs
#define vec3d_abs quatd_abs

#define vec2_abs(x) vec2_fwd(abs,x)
#define vec3_abs(x) vec3_fwd(abs,x)
#define quat_abs(x) quat_fwd(abs,x)

// elementwise floor
static inline vec2f_t vec2f_floor(vec2f_t x) { return vec2f(floorf(x[0]),floorf(x[1])) ; }
static inline vec2d_t vec2d_floor(vec2d_t x) { return vec2d(floor (x[0]),floor (x[1])) ; }
static inline quatf_t quatf_floor(quatf_t x) { return quatf(floorf(x[0]),floorf(x[1]),floorf(x[2]), floorf(x[3])) ; }
static inline quatd_t quatd_floor(quatd_t x) { return quatd(floor (x[0]),floor (x[1]),floor (x[2]), floor (x[3])) ; }

#define vec3f_floor quatf_floor
#define vec3d_floor quatd_floor

#define vec2_floor(x) vec2_fwd(floor,x)
#define vec3_floor(x) vec3_fwd(floor,x)
#define quat_floor(x) quat_fwd(floor,x)

#define vec2_round_up(x) vec2_floor((x)+0.5f)
#define vec3_round_up(x) vec3_floor((x)+0.5f)
#define quat_round_up(x) quat_floor((x)+0.5f)


//*******************************************************
// An special case implementations that have to punt to
// direct intrinsic calls for some reason or another.


#if defined(__x86_64__)

// Currently neither GCC/clang match intel FMAs with even/odd element
// add/sub (or vice versa) additive part.
// The intrisic used has the opposite name because of the notion of
// element orderings are reversed.

// can't think of a way to handle this one since f32x2 is
// synthesized on intel as 128-bit. humm...
// It should be one op but it'll be many. 
static inline vec2f_t vec2f_fmadd_sub(vec2f_t a, vec2f_t b, vec2f_t c)
{
  return vec2f(fmaf(a[0],b[0], c[0]), fmaf(a[1],b[1],-c[1]));
}

static inline vec2d_t vec2d_fmadd_sub(vec2d_t a, vec2d_t b, vec2d_t c)
{
  __m128d ia = type_pun(a,__m128d);
  __m128d ib = type_pun(b,__m128d);
  __m128d ic = type_pun(c,__m128d);
  return type_pun(_mm_fmsubadd_pd(ia,ib,ic), vec2d_t);
}

static inline quatf_t quatf_fmadd_sub(quatf_t a, quatf_t b, quatf_t c)
{
  __m128 ia = type_pun(a,__m128);
  __m128 ib = type_pun(b,__m128);
  __m128 ic = type_pun(c,__m128);
  return type_pun(_mm_fmsubadd_ps(ia,ib,ic), f32x4_t);
}

static inline quatd_t quatd_fmadd_sub(quatd_t a, quatd_t b, quatd_t c)
{
  __m256d ia = type_pun(a,__m256d);
  __m256d ib = type_pun(b,__m256d);
  __m256d ic = type_pun(c,__m256d);
  return type_pun(_mm256_fmsubadd_pd(ia,ib,ic), f64x4_t);
}

#else

static inline vec2f_t vec2f_fmadd_sub(vec2f_t a, vec2f_t b, vec2f_t c)
{
  return vec2f(fmaf(a[0],b[0], c[0]), fmaf(a[1],b[1],-c[1]));
}

static inline vec2d_t vec2d_fmadd_sub(vec2d_t a, vec2d_t b, vec2d_t c)
{
  return vec2d(fma(a[0],b[0], c[0]), fma(a[1],b[1],-c[1]));
}

static inline quatf_t quatf_fmadd_sub(quatf_t a, quatf_t b, quatf_t c)
{
  return quatf(fmaf(a[0],b[0], c[0]),
               fmaf(a[1],b[1],-c[1]),
               fmaf(a[2],b[2], c[2]),
               fmaf(a[3],b[3],-c[3]));
}

static inline quatd_t quatd_fmadd_sub(quatd_t a, quatd_t b, quatd_t c)
{
  return quatf(fma(a[0],b[0], c[0]),
               fma(a[1],b[1],-c[1]),
               fma(a[2],b[2], c[2]),
               fma(a[3],b[3],-c[3]));
}

#endif

static inline vec3f_t vec3f_fmadd_sub(vec3f_t a, vec3f_t b, vec3f_t c) { return quatf_fmadd_sub(a,b,c); }
static inline vec3d_t vec3d_fmadd_sub(vec3d_t a, vec3d_t b, vec3d_t c) { return quatd_fmadd_sub(a,b,c); }

#define vec2_fmadd_sub(a,b,c) vec2_fwd(fmadd_sub, a,b,c)
#define vec3_fmadd_sub(a,b,c) vec3_fwd(fmadd_sub, a,b,c)
#define quat_fmadd_sub(a,b,c) quat_fwd(fmadd_sub, a,b,c)


// WARNING: no generic version because defined to direct match the intel ops (hence the name) and the ordering
// of the sums are different. temp hack because it'll be a source of errors.

#if defined(__clang__) || !defined(__x86_64__)
static inline f32x4_t intel_hadd_f32x4(f32x4_t a, f32x4_t b) { return (f32x4_t){a[0]+a[1],a[2]+a[3],b[0]+b[1],b[2]+b[3]}; }
static inline f64x4_t intel_hadd_f64x4(f64x4_t a, f64x4_t b) { return (f64x4_t){a[0]+a[1],b[0]+b[1],a[2]+a[3],b[2]+b[3]}; }
static inline f32x4_t intel_hsub_f32x4(f32x4_t a, f32x4_t b) { return (f32x4_t){a[0]-a[1],a[2]-a[3],b[0]-b[1],b[2]-b[3]}; }
static inline f64x4_t intel_hsub_f64x4(f64x4_t a, f64x4_t b) { return (f64x4_t){a[0]-a[1],b[0]-b[1],a[2]-a[3],b[2]-b[3]}; }
#else

static inline f32x4_t intel_hadd_f32x4(f32x4_t a, f32x4_t b)
{
  __m128 ia = type_pun(a,__m128);
  __m128 ib = type_pun(b,__m128);
  return type_pun(_mm_hadd_ps(ia,ib), f32x4_t);
}

static inline f64x4_t intel_hadd_f64x4(f64x4_t a, f64x4_t b)
{
  __m256d ia = type_pun(a,__m256d);
  __m256d ib = type_pun(b,__m256d);
  return type_pun(_mm256_hadd_pd(ia,ib), f64x4_t);
}

static inline f32x4_t intel_hsub_f32x4(f32x4_t a, f32x4_t b)
{
  __m128 ia = type_pun(a,__m128);
  __m128 ib = type_pun(b,__m128);
  return type_pun(_mm_hsub_ps(ia,ib), f32x4_t);
}

static inline f64x4_t intel_hsub_f64x4(f64x4_t a, f64x4_t b)
{
  __m256d ia = type_pun(a,__m256d);
  __m256d ib = type_pun(b,__m256d);
  return type_pun(_mm256_hsub_pd(ia,ib), f64x4_t);
}

#endif

// sum the abs of the elements
static inline float  vec2f_asum(vec2f_t a) { return ssimd_abs(a[0]) + ssimd_abs(a[1]); }
static inline double vec2d_asum(vec2d_t a) { return ssimd_abs(a[0]) + ssimd_abs(a[1]); }
static inline float  vec3f_asum(vec3f_t a) { return ssimd_abs(a[0]) + ssimd_abs(a[1]) + ssimd_abs(a[2]); }
static inline double vec3d_asum(vec3d_t a) { return ssimd_abs(a[0]) + ssimd_abs(a[1]) + ssimd_abs(a[2]); }
static inline float  quatf_asum(quatf_t a) { return (ssimd_abs(a[0]) + ssimd_abs(a[1])) + (ssimd_abs(a[2]) + ssimd_abs(a[3])); }
static inline double quatd_asum(quatd_t a) { return (ssimd_abs(a[0]) + ssimd_abs(a[1])) + (ssimd_abs(a[2]) + ssimd_abs(a[3])); }

#define vec2_asum(a) vec2_fwd(asum, a)
#define vec3_asum(a) vec3_fwd(asum, a)
#define quat_asum(a) quat_fwd(asum, a)

// sum of absolute difference (the L^1 norm)
#define vec2_sad(a,b) ({typeof(a) _a=a,_b=b; _a-=_b; vec2_asum(_a);})
#define vec3_sad(a,b) ({typeof(a) _a=a,_b=b; _a-=_b; vec3_asum(_a);})
#define quat_sad(a,b) ({typeof(a) _a=a,_b=b; _a-=_b; quat_asum(_a);})

//*******************************************************
//
//

// (a & s) | (b & (~s)) : compiler needs to "know" 's' for this to
// lower into any blend hardware op.

#define ssimd_blend_i(a,b,c) ssimd_bitcast_if((ssimd_bitcast_fi(a)&s)|(ssimd_bitcast_fi(b)& (~s))); 

static inline vec2f_t vec2f_blend(vec2f_t a, vec2f_t b, i32x2_t s) { return ssimd_blend_i(a,b,s); }
static inline vec2d_t vec2d_blend(vec2d_t a, vec2d_t b, i64x2_t s) { return ssimd_blend_i(a,b,s); }
static inline quatf_t quatf_blend(quatf_t a, quatf_t b, i32x4_t s) { return ssimd_blend_i(a,b,s); }
static inline quatd_t quatd_blend(quatd_t a, quatd_t b, i64x4_t s) { return ssimd_blend_i(a,b,s); }
static inline vec3f_t vec3f_blend(vec3f_t a, vec3f_t b, i32x4_t s) { return ssimd_blend_i(a,b,s); }
static inline vec3d_t vec3d_blend(vec3d_t a, vec3d_t b, i64x4_t s) { return ssimd_blend_i(a,b,s); }

#define vec2_blend(a,b,s) vec2_fwd(blend, a,b,s)
#define vec3_blend(a,b,s) vec3_fwd(blend, a,b,s)
#define quat_blend(a,b,s) quat_fwd(blend, a,b,s)

#define ssimd_broadcast_intof(a,s) ((typeof(ssimd_bitcast_fi(a))){0}+s)


//*******************************************************
// end-point exact linear interpolation (LERP)
// (1-t)a + tb = a-ta+tb = (tb-(ta-a))

#define ssimd_lerp_i(type,A,B,T) type##_fma(B,T,-type##_fma(A,T,-A))

// LERP with vector (per element) parameterization
static inline vec2f_t vec2f_vlerp(vec2f_t a, vec2f_t b, vec2f_t t) { return ssimd_lerp_i(vec2f,a,b,t); }
static inline vec2d_t vec2d_vlerp(vec2d_t a, vec2d_t b, vec2d_t t) { return ssimd_lerp_i(vec2d,a,b,t); }
static inline vec3f_t vec3f_vlerp(vec3f_t a, vec3f_t b, vec3f_t t) { return ssimd_lerp_i(vec3f,a,b,t); }
static inline vec3d_t vec3d_vlerp(vec3d_t a, vec3d_t b, vec3d_t t) { return ssimd_lerp_i(vec3d,a,b,t); }
static inline quatf_t quatf_vlerp(vec3f_t a, vec3f_t b, vec3f_t t) { return ssimd_lerp_i(quatf,a,b,t); }
static inline quatd_t quatd_vlerp(quatd_t a, quatd_t b, quatd_t t) { return ssimd_lerp_i(quatd,a,b,t); }

// LERP with scalar parameterization
// • `vec3` are using `quat_broadcast` on purpose (SEE: vec3_broadcast)
// • `quat_nearest_lerp` is later in file.
static inline vec2f_t vec2f_lerp(vec2f_t a, vec2f_t b, float   t) { return vec2f_vlerp(a,b,vec2f_broadcast(t)); }
static inline vec2d_t vec2d_lerp(vec2d_t a, vec2d_t b, double  t) { return vec2d_vlerp(a,b,vec2d_broadcast(t)); }
static inline vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, float   t) { return vec3f_vlerp(a,b,quatf_broadcast(t)); } // (1)
static inline vec3d_t vec3d_lerp(vec3d_t a, vec3d_t b, double  t) { return vec3d_vlerp(a,b,quatd_broadcast(t)); } // (1)
static inline quatf_t quatf_lerp(vec3f_t a, quatf_t b, float   t) { return quatf_vlerp(a,b,quatf_broadcast(t)); }
static inline quatd_t quatd_lerp(quatd_t a, quatd_t b, double  t) { return quatd_vlerp(a,b,quatd_broadcast(t)); }

#define vec2_vlerp(a,b,t) vec2_fwd(vlerp,a,b,t)
#define vec3_vlerp(a,b,t) vec3_fwd(vlerp,a,b,t)
#define quat_vlerp(a,b,t) quat_fwd(vlerp,a,b,t)

#define vec2_lerp(a,b,t) vec2_fwd(lerp,a,b,t)
#define vec3_lerp(a,b,t) vec3_fwd(lerp,a,b,t)
#define quat_lerp(a,b,t) quat_fwd(lerp,a,b,t)

//*******************************************************
// weighted sum: a*sa + b*sb = fma(a,sa,b*sb)
// vwsum: sb & sb are both vectors
// wsum:  sb & sb are both scalars

#define ssimd_vwsum_i(type,A,B,SA,SB) type##_fma(A,SA,B*SB)

// vec3 redirecting to quat is on purpose
static inline vec2f_t vec2f_vwsum(vec2f_t a, vec2f_t b, vec2f_t sa, vec2f_t sb) { return ssimd_vwsum_i(vec2f,a,b,sa,sb); }
static inline vec2d_t vec2d_vwsum(vec2d_t a, vec2d_t b, vec2d_t sa, vec2d_t sb) { return ssimd_vwsum_i(vec2d,a,b,sa,sb); }
static inline quatf_t quatf_vwsum(vec3f_t a, vec3f_t b, vec3f_t sa, vec3f_t sb) { return ssimd_vwsum_i(quatf,a,b,sa,sb); }
static inline quatd_t quatd_vwsum(quatd_t a, quatd_t b, quatd_t sa, quatd_t sb) { return ssimd_vwsum_i(quatd,a,b,sa,sb); }
static inline vec3f_t vec3f_vwsum(vec3f_t a, vec3f_t b, vec3f_t sa, vec3f_t sb) { return quatf_vwsum(a,b,sa,sb); }
static inline vec3d_t vec3d_vwsum(vec3d_t a, vec3d_t b, vec3d_t sa, vec3d_t sb) { return quatd_vwsum(a,b,sa,sb); }

#define vec2_vwsum(a,b,sa,sb) vec2_fwd(vwsum, a,b,sa,sb)
#define vec3_vwsum(a,b,sa,sb) vec3_fwd(vwsum, a,b,sa,sb)
#define quat_vwsum(a,b,sa,sb) quat_fwd(vwsum, a,b,sa,sb)


#define ssimd_wsum_x(type,A,B,SA,SB) type##_vwsum(A,B,type##_broadcast(SA),B*type##_broadcast(SB))

static inline vec2f_t vec2f_wsum(vec2f_t a, vec2f_t b, float   sa, float   sb)  { return ssimd_wsum_x(vec2f,a,b,sa,sb); }
static inline vec2d_t vec2d_wsum(vec2d_t a, vec2d_t b, double  sa, double  sb)  { return ssimd_wsum_x(vec2d,a,b,sa,sb); }
static inline quatf_t quatf_wsum(vec3f_t a, vec3f_t b, float   sa, float   sb)  { return ssimd_wsum_x(quatf,a,b,sa,sb); }
static inline quatd_t quatd_wsum(quatd_t a, quatd_t b, double  sa, double  sb)  { return ssimd_wsum_x(quatd,a,b,sa,sb); }
static inline vec3f_t vec3f_wsum(vec3f_t a, vec3f_t b, float   sa, float   sb)  { return quatf_wsum(a,b,sa,sb); }
static inline vec3d_t vec3d_wsum(vec3d_t a, vec3d_t b, double  sa, double  sb)  { return quatd_wsum(a,b,sa,sb); }

#define vec2_wsum(a,b,sa,sb) vec2_fwd(wsum, a,b,sa,sb)
#define vec3_wsum(a,b,sa,sb) vec3_fwd(wsum, a,b,sa,sb)
#define quat_wsum(a,b,sa,sb) quat_fwd(wsum, a,b,sa,sb)


//*******************************************************
//

static const float   ssimd_ulp1_f32       = 0x1.0p-23f;
static const double  ssimd_ulp1_f64       = 0x1.0p-52;
static const float   ssimd_min_normal_f32 = 0x1.0p-126f;
static const double  ssimd_min_normal_f64 = 0x1.0p-1022;
static const int32_t ssimd_sign_bit_f32   = INT32_C(1)<<31;
static const int64_t ssimd_sign_bit_f64   = INT64_C(1)<<63;
static const int32_t ssimd_one_bits_f32   = INT32_C(0x3f800000);
static const int64_t ssimd_one_bits_f64   = INT64_C(0x3ff0000000000000);

static inline float  ssimd_sgn_f32(float  x) { return copysignf(1.f,x); }
static inline double ssimd_sgn_f64(double x) { return copysign (1.0,x); }

#define ssimd_sgn(x)  (_Generic((x),float:ssimd_sgn_f32,default:ssimd_sgn_f64)(x))

// yields F for binary32 and D for binary64
#define ssimd_select_const(x,F,D) \
  ({(x)+__builtin_choose_expr(__builtin_types_compatible_p(typeof(x), float),F,D);})

#define ssimd_min_normal(x) ssimd_select_const(x,ssimd_min_normal_f32,ssimd_min_normal_f64)
#define ssimd_ulp1(x)       ssimd_select_const(x,ssimd_min_normal_f32,ssimd_min_normal_f64)
#define ssimd_sign_bit(x)   ssimd_select_const(x,ssimd_sign_bit_f32,  ssimd_sign_bit_f64)
#define ssimd_one_bits(x)   ssimd_select_const(x,ssimd_one_bits_f32,  ssimd_one_bits_f64)

// returns: x+min_normal
#define ssimd_bias_min_normal(x) ((x)+ssimd_min_normal(x))

// broadcast(sign_bit(s))  (clean these up)
static inline i32x2_t vec2f_broadcast_signbit(float s)
{
  return (i32x2_t){0} + (ssimd_bitcast_fi_32(s) & ssimd_sign_bit_f32);
}

static inline i64x2_t vec2d_broadcast_signbit(double s)
{
  return (i64x2_t){0} + (ssimd_bitcast_fi_64(s) & ssimd_sign_bit_f64);
}

static inline i32x4_t quatf_broadcast_signbit(float s)
{
  return (i32x4_t){0} + (ssimd_bitcast_fi_32(s) & ssimd_sign_bit_f32);
}

static inline i64x4_t quatd_broadcast_signbit(double s)
{
  return (i64x4_t){0} + (ssimd_bitcast_fi_64(s) & ssimd_sign_bit_f64);
}

// flipping the sign bit of zero is OK
static inline i32x4_t vec3f_broadcast_signbit(float  s) { return quatf_broadcast_signbit(s); }
static inline i64x4_t vec3d_broadcast_signbit(double s) { return quatd_broadcast_signbit(s); }


#define vec2_broadcast_signbit(x) ssimd_fwd_sfunc(vec2,broadcast_signbit,x)
#define vec3_broadcast_signbit(x) ssimd_fwd_sfunc(vec3,broadcast_signbit,x)
#define quat_broadcast_signbit(x) ssimd_fwd_sfunc(quat,broadcast_signbit,x)



// these compute a * sgn(s).  much ado about nothing impedance matching:
//  computes: a ^ broadcast(sign_bit(s))

static inline vec2f_t vec2f_mul_sgn(vec2f_t a, float s)
{
  i32x2_t bs = vec2f_broadcast_signbit(s);
  i32x2_t ai = ssimd_bitcast_fi_32x2(a);
  
  return ssimd_bitcast_if_32x2(bs^ai);
}

static inline vec2d_t vec2d_mul_sgn(vec2d_t a, double s)
{
  i64x2_t bs = vec2d_broadcast_signbit(s);
  i64x2_t ai = ssimd_bitcast_fi_64x2(a);
  
  return ssimd_bitcast_if_64x2(bs^ai);
}

static inline quatf_t quatf_mul_sgn(quatf_t a, float s)
{
  i32x4_t bs = quatf_broadcast_signbit(s);
  i32x4_t ai = ssimd_bitcast_fi_32x4(a);
  
  return ssimd_bitcast_if_32x4(bs^ai);
}

static inline quatd_t quatd_mul_sgn(quatd_t a, double s)
{
  i64x4_t bs = quatd_broadcast_signbit(s);
  i64x4_t ai = ssimd_bitcast_fi_64x4(a);
  
  return ssimd_bitcast_if_64x4(bs^ai);
}

// this is fine because having a negative zero 4th element is OK
static inline vec3f_t vec3f_mul_sgn(vec3f_t v, float  s) { return quatf_mul_sgn(v,s); }
static inline vec3d_t vec3d_mul_sgn(vec3d_t v, double s) { return quatd_mul_sgn(v,s); }

#define vec2_mul_sgn(a,b) vec2_fwd(mul_sgn,a,b)
#define vec3_mul_sgn(a,b) vec3_fwd(mul_sgn,a,b)
#define quat_mul_sgn(a,b) quat_fwd(mul_sgn,a,b)

// -conj(a)
#if 0
static inline quatf_t quatf_neg_scalar(quatf_t a)
{
  static const i32x4_t bs = {0,0,0,0x80000000};
  i32x4_t ai = ssimd_bitcast_fi_32x4(a);
  
  return ssimd_bitcast_if_32x4(bs^ai);
}

static inline quatd_t quatd_neg_scalar(quatd_t a)
{
  static const i64x4_t bs = {0,0,0,0x8000000000000000};
  i64x4_t ai = ssimd_bitcast_fi_64x4(a);
  
  return ssimd_bitcast_if_64x4(bs^ai);
}

#define quat_neg_scalar(a) quat_fwd(neg_scalar,a)
#else
#define quat_neg_scalar(a) ({typeof(a) _a=a; _a[3]=-_a[3]; _a;})
#endif


//*******************************************************
// dot product & norm
// dot(a,b) = a•b

// GCC is struggling with both versions of some of these (oh bother) w various options. look closer later

static inline float  vec2f_dot(vec2f_t a, vec2f_t b) { a *= b; return a[0]+a[1]; }
static inline double vec2d_dot(vec2d_t a, vec2d_t b) { a *= b; return a[0]+a[1]; }
static inline float  vec3f_dot(vec3f_t a, vec3f_t b) { a *= b; return a[0]+a[1]+a[2]; }
static inline double vec3d_dot(vec3d_t a, vec3d_t b) { a *= b; return a[0]+a[1]+a[2]; }
static inline float  quatf_dot(quatf_t a, quatf_t b) { a *= b; return (a[0]+a[1])+(a[2]+a[3]); }
static inline double quatd_dot(quatd_t a, quatd_t b) { a *= b; return (a[0]+a[1])+(a[2]+a[3]); }

static inline float  vec2f_dot_fma(vec2f_t a, vec2f_t b) { return fmaf(a[0],b[0],a[1]*b[1]); }
static inline double vec2d_dot_fma(vec2d_t a, vec2d_t b) { return fma (a[0],b[0],a[1]*b[1]); }
static inline float  vec3f_dot_fma(vec3f_t a, vec3f_t b) { return fmaf(a[0],b[0],fmaf(a[1],b[1],a[2]*b[2])); }
static inline double vec3d_dot_fma(vec3d_t a, vec3d_t b) { return fma (a[0],b[0],fma (a[1],b[1],a[2]*b[2])); }
static inline float  quatf_dot_fma(quatf_t a, quatf_t b) { return fmaf(a[0],b[0],a[1]*b[1])+fmaf(a[2],b[2],a[3]*b[3]); }
static inline double quatd_dot_fma(quatd_t a, quatd_t b) { return fma (a[0],b[0],a[1]*b[1])+fma (a[2],b[2],a[3]*b[3]); }

#define vec2_dot_std(a,b) vec2_fwd(dot,a,b)
#define vec3_dot_std(a,b) vec3_fwd(dot,a,b)
#define quat_dot_std(a,b) quat_fwd(dot,a,b)

#define vec2_dot_fma(a,b) vec2_fwd(dot_fma,a,b)
#define vec3_dot_fma(a,b) vec3_fwd(dot_fma,a,b)
#define quat_dot_fma(a,b) quat_fwd(dot_fma,a,b)

#define vec2_dot vec2_dot_std
#define vec3_dot vec3_dot_std
#define quat_dot quat_dot_std

// norm(a) = a•a
#define vec2_norm_std(a) vec2_dot(a,a)
#define vec3_norm_std(a) vec3_dot(a,a)
#define quat_norm_std(a) quat_dot(a,a)
#define vec2_norm_fma(a) vec2_dot_fma(a,a)
#define vec3_norm_fma(a) vec3_dot_fma(a,a)
#define quat_norm_fam(a) quat_dot_fma(a,a)

#define vec2_norm vec2_norm_std
#define vec3_norm vec3_norm_std
#define quat_norm quat_norm_std

// broadcast (a•b) to all elements
// (maybe capture the inputs)
#define vec2_broadcast_dot(a,b) vec2_broadcast(vec2_dot(a,b))
#define vec3_broadcast_dot(a,b) vec3_broadcast(vec3_dot(a,b))
#define quat_broadcast_dot(a,b) quat_broadcast(quat_dot(a,b))

// to all 4 elements 
#define vec3_broadcast_dot_i(a,b) quat_broadcast(vec3_dot(a,b))


// biased_norm(a) = a•a + min_normal
//  the bias can't contribute to the result if 1/2 ulp(a•a) > min_normal
//  (1+u)2^-101 / (1+u)2^-969 for binary32/binary64 respectively with u = ulp(1)
//  cost: transforms a mul into an fma (~zero cost) and constant load
static inline float  vec2f_biased_norm(vec2f_t a) { return fmaf(a[0],a[0],fmaf(a[1],a[1],ssimd_min_normal_f32)); }
static inline double vec2d_biased_norm(vec2d_t a) { return fma (a[0],a[0],fma (a[1],a[1],ssimd_min_normal_f64)); }
static inline float  vec3f_biased_norm(vec3f_t a) { return fmaf(a[0],a[0],fmaf(a[1],a[1],fmaf(a[2],a[2],ssimd_min_normal_f32))); }
static inline double vec3d_biased_norm(vec3d_t a) { return fma (a[0],a[0],fma (a[1],a[1],fma (a[2],a[2],ssimd_min_normal_f64))); }
static inline float  quatf_biased_norm(quatf_t a) { return fmaf(a[0],a[0],a[1]*a[1])+fmaf(a[2],a[2],fmaf(a[3],a[3],ssimd_min_normal_f32)); }
static inline double quatd_biased_norm(quatd_t a) { return fma (a[0],a[0],a[1]*a[1])+fma (a[2],a[2],fma (a[3],a[3],ssimd_min_normal_f64)); }

#define vec2_biased_norm(a) vec2_fwd(biased_norm,a)
#define vec3_biased_norm(a) vec3_fwd(biased_norm,a)
#define quat_biased_norm(a) quat_fwd(biased_norm,a)

// normalize the quantities using biased_norm
// • (approximately) scale factor is correct for input magnitudes > 2^-50/2^-484 for binary32/binary64
// • otherwise the factor is too small and the magnitude of the result decreased until it reaches
//   zero (all zero input is zero output)
// • all finite inputs returns finite results except when (a•a) overflows
// • NOT using ssimd_rsqrt since denormal input is *impossible*
static inline vec2f_t vec2f_normalize(vec2f_t a) { return ssimd_sqrt(1.f/vec2f_biased_norm(a)) * a; }
static inline vec2d_t vec2d_normalize(vec2d_t a) { return ssimd_sqrt(1.f/vec2d_biased_norm(a)) * a; }
static inline vec3f_t vec3f_normalize(vec3f_t a) { return ssimd_sqrt(1.f/vec3f_biased_norm(a)) * a; }
static inline vec3d_t vec3d_normalize(vec3d_t a) { return ssimd_sqrt(1.f/vec3d_biased_norm(a)) * a; }
static inline quatf_t quatf_normalize(quatf_t a) { return ssimd_sqrt(1.f/quatf_biased_norm(a)) * a; }
static inline quatd_t quatd_normalize(quatd_t a) { return ssimd_sqrt(1.f/quatd_biased_norm(a)) * a; }

#define vec2_normalize(a) vec2_fwd(normalize,a)
#define vec3_normalize(a) vec3_fwd(normalize,a)
#define quat_normalize(a) quat_fwd(normalize,a)

// 
static inline float   vec2f_dist_sq(vec2f_t a, vec2f_t b) { a-=b; return vec2_norm_std(a); }
static inline double  vec2d_dist_sq(vec2d_t a, vec2d_t b) { a-=b; return vec2_norm_std(a); }
static inline float   vec3f_dist_sq(vec3f_t a, vec3f_t b) { a-=b; return vec3_norm_std(a); }
static inline double  vec3d_dist_sq(vec3d_t a, vec3d_t b) { a-=b; return vec3_norm_std(a); }
static inline float   quatf_dist_sq(quatf_t a, quatf_t b) { a-=b; return quat_norm_std(a); }
static inline double  quatd_dist_sq(quatd_t a, quatd_t b) { a-=b; return quat_norm_std(a); }

#define vec2_dist_sq(a,b) vec2_fwd(dist_sq,a,b)
#define vec3_dist_sq(a,b) vec3_fwd(dist_sq,a,b)
#define quat_dist_sq(a,b) quat_fwd(dist_sq,a,b)


//**********************************************************
//

// a - 2(a•b)b
// could do fma version but had it lowering to longer...revisit. low priority function AFAIC
static inline vec2f_t vec2f_reflect(vec2f_t a, vec2f_t b) { return a - 2.f*vec2f_dot(a,b)*b; }
static inline vec2d_t vec2d_reflect(vec2d_t a, vec2d_t b) { return a - 2.0*vec2d_dot(a,b)*b; }
static inline vec3f_t vec3f_reflect(vec3f_t a, vec3f_t b) { return a - 2.f*vec3f_dot(a,b)*b; }
static inline vec3d_t vec3d_reflect(vec3d_t a, vec3d_t b) { return a - 2.0*vec3d_dot(a,b)*b; }

#define vec3_reflect(a,b) vec3_fwd(reflect,a,b)
#define vec2_reflect(a,b) vec2_fwd(reflect,a,b)


//**********************************************************
// 2D vector specific & type also implements complex numbers

// othogonal vector: (x,y) → (-y,x)
#define vec2_ortho(v) ({ typeof(v) r = v; (typeof(v)){-r[1],r[0]}; }

// complex conjugate: (x,y) → (x,-y)
#define vec2_conj(v) ({ typeof(v) r = v; r = -r; r[1] = -r[1]; r; })

// complex product:
static inline vec2f_t vec2f_mul(vec2f_t a, vec2f_t b)
{
  return vec2(ssimd_fma(a[0],b[0],-a[1]*b[1]), ssimd_fma(a[1],b[0],a[0]*b[1]));
}

static inline vec2d_t vec2d_mul(vec2d_t a, vec2d_t b)
{
  return vec2(ssimd_fma(a[0],b[0],-a[1]*b[1]), ssimd_fma(a[1],b[0],a[0]*b[1]));
}

#define vec2_mul(a,b) vec2_fwd(mul,a,b)

// SEE cmul below (bivector part as scalar)
static inline float  vec2f_cross(vec2f_t a, vec2f_t b) { return ssimd_fma(a[0],b[1],-a[1]*b[0]); }
static inline double vec2d_cross(vec2d_t a, vec2d_t b) { return ssimd_fma(a[0],b[1],-a[1]*b[0]); }


#define vec2_cross(a,b) vec2_fwd(cross,a,b)

// complex numbers:
// Let a = (ax,ay), b = (bx,by)
// 
// b in the space of a is: 
//   b*conj(a) = (ax bx + ay by, ax by - ay bx)
//             = (a•b, a×b)
// 
// This computes conj(a)*b = b*conj(a)
static inline vec2f_t vec2f_cmul(vec2f_t a, vec2f_t b) { return vec2(vec2_dot(a,b), vec2_cross(a,b)); }
static inline vec2d_t vec2d_cmul(vec2d_t a, vec2d_t b) { return vec2(vec2_dot(a,b), vec2_cross(a,b)); }

#define vec2_cmul(a,b) vec2_fwd(cmul,a,b)


// complex sqrt(a) where |a|=1
static inline vec2f_t vec2f_usqrt(vec2f_t a)
{
  float x  = a[0], y = a[1];
  float m  = x+1.f; m = ssimd_sqrt(m+m);
  float rx = 0.5f * m;
  float ry = y/(m + ssimd_min_normal_f32);

  return vec2(rx,ry);
}

static inline vec2d_t vec2d_usqrt(vec2f_t a)
{
  double x  = a[0], y = a[1];
  double m  = x+1.0; m = ssimd_sqrt(m+m);
  double rx = 0.5 * m;
  double ry = y/(m + ssimd_min_normal_f64);

  return vec2(rx,ry);
}

#define vec2_usqrt(a) vec2_fwd(usqrt,a)


// complex square: (x,y)² = (x²-y², 2xy)
static inline vec2f_t vec2f_sq(vec2f_t a) { return vec2(ssimd_fma(a[0],a[0],-a[1]*a[1]), 2.f*a[0]*a[1]); }
static inline vec2d_t vec2d_sq(vec2d_t a) { return vec2(ssimd_fma(a[0],a[0],-a[1]*a[1]), 2.0*a[0]*a[1]); }

#define vec2_sq(a) vec2_fwd(sq,a)


//**********************************************************
//

static inline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
  // the fourth component is zero for any mixture of
  // quatf_t and vec3f_t types.
  vec3f_t a1 = vec3_shuffle(a, 2,0,1);     // (az,ax,ay)
  vec3f_t b1 = vec3_shuffle(b, 1,2,0);     // (by,bz,bx)
  vec3f_t r0 = a1*b1;                      // (az by,ax bz,ay bx)
  vec3f_t a0 = vec3_shuffle(a, 1,2,0);     // (ay,az,ax)
  vec3f_t b0 = vec3_shuffle(b, 2,0,1);     // (bz,bx,by)
  
  return vec3_fma(a0,b0,-r0);
}

static inline vec3d_t vec3d_cross(vec3d_t a, vec3d_t b)
{
  vec3d_t a1 = vec3_shuffle(a, 2,0,1);
  vec3d_t b1 = vec3_shuffle(b, 1,2,0);
  vec3d_t r0 = a1*b1;
  vec3d_t a0 = vec3_shuffle(a, 1,2,0);
  vec3d_t b0 = vec3_shuffle(b, 2,0,1);
  
  return vec3_fma(a0,b0,-r0);
}

#define vec3_cross(a,b) vec3_fwd(cross,a,b)

// ensure the fourth component is zero
// move to a more logical spot in the file
#define vec3_canonical(v) ({ typeof(v) r = v; r[3] = 0; r; })


// vector triple product: a×(b×c) = (a•c)b-(a•b)c
static inline vec3f_t vec3f_triple_product(vec3f_t a, vec3f_t b, vec3f_t c)
{
  vec3f_t sb = vec3_broadcast_dot_i(a,c);
  vec3f_t sc = vec3_broadcast_dot_i(a,b);
  return vec3_vwsum(b,c,sb,-sc);
}

static inline vec3d_t vec3d_triple_product(vec3d_t a, vec3d_t b, vec3d_t c)
{
  vec3d_t sb = vec3_broadcast_dot_i(a,c);
  vec3d_t sc = vec3_broadcast_dot_i(a,b);
  return vec3_vwsum(b,c,sb,-sc);
}

#define vec3_triple_product(a,b,c) vec3_fwd(triple_product,a,b,c)


// return a unit vector orthogonal to unit vector 'v'
// (expand comments)
static inline vec3f_t vec3f_ortho(vec3f_t v)
{
  float x  = v[0], y = v[1], z = v[2]; 
  float sz = ssimd_sgn(z);
  float a  = y/(z+sz);

  x = -x;

  return vec3(ssimd_fma(a,y,z), x*a, x);
}

static inline vec3d_t vec3d_ortho(vec3d_t v)
{
  double x  = v[0], y = v[1], z = v[2]; 
  double sz = ssimd_sgn(z);
  double a  = y/(z+sz);

  x = -x;

  return vec3(ssimd_fma(a,y,z), x*a, x);
}

#define vec3_ortho(a) vec3_fwd(ortho,a,b)




//**********************************************************
//

// norm of the bivector part
#define quat_bdot(a,b) \
  ({_Generic(x, quatf_t:vec3f_dot, default: vec3d_dot)(a,b);})  

// norm of the bivector part
#define quat_bnorm(q) quat_bdot(q,q)

// complex conjugate: (s,V) → (s,-V)
#define quat_conj(q) ({ typeof(q) r = q; r = -r; r[3] = -r[3]; r; })

// bivector part: (s,V) → V
#define quat_bivector(q) ({ typeof(q) r = q; r[3] = 0; r; })


// if the scalar part (w) is positive returns `q` otherwise `-q`
static inline quatf_t quatf_canonical(quatf_t q) { return quat_blend(q,-q, (quat_broadcast(q[3]) >= 0)); }
static inline quatd_t quatd_canonical(quatd_t q) { return quat_blend(q,-q, (quat_broadcast(q[3]) >= 0)); }

#if 0
// bithack version (should be about the same as above but I'll leave in place for now)
static inline quatf_t quat_canonical(quatf_t q)
{
  i32x4_t s = (quat_broadcast(q[3]) < 0.f) << 31;
  i32x4_t i = ssimd_bitcast_fi(q);
  return ssimd_bitcast_if(i ^ s);
}
#endif

#define quat_canonical(a) quat_fwd(canonical,a)

// nearest equivalent of 'b' to 'a' 
//   dot(a,b) >= 0 ? b : -b 
static inline quatf_t quatf_nearest(quatf_t a, quatf_t b) { return quat_blend(b,-b, (quat_broadcast(quat_dot(a,b)) >= 0)); }
static inline quatd_t quatd_nearest(quatd_t a, quatd_t b) { return quat_blend(b,-b, (quat_broadcast(quat_dot(a,b)) >= 0)); }

#define quat_nearest(a,b) quat_fwd(nearest,a,b)

// LERP between a and {b,-b} (shorter path choice)
static inline quatf_t quatf_lerp_nearest(quatf_t a, quatf_t b, float  s) { return quat_lerp(a, quat_nearest(a,b), s); }
static inline quatd_t quatd_lerp_nearest(quatd_t a, quatd_t b, double s) { return quat_lerp(a, quat_nearest(a,b), s); }

#define quat_nearest_lerp(a,b) quat_fwd(lerp_nearnest,a,b)


// scale bivector part by 's' and replace scalar with 'w'
static inline quatf_t quatf_sb_rs(quatf_t q, float  s, float  w) { q *= s; q[3] = w; return q; }
static inline quatd_t quatd_sb_rs(quatd_t q, double s, double w) { q *= s; q[3] = w; return q; }

#define quat_sb_rs(q,s,w) quat_fwd(sb_rs,q,s,w)


// Q = cos(t)+sin(t)U, |Q|=1
// sqrt(Q) = cos(t/2) + sin(t/2)U
// if 'w' is approaching -1 we're approaching an infinite number of solutions. Need a
// specialized version (as part of the actual problem) that case to to handle since
// U is approaching unknown.
static inline quatf_t quatf_usqrt(quatf_t q)
{
  float d = 1.f+q[3];
  float s = ssimd_rsqrt(d+d);

  return quat_sb_rs(q,s,d*s);
}

static inline quatd_t quatd_usqrt(quatd_t q)
{
  double d = 1.f+q[3];
  double s = ssimd_rsqrt(d+d);

  return quat_sb_rs(q,s,d*s);
}

#define quat_usqrt(q) quat_fwd(usqrt,q)

// q^2 for |q|=1
static inline quatf_t quatf_upow2(quatf_t q)
{
  // validate I didn't screw this up
  float w = q[3];
  float s = 2.f*w;
  
  return quatf_sb_rs(q,s,ssimd_fma(w,s,-1.f));
}

static inline quatd_t quatd_upow2(quatd_t q)
{
  double w = q[3];
  double s = 2.0*w;
  return quatd_sb_rs(q,s,ssimd_fma(w,s,-1.0));
}

#define quat_upow2(q) quat_fwd(upow2,q)


// slerp(a,b,1/2) = b*sqrt(b^* a)
//
// Finds the rotation half-way between 'a' and
// 'b' or '-b' whichever is the shortest path.
// More simply: normalize the midpoint:
// 
// if (dot(a,b) >= 0)
//   return normalize(a+b)
// else
//   return normalize(a-b)

static inline quatf_t quatf_bisect(quatf_t a, quatf_t b)
{
  float   d = quat_dot(a,b);
  i32x4_t s = quat_broadcast_signbit(d);

  // norm(a+b) = 2(1+dot(a+b)) and norm(a-b) = 2(1-dot(a+b))
  // so compute: 2(1+sgn(dot(a,b))
  d  = 2.f + ssimd_bitcast_if_32(ssimd_bitcast_fi_32(d+d) ^ s[0]);

  // a + sgn(dot(a,b))*b  : (a+b) or (a-b)
  a += ssimd_bitcast_if_32x4(ssimd_bitcast_fi_32x4(b) ^ s);
  a *= ssimd_rsqrt(d);

  return a;
}

static inline quatd_t quatd_bisect(quatd_t a, quatd_t b)
{
  double  d = quat_dot(a,b);
  i64x4_t s = quat_broadcast_signbit(d);

  d  = 2.0 + ssimd_bitcast_if_64(ssimd_bitcast_fi_64(d+d) ^ s[0]);
  a += ssimd_bitcast_if_64x4(ssimd_bitcast_fi_64x4(b) ^ s);
  a *= sqrt(1.0/d);

  return a;
}

#define quat_bisect(a,b) quat_fwd(upow2,a,b)


// return 'v' rotated by 'q':  qvq^*
// NOTE: no way to warn about types being backwards since they
// are compatiable.
static inline vec3f_t quatf_rot(quatf_t q, vec3f_t v)
{
#if 1
  // SEE: fgiesen.wordpress.com/2019/02/09/rotating-a-single-vector-using-a-quaternion/
  // mixed cross-product returns zero in the 4th element
  vec3f_t t = vec3_cross(q+q,v);

  v += q[3]*t + vec3_cross(q,t);

  return v;
#else
  float   w  = q[3];
  float   k0 = ssimd_fma(w,w,-0.5f);
  float   k1 = vec3_dot(v,q);
  vec3f_t r  = vec3_wsum(v,q,k0,k1);

  r += w*vec3_cross(q,v);

  return r+r;
#endif
}

static inline vec3d_t quatd_rot(quatd_t q, vec3d_t v)
{
  // revisit as per above
  vec3d_t t = vec3_cross(q+q,v);
  v += q[3]*t + vec3_cross(q,t);
  return v;
}

#define quat_rot(q,v) quat_fwd(rot,q,v)



#if !defined(__clang__)

// scalar implementation: see notes below
// not happy because not bit identical. could be made so
// but that means the alternate is "really" fixed.
static inline quatf_t quatf_mul(quatf_t a, quatf_t b)
{
  float x = a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1];  
  float y = a[3]*b[1] - a[0]*b[2] + a[1]*b[3] + a[2]*b[0];
  float z = a[3]*b[2] + a[0]*b[1] - a[1]*b[0] + a[2]*b[3];
  float w = a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2];

  return quatf(x,y,z,w);
}

static inline quatd_t quatd_mul(quatd_t a, quatd_t b)
{
  double x = a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1];  
  double y = a[3]*b[1] - a[0]*b[2] + a[1]*b[3] + a[2]*b[0];
  double z = a[3]*b[2] + a[0]*b[1] - a[1]*b[0] + a[2]*b[3];
  double w = a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2];

  return quatd(x,y,z,w);
}

#else

// implementing directly like the expansion:
//   (A+aw)(B+bw) = (AxB + bw A + aw B) + aw bw - dot(A,B)
//                = (AxB + bw A + aw B) -(dot(A,B) - aw bw)
// doesn't (or I'm failing) lower well.
// 
//                r
//        r0             r1
//    t0      t1     t2       tn
//   ax bw + aw bx + ay bz - az by
//   az bx + ay bw + aw by - ax bz
//   ax by + az bw + aw bz - ay bx
// -(az bz + ax bx + ay by - aw bw)
//
// uiCA throughput prediction (skylake)
//   clang 17.5 / gcc 29.00 {this}
//   clang 18.0 / gcc 21.50 {scalar version above}
//
// structured to break dep-chains instead of minimizing
// rounding error (nested FMAs)
static inline quatf_t quatf_mul(quatf_t a, quatf_t b)
{
  // use of typeof(a) is to be able to copy-paste between
  // the two bit-width versions (pre C23 auto)

  typeof(a) a0 = quat_shuffle(a, 0,2,0,2);     // [ax   ,az   ,ax   ,az   ] {t0}
  typeof(a) b0 = quat_shuffle(b, 3,0,1,2);     // [   bw,   bx,   by,   bz] {t0}
  typeof(a) a1 = quat_shuffle(a, 3,1,2,0);     // [aw   ,ay   ,az   ,ax   ] {t1}
  typeof(a) b1 = quat_shuffle(b, 0,3,3,0);     // [   bx,   bw,   bw,   bx] {t1}
  typeof(a) r0 = quat_fma(a0,b0,a1*b1);        //                           {r0}
    
  typeof(a) a2 = quat_shuffle(a, 1,3,3,1);     // [ay   ,aw   ,aw   ,ay   ] {t2}
  typeof(a) b2 = quat_shuffle(b, 2,1,2,1);     // [   bz,   by,   bz,   by] {t2}
  typeof(a) an = quat_shuffle(a, 2,0,1,3);     // [az   ,ax   ,ay   ,aw   ] {tn}
  typeof(a) bn = quat_shuffle(b, 1,2,0,3);     // [   by,   bz,   bx,   bw] {tn}
  typeof(a) r1 = quat_fma(a2,b2,-an*bn);       //                           {t2 - tn}

  typeof(a) r  = r0 + r1;                      //                           {r}

  return quat_neg_scalar(r);
}

static inline quatd_t quatd_mul(quatd_t a, quatd_t b)
{
  typeof(a) a0 = quat_shuffle(a, 0,2,0,2);     // [ax   ,az   ,ax   ,az   ] {t0}
  typeof(a) b0 = quat_shuffle(b, 3,0,1,2);     // [   bw,   bx,   by,   bz] {t0}
  typeof(a) a1 = quat_shuffle(a, 3,1,2,0);     // [aw   ,ay   ,az   ,ax   ] {t1}
  typeof(a) b1 = quat_shuffle(b, 0,3,3,0);     // [   bx,   bw,   bw,   bx] {t1}
  typeof(a) r0 = quat_fma(a0,b0,a1*b1);        //                           {r0}
    
  typeof(a) a2 = quat_shuffle(a, 1,3,3,1);     // [ay   ,aw   ,aw   ,ay   ] {t2}
  typeof(a) b2 = quat_shuffle(b, 2,1,2,1);     // [   bz,   by,   bz,   by] {t2}

  typeof(a) an = quat_shuffle(a, 2,0,1,3);     // [az   ,ax   ,ay   ,aw   ] {tn}
  typeof(a) bn = quat_shuffle(b, 1,2,0,3);     // [   by,   bz,   bx,   bw] {tn}
  typeof(a) r1 = quat_fma(a2,b2,-an*bn);       //                           {t2 - tn}

  typeof(a) r  = r0 + r1;

  return quat_neg_scalar(r);
}

#endif

#define quat_mul(a,b) quat_fwd(mul,a,b)


// unit quaterions Q and Ql where Q=QlQr: solve for Qr
#define quat_solve_qr(q,ql) quat_mul(quat_conj(ql),q)

// unit quaterions Q and Qr where Q=QlQr: solve for Ql
#define quat_solve_ql(q,qr) quat_mul(q,quat_conj(qr))


// sqrt(ba^*)
// Find the minimal angle rotation that maps 'a' to 'b'.
// Requires b not approaching -a (infinite solution case)
//   dot(a,b) > -0.9 : fine
// SEE:
//  (from vectors) https://marc-b-reynolds.github.io/quaternions/2018/06/12/SolveQRotVect.html
//  (from quats)   https://marc-b-reynolds.github.io/quaternions/2016/08/09/TwoNormToRot.html
//  the quaternion post has more details
static inline quatf_t quatf_from_normals(vec3f_t a, vec3f_t b)
{
  float   d = 1.f+vec3_dot(a,b);
  float   s = ssimd_rsqrt(d+d);
  quatf_t v = vec3_cross(a,b);

  v[3] = d;

  return s*v;
}

static inline quatd_t quatd_from_normals(vec3d_t a, vec3d_t b)
{
  double  d = 1.0+vec3_dot(a,b);
  double  s = ssimd_rsqrt(d+d);
  quatd_t v = vec3_cross(a,b);

  v[3] = d;

  return s*v;
}

#define quat_from_normals(a,b) quat_fwd(from_normals,a,b)


// factor the "twist" component of 'q' in reference direction 'a'
// (where both are unit magnitude)
//    Q = Qs*Qt (standard order, Qs = swing, Qt = twist)
// to compute Qt & Qs from above convention use:
//    qt = quat_factor_twist(q,v)
//    qs = quat_solve_ql(q,qt)
// for the opposite (Q=Qt*Qs) then 'qs':
//    qs = quat_solve_qr(q,qt)
// (point to specialized version after I adapt)
// (this isn't very efficient. esp for computing both)
static inline quatf_t quatf_factor_twist(quatf_t q, vec3f_t a)
{
  vec3f_t t = vec3_dot(q,a)*a;
  return quat_normalize(quat_bs(t,q[3]));
}

static inline quatd_t quatd_factor_twist(quatd_t q, vec3d_t a)
{
  vec3d_t t = vec3_dot(q,a)*a;
  return quat_normalize(quat_bs(t,q[3]));
}

#define quat_factor_twist(a,b) quat_fwd(factor_twist,a,b)



//**********************************************************
// quat_map_{a}2{b} rotates bivector part a to b:  let t=sqrt(ba^*) q=tat^*
//   this is for variable rename. implement WRT to one and expand other variants.
//   (point to an example of doing this)
#define quat_map_i(a,b,c)  typeof(q) r = quat_shuffle(q,a,b,c,3)

#define quat_map_x2y(q) ({ quat_map_i(1,0,2); r[0] = -r[0]; r;})
#define quat_map_x2z(q) ({ quat_map_i(2,1,0); r[0] = -r[0]; r;})
#define quat_map_y2x(q) ({ quat_map_i(1,0,2); r[1] = -r[1]; r;})
#define quat_map_y2z(q) ({ quat_map_i(0,2,1); r[1] = -r[1]; r;})
#define quat_map_z2x(q) ({ quat_map_i(2,1,0); r[2] = -r[2]; r;})
#define quat_map_z2y(q) ({ quat_map_i(0,2,1); r[2] = -r[2]; r;})


//**********************************************************
// some maps between unit quaterions and 3D unit ball
// SEE:
//   https://marc-b-reynolds.github.io/quaternions/2016/05/30/QuatHDAngleCayley.html
//   https://marc-b-reynolds.github.io/quaternions/2017/05/02/QuatQuantPart1.html

// sqrt(Q), |Q|=1, Q.1 >= 0, return bivector part
static inline vec3f_t quatf_fha(quatf_t q)
{
  float d = 1.f + q[3];
  float s = ssimd_rsqrt(d+d);

  q *= s; q[3] = 0;
  
  return q;
}

static inline vec3d_t quatd_fha(quatd_t q)
{
  double d = 1.f + q[3];
  double s = ssimd_rsqrt(d+d);

  q *= s; q[3] = 0;
  
  return q;
}

// inverse half-angle from bivector part
static inline quatf_t quatf_iha(vec3f_t v)
{
  float d = vec3_norm(v);
  float s = ssimd_sqrt(1.f-d);

  return quat_sb_rs(v, s+s, 1.0f-(d+d));
}

static inline quatd_t quatd_iha(vec3d_t v)
{
  double d = vec3_norm(v);
  double s = ssimd_sqrt(1.f-d);

  return quat_sb_rs(v, s+s, 1.0f-(d+d));
}

#define quat_fha(a) quat_fwd(fha,a)
#define quat_iha(a) quat_fwd(iha,a)


// forward Cayley: |Q|=1, Q.1 >= 0
static inline vec3f_t quatf_fct(quatf_t q)
{
  q *= 1.f/(1.f+q[3]); q[3] = 0;

  return q;
}

static inline vec3d_t quatd_fct(quatd_t q)
{
  q *= 1.f/(1.f+q[3]); q[3] = 0;

  return q;
}

// inverse Cayley: Q.1 = 0
static inline quatf_t quatf_ict(vec3f_t v)
{
  float s = 2.f/(1.f+vec3_norm(v));
  return quat_sb_rs(v, s, s-1.f);
}

static inline quatd_t quatd_ict(vec3d_t v)
{
  double s = 2.f/(1.f+vec3_norm(v));
  return quat_sb_rs(v, s, s-1.f);
}

#define quat_fct(a) quat_fwd(fct,a)
#define quat_ict(a) quat_fwd(ict,a)


// harmonic mean:  (q-1)(q+1)^-1
static inline vec3f_t quatf_fhm(quatf_t q)
{
  static const float K = 0x1.3504f4p1f;    // sqrt(2)+1
  float s = K/(1.f+q[3]+ssimd_sqrt(2.f+2.f*q[3]));

  q *= s; q[3] = 0;
  
  return q;
}

static inline vec3d_t quatd_fhm(quatd_t q)
{
  static const double K = 0x1.3504f333f9de6p1; // sqrt(2)+1
  double s = K/(1.0+q[3]+ssimd_sqrt(2.0+2.0*q[3]));

  q *= s; q[3] = 0;
  
  return q;
}

static inline quatf_t quatf_ihm(vec3f_t v)
{
  static const float K0 = 4.f*0x1.a8279ap-2f;  // 4(sqrt(2)-1)
  static const float K1 = 0x1.5f619ap-3f;      // 3-2*sqrt(2)

  float d = K1*vec3_norm(v);
  float a = (1.f+d);
  float b = (1.f-d)*K0;
  float c = 1.f/(a*a);

  return quat_bs(v*b*c, (1.f+d*(d-6.f))*c);
}

static inline quatd_t quatd_ihm(vec3d_t v)
{
  static const double K0 = 4.0*0x1.a827999fcef32p-2; // 4(sqrt(2)-1)
  static const double K1 = 0x1.5f619980c4337p-3;     // 3-2*sqrt(2)

  double d = K1*vec3_norm(v);
  double a = (1.0+d);
  double b = (1.0-d)*K0;
  double c = 1.0/(a*a);

  return quat_bs(v*b*c, (1.0+d*(d-6.0))*c);
}


#define quat_fhm(a) quat_fwd(fhm,a)
#define quat_ihm(a) quat_fwd(ihm,a)


#if defined(SIMD_IMPLEMENTATION)
#else
#endif

#else
#error "requires extended C features"
#endif

//**********************************************************

