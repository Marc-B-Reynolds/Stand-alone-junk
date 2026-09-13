// Marc B. Reynolds, 2021-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once
#define PRNG_SMALL_GLOBAL_H

// small feature pseudorandom number generator with exactly one
// global generator. (for single threaded mini-programs)
// 
// * single sequence combined generator formed from a LCG
//   and a XGB (a state update from xorshiro family) with
//   a period of 2^64(2^64-1)
// * no parameterization support for multiple generators.
//   instead the XGB portion can be fast forwarded by 2^64


// slots of each state element
enum { PRNG_LCG_0, PRNG_XGB_L, PRNG_XGB_H, PRNG_LENGTH };

typedef uint64_t prng_t[3];

// define the LCG sequence
static const uint64_t prng_mul_k = UINT64_C(0xd1342543de82ef95);
static const uint64_t prng_add_k = UINT64_C(0x2545f4914f6cdd1d);

// significand bit positions & IEEE bit pattern of one
#ifndef F32_UTIL_H
static const uint32_t f32_mag_bits_k = UINT32_C(0x007fffff);
static const uint32_t f32_one_bits_k = UINT32_C(0x3f800000);
#endif

#if 0

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// strawman "global" allocator & stub to placehold actual base generation
volatile uint64_t state;
uint64_t prng_u64(void) { return state; };

// type-pun float/uint
static inline uint32_t f32_to_bits(float x)      { uint32_t u; memcpy(&u,&x,4); return u; }
static inline float    f32_from_bits(uint32_t x) { float f;    memcpy(&f,&x,4); return f; }


// standard equidistant base method (shifting)
static inline float prng_base_sm_f32(float s)
{
  uint64_t u = prng_u64();               // uniform 64-bit draw
  uint32_t a = (uint32_t)(u>>40);        // 24 uniform bits
  float    f = (float)a;                 // uniform binary32 (equidistant)

  return (s*0x1.0p-24f)*f;               // scale to [0,1)
}

// standard equidistant base method (bit-hack)
static inline float prng_base_bh_f32(float s)
{
  uint64_t u = prng_u64();               // uniform 64-bit draw
  uint32_t a = (uint32_t)(u>>40);        // 24 uniform bits
  uint32_t b = a ^ f32_one_bits_k;       // [1,2) {bit pattern}
  float    f = f32_from_bits(u);         // [1,2) {binary32}

  return fmaf(s,f,-s);                   // [0,s)
}

// standard equidistant base method (shifting)
static inline float prng_base_sm_f32(float s)
{
  uint64_t u = prng_u64();               // uniform 64-bit draw
  uint32_t a = (uint32_t)(u>>40);        // 24 uniform bits
  uint32_t x = ((uint32_t)u)>>8;         // 24 uniform bits (independent from a)
  float    f = (float)a;                 // uniform binary32 (equidistant)

  f = f32_from_bits(f32_to_bits(f) ^ x); // all uniform significand bits

  return (s*0x1.0p-24f)*f;               // scale to [0,s)
}

// modifed equidistant base method (bit-hack)
static inline float prng_base_bhx_f32(float s)
{
  uint64_t u = prng_u64();               // uniform 64-bit draw
  uint32_t a = (uint32_t)(u>>40);        // 24 uniform bits
  uint32_t x = ((uint32_t)u)>>8;         // 24 uniform bits (independent from a)
  uint32_t b = a ^ f32_one_bits_k;       // [1,2) {bit pattern}
  float    f = f32_from_bits(u);         // [1,2) {binary32}

  f = f32_from_bits(f32_to_bits(f) ^ x); // [1,2)
  
  return fmaf(s,f,-s);                   // [0,s)
}


float prng0_f32(void) { return prng_base_sm_f32(1.f); }
float prng1_f32(void) { return prng_base_bh_f32(1.f); }


#endif




#if defined(PRNG_IMPLEMENTATION)
// single generator
prng_t prng_state = {0};

static inline uint64_t prng_u64(void);

void prng_init(uint64_t data)
{
  // all choices of 'data' put the sequence at least 2^{128-1}
  // steps apart (from any other choice)
  prng_state[PRNG_LCG_0] = data;
  prng_state[PRNG_XGB_L] = UINT64_C(0xc2b2ae3d27d4eb4f);
  prng_state[PRNG_XGB_H] = UINT64_C(0x99bcf6822b23ca35);

  // walk 3 steps to make myself feel better
  prng_u64();
  prng_u64();
  prng_u64();
}

#if !defined(_MSC_VER)
static __attribute__((constructor))
void prng_auto_init(void)
{
  uint64_t data;

#if defined(__x86_64__)
  data =  __builtin_ia32_rdtsc();
#elif defined(__aarch64__)
  asm volatile("mrs %0, cntvct_el0" : "=r" (data));
#else
#error "in this life. you're on your own."               
#endif

  prng_init(data);
}
#endif

#else
extern prng_t prng_state;
extern void prng_init(uint64_t data);
#endif


static inline uint64_t prng_rot(const uint64_t v, int i)
{
#if defined(__clang__)    
  return __builtin_rotateleft64(v,(uint32_t)i);
#else
  return (v << i)|(v >> (64-i));
#endif  
}


static inline uint64_t prng_mix_64(uint64_t x)
{
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
  x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
  x = (x ^ (x >> 31));
  return x;
}


static inline uint64_t prng_u64(void)
{
  uint64_t s0 = prng_state[PRNG_XGB_L];
  uint64_t s1 = prng_state[PRNG_XGB_H];
  uint64_t s2 = prng_state[PRNG_LCG_0];
  uint64_t r  = prng_mix_64(s0 + s2);
  
  s1 ^= s0;
  prng_state[PRNG_LCG_0] = prng_mul_k * s2 + prng_add_k;
  prng_state[PRNG_XGB_L] = prng_rot(s0,55) ^ s1 ^ (s1<<14);
  prng_state[PRNG_XGB_H] = prng_rot(s1,36);
  
  return r;
}

static inline uint32_t prng_u32(void)
{
  return (uint32_t)prng_u64();
}

static inline float prng_f32(void)
{
  return (float)(prng_u64() >> 40) * 0x1.0p-24f;
}

static inline double prng_f64(void)
{
  return (double)(prng_u64() >> 11) * 0x1.0p-53;
}

#if 0
// modified equidistant. all signfi
static inline double prng_odd_f64(void)
{
  double   d = prng_f64();
  uint64_t u = prng_u64() >> 11;

  d = type_pun(type_pun(d,uint64_t) ^ u, double);

  return d;
}
#endif



//------------------------------------------------
// all quick adaption hacks below here

#if defined(SFH_SIMD_2D3D_H)

static inline float uniform_disc_norm_f32(vec2f_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = prng_u64();
    x = (float)(v >> 40)      * 0x1.0p-24f;
    y = (float)(v & 0xFFFFFF) * 0x1.0p-24f;
    x = 2.f*x-1.f; d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p[0][0] = x;
  p[0][1] = y;

  return d;
}

static inline float uniform_hdisc_norm_f32(vec2f_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = prng_u64();
    x = (float)(v >> 40)      * 0x1.0p-24f;
    y = (float)(v & 0xFFFFFF) * 0x1.0p-24f;
    d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p[0][0] = x;
  p[0][1] = y;

  return d;
}

// unit square [0,1)²
static inline vec2f_t uniform_square_f32(void) { return vec2f(prng_f32(), prng_f32()); }
static inline vec2d_t uniform_square_f64(void) { return vec2d(prng_f64(), prng_f64()); }


static inline vec3f_t uniform_sphere_f32(void)
{
  float   d,s;
  vec2f_t v;

  d = uniform_disc_norm_f32(&v);  
  s = 2.f*sqrtf(1.f-d);
  
  return vec3f(s*v[0], s*v[1], 1.f-2.f*d);
}



static inline quatf_t uniform_quat_f32(void)
{
  vec2f_t p0,p1;
  float  d1 = uniform_disc_norm_f32(&p1);
  float  s1 = sqrtf(1.f/d1);
  float  d0 = uniform_hdisc_norm_f32(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  return quatf(p0[1], s*p1[0], s*p1[1], p0[0]);
}

#endif
