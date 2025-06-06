// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2017-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once


//*******************************************************
// D_2 lattice: (checkerboard, diagonal square, centered square)
// • covering radius (R) is 1
// • "Fast Quantizing and Decoding Algorithms for Lattice Quantizers
//    and Codes", Conway & Sloane, 1982 (algorithm 2)

// NOTE: that going straight to integer is possible but will increase
// quantization error for small inputs. Example is if all targets
// don't support direct floating-point to unsigned integer conversions
// then we lose a bit by being required to fit in signed. In that case
// for encoding to 31 bits we need a 16-bit integer part. But anyway
// 

static inline i32x2_t vec2f_lattice_nearest_d2(vec2f_t v)
{
  i32x2_t r = vec2_to_int(v);                // 'v' truncated as integer
  vec2f_t f = vec2_from_int(r);              // 'r' back in floating point
  vec2f_t d = v-f;                           // offset to 'v' (all positive elements)
  i32x2_t t = d > 0.5f;                      // selector of elements that round up
  i32x2_t c = r-t;                           // rounded coordinate (candidate)
  
  // if the sum of the elements of 'c' have an even parity then
  // it will be our result. Otherwise we need to find the element
  // with the largest magnitude in 'd' and "round" that element in
  // the opposite direction. (flip floor/ceiling)

  vec2f_t ad = vec2_abs(v-vec2_from_int(c));
  int32_t e  = ((c[0] ^ c[1]) & 1);          // parity of sum
  int32_t i  = ad[1] > ad[0] ? 1 : 0;        // index of largest delta

  t[i] ^= -e;                                // flip largest if odd parity
  r    -=  t;                                // the result coordinate

  return r;
}

static inline i64x2_t vec2d_lattice_nearest_d2(vec2d_t v)
{
  i64x2_t r = vec2_to_int(v);                // 'v' truncated as integer
  vec2d_t f = vec2_from_int(r);              // 'r' back in floating point
  vec2d_t d = v-f;                           // offset to 'v' (all positive elements)
  i64x2_t t = d > 0.5;                       // selector of elements that round up
  i64x2_t c = r-t;                           // rounded coordinate (candidate)

  // if the sum of the elements of 'c' have an even parity then
  // it will be our result. Otherwise we need to find the element
  // with the largest magnitude in 'd' and "round" that element in
  // the opposite direction. (flip floor/ceiling)

  vec2d_t ad = vec2_abs(v-vec2_from_int(c));
  int64_t e  = ((c[0] ^ c[1]) & 1);          // parity of sum
  int64_t i  = ad[1] > ad[0] ? 1 : 0;        // index of largest delta

  t[i] ^= -e;                                // flip largest if odd parity
  r    -=  t;                                // the result coordinate

  return r;
}

//*******************************************************
// D_3 lattice:
// • covering radius (R) is 1
// • same as D_2 (just one more dimension)

static inline i32x4_t vec3f_lattice_nearest_d3(vec3f_t v)
{
  i32x4_t r = vec3_to_int(v);
  vec3f_t f = vec3_from_int(r);
  vec3f_t d  = v-f;
  i32x4_t t  = d > 0.5f;
  i32x4_t c  = r-t;
  vec3f_t ad = vec3_abs(v-vec3_from_int(c));
  int32_t e  = ((c[0]^c[1]^c[2]) & 1);
  int32_t i  = ad[1] > ad[0] ? 1 : 0;

  i = ad[i] > ad[2] ? i : 2;

  t[i] ^= -e;
  r    -=  t;

  return r;
}

static inline i64x4_t vec3d_lattice_nearest_d3(vec3d_t v)
{
  i64x4_t r = vec3_to_int(v);
  vec3d_t f = vec3_from_int(r);
  vec3d_t d  = v-f;
  i64x4_t t  = d > 0.5;
  i64x4_t c  = r-t;
  vec3d_t ad = vec3_abs(v-vec3_from_int(c));
  int64_t e  = ((c[0]^c[1]^c[2]) & 1);
  int64_t i  = ad[1] > ad[0] ? 1 : 0;

  i = ad[i] > ad[2] ? i : 2;

  t[i] ^= -e;
  r    -=  t;

  return r;
}

