// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

// scalar uniform quantization (uses simd.h)
// this is current SUPER evil, fragile, ugly
// also currently mostly untested

// rough idea was: generic versions of same number of elements
// and bit-widths encode/decode pairs as helpers.
// should add parameter validation

#pragma once

#if !defined(SFH_SIMD_H)
#warning "uniform_quantization.h: requires simd.h"
#else

// struct and code for building correctly rounded divisions
typedef struct { float  h,l; } simd_suq_scale_32_t;
typedef struct { double h,l; } simd_suq_scale_64_t;

static inline simd_suq_scale_32_t simd_suq_scale_f32(uint32_t n)
{
  const float y  = (float)n;
  const float rh = 1.f/y;
  const float rl = -fmaf(rh, y, -1.f)/y;

  return (simd_suq_scale_32_t){.h = rh, .l = rl};
}

static inline simd_suq_scale_64_t simd_suq_scale_f64(uint32_t n)
{
  const double y  = (double)n;
  const double rh = 1.0/y;
  const double rl = -fma(rh, y, -1.0)/y;

  return (simd_suq_scale_64_t){.h = rh, .l = rl};
}

// use the type T (float or double) to select method
#define simd_suq_scale(T,N)   _Generic(typeof(T), float: simd_suq_scale_f32, default: simd_suq_scale_f64)(N)
#define simd_suq_vscale(T,N)  simd_suq_scale(T[0],N)


// given vector 'v' scale by 'n' and convert to signed/unsigned vector
// with the same number of elements (trunction)

//#define simd_suq_scale_i(V,N) ({typeof(V) _V=V; simd_convert_fi(_V*(n)); })
#define simd_suq_scale_i(V,N) ({typeof(V) _V=V; simd_convert_fi(_V*((typeof(_V[0]))n)); })

#define simd_suq_scale_u(V,N) simd_bitcast_iu(simd_suq_scale_i(V,N))

// ditto: but round
#define simd_suq_scale_round_i(V,N) simd_convert_fi(simd_fma(V,N,0.5f))
#define simd_suq_scale_round_u(V,N) simd_bitcast_iu(simd_suq_scale_round_i(V,N))

// quantization: f=floor, r=rounded
#define simd_suq_encode_f(V,N)  simd_suq_scale_i(V,N)
#define simd_suq_encode_r(V,N)  simd_suq_scale_round_i(V,N)

// 1.0/n (with 'n' coverted to the float format)
#define simd_suq_div_n(S,N) 1.f/((typeof(S))(N))

// get type (fBxN) of float vector 'X' then: define iBxN NAME and convert
#define simd_def_convert_if(NAME,X) simd_if_typeof(X) NAME = simd_convert_if(X);


// center reconstruction:
#define simd_suq_decode_c(U,N)  \
({                              \
  simd_def_convert_if(_F,U);    \
  typeof(_F[0]) _S;             \
  _S = simd_suq_div_n(_S,N);    \
  simd_fma(_F,_S,0.5f*_S);      \
})

// center reconstruction (correctly rounded)
#define simd_suq_decode_c_cr(U,N)                             \
({                                                            \
  simd_if_typeof(U)             _F = (typeof(_F))(U) + 0.5f;  \
  typeof(simd_suq_vscale(_F,N)) _S = simd_suq_vscale(_F,N);   \
  simd_fma(_F,_S.h,_F*_S.l);                                  \
})

// left reconstruction : u/(n-1)
#define simd_suq_decode_l(U,N)  \
({                              \
  typeof(N)     _N = N;         \
  simd_def_convert_if(_F,U);    \
  typeof(_F[0]) _S;             \
  _S = simd_suq_div_n(_S,_N-1); \
  _S*_F;                        \
})

// left reconstruction : u/(n-1) (correctly rounded)
#define simd_suq_decode_l_cr(U,N)                              \
({                                                             \
  typeof(N)                     _N = N;                        \
  simd_if_typeof(U)             _F = (typeof(_F))(U);          \
  typeof(simd_suq_vscale(_F,N)) _S = simd_suq_vscale(_F,_N-1); \
  simd_fma(_F,_S.h,_F*_S.l);                                   \
})


// thinking about auto-selection correctly rounded decoding so:
// need one where explictly not using correctly round and another
// that expands correctly rounded if needed (need to recheck that
// GCC and clang won't do the lifing here)
//#define foo(X) ({ __builtin_choose_expr((X)&(X-1),simd_suq_decode_rl,simd_suq_decode_rl)(X,N); })


//**** encode/decode pairs helpers:
// for easy to use encode/decode pairs that aren't unfortunate to use together
// * these are assuming N is a power of two and 'rl' is auto-choosing
//   correctly rounded expansion. this is all pretty hacky.

// output point set excludes 0 & 1. point distance = 1/n {floor/center}
//   x on [0,1)   -> [0,n-1]
//   u on [0,n-1] -> (0,1)
#define simd_suq_encode_fc(X,N) simd_suq_encode_f(X,N)
#define simd_suq_decode_fc(U,N) simd_suq_decode_c(U,N)

// output point set includes 0 & 1. point distance = 1/(n-1)  {rounded/left}
//   x on [0,1]   -> [0,n-1]
//   u on [0,n-1] -> [0,1]
#define simd_suq_encode_rl(X,N) simd_suq_encode_r(X,N); }
#define simd_suq_decode_rl(U,N) simd_suq_decode_l_cr(U,N); }


// wrapped quantization (assumes n is power-of-two).
// domain of input is x mod 1 (so n assumption: mod to mask)
// Zero is member of output set (1 is equivalent to 0)
// point distance = 1/n 
//   x on [0,1)   -> [0,n-1]   {well input is mod 1}
//   u on [0,n-1] -> [0,1) 
#define simd_suq_encode_w(V,N) (simd_suq_scale_round_u(V,N) % (N))


//*******************************************************
// float unorm8 : manually baked

static inline int32_t simd_unorm8_encode(float x)
{
  return (int32_t)fmaf(255.f,x,0.5f);
}

// SEE: https://fgiesen.wordpress.com/2024/11/06/exact-unorm8-to-float/
static inline float simd_unorm8_decode_cr(int32_t u)
{
  static const float k = 1.f/(255.f*3.f);

  float f = (float)(3*u);
  float r = k*f;

  return r;
}

static inline i32x2_t simd_unorm8_encode_32x2(f32x2_t f) { return simd_component_map_fi(simd_unorm8_encode,f); }
static inline i32x4_t simd_unorm8_encode_32x4(f32x4_t f) { return simd_component_map_fi(simd_unorm8_encode,f); }
static inline i32x8_t simd_unorm8_encode_32x8(f32x8_t f) { return simd_component_map_fi(simd_unorm8_encode,f); }

static inline f32x2_t simd_unorm8_decode_32x2(i32x2_t e) { return simd_component_map_if(simd_unorm8_decode_cr,e); }
static inline f32x4_t simd_unorm8_decode_32x4(i32x4_t e) { return simd_component_map_if(simd_unorm8_decode_cr,e); }
static inline f32x8_t simd_unorm8_decode_32x8(i32x8_t e) { return simd_component_map_if(simd_unorm8_decode_cr,e); }

#if defined(SIMD_ENABLE_512)
static inline i32x16_t simd_unorm8_encode_32x16(f32x16_t f) { return simd_component_map_fi(simd_unorm8_encode,f); }
static inline f32x16_t simd_unorm8_decode_32x16(i32x16_t e) { return simd_component_map_if(simd_unorm8_decode_cr,e); }
#endif


#endif
