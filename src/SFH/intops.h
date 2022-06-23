// Marc B. Reynolds, 2016-2022
// Public Domain under http://unlicense.org, see link for details.

#ifndef __INTOPS_H__
#define __INTOPS_H__

//  name_(u|s)?(d+)

#if !defined(_MSC_VER)
#else
#include <intrin.h>
#endif


static inline void cs_adder_64(uint64_t* h, uint64_t* l, uint64_t a, uint64_t b, uint64_t c)
{
  uint64_t s = a^b;
  h[0] = (a & b)|(s & c);
  l[0] = s^c;
}

static inline uint32_t log2_32(uint32_t x)       { return (31 - clz_32(x));   }
static inline uint64_t log2_64(uint64_t x)       { return (63 - clz_64(x));   }
static inline uint32_t log2_ceil_32(uint32_t x)  { return (32 - clz_32(x-1)); }
static inline uint64_t log2_ceil_64(uint64_t x)  { return (64 - clz_64(x-1)); }

// floor and ceiling of (a+b)/2 without overflow
static inline uint32_t ave_u32(uint32_t a, uint32_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint64_t ave_u64(uint64_t a, uint64_t b)      { return (a&b) + ((a^b)>>1); }
static inline uint32_t ave_ceil_u32(uint32_t a, uint32_t b) { return (a|b) - ((a^b)>>1); }
static inline uint64_t ave_ceil_u64(uint64_t a, uint64_t b) { return (a|b) - ((a^b)>>1); }

#endif
