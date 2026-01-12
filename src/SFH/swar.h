// Marc B. Reynolds, 2023-2026
// Public Domain under http://unlicense.org, see link for details.

#ifndef SWAR_H
#define SWAR_H

// some SWAR (SIMD Within A Register) rountines that I'm deeming to be
// even more niche than those in "bitops.h". Mostly fixed lane partitions

// duplicate the bottom (8,16,32) bits of 'x' in every field with the corresponding low bit set in 'm'
static inline uint64_t broadcast_select_8x8 (uint64_t x, uint64_t m) { return (0x0101010101010101 & m) * (x & 0xff); }
static inline uint64_t broadcast_select_16x4(uint64_t x, uint64_t m) { return (0x0001000100010001 & m) * (x & 0xffff); }
static inline uint64_t broadcast_select_32x2(uint64_t x, uint64_t m) { return (0x0000000100000001 & m) * (x & 0xffffffff); }

// duplicate the bottom (8,16,32) bits in all other same sized fields.
static inline uint64_t broadcast_8x8 (uint64_t x) { return 0x0101010101010101 * (x & 0xff); }
static inline uint64_t broadcast_16x4(uint64_t x) { return 0x0001000100010001 * (x & 0xffff); }  // 2 xor-shift is an alternate
static inline uint64_t broadcast_32x2(uint64_t x) { uint64_t t = (x & 0xffffffff); return t^(t<<32); }

// returns the high bit of the field (byte,etc) set if the field is non-zero/zero
// SEE: Wojciech Muła (http://0x80.pl/articles/mask-zero-nonzero-bytes.html)
static inline uint64_t nonzero_test_64(uint64_t x, uint64_t k) { return ((x|((x & k) + k)) & (~k)); }
static inline uint64_t zero_test_64   (uint64_t x, uint64_t k) { return ((x|((x & k) + k)) & (~k)) ^ (~k); }

static inline uint64_t nonzero_test_8x8 (uint64_t x) { return nonzero_test_64(x, broadcast_8x8 (0x0000007f)); }
static inline uint64_t nonzero_test_16x4(uint64_t x) { return nonzero_test_64(x, broadcast_16x4(0x00007fff)); }
static inline uint64_t nonzero_test_32x2(uint64_t x) { return nonzero_test_64(x, broadcast_32x2(0x7fffffff)); }
static inline uint64_t zero_test_8x8    (uint64_t x) { return    zero_test_64(x, broadcast_8x8 (0x0000007f)); }
static inline uint64_t zero_test_16x4   (uint64_t x) { return    zero_test_64(x, broadcast_16x4(0x00007fff)); }
static inline uint64_t zero_test_32x2   (uint64_t x) { return    zero_test_64(x, broadcast_32x2(0x7fffffff)); }

// returns the number of zero fields 
static inline uint32_t zero_field_count_8x8 (uint64_t x) { return pop_64(zero_test_8x8 (x)); }
static inline uint32_t zero_field_count_16x4(uint64_t x) { return pop_64(zero_test_16x4(x)); }
static inline uint32_t zero_field_count_32x2(uint64_t x) { return pop_64(zero_test_32x2(x)); }


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

// rotate right each byte by 'n' : b[i] = ror(b[i],n)
static inline uint64_t ror_8x8(uint64_t x, uint32_t n)
{
  static const uint64_t k = UINT64_C(0x0101010101010101);
  
  n &= 7;
  uint64_t m = (k << n)-k;
  uint64_t a = x & ( m);
  uint64_t b = x & (~m);
  return (a<<(8-n))^(b>>n);
}

// rotate right each 16-bit word by 'n' : b[i] = ror(b[i],n)
static inline uint64_t ror_16x4(uint64_t x, uint32_t n)
{
  static const uint64_t k = UINT64_C(0x0001000100010001);
  
  n &= 15;
  uint64_t m = (k << n)-k;
  uint64_t a = x & ( m);
  uint64_t b = x & (~m);
  return (a<<(16-n))^(b>>n);
}

// rotate left (and and alias)
static inline uint64_t rol_8x8 (uint64_t x, uint32_t n) { return ror_8x8 (x, 8-n); }
static inline uint64_t rol_16x4(uint64_t x, uint32_t n) { return ror_16x4(x,16-n); }
static inline uint64_t rot_8x8 (uint64_t x, uint32_t n) { return rol_8x8 (x,   n); }
static inline uint64_t rot_16x4(uint64_t x, uint32_t n) { return rol_16x4(x,   n); }


// rotate left each byte by pos: b[i] = rol(b[i],i)
static inline uint64_t rol_by_pos_8x8(uint64_t x)
{
#if (BITOPS_HAS_SCATTER_GATHER)
  x = bit_permute_sg_step_64(x, 0xfefcf8f0e0c080ff, 0x7f3f1f0f070301ff);
#else  
  x = bit_permute_step_64(x, 0x5500550055005500, 1);
  x = bit_permute_step_64(x, 0x2233110022331100, 2);
  x = bit_permute_step_64(x, 0x080c0e0f07030100, 4);
#endif  
  
  return x;
}

// rotate right each byte by pos: b[i] = ror(b[i],i)
static inline uint64_t ror_by_pos_8x8(uint64_t x)
{
#if (BITOPS_HAS_SCATTER_GATHER)
  x = bit_permute_sg_step_64(x, 0x80c0e0f0f8fcfeff, 0x0103070f1f3f7fff);
#else  
  x = bit_permute_step_64(x, 0x5500550055005500, 1);
  x = bit_permute_step_64(x, 0x1133220011332200, 2);
  x = bit_permute_step_64(x, 0x0103070f0e0c0800, 4);
#endif  
  
  return x;
}

// gather the low bit each each byte
static inline uint64_t bit_gather_lsb_8x8(uint64_t x)
{
  const uint64_t m = UINT64_C(0x0101010101010101);

#if (BITOPS_HAS_SCATTER_GATHER)
  return bit_gather_64(x,m);
#else
  const uint64_t k = UINT64_C(0x0102040810204080);
  
  return (k * (x & m)) >> 56;
#endif  
}

// scatters low 8 bits to the lsb of each byte
static inline uint64_t bit_scatter_lsb_8x8(uint64_t x)
{
  const uint64_t m = UINT64_C(0x0101010101010101);

#if (BITOPS_HAS_SCATTER_GATHER)
  return bit_scatter_64(x,m);
#else  
  return (x & 1) | (((x & 0xfe) * 0x2040810204080) & m);
#endif  
}


// scatters low 4 bits to the lsb of each 16-bit element
static inline uint64_t bit_scatter_lsb_16x4(uint64_t x)
{
  const uint64_t m = UINT64_C(0x0001000100010001);

#if (BITOPS_HAS_SCATTER_GATHER)
  return bit_scatter_64(x,m);
#else
  return (((x & 0xf) * 0x0000200040008001) & m);
#endif  
}

static inline uint64_t bit_gather_lsb_16x4(uint64_t x)
{
  const uint64_t m = UINT64_C(0x0001000100010001);

#if (BITOPS_HAS_SCATTER_GATHER)
  return bit_gather_64(x,m);
#else
  return (((x & m) * 0x0002040810204081) >> 49) & 0xff;
#endif  
}

#endif
