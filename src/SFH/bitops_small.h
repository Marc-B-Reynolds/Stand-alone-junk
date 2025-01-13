// Marc B. Reynolds, 2023-2025
// Public Domain under http://unlicense.org, see link for details.

// small collection of operations on (unpacked) 8/16 bit values

#ifndef BITOPS_SMALL_H
#define BITOPS_SMALL_H

#define BIT_PERMUTE_T(T,X,M,S)  ((T)((X & M) << S) | ((X >> S) & M))
#define BIT_PERMUTE_8(X,M,S)    BIT_PERMUTE_T(uint8_t,  X,M,S)
#define BIT_PERMUTE_16(X,M,S)   BIT_PERMUTE_T(uint16_t, X,M,S)

inline uint8_t  rot_8 (uint8_t  x, uint32_t n) { n &= 0x07; return (uint8_t) ((x<<n) | (x>>(-n & 0x07))); }
inline uint8_t  ror_8 (uint8_t  x, uint32_t n) { n &= 0x07; return (uint8_t) ((x>>n) | (x<<(-n & 0x07))); }
inline uint16_t rot_16(uint16_t x, uint32_t n) { n &= 0x0f; return (uint16_t)((x<<n) | (x>>(-n & 0x0f))); }
inline uint16_t ror_16(uint16_t x, uint32_t n) { n &= 0x0f; return (uint16_t)((x>>n) | (x<<(-n & 0x0f))); }
inline uint8_t  rol_8 (uint8_t  x, uint32_t n) { return rot_8 (x,n); }
inline uint16_t rol_16(uint16_t x, uint32_t n) { return rot_16(x,n); }


#if (BITS_HAS_BIT_REVERSE)

static inline uint8_t  bit_reverse_8 (uint8_t  x) { return (uint8_t) (bit_reverse_32(x) >> 24); }
static inline uint16_t bit_reverse_16(uint16_t x) { return (uint16_t)(bit_reverse_32(x) >> 16); }

#else

// software fallback for no hardware bitreverse. actually
// clang will compile this to the above versions if there is.

static inline uint8_t bit_reverse_8(uint8_t x)
{
  x = BIT_PERMUTE_T(uint8_t, x, 0x0f, 4);
  x = BIT_PERMUTE_T(uint8_t, x, 0x33, 2);
  x = BIT_PERMUTE_T(uint8_t, x, 0x55, 1);
  return x;
}

static inline uint16_t bit_reverse_16(uint16_t x)
{
  x = BIT_PERMUTE_T(uint16_t, x, 0x00ff, 8);
  x = BIT_PERMUTE_T(uint16_t, x, 0x0f0f, 4);
  x = BIT_PERMUTE_T(uint16_t, x, 0x3333, 2);
  x = BIT_PERMUTE_T(uint16_t, x, 0x5555, 1);
  return x;
}

#endif


#endif
