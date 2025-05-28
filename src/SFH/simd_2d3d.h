// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

// Logically extends `simd.h` with some 2D & 3D types. Meaning that
// it can be used either in isolation or with `simd.h` (expand a bit)
// If `simd.h` isn't include then:
// • There's no attempt compiler option inspection. (expand)
// • note other stuff

// Usage: include this header (+simd.h):
// • `SIMD_IMPLEMENTATION`
// • make real note

// NOTE:
// • Since both 3D vectors and quaternions are 4 element vectors
//   the routines can't distinguish between them.
// • 
// •

// TODO: (tons)
// • big functions being inlines is temp hack

// temp hack
//#pragma once

#if defined(__GNUC__)

#if !defined(SFH_SIMD_H)
#include <stdint.h>
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
typedef int32_t i32x2_t SSIMD_TYPE_ATTR(32,2);
typedef int64_t i64x2_t SSIMD_TYPE_ATTR(64,2);
typedef int32_t i32x4_t SSIMD_TYPE_ATTR(32,4);
typedef int64_t i64x4_t SSIMD_TYPE_ATTR(64,4);

// argcount CAT (temp hack: should be nuked)
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

#ifndef type_pun
#define type_pun(X,TYPE) ({			    \
 static_assert(sizeof(X) == sizeof(TYPE),"size mismatch"); \
  typeof(X) __x = (X);                              \
  TYPE __d;                                         \
  memcpy(&__d, &__x, sizeof(TYPE));                 \
  __d;                                              \
})
#endif
#endif

// 3D vector is stored 4 element
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
// • 


//*******************************************************
// basic support functionality

// "constructor" like functions
static inline vec2f_t vec2f(float  x, float  y)                     { return (vec2f_t){x,y}; }
static inline vec2d_t vec2d(double x, double y)                     { return (vec2d_t){x,y}; }
static inline vec3f_t vec3f(float  x, float  y, float  z)           { return (vec3f_t){x,y,z,0}; }
static inline vec3d_t vec3d(double x, double y, double z)           { return (vec3d_t){x,y,z,0}; }
static inline quatf_t quatf(float  x, float  y, float  z, float  w) { return (quatf_t){x,y,z,w}; }
static inline quatd_t quatd(double x, double y, double z, double w) { return (quatd_t){x,y,z,w}; }

// set from bivector + scalar: Q = (b,s)
static inline quatf_t quatf_bs(vec3f_t b, float  s) { return (quatf_t){b[0],b[1],b[2],s}; }
static inline quatd_t quatd_bs(vec3d_t b, double s) { return (quatd_t){b[0],b[1],b[2],s}; }

#define quat_bs(b,s) quat_fwd(bs,b,s)

// nerdy accept either way of setting
#define quatf_set(x,...) \
({_Generic(x, quatf_t:quatf_bs, default: quatf)(x __VA_OPT__(,__VA_ARGS__));})

#define quatd_set(x,...)                                                \
({_Generic(x, quatd_t:quatd_bs, default: quatd)(x __VA_OPT__(,__VA_ARGS__));})


// expand generic expression which forward to a function and first parameter is a scalar
// (no need to perform any parameter capturing)
#define ssimid_fwd_sfunc(type, name,x,...)  \
  ({_Generic(x, float:type##f_##name, default:type##d_##name)(x __VA_OPT__(,__VA_ARGS__));})

// same but first parameter must be vector type
#define vec2_fwd(name,x,...) ({_Generic(x, vec2f_t:vec2f_##name, default:vec2d_##name)(x __VA_OPT__(,__VA_ARGS__));})
#define vec3_fwd(name,x,...) ({_Generic(x, vec3f_t:vec3f_##name, default:vec3d_##name)(x __VA_OPT__(,__VA_ARGS__));})
#define quat_fwd(name,x,...) ({_Generic(x, quatf_t:quatf_##name, default:quatd_##name)(x __VA_OPT__(,__VA_ARGS__));})

static inline vec2f_t vec2f_broadcast(float  v) { return vec2f(v,v); }
static inline vec2d_t vec2d_broadcast(double v) { return vec2d(v,v); }
static inline vec3f_t vec3f_broadcast(float  v) { return vec3f(v,v,v); }
static inline vec3d_t vec3d_broadcast(double v) { return vec3d(v,v,v); }
static inline quatf_t quatf_broadcast(float  v) { return quatf(v,v,v,v); }
static inline quatd_t quatd_broadcast(double v) { return quatd(v,v,v,v); }

#define vec2_broadcast(x) ssimid_fwd_sfunc(vec2,broadcast,x)
#define vec3_broadcast(x) ssimid_fwd_sfunc(vec3,broadcast,x)
#define quat_broadcast(x) ssimid_fwd_sfunc(quat,broadcast,x)

// single "register" shuffles
#define vec2_shuffle(V,A,B)     __builtin_shufflevector(V,V,A,B)
#define vec3_shuffle(V,A,B,C)   __builtin_shufflevector(V,V,A,B,C,3)
#define quat_shuffle(Q,A,B,C,D) __builtin_shufflevector(Q,Q,A,B,C,D)

// expansions for vector binary32/binary64 unary/binary functions
// (for defining macro that maps to explict version)
// probably nuke the CAT macros
// KILL these
#define ssimd_def_ufunc(type,W,name,A) ({    \
  typeof(A) _a = A;                          \
    _Generic(_a,                             \
      CAT(f32x,W,_t): CAT(type,f_,name),     \
      CAT(f64x,W,_t): CAT(type,d_,name))     \
      (_a);                                  \
})

#define ssimd_def_bfunc(type,W,name,A,B) ({ \
  typeof(A) _a = A;                         \
  typeof(B) _b = B;                         \
    _Generic(_a,                            \
      CAT(f32x,W,_t): CAT(type,f_,name),    \
      CAT(f64x,W,_t): CAT(type,d_,name))    \
      (_a,_b);                              \
})

#define ssimd_def_3func(type,W,name,A,B) ({ \
  typeof(A) _a = A;                         \
  typeof(B) _b = B;                         \
  typeof(B) _c = C;                         \
    _Generic(_a,                            \
      CAT(f32x,W,_t): CAT(type,f_,name),    \
      CAT(f64x,W,_t): CAT(type,d_,name))    \
      (_a,_b);                              \
})

// kill these
#define vec2_def_ufunc(name,A) ssimd_def_ufunc(vec2,2,name,A)
#define vec3_def_ufunc(name,A) ssimd_def_ufunc(vec3,4,name,A)
#define quat_def_ufunc(name,A) ssimd_def_ufunc(quat,4,name,A)

#define vec2_def_bfunc(name,A,B) ssimd_def_bfunc(vec2,2,name,A,B)
#define vec3_def_bfunc(name,A,B) ssimd_def_bfunc(vec3,4,name,A,B)
#define quat_def_bfunc(name,A,B) ssimd_def_bfunc(quat,4,name,A,B)

#define vec2_def_3func(name,A,B,C) ssimd_def_3func(vec2,2,name,A,B)
#define vec3_def_3func(name,A,B,C) ssimd_def_3func(vec3,4,name,A,B,C)
#define quat_def_3func(name,A,B,C) ssimd_def_3func(quat,4,name,A,B,C)

// promote/demote (widen/narrow) between binary32/binary64 elements
static inline vec2d_t vec2f_promote(vec2f_t v) { return __builtin_convertvector(v,vec2d_t); }
static inline vec2f_t vec2d_demote (vec2d_t v) { return __builtin_convertvector(v,vec2f_t); }
static inline vec3d_t vec3f_promote(vec3f_t v) { return __builtin_convertvector(v,vec3d_t); }
static inline vec3f_t vec3d_demote (vec3d_t v) { return __builtin_convertvector(v,vec3f_t); }
static inline quatd_t quatf_promote(quatf_t q) { return __builtin_convertvector(q,quatd_t); }
static inline quatf_t quatd_demote (quatd_t q) { return __builtin_convertvector(q,quatf_t); }

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


// float → int (bit conversion)
static inline i32x2_t vec2f_as_int(vec2f_t x) { return type_pun(x, i32x2_t); }
static inline i64x2_t vec2d_as_int(vec2d_t x) { return type_pun(x, i64x2_t); }
static inline i32x4_t quatf_as_int(quatf_t x) { return type_pun(x, i32x4_t); }
static inline i64x4_t quatd_as_int(quatd_t x) { return type_pun(x, i64x4_t); }
static inline i32x4_t vec3f_as_int(vec3f_t x) { return quatf_as_int(x);      }
static inline i64x4_t vec3d_as_int(vec3d_t x) { return quatd_as_int(x);      }

#define vec2_as_int(x) vec2_fwd(as_int,a,b,c)
#define vec3_as_int(x) vec3_fwd(as_int,a,b,c)
#define quat_as_int(x) quat_fwd(as_int,a,b,c)


// int → float (bit conversion)
static inline vec2f_t vec2f_from_int(i32x2_t x) { return type_pun(x, vec2f_t); }
static inline vec2d_t vec2d_from_int(i64x2_t x) { return type_pun(x, vec2d_t); }
static inline quatf_t quatf_from_int(i32x4_t x) { return type_pun(x, quatf_t); }
static inline quatd_t quatd_from_int(i64x4_t x) { return type_pun(x, quatd_t); }
static inline vec3f_t vec3f_from_int(i32x4_t x) { return quatf_from_int(x);    }
static inline vec3d_t vec3d_from_int(i64x4_t x) { return quatd_from_int(x);    }

#define vec2_from_int(x) ({ _Generic(x, i32x2_t:vec2f_from_int, default:vec2d_from_int)(x); })
#define vec3_from_int(x) ({ _Generic(x, i32x4_t:vec3f_from_int, default:vec3d_from_int)(x); })
#define quat_from_int(x) ({ _Generic(x, i32x4_t:quatf_from_int, default:quatd_from_int)(x); })


//*******************************************************
//
//

// (a & s) | (b & (~s)) : compiler needs to "know" 's' for this to
// lower into any blend hardware op.
static inline vec2f_t vec2f_blend(vec2f_t a, vec2f_t b, i32x2_t s)
{
  return vec2f_from_int((vec2f_as_int(a)&s)|(vec2f_as_int(b)& (~s))); 
}

static inline vec2d_t vec2d_blend(vec2d_t a, vec2d_t b, i64x2_t s)
{
  return vec2d_from_int((vec2d_as_int(a)&s)|(vec2d_as_int(b)& (~s))); 
}

static inline quatf_t quatf_blend(quatf_t a, quatf_t b, i32x4_t s)
{
  return quatf_from_int((quatf_as_int(a)&s)|(quatf_as_int(b)& (~s))); 
}

static inline quatd_t quatd_blend(quatd_t a, quatd_t b, i64x4_t s)
{
  return quatd_from_int((quatd_as_int(a)&s)|(quatd_as_int(b)& (~s))); 
}

static inline vec3f_t vec3f_blend(vec3f_t a, vec3f_t b, i32x4_t s) { return quatf_blend(a,b,s); }
static inline vec3d_t vec3d_blend(vec3d_t a, vec3d_t b, i64x4_t s) { return quatd_blend(a,b,s); }

#define vec2_blend(a,b,s) vec2_fwd(blend, a,b,s)
#define vec3_blend(a,b,s) vec3_fwd(blend, a,b,s)
#define quat_blend(a,b,s) quat_fwd(blend, a,b,s)


//*******************************************************
// end-point exact linear interpolation (LERP)
//

#define ssimd_lerp_i(type,A,B,T) type##_fma(B,T,type##_fma(A,T,-A))

static inline vec2f_t vec2f_vlerp(vec2f_t a, vec2f_t b, vec2f_t t) { return ssimd_lerp_i(vec2f,a,b,t); }
static inline vec2d_t vec2d_vlerp(vec2d_t a, vec2d_t b, vec2d_t t) { return ssimd_lerp_i(vec2d,a,b,t); }
static inline vec3f_t vec3f_vlerp(vec3f_t a, vec3f_t b, vec3f_t t) { return ssimd_lerp_i(vec3f,a,b,t); }
static inline vec3d_t vec3d_vlerp(vec3d_t a, vec3d_t b, vec3d_t t) { return ssimd_lerp_i(vec3d,a,b,t); }
static inline quatf_t quatf_vlerp(vec3f_t a, vec3f_t b, vec3f_t t) { return ssimd_lerp_i(quatf,a,b,t); }
static inline quatd_t quatd_vlerp(quatd_t a, quatd_t b, quatd_t t) { return ssimd_lerp_i(quatd,a,b,t); }

static inline vec2f_t vec2f_lerp(vec2f_t a, vec2f_t b, float   t) { return vec2f_vlerp(a,b,vec2f_broadcast(t)); }
static inline vec2d_t vec2d_lerp(vec2d_t a, vec2d_t b, double  t) { return vec2d_vlerp(a,b,vec2d_broadcast(t)); }
static inline vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, float   t) { return vec3f_vlerp(a,b,vec3f_broadcast(t)); }
static inline vec3d_t vec3d_lerp(vec3d_t a, vec3d_t b, double  t) { return vec3d_vlerp(a,b,vec3d_broadcast(t)); }
static inline quatf_t quatf_lerp(vec3f_t a, quatf_t b, float   t) { return quatf_vlerp(a,b,quatf_broadcast(t)); }
static inline quatd_t quatd_lerp(quatd_t a, quatd_t b, double  t) { return quatd_vlerp(a,b,quatd_broadcast(t)); }

#define vec2_vlerp(a,b,t) vec2_fwd(vlerp,a,b,t)
#define vec3_vlerp(a,b,t) vec3_fwd(vlerp,a,b,t)
#define quat_vlerp(a,b,t) quat_fwd(vlerp,a,b,t)

#define vec2_lerp(a,b,t) vec2_fwd(lerp,a,b,t)
#define vec3_lerp(a,b,t) vec3_fwd(lerp,a,b,t)
#define quat_lerp(a,b,t) quat_fwd(lerp,a,b,t)

//*******************************************************
// 
//

// a*sa + b*sb = fma(a,sa,b*sb)
// vwsum: sb & sb are both vectors
// wsum:  sb & sb are both scalars

#define ssimd_vwsum_i(type,A,B,SA,SB) type##_fma(A,SA,B*SB)

static inline vec2f_t vec2f_vwsum(vec2f_t a, vec2f_t b, vec2f_t sa, vec2f_t sb) { return ssimd_vwsum_i(vec2f,a,b,sa,sb); }
static inline vec2d_t vec2d_vwsum(vec2d_t a, vec2d_t b, vec2d_t sa, vec2d_t sb) { return ssimd_vwsum_i(vec2d,a,b,sa,sb); }
static inline vec3f_t vec3f_vwsum(vec3f_t a, vec3f_t b, vec3f_t sa, vec3f_t sb) { return ssimd_vwsum_i(vec3f,a,b,sa,sb); }
static inline vec3d_t vec3d_vwsum(vec3d_t a, vec3d_t b, vec3d_t sa, vec3d_t sb) { return ssimd_vwsum_i(vec3d,a,b,sa,sb); }
static inline quatf_t quatf_vwsum(vec3f_t a, vec3f_t b, vec3f_t sa, vec3f_t sb) { return ssimd_vwsum_i(quatf,a,b,sa,sb); }
static inline quatd_t quatd_vwsum(quatd_t a, quatd_t b, quatd_t sa, quatd_t sb) { return ssimd_vwsum_i(quatd,a,b,sa,sb); }

#define vec2_blend(a,b,s) vec2_fwd(blend, a,b,s)
#define vec3_blend(a,b,s) vec3_fwd(blend, a,b,s)
#define quat_blend(a,b,s) quat_fwd(blend, a,b,s)


#define ssimd_wsum_x(type,A,B,SA,SB) type##_vwsum(A,B,type##_broadcast(SA),B*type##_broadcast(SB))

static inline vec2f_t vec2f_wsum(vec2f_t a, vec2f_t b, float   sa, float   sb)  { return ssimd_wsum_x(vec2f,a,b,sa,sb); }
static inline vec2d_t vec2d_wsum(vec2d_t a, vec2d_t b, double  sa, double  sb)  { return ssimd_wsum_x(vec2d,a,b,sa,sb); }
static inline vec3f_t vec3f_wsum(vec3f_t a, vec3f_t b, float   sa, float   sb)  { return ssimd_wsum_x(vec3f,a,b,sa,sb); }
static inline vec3d_t vec3d_wsum(vec3d_t a, vec3d_t b, double  sa, double  sb)  { return ssimd_wsum_x(vec3d,a,b,sa,sb); }
static inline quatf_t quatf_wsum(vec3f_t a, vec3f_t b, float   sa, float   sb)  { return ssimd_wsum_x(quatf,a,b,sa,sb); }
static inline quatd_t quatd_wsum(quatd_t a, quatd_t b, double  sa, double  sb)  { return ssimd_wsum_x(quatd,a,b,sa,sb); }

#define vec2_wsum(a,b,sa,sb) vec2_fwd(wsum, a,b,sa,sb)
#define vec3_wsum(a,b,sa,sb) vec3_fwd(wsum, a,b,sa,sb)
#define quat_wsum(a,b,sa,sb) quat_fwd(wsum, a,b,sa,sb)


//*******************************************************
//

static const float  ssmid_f32_ulp1       = 0x1.0p-23f;
static const double ssimd_f64_ulp1       = 0x1.0p-52;
static const float  ssimd_f32_min_normal = 0x1.0p-126f;
static const double ssimd_f64_min_normal = 0x1.0p-1022;

static inline float  ssimd_f32_sgn(float  x) { return copysignf(1.f,x); }
static inline double ssimd_f64_sgn(double x) { return copysign (1.0,x); }

//*******************************************************
// dot product & norm
// dot(a,b) = a•b

// GCC is struggling with both versions of some of these (oh bother) w various options. look closer later
#if defined(SIMD_DOT_NO_FMA)
static inline float  vec2f_dot(vec2f_t a, vec2f_t b) { return a[0]*b[0]+a[1]*b[1]; }
static inline double vec2d_dot(vec2d_t a, vec2d_t b) { return a[0]*b[0]+a[1]*b[1]; }
static inline float  vec3f_dot(vec3f_t a, vec3f_t b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
static inline double vec3d_dot(vec3d_t a, vec3d_t b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
static inline float  quatf_dot(quatf_t a, quatf_t b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]; }
static inline double quatd_dot(quatd_t a, quatd_t b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]; }
#else
static inline float  vec2f_dot(vec2f_t a, vec2f_t b) { return fmaf(a[0],b[0],a[1]*b[1]); }
static inline double vec2d_dot(vec2d_t a, vec2d_t b) { return fma (a[0],b[0],a[1]*b[1]); }
static inline float  vec3f_dot(vec3f_t a, vec3f_t b) { return fmaf(a[0],b[0],fmaf(a[1],b[1],a[2]*b[2])); }
static inline double vec3d_dot(vec3d_t a, vec3d_t b) { return fma (a[0],b[0],fma (a[1],b[1],a[2]*b[2])); }
static inline float  quatf_dot(quatf_t a, quatf_t b) { return fmaf(a[0],b[0],a[1]*b[1])+fmaf(a[2],b[2],a[3]*b[3]); }
static inline double quatd_dot(quatd_t a, quatd_t b) { return fma (a[0],b[0],a[1]*b[1])+fma (a[2],b[2],a[3]*b[3]); }
#endif

#define vec2_dot(a,b) vec2_fwd(dot,a,b)
#define vec3_dot(a,b) vec3_fwd(dot,a,b)
#define quat_dot(a,b) quat_fwd(dot,a,b)

// norm(a) = a•a
static inline float  vec2f_norm(vec2f_t a) { return vec2f_dot(a,a); }
static inline double vec2d_norm(vec2d_t a) { return vec2d_dot(a,a); }
static inline float  vec3f_norm(vec3f_t a) { return vec3f_dot(a,a); }
static inline double vec3d_norm(vec3d_t a) { return vec3d_dot(a,a); }
static inline float  quatf_norm(quatf_t a) { return quatf_dot(a,a); }
static inline double quatd_norm(quatd_t a) { return quatd_dot(a,a); }

#define vec2_norm(a) vec2_dot(a,a)
#define vec3_norm(a) vec3_dot(a,a)
#define quat_norm(a) quat_dot(a,a)

// biased_norm(a) = a•a + min_normal
//  the bias can't contribute to the result if 1/2 ulp(a•a) > min_normal
//  (1+u)2^-101 / (1+u)2^-969 for binary32/binary64 respectively with u = ulp(1)
//  cost: transforms a mul into an fma (~zero cost) and constant load
static inline float  vec2f_biased_norm(vec2f_t a) { return fmaf(a[0],a[0],fmaf(a[1],a[1],ssimd_f32_min_normal)); }
static inline double vec2d_biased_norm(vec2d_t a) { return fma (a[0],a[0],fma (a[1],a[1],ssimd_f64_min_normal)); }
static inline float  vec3f_biased_norm(vec3f_t a) { return fmaf(a[0],a[0],fmaf(a[1],a[1],fmaf(a[2],a[2],ssimd_f32_min_normal))); }
static inline double vec3d_biased_norm(vec3d_t a) { return fma (a[0],a[0],fma (a[1],a[1],fma (a[2],a[2],ssimd_f64_min_normal))); }
static inline float  quatf_biased_norm(quatf_t a) { return fmaf(a[0],a[0],a[1]*a[1])+fmaf(a[2],a[2],fmaf(a[3],a[3],ssimd_f32_min_normal)); }
static inline double quatd_biased_norm(quatd_t a) { return fma (a[0],a[0],a[1]*a[1])+fma (a[2],a[2],fma (a[3],a[3],ssimd_f64_min_normal)); }

#define vec2_biased_norm(a) vec2_fwd(biased_norm,a)
#define vec3_biased_norm(a) vec3_fwd(biased_norm,a)
#define quat_biased_norm(a) quat_fwd(biased_norm,a)

// normalize the quantities using biased_norm
// • (approximately) scale factor is correct for input magnitudes > 2^-50/2^-484 for binary32/binary64
// • otherwise the factor is too small and the magnitude of the result decreased until it reaches
//   zero (all zero input is zero output)
// • all finite input returns finite results except when (a•a) overflows
static inline vec2f_t vec2f_normalize(vec2f_t a) { return sqrtf(1.f/(vec2f_biased_norm(a))) * a; }
static inline vec2d_t vec2d_normalize(vec2d_t a) { return sqrt (1.0/(vec2d_biased_norm(a))) * a; }
static inline vec3f_t vec3f_normalize(vec3f_t a) { return sqrtf(1.f/(vec3f_biased_norm(a))) * a; }
static inline vec3d_t vec3d_normalize(vec3d_t a) { return sqrt (1.0/(vec3d_biased_norm(a))) * a; }
static inline quatf_t quatf_normalize(quatf_t a) { return sqrtf(1.f/(vec3f_biased_norm(a))) * a; }
static inline quatd_t quatd_normalize(quatd_t a) { return sqrt (1.0/(vec3d_biased_norm(a))) * a; }

#define vec2_normalize(a) vec2_fwd(normalize,a)
#define vec3_normalize(a) vec3_fwd(normalize,a)
#define quat_normalize(a) quat_fwd(normalize,a)


//**********************************************************
//

// a - 2(a•b)b
// could do fma version but had it lowering to longer...revisit. low priority function AFAIK
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

// complex product
static inline vec2f_t vec2f_mul(vec2f_t a, vec2f_t b)
{
  return vec2f(fmaf(a[0],b[0],-a[1]*b[1]), fmaf(a[1],b[0],a[0]*b[1]));
}

static inline vec2d_t vec2d_mul(vec2d_t a, vec2d_t b)
{
  return vec2d(fma(a[0],b[0],-a[1]*b[1]), fma(a[1],b[0],a[0]*b[1]));
}

#define vec2_mul(a,b) vec2_fwd(mul,a,b)

// SEE cmul below (bivector part as scalar)
static inline float vec2f_cross(vec2f_t a, vec2f_t b)
{
  return fmaf(a[0],b[1],-a[1]*b[0]);
}

static inline double vec2d_cross(vec2d_t a, vec2d_t b)
{
  return fma(a[0],b[1],-a[1]*b[0]);
}


#define vec2_cross(a,b) vec2_fwd(cross,a,b)

// complex numbers:
// Let a = (ax,ay), b = (bx,by)
// 
// b in the space of a is: 
//   b*conj(a) = (ax bx + ay by, ax by - ay bx)
//             = (a•b, a×b)
// 
// This computes conj(a)*b = b*conj(a)
static inline vec2f_t vec2f_cmul(vec2f_t a, vec2f_t b)
{
  return vec2f(vec2f_dot(a,b), vec2f_cross(a,b));
}

static inline vec2d_t vec2d_cmul(vec2d_t a, vec2d_t b)
{
  return vec2d(vec2d_dot(a,b), vec2d_cross(a,b));
}

#define vec2_cmul(a,b) vec2_fwd(cross,a,b)


// complex sqrt(a) where |a|=1
static inline vec2f_t vec2f_usqrt(vec2f_t a)
{
  float x  = a[0], y = a[1];
  float m  = x+1.f; m = sqrtf(m+m);
  float rx = 0.5f * m;
  float ry = y/(m + ssimd_f32_min_normal);

  return vec2f(rx,ry);
}

static inline vec2d_t vec2d_usqrt(vec2f_t a)
{
  double x  = a[0], y = a[1];
  double m  = x+1.0; m = sqrt(m+m);
  double rx = 0.5 * m;
  double ry = y/(m + ssimd_f64_min_normal);

  return vec2d(rx,ry);
}

#define vec2_usqrt(a) vec2_fwd(usqrt,a)


// complex square: (x,y)² = (x²-y², 2xy)
static inline vec2f_t vec2f_sq(vec2f_t a)
{
  return vec2f(fmaf(a[0],a[0],-a[1]*a[1]), 2.f*a[0]*a[1]);
}

static inline vec2d_t vec2d_sq(vec2d_t a)
{
  return vec2d(fma(a[0],a[0],-a[1]*a[1]), 2.0*a[0]*a[1]);
}

#define vec2_sq(a) vec2_fwd(sq,a)


//**********************************************************
//

static inline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
  float x = a[1]*b[2] - a[2]*b[1];
  float y = a[2]*b[0] - a[0]*b[2];
  float z = a[0]*b[1] - a[1]*b[0];

  return vec3f(x,y,z);
}

static inline vec3d_t vec3d_cross(vec3d_t a, vec3d_t b)
{
  double x = a[1]*b[2] - a[2]*b[1];
  double y = a[2]*b[0] - a[0]*b[2];
  double z = a[0]*b[1] - a[1]*b[0];

  return vec3d(x,y,z);
}

#define vec3_cross(a,b) vec3_fwd(cross,a,b)




//**********************************************************
//

// complex conjugate: (s,V) → (s,-V)
#define quat_conj(q) ({ typeof(q) r = q; r = -r; r[3] = -r[3]; r; })

// bivector part: (s,V) → V
#define quat_bivector(q) ({ typeof(q) r = q; r[3] = 0; r; })


// if the scalar part (w) is positive returns `q` otherwise `-q`
static inline quatf_t quatf_canonical(quatf_t q)
{
  return quatf_blend(q,-q, (quat_shuffle(q,3,3,3,3) < 0));
}

static inline quatd_t quatd_canonical(quatd_t q)
{
  return quatd_blend(q,-q, (quat_shuffle(q,3,3,3,3) < 0));
}

#define quat_canonical(a) quat_fwd(canonical,a)


// scale bivector part by 's' and replace scalar with 'w'
static inline quatf_t quatf_sb_rs(quatf_t q, float  s, float  w) { q *= s; q[3] = w; return q; }
static inline quatd_t quatd_sb_rs(quatd_t q, double s, double w) { q *= s; q[3] = w; return q; }

#define quat_sb_rs(a) quat_fwd(sb_rs,a)


// Q = cos(t)+sin(t)U, |Q|=1
// sqrt(Q) = cos(t/2) + sin(t/2)U
// if 'w' is approaching -1 we're approaching an infinite number of solutions. Need a
// specialized version (as part of the actual problem) that case to to handle since
// U is approaching unknown.
static inline quatf_t quatf_usqrt(quatf_t q)
{
  float d = 1.f+q[3];
  float s = sqrtf(1.f/(d+d));

  return quatf_sb_rs(q,s,d*s);
}

static inline quatd_t quatd_usqrt(quatd_t q)
{
  double d = 1.0+q[3];
  double s = sqrt(1.0/(d+d));

  return quatd_sb_rs(q,s,d*s);
}

#define quat_usqrt(a) quat_fwd(usqrt,a)


//**********************************************************
// quat_map_{a}2{b} rotates bivector part a to b:  let t=sqrt(ba^*) q=tat^*
//   this is for variable rename. implement WRT to one and expand other variants.
//   (point to an example of doing this)
#define quat_map_i(a,b,c)  typeof(q) r = quat_shuffle(q,a,b,c,3)

#define quat_map_x2y(q) ({ quat_map_i(Y,X,Z,W); r[X] = -r[X]; r;})
#define quat_map_x2z(q) ({ quat_map_i(Z,Y,X,W); r[X] = -r[X]; r;})
#define quat_map_y2x(q) ({ quat_map_i(Y,X,Z,W); r[Y] = -r[Y]; r;})
#define quat_map_y2z(q) ({ quat_map_i(X,Z,Y,W); r[Y] = -r[Y]; r;})
#define quat_map_z2x(q) ({ quat_map_i(Z,Y,X,W); r[Z] = -r[Z]; r;})
#define quat_map_z2y(q) ({ quat_map_i(X,Z,Y,W); r[Z] = -r[Z]; r;})

//**********************************************************





#if defined(SIMD_IMPLEMENTATION)
#else
#endif

#else
#error "requires extended C features"
#endif

//**********************************************************

