// Marc B. Reynolds, 2023-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef SWAR_H
#define SWAR_H

// some SWAR (SIMD Within A Register) rountines that I'm deeming to be
// even more niche than those in "bitops.h"

// bit reverse each byte
static inline uint64_t bit_reverse_8x8(uint64_t x)
{
#if (BITOPS_HAS_BIT_REVERSE && BITOPS_HAS_BYTESWAP)
  x = bit_reverse_64(byteswap_64(x));
#else
  x = bit_swap_4_64(x);
  x = bit_swap_2_64(x);
  x = bit_swap_1_64(x);
#endif  
  
  return x;
}

// bit reverse each 16-bit chunk
static inline uint64_t bit_reverse_16x4(uint64_t x)
{
  x = bit_reverse_8x8(x);
  x = bit_swap_8_64(x);

  return x;
}

// bit reverse the 2 32-bit chunks
static inline uint64_t bit_reverse_32x2(uint64_t x)
{
#if (BITOPS_HAS_BIT_REVERSE)
  x = rot_64(bit_reverse_64(x),32);
#else
  x = byteswap_64(x);
  x = bit_swap_32_64(x);
  x = bit_swap_4_64(x);
  x = bit_swap_2_64(x);
  x = bit_swap_1_64(x);
#endif
  
  return x;
}

// 
static inline uint64_t ror_8x8(uint64_t x, uint32_t n)
{
  static const uint64_t k = UINT64_C(0x0101010101010101);
  
  n &= 7;
  uint64_t m = (k << n)-k;
  uint64_t a = x & ( m);
  uint64_t b = x & (~m);
  return (a<<(8-n))^(b>>n);
}

static inline uint64_t ror_16x4(uint64_t x, uint32_t n)
{
  static const uint64_t k = UINT64_C(0x0001000100010001);
  
  n &= 15;
  uint64_t m = (k << n)-k;
  uint64_t a = x & ( m);
  uint64_t b = x & (~m);
  return (a<<(16-n))^(b>>n);
}

static inline uint64_t rol_8x8 (uint64_t x, uint32_t n) { return ror_8x8 (x, 8-n); }
static inline uint64_t rol_16x4(uint64_t x, uint32_t n) { return ror_16x4(x,16-n); }



#endif
