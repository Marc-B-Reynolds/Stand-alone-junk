// Marc B. Reynolds, 2021-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef PRNG_H
#define PRNG_H

// small feature pseudorandom number generator
// * 
// * single sequence combined generator formed from a LCG
//   and a XGB (a state update from xorshiro family) with
//   a period of 2^64(2^64-1)
// * no parameterization support for multiple generators.
//   instead the XGB portion can be fast forwarded by 2^64


#if defined(PRNG_SMALL_NO_MIX)

static inline uint64_t prng_mix(uint64_t x) { return x; }

#else

// helper functions:
#if !defined(_MSC_VER)
// seems like "pretty much" everybody (stern eyes) can match this pattern
static inline uint64_t prng_rot(uint64_t x, uint32_t n) { n &= 0x3f; return (x<<n) | (x>>(-n & 0x3f)); }
#else
static inline uint64_t prng_rot(uint64_t x, uint32_t n) { return (uint64_t)_rotl64(x,n); }
#endif

// complete this
#if defined(__ARM_ARCH) && defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
static inline uint64_t prng_crc32c_64(uint64_t x, uint32_t k) { return __crc32cd(k,x); }
#else
#if !defined(_MSC_VER)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

static inline uint64_t prng_crc32c_64(uint64_t x, uint32_t k) { return _mm_crc32_u64(k,x); }
#endif

static inline uint64_t prng_mix(uint64_t x) { return x; }

#endif

enum { PRNG_LCG_0, PRNG_XGB_L, PRNG_XGB_H, PRNG_LENGTH };

typedef struct { uint64_t state[3]; } prng_t;

static const uint64_t prng_mul_k = UINT64_C(0xd1342543de82ef95);
static const uint64_t prng_add_k = UINT64_C(0x2545f4914f6cdd1d);

static inline uint64_t prng_u64(prng_t* prng)
{
  uint64_t s0 = prng->state[PRNG_LCG_0];
  uint64_t s1 = prng->state[PRNG_XGB_L];
  uint64_t s2 = prng->state[PRNG_XGB_H];
  uint64_t r  = prng_mix(s0 + s2);
  
  s1 ^= s0;
  prng->state[PRNG_LCG_0] = prng_mul_k * s2 + prng_add_k;
  prng->state[PRNG_XGB_L] = prng_rot(s0,55) ^ s1 ^ (s1<<14);
  prng->state[PRNG_XGB_H] = prng_rot(s1,36);
  
  return r;
}

static inline uint32_t prng_u32(prng_t* prng)
{
  return (uint32_t)prng_u64(prng);
}

#endif
