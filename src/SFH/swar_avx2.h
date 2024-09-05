// Marc B. Reynolds, 2023-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef SWAR_AVX2_H
#define SWAR_AVX2_H

//------------------------------------------------------------------------------
// the idea is to cover Intel some 256 bit integer ops (including scalar interactions)
// so various AVX512 features should be added that simplify an otherwise AVX2
// specific implementation. But anyway..just covering stuff that I need for other
// stuff. Not a portability libray or somesuch. 
//
// these are bunch of trival wrappers. I hate stuff like this too. It's not
// just you. BUT:
//   * copypasta-a-rific. handy for scanning same op across different widths (IMHO)
//   * a babystep toward allowing usage of extern libraries like SIMDe less painful
//   * they really aren't intended to external usage anywho
//   * and at some point I should look at how it plays out just direct using the
//     (below) W macro instead of creating the rename macro wrappers.
//
// On the whole they follow the pattern:
//   1) strip _mm256_ prefix
//   2) replace element width suffixes. ep{iu}8 -> 8x32, etc
//   3) if op has signed and unsigned variants adds u/s prefix
// 
// Those that don't follow this pattern are grouped together in the next
// section.

// base expansion: adds any needed prefix for the intrinsic
#if !defined(AVX_PREFIX)
#if  defined(SIMDE_VERSION)
#define AVX2_PREFIX simde_mm256_
typedef simde__m256i u256_t;

// emit macros for intrinsics w required constant "input"
// many of these require GCC expression statements though.
#define AVX2_MACRO_KOP
#else
#define AVX2_PREFIX _mm256_
typedef __m256i u256_t;
#endif
#endif

// define concat macro if needed
#ifndef SFH_CAT
#define SFH_CAT(a, ...) SFH_CAT_X(a, __VA_ARGS__)
#define SFH_CAT_X(a, ...) a ## __VA_ARGS__
#endif

// temp hack
#ifndef SSE_MM_SHUFFLE
#define SSE_MM_SHUFFLE(A,B,C,D) (((A)<<6)|((B)<<4)|((C) << 2)|(D))
#endif

// punting to expression-statements in this case for the more involved
#if defined(AVX2_MACRO_KOP)
#pragma GCC diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#endif

// just macros to have lower friction to nuke (like: preprocessor/copy/paste)
#define loadu_256      SFH_CAT(AVX2_PREFIX, loadu_si256)
#define storeu_256     SFH_CAT(AVX2_PREFIX, storeu_si256)

#define movemask_8x32  (uint32_t)SFH_CAT(AVX2_PREFIX, movemask_epi8)

#define or_256         SFH_CAT(AVX2_PREFIX, or_si256)
#define xor_256        SFH_CAT(AVX2_PREFIX, xor_si256)
#define and_256        SFH_CAT(AVX2_PREFIX, and_si256)

#define andnot_256     SFH_CAT(AVX2_PREFIX, andnot_si256)

#define abs_8x32       SFH_CAT(AVX2_PREFIX, abs_epi8)
#define abs_16x16      SFH_CAT(AVX2_PREFIX, abs_epi16)
#define abs_32x8       SFH_CAT(AVX2_PREFIX, abs_epi32)

#define add_8x32       SFH_CAT(AVX2_PREFIX, add_epi8)
#define add_16x16      SFH_CAT(AVX2_PREFIX, add_epi16)
#define add_32x8       SFH_CAT(AVX2_PREFIX, add_epi32)
#define add_64x4       SFH_CAT(AVX2_PREFIX, add_epi64)

#define sub_8x32       SFH_CAT(AVX2_PREFIX, sub_epi8)
#define sub_16x16      SFH_CAT(AVX2_PREFIX, sub_epi16)
#define sub_32x8       SFH_CAT(AVX2_PREFIX, sub_epi32)
#define sub_64x4       SFH_CAT(AVX2_PREFIX, sub_epi64)

#define mullo_16x16    SFH_CAT(AVX2_PREFIX, mullo_epi16)
#define mullo_32x8     SFH_CAT(AVX2_PREFIX, mullo_epi32)
#define mulhi_16x16    SFH_CAT(AVX2_PREFIX, mulhi_epi16)
#define mulhi_32x8     SFH_CAT(AVX2_PREFIX, mulhi_epi32)

// r[i] = (a[i]+b[i]+1) >> 1 (without overflow)
// aka: ceil((x+y)/2) = (x|y) - ((x^y)>>1)
#define avg_8x32       SFH_CAT(AVX2_PREFIX, avg_epu8)
#define avg_16x16      SFH_CAT(AVX2_PREFIX, avg_epu16)

#define sad_8x32       SFH_CAT(AVX2_PREFIX, sad_epu8)

#define hadd_16x16     SFH_CAT(AVX2_PREFIX, hadd_epi16)
#define hadd_32x8      SFH_CAT(AVX2_PREFIX, hadd_epi32)
#define hsub_16x16     SFH_CAT(AVX2_PREFIX, hsub_epi16)
#define hsub_32x8      SFH_CAT(AVX2_PREFIX, hsub_epi32)

// min/max of unsigned elements (add prefix u)
#define umin_8x32      SFH_CAT(AVX2_PREFIX, min_epu8)
#define umin_16x16     SFH_CAT(AVX2_PREFIX, min_epu16)
#define umin_32x8      SFH_CAT(AVX2_PREFIX, min_epu32)
#define umax_8x32      SFH_CAT(AVX2_PREFIX, max_epu8)
#define umax_16x16     SFH_CAT(AVX2_PREFIX, max_epu16)
#define umax_32x8      SFH_CAT(AVX2_PREFIX, max_epu32)

// min/max of signed elements (add prefix s)
#define smin_8x32      SFH_CAT(AVX2_PREFIX, min_epi8)
#define smin_16x16     SFH_CAT(AVX2_PREFIX, min_epi16)
#define smin_32x8      SFH_CAT(AVX2_PREFIX, min_epi32)
#define smax_8x32      SFH_CAT(AVX2_PREFIX, max_epi8) 
#define smax_16x16     SFH_CAT(AVX2_PREFIX, max_epi16)
#define smax_32x8      SFH_CAT(AVX2_PREFIX, max_epi32)

#if 0
//  AVX512VL
#define abs_64x4       SFH_CAT(AVX2_PREFIX, abs_epi64)
#define umax_64x4      SFH_CAT(AVX2_PREFIX, max_epu64)
#define smax_64x4      SFH_CAT(AVX2_PREFIX, max_epi64)
#endif

// sign(a,b) = a*sign(b): r[n] = (b[n]<0) ? -a[n] : ((b[n] != 0) ? a[n] : 0)
#define sign_8x32      SFH_CAT(AVX2_PREFIX, sign_epi8)
#define sign_16x16     SFH_CAT(AVX2_PREFIX, sign_epi16)
#define sign_32x8      SFH_CAT(AVX2_PREFIX, sign_epi32)


// compile time constant shifts (all elems same shift amount)
#define srli_16x16     SFH_CAT(AVX2_PREFIX, srli_epi16)
#define srli_32x8      SFH_CAT(AVX2_PREFIX, srli_epi32)
#define srli_64x4      SFH_CAT(AVX2_PREFIX, srli_epi64)
#define slli_16x16     SFH_CAT(AVX2_PREFIX, slli_epi16)
#define slli_32x8      SFH_CAT(AVX2_PREFIX, slli_epi32)
#define slli_64x4      SFH_CAT(AVX2_PREFIX, slli_epi64)

#define bslli_128x2    SFH_CAT(AVX2_PREFIX, bslli_epi128)
#define bsrli_128x2    SFH_CAT(AVX2_PREFIX, bsrli_epi128)

// all elems same shift amount (low 64-bit of count reg)
#define srl_16x16      SFH_CAT(AVX2_PREFIX, srl_epi16)
#define srl_32x8       SFH_CAT(AVX2_PREFIX, srl_epi32)
#define srl_64x4       SFH_CAT(AVX2_PREFIX, srl_epi64)
#define sll_16x16      SFH_CAT(AVX2_PREFIX, sll_epi16)
#define sll_32x8       SFH_CAT(AVX2_PREFIX, sll_epi32)
#define sll_64x4       SFH_CAT(AVX2_PREFIX, sll_epi64)

#define sllv_32x8      SFH_CAT(AVX2_PREFIX, sllv_epi32)
#define sllv_64x4      SFH_CAT(AVX2_PREFIX, sllv_epi64)
#define srlv_32x8      SFH_CAT(AVX2_PREFIX, srlv_epi32)
#define srlv_64x4      SFH_CAT(AVX2_PREFIX, srlv_epi64)

#define sra_16x16      SFH_CAT(AVX2_PREFIX, sra_epi16)
#define sra_32x8       SFH_CAT(AVX2_PREFIX, sra_epi32)
#define srai_16x16     SFH_CAT(AVX2_PREFIX, srai_epi16)
#define srai_32x8      SFH_CAT(AVX2_PREFIX, srai_epi32)
#define srav_32x8      SFH_CAT(AVX2_PREFIX, srav_epi32)

#define cmpeq_8x32     SFH_CAT(AVX2_PREFIX, cmpeq_epi8)
#define cmpeq_16x16    SFH_CAT(AVX2_PREFIX, cmpeq_epi16)
#define cmpeq_32x8     SFH_CAT(AVX2_PREFIX, cmpeq_epi32)
#define cmpeq_64x4     SFH_CAT(AVX2_PREFIX, cmpeq_epi64)

#define cmpgt_8x32     SFH_CAT(AVX2_PREFIX, cmpgt_epi8)
#define cmpgt_16x16    SFH_CAT(AVX2_PREFIX, cmpgt_epi16)
#define cmpgt_32x8     SFH_CAT(AVX2_PREFIX, cmpgt_epi32)
#define cmpgt_64x4     SFH_CAT(AVX2_PREFIX, cmpgt_epi64)

#define blend_8x32     SFH_CAT(AVX2_PREFIX, blend_epi8)
#define blend_16x16    SFH_CAT(AVX2_PREFIX, blend_epi16)
#define blend_32x8     SFH_CAT(AVX2_PREFIX, blend_epi32)

#define unpackhi_8x32  SFH_CAT(AVX2_PREFIX, unpackhi_epi8)
#define unpacklo_8x32  SFH_CAT(AVX2_PREFIX, unpacklo_epi8)
#define unpackhi_16x16 SFH_CAT(AVX2_PREFIX, unpackhi_epi16)
#define unpacklo_16x16 SFH_CAT(AVX2_PREFIX, unpackhi_epi16)
#define unpackhi_32x8  SFH_CAT(AVX2_PREFIX, unpackhi_epi32)
#define unpacklo_32x8  SFH_CAT(AVX2_PREFIX, unpackhi_epi32)
#define unpackhi_64x4  SFH_CAT(AVX2_PREFIX, unpackhi_epi64)
#define unpacklo_64x4  SFH_CAT(AVX2_PREFIX, unpackhi_epi64)

//------------------------------------------------------------------------------
// these (to some extent) break the wrapper naming scheme (beyond signed/unsigned prefixes)

//static inline uint32_t movd_256(u256_t a) { return (uint32_t)SFH_CAT(AVX2_PREFIX, cvtsi256_si32)(a); }

#define alignr_128x2    SFH_CAT(AVX2_PREFIX, alignr_epi8)
#define zero_256        SFH_CAT(AVX2_PREFIX, setzero_si256)
#define pshufb_128x2    SFH_CAT(AVX2_PREFIX, shuffle_epi8)

// broadcast (scalar) 'k' to all elements
#define broadcast_8x32  SFH_CAT(AVX2_PREFIX, set1_epi8 )
#define broadcast_16x16 SFH_CAT(AVX2_PREFIX, set1_epi16)
#define broadcast_32x8  SFH_CAT(AVX2_PREFIX, set1_epi32)
#define broadcast_64x4  SFH_CAT(AVX2_PREFIX, set1_epi64x)

// the reversal is unfortunate
#define permute_64x4    SFH_CAT(AVX2_PREFIX, permute4x64_epi64)

#define cast_128_256    SFH_CAT(AVX2_PREFIX, castsi256_si128)

// low element of 'x' broadcast to all (the 'cast' isn't a real operation)
static inline u256_t broadcast_lo_8x32 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastb_epi8 )(cast_128_256(x)); }
static inline u256_t broadcast_lo_16x16(u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastw_epi16)(cast_128_256(x)); }
static inline u256_t broadcast_lo_32x8 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastd_epi32)(cast_128_256(x)); }
static inline u256_t broadcast_lo_64x4 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastq_epi64)(cast_128_256(x)); }


//------------------------------------------------------------------------------
// start of composed functions

// return (scalar) true/false if all elements of a & b are equal
static inline bool cmp_equal_256(u256_t a, u256_t b) { return movemask_8x32(cmpeq_8x32(a,b)) == 0xFFFFFFFF; }

// all bits set : should lower to vpcmpeqd ymmR,ymmN,ymmN (ymmN is any)
static inline u256_t bit_allset_256(void) { u256_t t = zero_256(); return cmpeq_8x32(t,t); }

// ~x
static inline u256_t bit_not_256(u256_t x) { return xor_256(x,bit_allset_256()); }

// each element -1
static inline u256_t dec_8x32 (u256_t x) { return add_8x32 (x,bit_allset_256()); }
static inline u256_t dec_16x16(u256_t x) { return add_16x16(x,bit_allset_256()); }
static inline u256_t dec_32x8 (u256_t x) { return add_32x8 (x,bit_allset_256()); }
static inline u256_t dec_64x4 (u256_t x) { return add_64x4 (x,bit_allset_256()); }

// each element +1
static inline u256_t inc_8x32 (u256_t x) { return sub_8x32 (x,bit_allset_256()); }
static inline u256_t inc_16x16(u256_t x) { return sub_16x16(x,bit_allset_256()); }
static inline u256_t inc_32x8 (u256_t x) { return sub_32x8 (x,bit_allset_256()); }
static inline u256_t inc_64x4 (u256_t x) { return sub_64x4 (x,bit_allset_256()); }

// negate each element
static inline u256_t negate_8x32 (u256_t x) { return sub_8x32 (zero_256(), x); }
static inline u256_t negate_16x16(u256_t x) { return sub_16x16(zero_256(), x); }
static inline u256_t negate_32x8 (u256_t x) { return sub_32x8 (zero_256(), x); }
static inline u256_t negate_64x4 (u256_t x) { return sub_64x4 (zero_256(), x); }

// returns the sum of bytes in each 64-bit element
// * the cleverness of using SAD is from: "Faster Population Counts Using AVX2 Instructions", Mula, Kurz & Lemire
//   (https://arxiv.org/pdf/1611.07612)
static inline u256_t byte_sum_64x4(u256_t x) { return sad_8x32(x, zero_256()); }

// alignr : amount compile time
#define byte_barrel_shift_128x2(X,I) alignr_128x2(X,X,I)

//------------------------------------------------------------------------------


#if !defined(rorv_32x8)
static inline u256_t rorv_32x8(u256_t x, u256_t n)
{
  u256_t a = broadcast_32x8(32);
  u256_t b = dec_32x8(a);

  n = and_256(n,b); 
  b = sub_32x8(a,n);

  return xor_256(sllv_32x8(x,n), srlv_32x8(x,b));
}
#endif

#if !defined(rorv_64x4)
static inline u256_t rorv_64x4(u256_t x, u256_t n)
{
  u256_t a = broadcast_64x4(64);
  u256_t b = dec_64x4(a);

  n = and_256(n,b); 
  b = sub_64x4(a,n);

  return xor_256(sllv_64x4(x,n), srlv_64x4(x,b));
}
#endif



#if !defined(AVX2_MACRO_KOP)
// per element: x ^ (x >> s)
static inline u256_t rxorshift_16x16(u256_t x, const int s) { return xor_256(x,srli_16x16(x,s)); }
static inline u256_t rxorshift_32x8 (u256_t x, const int s) { return xor_256(x,srli_32x8 (x,s)); }
static inline u256_t rxorshift_64x4 (u256_t x, const int s) { return xor_256(x,srli_64x4 (x,s)); }

// per element: x ^ (x << s)
static inline u256_t lxorshift_16x16(u256_t x, const int s) { return xor_256(x,slli_16x16(x,s)); }
static inline u256_t lxorshift_32x8 (u256_t x, const int s) { return xor_256(x,slli_32x8 (x,s)); }
static inline u256_t lxorshift_64x4 (u256_t x, const int s) { return xor_256(x,slli_64x4 (x,s)); }
#else
#define rxorshift_16x16(X,S) xor_256(X,srli_16x16(X,S))
#define rxorshift_32x8(X,S)  xor_256(X,srli_32x8 (X,S))
#define rxorshift_64x4(X,S)  xor_256(X,srli_64x4 (X,S))

#define lxorshift_16x16(X,S) xor_256(X,slli_16x16(X,S))
#define lxorshift_32x8(X,S)  xor_256(X,slli_32x8 (X,S))
#define lxorshift_64x4(X,S)  xor_256(X,slli_64x4 (X,S))
#endif

// isolate lowest set bit per element (bit set in location. all others clear)
static inline u256_t bit_lowest_set_8x32 (u256_t x) { return and_256(x,negate_8x32 (x)); }
static inline u256_t bit_lowest_set_16x16(u256_t x) { return and_256(x,negate_16x16(x)); }
static inline u256_t bit_lowest_set_32x8 (u256_t x) { return and_256(x,negate_32x8 (x)); }
static inline u256_t bit_lowest_set_64x4 (u256_t x) { return and_256(x,negate_64x4 (x)); }

// isolate lowest clear bit per element (bit set in location. all others clear)
static inline u256_t bit_lowest_clear_8x32 (u256_t x) { return and_256(inc_8x32 (x),bit_not_256(x)); }
static inline u256_t bit_lowest_clear_16x16(u256_t x) { return and_256(inc_16x16(x),bit_not_256(x)); }
static inline u256_t bit_lowest_clear_32x8 (u256_t x) { return and_256(inc_32x8 (x),bit_not_256(x)); }
static inline u256_t bit_lowest_clear_64x4 (u256_t x) { return and_256(inc_64x4 (x),bit_not_256(x)); }

// isolate lowest bit that differs from the LSB per element (bit set in location. all others clear)
static inline u256_t bit_lowest_changed_8x32 (u256_t x) { return and_256(inc_8x32 (x),negate_8x32 (x)); }
static inline u256_t bit_lowest_changed_16x16(u256_t x) { return and_256(inc_16x16(x),negate_16x16(x)); }
static inline u256_t bit_lowest_changed_32x8 (u256_t x) { return and_256(inc_32x8 (x),negate_32x8 (x)); }
static inline u256_t bit_lowest_changed_64x4 (u256_t x) { return and_256(inc_64x4 (x),negate_64x4 (x)); }


//-------------------------------------------------------------------------------------------------------------

#if !defined(AVX2_MACRO_KOP)
// table entries are duplicated per 128 bit lane
static inline u256_t
pshufb_table_128x2(char B0,char B1,char B2,char B3,
		   char B4,char B5,char B6,char B7,
		   char B8,char B9,char BA,char BB,
		   char BC,char BD,char BE,char BF)
{
  return SFH_CAT(AVX2_PREFIX, setr_epi8)(B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF,
		      B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF);
}
#else
#define pshufb_table_128x2(B0,B1,B2,B3,B4,B5,B6,B7, \
                           B8,B9,BA,BB,BC,BD,BE,BF) \
  SFH_CAT(AVX2_PREFIX, setr_epi8)(B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF)
#endif

// meh
static inline u256_t byte_shuffle_128x2(u256_t x, u256_t table) { return SFH_CAT(AVX2_PREFIX, shuffle_epi8)(x,table); }

typedef struct { u256_t lo,hi; } m256_pair_t;

// lo/hi nibble PSHUFB lookup worker. (r.lo = lo[low nibble], r.hi = hi[high nibble])
static inline m256_pair_t pshufb_lookup_256(u256_t x, u256_t lo, u256_t hi)
{
  u256_t m  = broadcast_8x32(0x0f);
  u256_t t;

  t = andnot_256(m,x);
  x = and_256   (m,x);
  t = srli_16x16(t,4);
  x = pshufb_128x2(lo,x);
  t = pshufb_128x2(hi,t);

  return (m256_pair_t){.lo=x, .hi=t};
}


// swap upper/lower 128 bit lanes
static inline u256_t swap_128x2(u256_t x) { return permute_64x4(x, SSE_MM_SHUFFLE(1,0,3,2)); }

// reverse the bytes in each element
static inline u256_t byte_reverse_16x16(u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14)); }
static inline u256_t byte_reverse_32x8 (u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12)); }
static inline u256_t byte_reverse_64x4 (u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8)); }
static inline u256_t byte_reverse_128x2(u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)); }

static inline u256_t byte_reverse_256(u256_t x)
{
  return swap_128x2(byte_reverse_128x2(x));
}


static inline u256_t bit_reverse_8x32(u256_t x)
{
  u256_t      k = pshufb_table_128x2(0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15);
  m256_pair_t r = pshufb_lookup_256(x,k,k);
  u256_t      t = slli_16x16(r.lo,4);
  
  return xor_256(r.hi,t);
}

static inline u256_t bit_reverse_16x16(u256_t x) {  return byte_reverse_16x16(bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_32x8 (u256_t x) {  return byte_reverse_32x8 (bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_64x4 (u256_t x) {  return byte_reverse_64x4 (bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_128x2(u256_t x) {  return byte_reverse_128x2(bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_256  (u256_t x) {  return byte_reverse_256  (bit_reverse_8x32(x)); }


// computes the population count of each data width

#if 1
static inline u256_t pop_8x32(u256_t x)
{
  u256_t k = pshufb_table_128x2(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
  
  m256_pair_t r = pshufb_lookup_256(x,k,k);
  
  return add_8x32(r.hi,r.lo);
}

static inline u256_t pop_16x16(u256_t x)
{
  u256_t m = broadcast_16x16(0xff);
  u256_t t;

  x = pop_8x32(x);
  t = srli_16x16(x,8);
  x = add_8x32(x,t);
  
  return and_256(m,x);
}

static inline u256_t pop_32x8(u256_t x)
{
  u256_t m = broadcast_32x8(0xff);
  u256_t t;

  x = pop_8x32(x);
  t = srli_16x16(x,8);
  x = add_8x32(x,t);
  t = srli_32x8(x,16);
  x = add_8x32(x,t);
  
  return and_256(m,x);
}

static inline u256_t pop_64x4(u256_t x) { return byte_sum_64x4(pop_8x32(x)); }

#else
// AVX512_BITALG + AVX512VL
#define pop_8x32     SFH_CAT(AVX2_PREFIX, popcnt_epi8)
#define pop_16x16    SFH_CAT(AVX2_PREFIX, popcnt_epi16)
#define pop_32x8     SFH_CAT(AVX2_PREFIX, popcnt_epi32)
#define pop_64x4     SFH_CAT(AVX2_PREFIX, popcnt_epi64)
#endif




// computes the parity of each data width. result is LSB in width
static inline u256_t bit_parity_8x32(u256_t x)
{
  u256_t k  = pshufb_table_128x2(0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0);

  m256_pair_t r = pshufb_lookup_256(x,k,k);

  return xor_256(r.hi,r.lo);
}

static inline u256_t bit_parity_16x16(u256_t x)
{
  x = bit_parity_8x32(x);                   // per byte parity
  x = rxorshift_16x16(x,8);                 // add the 2 subresults
  x = and_256(x,broadcast_16x16(1));        // isolate the final results

  return x;
}

static inline u256_t bit_parity_32x8(u256_t x)
{
  x = bit_parity_8x32(x);
  x = rxorshift_16x16(x, 8);
  x = rxorshift_32x8 (x,16);
  x = and_256(x,broadcast_32x8(1));

  return x;
}

static inline u256_t bit_parity_64x4(u256_t x)
{
  x = bit_parity_8x32(x);
  x = byte_sum_64x4(x);
  x = and_256(x,broadcast_64x4(1));

  return x;
}

static inline u256_t bit_prefix_sum_16x16(u256_t x)
{
  x = rxorshift_16x16(x, 1);
  x = rxorshift_16x16(x, 2);
  x = rxorshift_16x16(x, 4);
  x = rxorshift_16x16(x, 8);

  return x;
}

static inline u256_t bit_suffix_sum_16x16(u256_t x)
{
  x = lxorshift_16x16(x, 1);
  x = lxorshift_16x16(x, 2);
  x = lxorshift_16x16(x, 4);
  x = lxorshift_16x16(x, 8);

  return x;
}

static inline u256_t bit_prefix_sum_32x8(u256_t x)
{
  x = rxorshift_32x8(x, 1);
  x = rxorshift_32x8(x, 2);
  x = rxorshift_32x8(x, 4);
  x = rxorshift_32x8(x, 8);
  x = rxorshift_32x8(x,16);

  return x;
}

static inline u256_t bit_suffix_sum_32x8(u256_t x)
{
  x = lxorshift_32x8(x, 1);
  x = lxorshift_32x8(x, 2);
  x = lxorshift_32x8(x, 4);
  x = lxorshift_32x8(x, 8);
  x = lxorshift_32x8(x,16);

  return x;
}

static inline u256_t bit_prefix_sum_64x4(u256_t x)
{
  x = rxorshift_64x4(x, 1);
  x = rxorshift_64x4(x, 2);
  x = rxorshift_64x4(x, 4);
  x = rxorshift_64x4(x, 8);
  x = rxorshift_64x4(x,16);
  x = rxorshift_64x4(x,32);

  return x;
}

static inline u256_t bit_suffix_sum_64x4(u256_t x)
{
  x = lxorshift_64x4(x, 1);
  x = lxorshift_64x4(x, 2);
  x = lxorshift_64x4(x, 4);
  x = lxorshift_64x4(x, 8);
  x = lxorshift_64x4(x,16);
  x = lxorshift_64x4(x,32);

  return x;
}


// number of leading zeros in each byte
static inline u256_t clz_8x32(u256_t x)
{
  u256_t      lo = pshufb_table_128x2(8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4);
  u256_t      hi = pshufb_table_128x2(8,3,2,2,1,1,1,1,0,0,0,0,0,0,0,0);
  m256_pair_t r  = pshufb_lookup_256(x,lo,hi);

  return umin_8x32(r.hi,r.lo);
}

// number of trailing zeros in each byte
static inline u256_t ctz_8x32(u256_t x)
{
  u256_t      lo = pshufb_table_128x2(8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0);
  u256_t      hi = pshufb_table_128x2(8,4,5,4,6,4,5,4,7,4,5,4,6,4,5,4);
  m256_pair_t r  = pshufb_lookup_256(x,lo,hi);

  return umin_8x32(r.hi,r.lo);
}

static inline u256_t clz_16x16(u256_t x)
{
  u256_t t,s,m;

  x = clz_8x32(x);

  m = broadcast_16x16(0xff);
  s = broadcast_8x32(8);
  t = srli_16x16(x,8);
  
  s = cmpeq_8x32(t,s);
  x = and_256 (x,s);
  
  x = add_8x32(t,x);
  
  return and_256(x,m);
}

static inline u256_t ctz_16x16(u256_t x)
{
  u256_t t,s,m;

  x = ctz_8x32(x);

  m = broadcast_16x16(0xff);  
  s = broadcast_8x32(8);  
  t = srli_16x16(x,8);    

  s = cmpeq_8x32(x,s);  
  t = and_256 (t,s);

  x = add_8x32(t,x);

  return and_256(x,m);  
}

#if 0
// AVX512CD + AVX512VL
#define ctz_32x8       SFH_CAT(AVX2_PREFIX, lzcnt_epi32)
#define ctz_64x4       SFH_CAT(AVX2_PREFIX, lzcnt_epi64)
#endif


// per 128 lane (riffle 2x16)
static inline u256_t byte_zip_128x2(u256_t x)
{
  u256_t m = pshufb_table_128x2(0x0,0x8,0x1,0x9,0x2,0xa,0x3,0xb,0x4,0xc,0x5,0xd,0x6,0xe,0x7,0xf);
  
  return pshufb_128x2(x,m);
}

// per 128-bit lane (unriffle 2x16)
static inline u256_t byte_unzip_128x2(u256_t x)
{
  u256_t m = pshufb_table_128x2(0x0,0x2,0x4,0x6,0x8,0xa,0xc,0xe,0x1,0x3,0x5,0x7,0x9,0xb,0xd,0xf);
  
  return pshufb_128x2(x,m);
}

// macro version
#define DELTA_SWAP_64x4(X,Y,M,S) { u256_t t; t = and_256(xor_256(X, srli_64x4(Y,S)),M); X = xor_256(X, t); Y = xor_256(Y, slli_64x4(t,S));}

#if !defined(AVX2_MACRO_KOP)

// perform a delta swap in each 64-bit lane: bits indicated by 'm' are swapped with the bits
static inline u256_t bit_permute_step_64x4(u256_t x, u256_t m, const int s)
{
  u256_t a,b,c;
  
  a = rxorshift_64x4(x,s); b = and_256(a,m);
  c = lxorshift_64x4(b,s); x = xor_256(c,x);
  
  return x;
}

static inline u256_t bit_permute_step_simple_64x4(u256_t x, u256_t m, const int s)
{
  u256_t t0 = slli_64x4(and_256 (x,m),s);
  u256_t t1 = and_256 (srli_64x4(x,s),m);
  
  return xor_256(t0,t1);
}

#else

#define bit_permute_step_64x4(X,M,S)   \
({                                     \
  u256_t x_ = X;                       \
  u256_t m_ = M;                       \
  u256_t a  = rxorshift_64x4(x_,S);    \
  u256_t b  = and_256(a,m_);           \
  u256_t c  = lxorshift_64x4(b,S);     \
  xor_256(c,x_);                       \
})

#define bit_permute_step_simple_64x4(X,M,S)  \
({                                           \
  u256_t mx = X;                             \
  u256_t mm = M;                             \
  u256_t t0 = slli_64x4(and_256 (mx,mm), S); \
  u256_t t1 = and_256 (srli_64x4(mx, S),mm); \
  xor_256(t0,t1);                            \
})

#endif

#endif
