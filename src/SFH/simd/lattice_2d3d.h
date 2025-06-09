// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2017-2025
// Public Domain under http://unlicense.org, see link for details.

// 1) Nearest lattice points for:
//    2D: D₂
//    3D: D₃,A₃
// 
// 2) Fixed package (bit size) quantization for the above
//    lattices plus Z^2 and Z^3 with version for standard
//    and tiled space.
//    The routines accept the number of bits per direction
//    'b' so 'b' must be a known constant to to compiler
//    to not suck performance wise.

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


//*******************************************************
// 2D quantized to standard integer lattice (Z²),
// b-bits per component for 2b bits total.
// • covering radius (R) is 1/sqrt(2)
// • maximum error is 2^{-b} R

//  requires: b∈[1,16]
//  bottom b bits 'x' followed by b bits of y.
static inline uint32_t vec2f_tiled_square_encode_z2(vec2f_t v, uint32_t b)
{
  uint32_t s = (1<<b);
  uint32_t m = s-1;
  i32x2_t  r = vec2_scale_round_to_int(v,(float)s);
  
  r &= (int32_t)m;

  return (uint32_t)((r[1]<<b)^r[0]);
}

// ditto execept requires: b∈[1,32]
static inline uint64_t vec2d_tiled_square_encode_z2(vec2d_t v, uint32_t b)
{
  uint64_t s = (UINT64_C(1)<<b);
  uint64_t m = s-1;
  i64x2_t  r = vec2_scale_round_to_int(v,(double)s);
  
  r &= (int64_t)m;
  
  return (uint64_t)((r[1]<<b)^r[0]);
}

// decode.
static inline vec2f_t vec2f_tiled_square_decode_z2(uint32_t u, uint32_t b)
{
  uint32_t s = (1<<b);
  uint32_t m = s-1;
  float    f = 1.f/((float)s);
  float    x = (float)(u & m) * f; u >>= b;
  float    y = (float)(u & m) * f;
  
  return vec2f(x,y);
}

static inline vec2d_t vec2d_tiled_square_decode_z2(uint64_t u, uint32_t b)
{
  uint64_t s = (1<<b);
  uint64_t m = s-1;
  double   f = 1.0/((double)s);
  double   x = (double)(u & m) * f; u >>= b;
  double   y = (double)(u & m) * f;
  
  return vec2d(x,y);
}

//*******************************************************
// 2D quantized to D_2
// b-bits per component for 2b+1 bits total.
// • covering radius (R) is 1
// • maximum error is 2^-(b+1)

//  requires: b∈[1,15]
static inline uint32_t vec2f_tiled_square_encode_d2(vec2f_t v, uint32_t b)
{
  // scale and find nearest lattice point
  i32x2_t r = vec2f_lattice_nearest_d2(v * (float)(1<<(b+1)));
  
  // bit-pack the quantized point (r) low bit of
  // either (using x) indicates if the coordinates
  // are odd or even.
  uint32_t m  = (uint32_t)((1<<(b+1))-1);
  uint32_t ix = (uint32_t)(r[0]) & (m  );
  uint32_t iy = (uint32_t)(r[1]) & (m^1);
  uint32_t q  = ix ^ (iy << b);

  return q;
}

//  requires: b∈[1,31]
static inline uint64_t vec2d_tiled_square_encode_d2(vec2d_t v, uint32_t b)
{
  // scale and find nearest lattice point
  i64x2_t r = vec2d_lattice_nearest_d2(v * (double)(1<<(b+1)));
  
  // bit-pack the quantized point (r) low bit of
  // either (using x) indicates if the coordinates
  // are odd or even.
  uint64_t m  = (uint64_t)((1<<(b+1))-1);
  uint64_t ix = (uint64_t)(r[0]) & (m  );
  uint64_t iy = (uint64_t)(r[1]) & (m^1);
  uint64_t q  = ix ^ (iy << b);

  return q;
}

static inline vec2f_t vec2f_tiled_square_decode_d2(uint32_t u, uint32_t b)
{
  float    s  = 1.f/(float)(1<<(b+1));
  uint32_t m  = (uint32_t)((1<<(b+1))-1);
  uint32_t ix = u & m;
  uint32_t iy = ((u >> b) & (m^1)) ^ (u & 1);
  float    x  = (float)ix * s;
  float    y  = (float)iy * s;

  return vec2(x,y);
}

static inline vec2d_t vec2d_tiled_square_decode_d2(uint64_t u, uint32_t b)
{
  double   s  = 1.f/(double)(1<<(b+1));
  uint64_t m  = (uint64_t)((1<<(b+1))-1);
  uint64_t ix = u & m;
  uint64_t iy = ((u >> b) & (m^1)) ^ (u & 1);
  double   x  = (double)ix * s;
  double   y  = (double)iy * s;

  return vec2(x,y);
}

//*******************************************************
// 
