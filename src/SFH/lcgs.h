// Marc B. Reynolds, 2016
// Public Domain under http://unlicense.org, see link for details.
//
// SEE: http://marc-b-reynolds.github.io/shf/2016/04/19/prns.html

#ifndef LCGS_H
#define LCGS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(LCGS_64)
#define LCGS_UINT uint64_t
#define LCGS_SINT int64_t
#if !defined(LCGS_MF)
#if defined(LCGS_MLCG)
#define LCGS_MF 0x106689d45497fdb5L 
#define LCGS_MR 0xbb91f78bdac4c89dL
#else
#define LCGS_MF 0x27bb2ee687b0b0fdL
#define LCGS_MR 0xdfe66807999cec55L
#define LCGS_AF 0x1234567L
#endif
#endif
#else  
#define LCGS_UINT uint32_t
#define LCGS_SINT int32_t
#if !defined(LCGS_MF)
#if defined(LCGS_MLCG)
#define LCGS_MF 0x2c2c57ed
#define LCGS_MR 0xf4ed9de5
#else
#define LCGS_MF 0xac564b05
#define LCGS_MR 0xdc33c9cd
#define LCGS_AF 0x1233
#endif
#endif
#endif

#ifndef LCGS_AF
#define LCGS_AF 0
#ifndef LCGS_MLCG
#define LCGS_MLCG
#endif
#endif
  
#define LCGS_AR  ((LCGS_UINT)0-(LCGS_MR*LCGS_AF))

#ifndef LCGS_MIX
#define LCGS_MIX(X) lcgs_mix(X)
#endif 

typedef struct { LCGS_UINT i; } lcgs_t;

   
#if defined(LCGS_IMPLEMENTATION)
LCGS_UINT lcgs_state(LCGS_UINT state, LCGS_SINT dp)
{
  LCGS_UINT m = 1;
  LCGS_UINT a = 0;
  LCGS_UINT dm,da;
  
  if (dp >= 0) {
    dm = LCGS_MF;
    da = LCGS_AF;
  }
  else {
    dm = LCGS_MR;
    da = LCGS_AR;
    dp = -dp;
  }

  while (dp) {
    if (dp & 1) {
      m = m*dm;
      a = a*dm + da;
    }
    da *= dm+1;
    dm *= dm;
    dp >>= 1;
  }

  return m*state + a;
}

LCGS_UINT lcgs_state_0(LCGS_UINT state, LCGS_SINT dp)
{
  LCGS_UINT m = 1;
  LCGS_UINT dm;
  
  if (dp >= 0) {
    dm = LCGS_MF;
  }
  else {
    dm = LCGS_MR;
    dp = -dp;
  }

  while (dp) {
    if (dp & 1) {
      m = m*dm;
    }
    dm *= dm;
    dp >>= 1;
  }

  return m*state;
}
#else
extern LCGS_UINT lcgs_state(LCGS_UINT state, LCGS_SINT dp);
extern LCGS_UINT lcgs_state_0(LCGS_UINT state, LCGS_SINT dp);
#endif

static inline LCGS_UINT lcgs_state_get(lcgs_t* gen)
{
  return gen->i;
}

static inline void lcgs_state_set(lcgs_t* gen, LCGS_UINT state)
{
  gen->i = state;
}

static inline void lcgs_seek(lcgs_t* gen, LCGS_SINT offset)
{
#ifndef LCGS_MLCG
  gen->i = lcgs_state(gen->i, offset);
#else
  gen->i = lcgs_state_0(gen->i, offset);
#endif
}


#if defined(LCGS_PCG_MIXING)

// ***** start of PCG copy-and-paste code, which is under:
// http://www.apache.org/licenses/LICENSE-2.0
// SEE: http://www.pcg-random.org

#if !defined(LCGS_64)
inline uint32_t pcg_output_rxs_m_xs_32_32(uint32_t state)
{
  uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

#else

static inline uint64_t pcg_output_rxs_m_xs_64_64(uint64_t state)
{
  uint64_t word = ((state >> ((state >> 59u) + 5u)) ^ state) * 12605985483714917081ull;
  return (word >> 43u) ^ word;
}

static inline uint32_t pcg_rotr_32(uint32_t value, unsigned int rot)
{
#if PCG_USE_INLINE_ASM && __clang__ && (__x86_64__  || __i386__)
    asm ("rorl   %%cl, %0" : "=r" (value) : "0" (value), "c" (rot));
    return value;
#else
    return (value >> rot) | (value << ((- rot) & 31));
#endif
}

static inline uint64_t pcg_output_xsl_rr_rr_64_64(uint64_t state)
{
  uint32_t rot1 = (uint32_t)(state >> 59u);
  uint32_t high = (uint32_t)(state >> 32u);
  uint32_t low  = (uint32_t)state;
  uint32_t xored = high ^ low;
  uint32_t newlow  = pcg_rotr_32(xored, rot1);
  uint32_t newhigh = pcg_rotr_32(high, newlow & 31u);
  return (((uint64_t)newhigh) << 32u) | newlow;
}

#endif

// ***** end of PCG copy-and-paste

#else
  
#if defined(_MSC_VER)
#include <stdlib.h> 
#define __builtin_bswap32(X) _byteswap_ulong(X)
#define __builtin_bswap64(X) _byteswap_uint64(X)
#endif
  
#endif

static inline LCGS_UINT lcgs_mix(LCGS_UINT x)
{
#if defined(LCGS_64)
  uint64_t s = __builtin_bswap64(x);
#else
  uint64_t s = __builtin_bswap32(x);
#endif

  return s^x;
}

static inline LCGS_UINT lcgs_peek(lcgs_t* gen)
{
  return LCGS_MIX(gen->i);
}

static inline LCGS_UINT lcgs_next(lcgs_t* gen)
{
  LCGS_UINT i = gen->i;
  LCGS_UINT r = LCGS_MIX(i);
  gen->i = LCGS_MF*i + LCGS_AF;
  return r;
}

static inline LCGS_UINT lcgs_prev(lcgs_t* gen)
{
  LCGS_UINT i = gen->i;
  LCGS_UINT r = LCGS_MIX(i);
  gen->i = LCGS_MR*i + LCGS_AR;
  return r;
}

#undef LCGS_UINT
#undef LCGS_SINT
  
#ifdef __cplusplus
extern }
#endif


#endif
