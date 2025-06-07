// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2017-2025
// Public Domain under http://unlicense.org, see link for details.

// Nearest lattice points for:
//   2D: D₂
//   3D: D₃,A₃
// Z lattice..what's the point of a routine?

#pragma once

// On lattice points routines:
// should implement versions that jump directly to fixed point.
// that has some gotchas is why I haven't bothered yet. example
// binary32 type we can expect hardware op to convert to signed
// integer which leaves us with 31 bits. but anyway these float
// based versions perform 2 float→int and 1 int→float which
// is a bit sad but we're exact.

//*******************************************************
// D_2 lattice: (checkerboard, diagonal square, centered square)
// • covering radius (R) is 1
// • odd number of bits to quantize (2b+1 bits with b per component)
// • quantization max distance error: 
// • "Fast Quantizing and Decoding Algorithms for Lattice Quantizers
//    and Codes", Conway & Sloane, 1982 (algorithm 2)

// returns the lattice coordinate (the sum of coordinates is even parity)
// direct result is convenient for quantization
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
// D_3 lattice: face-centered cubic (FCC)
// • covering radius (R) is 1
// • same as D_2 (just one more dimension)
// • 3b+2 bits to quantize with b per component
// • quantization max distance error: 

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

//*******************************************************
// A_3 lattice: body-centered cubic (BCC)
// • covering radius (R) is sqrt(5)/4
// • 3b+1 bits to quantize with b per component
// • quantization max distance error: 2^{-b} R

// these don't directly return the lattice coordinate which
// is: let 'r' be result then coordinate is 2r + s
// where 's' broadcast (all zeros or ones)
static inline i32x4_t vec3f_lattice_nearest_a3(vec3f_t v, bool* s)
{
  vec3f_t  fv = vec3_floor(v);                    // a base cell corner
  vec3f_t  nv = vec3_round_up(v);                 // nearest corner
  
  // compute the two possible distances and choose smaller
  // (standard because the wrap hasn't occurred yet for tiled)
  float    d0 = vec3_dist_sq(fv+0.5f, v);         // dist squared to center
  float    d1 = vec3_dist_sq(nv, v);              // dist squared to closest corner
  bool     t  = d0 < d1;                          //   which?
  vec3f_t  qv = t ? fv : nv;                      // select closest
  i32x4_t  q  = vec3_to_int(qv);                  // convert to integer

  *s = t;
  
  return q;
}

static inline i64x4_t vec3d_lattice_nearest_a3(vec3d_t v, bool* s)
{
  vec3d_t  fv = vec3_floor(v);                    // a base cell corner
  vec3d_t  nv = vec3_round_up(v);                 // nearest corner
  
  // compute the two possible distances and choose smaller
  // (standard because the wrap hasn't occurred yet for tiled)
  double   d0 = vec3_dist_sq(fv+0.5f, v);         // dist squared to center
  double   d1 = vec3_dist_sq(nv, v);              // dist squared to closest corner
  bool     t  = d0 < d1;                          //   which?
  vec3d_t  qv = t ? fv : nv;                      // select closest
  i64x4_t  q  = vec3_to_int(qv);                  // convert to integer

  *s = t;
  
  return q;
}
