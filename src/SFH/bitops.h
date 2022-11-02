// Marc B. Reynolds, 2016-2022
// Public Domain under http://unlicense.org, see link for details.

#ifndef __BITOPS_H__
#define __BITOPS_H__

//  name_(u|s)?(d+)

#if !defined(_MSC_VER)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

// NOTE: there are additional bitops in carryless.h


#if !defined(_MSC_VER)
static inline uint32_t byteswap_32(uint32_t x) { return __builtin_bswap32(x); }
static inline uint64_t byteswap_64(uint64_t x) { return __builtin_bswap64(x); }
#else
static inline uint32_t byteswap_32(uint32_t x) { return _byteswap_ulong(x);   }
static inline uint64_t byteswap_64(uint64_t x) { return _byteswap_uint64(x);  }
#endif

#if defined(__clang__)
static inline uint32_t bit_reverse_32(uint32_t x) { return __builtin_bitreverse32(x); }
static inline uint64_t bit_reverse_64(uint64_t x) { return __builtin_bitreverse64(x); }
#else
#define BITOPS_DEFINE_BIT_REVERSE
#endif

#if !defined(_MSC_VER)
// seems like "pretty much" everybody (stern eyes) can match this pattern
static inline uint32_t rot_32(uint32_t x, uint32_t n) { n &= 0x1f; return (x<<n) | (x>>(-n & 0x1f)); }
static inline uint64_t rot_64(uint64_t x, uint32_t n) { n &= 0x3f; return (x<<n) | (x>>(-n & 0x3f)); }
#else
static inline uint32_t rot_32(uint32_t x, uint32_t n) { return (uint32_t)_rotl(x,n); }
static inline uint64_t rot_64(uint64_t x, uint32_t n) { return (uint64_t)_rotl64(x,n); }
#endif

// bit_set_even_N_T :
// * all even (lower) bits of 2N groups are set, otherwise clear
// * bit (index i) set when (i & (1 << (N-1)))
static const uint64_t bit_set_even_1_64  = UINT64_C(0x5555555555555555); // |01|01|
static const uint64_t bit_set_even_2_64  = UINT64_C(0x3333333333333333); // |0011|0011|
static const uint64_t bit_set_even_4_64  = UINT64_C(0x0f0f0f0f0f0f0f0f); // |0000ffff|
static const uint64_t bit_set_even_8_64  = UINT64_C(0x00ff00ff00ff00ff); // etc
static const uint64_t bit_set_even_16_64 = UINT64_C(0x0000ffff0000ffff); //... or as
static const uint32_t bit_set_even_1_32  = (uint32_t)0x55555555;         // bits
static const uint32_t bit_set_even_2_32  = (uint32_t)0x33333333;         // bit pairs 
static const uint32_t bit_set_even_4_32  = (uint32_t)0x0f0f0f0f;         // nibbles
static const uint32_t bit_set_even_8_32  = (uint32_t)0x00ff00ff;         // bytes
static const uint32_t bit_set_even_16_32 = (uint32_t)0x0000ffff;

// each nibble is 0x1 (0b0001)
static const uint64_t bit_set_nibble_1_64 = UINT64_C(0x1111111111111111);
static const uint32_t bit_set_nibble_1_32 = (uint32_t)0x11111111;

#if !defined(_MSC_VER)

#if !defined(__x86_64__)
// UB if input is zero (even with hw support) problem case is when
// 'x' ends up being constant and zero. seems to only be an issue with
// intel and clang (but only for intel targets? ARM doesn't) YMMV
static inline uint32_t clz_32(uint32_t x) { return (uint32_t)__builtin_clz(x);  }
static inline uint32_t clz_64(uint64_t x) { return (uint32_t)__builtin_clzl(x); }
static inline uint32_t ctz_32(uint32_t x) { return (uint32_t)__builtin_ctz(x);  }
static inline uint32_t ctz_64(uint64_t x) { return (uint32_t)__builtin_ctzl(x); }
#else
static inline uint32_t clz_32(uint32_t x) { return (uint32_t)_lzcnt_u32(x); }
static inline uint32_t clz_64(uint64_t x) { return (uint32_t)_lzcnt_u64(x); }
static inline uint32_t ctz_32(uint32_t x) { return (uint32_t)_tzcnt_u32(x); }
static inline uint32_t ctz_64(uint64_t x) { return (uint32_t)_tzcnt_u64(x); }
#endif

static inline uint32_t pop_32(uint32_t x) { return (uint32_t)__builtin_popcount(x);  }
static inline uint32_t pop_64(uint64_t x) { return (uint32_t)__builtin_popcountl(x); }
#else
static inline uint32_t clz_32(uint32_t x) { return (uint32_t)__lzcnt(x);    }
static inline uint32_t ctz_32(uint32_t x) { return (uint32_t)_tzcnt_u32(x); }
static inline uint32_t clz_64(uint64_t x) { return (uint32_t)__lzcnt64(x);  }
static inline uint32_t ctz_64(uint64_t x) { return (uint32_t)_tzcnt_u64(x); }
static inline uint32_t pop_32(uint32_t x) { return (uint32_t)__popcnt(x);   }
static inline uint32_t pop_64(uint64_t x) { return (uint32_t)__popcnt64(x); }
#endif

// inverse function in "carryless.h" (move to approp place)
// add arm __crc32cw
static inline uint32_t crc32c(uint32_t x, uint32_t k)    { return _mm_crc32_u32(x,k); }
static inline uint32_t crc32c_64(uint64_t x, uint32_t k) { return (uint32_t)_mm_crc32_u64(k,x); }

#if !defined(BITOPS_MISSING_POPCOUNT)

static inline uint32_t bit_parity_32(uint32_t x) { return pop_32(x) & 1; }
static inline uint64_t bit_parity_64(uint64_t x) { return pop_64(x) & 1; }

#else

// parity w/o hardware popcount

static inline uint64_t bit_parity_64(uint64_t v)
{
  static const uint64_t K = bit_set_nibble_1_64;
  
  v ^= v >> 1;
  v ^= v >> 2;
  v = (v & K) * K;
  return (v >> (64-4)) & 1;
}

static inline uint32_t bit_parity_32(uint32_t v)
{
  static const uint32_t K = bit_set_nibble_1_32;
  
  v ^= v >> 1;
  v ^= v >> 2;
  v = (v & K) * K;
  return (v >> (32-4)) & 1;
}

#endif


static inline uint32_t bit_parity_mask_32(uint32_t x) { return -bit_parity_32(x); }
static inline uint64_t bit_parity_mask_64(uint64_t x) { return -bit_parity_64(x); }

// scatter/gather ops generically...skipping that ATM.
static inline uint32_t bit_scatter_32(uint32_t x, uint32_t m) { return _pdep_u32(x, m); } 
static inline uint64_t bit_scatter_64(uint64_t x, uint64_t m) { return _pdep_u64(x, m); } 
static inline uint32_t bit_gather_32(uint32_t x, uint32_t m)  { return _pext_u32(x, m); } 
static inline uint64_t bit_gather_64(uint64_t x, uint64_t m)  { return _pext_u64(x, m); } 

// Jasper Neumann : https://programming.sirrida.de/perm_fn.html#bit_permute_step_simple

#define BIT_PERMUTE(X,M,S)    (((X & M) << S) | ((X >> S) & M))

static inline uint32_t bit_permute_step_simple_32(uint32_t x, uint32_t m, uint32_t s)
{
  return BIT_PERMUTE(x,m,s);
}

static inline uint64_t bit_permute_step_simple_64(uint64_t x, uint64_t m, uint32_t s)
{
  return BIT_PERMUTE(x,m,s);
}

#define  BIT_GROUP_SWAP(X,L,T) BIT_PERMUTE(X,bit_set_even_ ## L ## _ ## T,L)

static inline uint32_t bit_swap_1_32(uint32_t x)  { return BIT_GROUP_SWAP(x,1,32); }
static inline uint32_t bit_swap_2_32(uint32_t x)  { return BIT_GROUP_SWAP(x,2,32); }
static inline uint32_t bit_swap_4_32(uint32_t x)  { return BIT_GROUP_SWAP(x,4,32); }
static inline uint32_t bit_swap_8_32(uint32_t x)  { return byteswap_32(x); }
static inline uint32_t bit_swap_16_32(uint32_t x) { return rot_32(x,16);  }

static inline uint64_t bit_swap_1_64(uint64_t x)  { return BIT_GROUP_SWAP(x,1,64); }
static inline uint64_t bit_swap_2_64(uint64_t x)  { return BIT_GROUP_SWAP(x,2,64); }
static inline uint64_t bit_swap_4_64(uint64_t x)  { return BIT_GROUP_SWAP(x,4,64); }
static inline uint64_t bit_swap_8_64(uint64_t x)  { return byteswap_64(x); }
static inline uint64_t bit_swap_16_64(uint64_t x) { return BIT_GROUP_SWAP(x,16,64);}
static inline uint64_t bit_swap_32_64(uint64_t x) { return rot_64(x,32);  }


// if no intrinsic version is available
#if defined(BITOPS_DEFINE_BIT_REVERSE)
static inline uint32_t bit_reverse_32(uint32_t x)
{
  x = byteswap_32(x);
  x = bit_swap_4_32(x);
  x = bit_swap_2_32(x);
  x = bit_swap_1_32(x);
  return x;
}

static inline uint64_t bit_reverse_64(uint64_t x)
{
  x = byteswap_64(x);
  x = bit_swap_4_64(x);
  x = bit_swap_2_64(x);
  x = bit_swap_1_64(x);
  return x;
}
#endif

// number of bit strings (runs of 1's) in input 'x'
static inline uint32_t bit_str_count_32(uint32_t x) { return pop_32(x & (x^(x >> 1))); }
static inline uint64_t bit_str_count_64(uint64_t x) { return pop_64(x & (x^(x >> 1))); }

// clears the highest/lowest bit of every bit string
static inline uint32_t bit_str_clear_hi_32(uint32_t x)   { return x & (x>>1); }
static inline uint64_t bit_str_clear_hi_64(uint64_t x)   { return x & (x>>1); }
static inline uint32_t bit_str_clear_lo_32(uint32_t x)   { return x & (x<<1); }
static inline uint64_t bit_str_clear_lo_64(uint64_t x)   { return x & (x<<1); }

// isolate the lowest bit string
static inline uint32_t bit_str_lo_32(uint32_t x)
{
  uint32_t t = x + (x & (-x));
  return x & (~t);
}

static inline uint64_t bit_str_lo_64(uint64_t x)
{
  uint64_t t = x + (x & (-x));
  return x & (~t);
}

// clears the 'n^th' set bit in x
static inline uint32_t bit_clear_nth_set_32(uint32_t x, uint32_t n)
{
  const uint32_t m = ~UINT32_C(0);
  const uint32_t o =  UINT32_C(1);
  
  return bit_scatter_32(m ^ (o<<n), x);
}

static inline uint64_t bit_clear_nth_set_64(uint64_t x, uint32_t n)
{
  const uint64_t m = ~UINT64_C(0);
  const uint64_t o =  UINT64_C(1);
  
  return bit_scatter_64(m ^ (o<<n), x);
}

// sets the 'n^th' clear bit in x
static inline uint32_t bit_set_nth_clear_32(uint32_t x, uint32_t n)
{
  return ~bit_clear_nth_set_32(~x,n);
}

static inline uint64_t bit_set_nth_clear_64(uint64_t x, uint32_t n)
{
  return ~bit_clear_nth_set_64(~x,n);
}

static inline uint32_t bit_zip_32(uint32_t x)
{
  const uint32_t m = bit_set_even_1_32;
  uint32_t L = bit_scatter_32(x,       m);
  uint32_t R = bit_scatter_32(x >> 16, m);
  return (R<<1)^L;
}

static inline uint64_t bit_zip_64(uint64_t x)
{
  const uint64_t m = bit_set_even_1_64;
  uint64_t L = bit_scatter_64(x,       m);
  uint64_t R = bit_scatter_64(x >> 32, m);
  return (R<<1)^L;
}

static inline uint32_t pop_next_32(uint32_t x)
{
  uint32_t a = x & -x;
  uint32_t b = x + a;
  uint32_t c = x ^ b;
  uint32_t d = (2 + ctz_32(x));
  return b | (c >> d);
}

static inline uint64_t pop_next_64(uint64_t x)
{
  uint64_t a = x & -x;
  uint64_t b = x + a;
  uint64_t c = x ^ b;
  uint32_t d = (2 + ctz_64(x));
  return b | (c >> d);
}


#endif
