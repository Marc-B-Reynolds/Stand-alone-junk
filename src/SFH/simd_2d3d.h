// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once

// types are just aliases and 3D vector is stored 4 element
typedef f32x2_t vec2f_t;
typedef f64x2_t vec2d_t;
typedef f32x4_t vec3f_t;
typedef f64x4_t vec3d_t;
typedef f32x4_t quatf_t;
typedef f64x4_t quatd_t;

#pragma push_macro("X")
#pragma push_macro("Y")
#pragma push_macro("Z")
#pragma push_macro("W")
#undef  X
#undef  Y
#undef  Z
#undef  W

#define X 0
#define Y 1
#define Z 2
#define W 3

// working with a smaller types so use some reduced complexity xxx


// expansions for vector binary32/binary64 unary/binary functions
// (these don't accept scalar promotion of parameters)
// TODO: add the static_asserts
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

// nope. and there should be a more general version in base header


#define quat_shuffle(Q,I0,I1,I2,I3) __builtin_shufflevector(Q,Q,I0,I1,I2,I3)

#define vec2_def_bfunc(name,A,B) ssimd_def_bfunc(vec2,2,name,A,B)
#define vec3_def_bfunc(name,A,B) ssimd_def_bfunc(vec3,4,name,A,B)
#define quat_def_bfunc(name,A,B) ssimd_def_bfunc(quat,4,name,A,B)

// "constructor" like functions
static inline vec2f_t vec2f(float  x, float  y)                     { return (vec2f_t){x,y}; }
static inline vec2d_t vec2d(double x, double y)                     { return (vec2d_t){x,y}; }
static inline vec3f_t vec3f(float  x, float  y, float  z)           { return (vec3f_t){x,y,z,0}; }
static inline vec3d_t vec3d(double x, double y, double z)           { return (vec3d_t){x,y,z,0}; }
static inline quatf_t quatf(float  x, float  y, float  z, float  w) { return (quatf_t){x,y,z,w}; }
static inline quatd_t quatd(double x, double y, double z, double w) { return (quatd_t){x,y,z,w}; }


// promote/demote (widen/narrow) between binary32/binary64 elements
static inline vec2d_t vec2f_promote(vec2f_t v) { return simd_convert(v,vec2d_t); }
static inline vec2f_t vec2d_demote (vec2d_t v) { return simd_convert(v,vec2f_t); }
static inline vec3d_t vec3f_promote(vec3f_t v) { return simd_convert(v,vec3d_t); }
static inline vec3f_t vec3d_demote (vec3d_t v) { return simd_convert(v,vec3f_t); }
static inline quatd_t quatf_promote(quatf_t q) { return simd_convert(v,quatd_t); }
static inline quatf_t quatd_demote (quatd_t q) { return simd_convert(v,quatf_t); }


// dot(a,b) = a•b
static inline float  vec2f_dot(vec2f_t a, vec2f_t b) { return fmaf(a[X],b[X],a[Y]*b[Y]); }
static inline double vec2d_dot(vec2d_t a, vec2d_t b) { return fma (a[X],b[X],a[Y]*b[Y]); }
static inline float  vec3f_dot(vec3f_t a, vec3f_t b) { return fmaf(a[X],b[X],fmaf(a[Y],b[Y],a[Z]*b[Z])); }
static inline double vec3d_dot(vec3d_t a, vec3f_t b) { return fma (a[X],b[X],fma (a[Y],b[Y],a[Z]*b[Z])); }
static inline float  quatf_dot(quatf_t a, quatf_t b) { return fmaf(a[X],b[X],a[Y]*b[Y])+fmaf(a[Z],b[Z],a[W]*b[W]); }
static inline double quatd_dot(quatd_t a, quatd_t b) { return fma (a[X],b[X],a[Y]*b[Y])+fma (a[Z],b[Z],a[W]*b[W]); }

#define vec2_dot(a,b) vec2_def_bfunc(dot,a,b)
#define vec3_dot(a,b) vec3_def_bfunc(dot,a,b)
#define quat_dot(a,b) quat_def_bfunc(dot,a,b)

// norm(a) = a•a
#define vec2f_norm(a) vec2f_dot(a,a)
#define vec3f_norm(a) vec3f_dot(a,a)
#define quatf_norm(a) quatf_dot(a,a)

#define vec2d_norm(a) vec2d_dot(a,a)
#define vec3d_norm(a) vec3d_dot(a,a)
#define quatd_norm(a) quatd_dot(a,a)

#define vec2_norm(a) vec2_dot(a,a)
#define vec3_norm(a) vec3_dot(a,a)
#define quat_norm(a) quat_dot(a,a)


static inline quatf_t quatf_conj(quatf_t q) { q = -q; q[W] = -q[W]; return q; }
static inline quatd_t quatd_conj(quatf_t q) { q = -q; q[W] = -q[W]; return q; }
#define quat_conj(q) quat_def_ufunc(conj,q)

static inline vec3f_t quatf_bivector(quatf_t q) { q[W] = 0; return q; }
static inline vec3d_t quatf_bivector(quatd_t q) { q[W] = 0; return q; }
#define quat_bivector(q) quat_def_ufunc(conj,q)

// quat_map_{a}2{b} rotates bivector part a to b:  let t=sqrt(ba^*) q=tat^*
//   this is for variable rename. implement WRT to one and expand other variants.
//   (point to an example of doing this)
#define quat_map_i(a,b,c)  typeof(q) r = quat_shuffle(q,a,b,c,W)

#define quat_map_x2y(q) ({ quat_map_i(Y,X,Z,W); r[X] = -r[X]; r;})
#define quat_map_x2z(q) ({ quat_map_i(Z,Y,X,W); r[X] = -r[X]; r;})
#define quat_map_y2x(q) ({ quat_map_i(Y,X,Z,W); r[Y] = -r[Y]; r;})
#define quat_map_y2z(q) ({ quat_map_i(X,Z,Y,W); r[Y] = -r[Y]; r;})
#define quat_map_z2x(q) ({ quat_map_i(Z,Y,X,W); r[Z] = -r[Z]; r;})
#define quat_map_z2y(q) ({ quat_map_i(X,Z,Y,W); r[Z] = -r[Z]; r;})

//**********************************************************

typedef struct {
  float sx,sy,sc;   // swing: sin*(x,y) in plane & cos
  float tc,ts;      // twist: cos & sin about
} swing_twist_t;





#pragma pop_macro("X")
#pragma pop_macro("Y")
#pragma pop_macro("Z")
#pragma pop_macro("W")
