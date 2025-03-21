// Marc B. Reynolds, 2023-2025
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
//

// base expansion: adds any needed prefix for the intrinsic
#if !defined(AVX_PREFIX)
#if  defined(SIMDE_VERSION)
#define AVX_PREFIX  simde_mm_
#define AVX2_PREFIX simde_mm256_
typedef simde__m256i u256_t;
typedef simde__m128i u128_t;

// emit macros for intrinsics w required constant "input"
// many of these require GCC expression statements though.
#define AVX2_MACRO_KOP
#else
#define AVX_PREFIX _mm_
#define AVX2_PREFIX _mm256_
typedef __m256i u256_t;
typedef __m128i u128_t;
#endif
#endif

// define concat macro if needed
#ifndef SFH_CAT
#define SFH_CAT(a, ...) SFH_CAT_X(a, __VA_ARGS__)
#define SFH_CAT_X(a, ...) a ## __VA_ARGS__
#endif

// temp hack: and need a better name
// 4 2-bit selectors
#define AVX_SHUFFLE_4(A,B,C,D) (((A)<<6)|((B)<<4)|((C) << 2)|(D))

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

// signed/unsigned staturated add/sub
#define add_ss_8x32    SFH_CAT(AVX2_PREFIX, adds_epi8)
#define add_ss_16x16   SFH_CAT(AVX2_PREFIX, adds_epi16)
#define add_us_8x32    SFH_CAT(AVX2_PREFIX, adds_epu8)
#define add_us_16x16   SFH_CAT(AVX2_PREFIX, adds_epu16)

#define sub_ss_8x32    SFH_CAT(AVX2_PREFIX, subs_epi8)
#define sub_ss_16x16   SFH_CAT(AVX2_PREFIX, subs_epi16)
#define sub_us_8x32    SFH_CAT(AVX2_PREFIX, subs_epu8)
#define sub_us_16x16   SFH_CAT(AVX2_PREFIX, subs_epu16)

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
// umin/umax_64x4 composites below

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
#define srai_16x16     SFH_CAT(AVX2_PREFIX, srai_epi16)
#define srai_32x8      SFH_CAT(AVX2_PREFIX, srai_epi32)

#define bslli_128x2    SFH_CAT(AVX2_PREFIX, bslli_epi128)
#define bsrli_128x2    SFH_CAT(AVX2_PREFIX, bsrli_epi128)

// all elems same shift amount (low 64-bit of count reg)
#define srl_16x16      SFH_CAT(AVX2_PREFIX, srl_epi16)
#define srl_32x8       SFH_CAT(AVX2_PREFIX, srl_epi32)
#define srl_64x4       SFH_CAT(AVX2_PREFIX, srl_epi64)
#define sll_16x16      SFH_CAT(AVX2_PREFIX, sll_epi16)
#define sll_32x8       SFH_CAT(AVX2_PREFIX, sll_epi32)
#define sll_64x4       SFH_CAT(AVX2_PREFIX, sll_epi64)
#define sra_16x16      SFH_CAT(AVX2_PREFIX, sra_epi16)
#define sra_32x8       SFH_CAT(AVX2_PREFIX, sra_epi32)

#define sllv_32x8      SFH_CAT(AVX2_PREFIX, sllv_epi32)
#define sllv_64x4      SFH_CAT(AVX2_PREFIX, sllv_epi64)
#define srlv_32x8      SFH_CAT(AVX2_PREFIX, srlv_epi32)
#define srlv_64x4      SFH_CAT(AVX2_PREFIX, srlv_epi64)
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
#define blend_pd_256   SFH_CAT(AVX2_PREFIX, blend_pd)
// blend_64x4 hacky macro below

#define blendv_8x32    SFH_CAT(AVX2_PREFIX, blendv_epi8)
#define blendv_pd_256  SFH_CAT(AVX2_PREFIX, blendv_pd)
#define blendv_ps_256  SFH_CAT(AVX2_PREFIX, blendv_ps)
// blendv_32x8 hacky macro below
// blendv_64x4 hacky macro below

#define unpackhi_8x32  SFH_CAT(AVX2_PREFIX, unpackhi_epi8)
#define unpacklo_8x32  SFH_CAT(AVX2_PREFIX, unpacklo_epi8)
#define unpackhi_16x16 SFH_CAT(AVX2_PREFIX, unpackhi_epi16)
#define unpacklo_16x16 SFH_CAT(AVX2_PREFIX, unpackhi_epi16)
#define unpackhi_32x8  SFH_CAT(AVX2_PREFIX, unpackhi_epi32)
#define unpacklo_32x8  SFH_CAT(AVX2_PREFIX, unpackhi_epi32)
#define unpackhi_64x4  SFH_CAT(AVX2_PREFIX, unpackhi_epi64)
#define unpacklo_64x4  SFH_CAT(AVX2_PREFIX, unpackhi_epi64)

#define packus_32x8    SFH_CAT(AVX2_PREFIX, packus_epi32)
#define packus_16x16   SFH_CAT(AVX2_PREFIX, packus_epi16)

// inside each 128-bit lane
#define shuffle_32x8     SFH_CAT(AVX2_PREFIX, shuffle_epi32)

#define shuffle_lo_16x16 SFH_CAT(AVX2_PREFIX, shufflelo_epi16)
#define shuffle_hi_16x16 SFH_CAT(AVX2_PREFIX, shufflehi_epi16)

//------------------------------------------------------------------------------
// these (to some extent) break the wrapper naming scheme (beyond signed/unsigned prefixes)

#define alignr_128x2  SFH_CAT(AVX2_PREFIX, alignr_epi8)
#define zero_256      SFH_CAT(AVX2_PREFIX, setzero_si256)
#define pshufb_128x2  SFH_CAT(AVX2_PREFIX, shuffle_epi8)

#define sub_ssat_8x32   SFH_CAT(AVX2_PREFIX, subs_epi8)
#define sub_ssat_16x16  SFH_CAT(AVX2_PREFIX, subs_epi16)
#define sub_usat_8x32   SFH_CAT(AVX2_PREFIX, subs_epu8)
#define sub_usat_16x16  SFH_CAT(AVX2_PREFIX, subs_epu16)

#define set_8x32      SFH_CAT(AVX2_PREFIX, set_epi8 )
#define set_16x16     SFH_CAT(AVX2_PREFIX, set_epi16)
#define set_32x8      SFH_CAT(AVX2_PREFIX, set_epi32)
#define set_64x4      SFH_CAT(AVX2_PREFIX, set_epi64x)
#define set_128x2     SFH_CAT(AVX2_PREFIX, set_m128i)

#define setr_8x32     SFH_CAT(AVX2_PREFIX, setr_epi8 )
#define setr_16x16    SFH_CAT(AVX2_PREFIX, setr_epi16)
#define setr_32x8     SFH_CAT(AVX2_PREFIX, setr_epi32)
#define setr_64x4     SFH_CAT(AVX2_PREFIX, setr_epi64x)
#define setr_128x2    SFH_CAT(AVX2_PREFIX, setr_m128i)

#define set1_8x32     SFH_CAT(AVX2_PREFIX, set1_epi8 )
#define set1_16x16    SFH_CAT(AVX2_PREFIX, set1_epi16)
#define set1_32x8     SFH_CAT(AVX2_PREFIX, set1_epi32)
#define set1_64x4     SFH_CAT(AVX2_PREFIX, set1_epi64x)

#define extract_8_256  SFH_CAT(AVX2_PREFIX, extract_epi8)
#define extract_16_256 SFH_CAT(AVX2_PREFIX, extract_epi16)
#define extract_32_256 SFH_CAT(AVX2_PREFIX, extract_epi32)
#define extract_64_256 SFH_CAT(AVX2_PREFIX, extract_epi64)

#define extract_128_256 SFH_CAT(AVX2_PREFIX, extracti128_si256)

//--- leverage FP ops
#define movedup_pd    SFH_CAT(AVX2_PREFIX, movedup_pd)
#define moveldup_ps   SFH_CAT(AVX2_PREFIX, moveldup_ps)
#define movehdup_ps   SFH_CAT(AVX2_PREFIX, movehdup_ps)

#define to_f32_256    SFH_CAT(AVX2_PREFIX, castsi256_ps)
#define to_f64_256    SFH_CAT(AVX2_PREFIX, castsi256_pd)
#define from_f32_256  SFH_CAT(AVX2_PREFIX, castps_si256)
#define from_f64_256  SFH_CAT(AVX2_PREFIX, castpd_si256)

//--- via 128-bit
// put scalar 32 bit integer into low element and zero rest
static inline u128_t set_lo_32x4(uint32_t x)
{
  return SFH_CAT(AVX_PREFIX, cvtsi32_si128)((int32_t)x);
}

// put scalar 64 bit integer into low element and zero rest
static inline u128_t set_lo_64x2(uint64_t x)
{
  return SFH_CAT(AVX_PREFIX, cvtsi64_si128)((int64_t)x);
}

// sign/unsigned extend 'n' bit element (128-bit source)
// to 'm' bit elements (256-bit). when there's more input
// elements than out the it's the low out number converted
// {s,u}ext_nx?_mx?{..}
#define uext_8x16_16x16 SFH_CAT(AVX2_PREFIX, cvtepu8_epi16)
#define uext_8x16_32x8  SFH_CAT(AVX2_PREFIX, cvtepu8_epi32)
#define uext_8x16_64x4  SFH_CAT(AVX2_PREFIX, cvtepu8_epi64)
#define sext_8x16_16x16 SFH_CAT(AVX2_PREFIX, cvtepi8_epi16)
#define sext_8x16_32x8  SFH_CAT(AVX2_PREFIX, cvtepi8_epi32)
#define sext_8x16_64x4  SFH_CAT(AVX2_PREFIX, cvtepi8_epi64)

#define uext_16x8_32x8  SFH_CAT(AVX2_PREFIX, cvtepu16_epi32)
#define uext_16x8_64x4  SFH_CAT(AVX2_PREFIX, cvtepu16_epi64)
#define sext_16x8_32x8  SFH_CAT(AVX2_PREFIX, cvtepi16_epi32)
#define sext_16x8_64x4  SFH_CAT(AVX2_PREFIX, cvtepi16_epi64)

#define uext_32x4_64x4  SFH_CAT(AVX2_PREFIX, cvtepu32_epi64)
#define sext_32x4_64x4  SFH_CAT(AVX2_PREFIX, cvtepi32_epi64)

// duplicate even/odd elements
// {e[0],e[0],e[2],e[2]...}
static inline u256_t dup_even_32x8(u256_t x)
{
  return from_f32_256(moveldup_ps(to_f32_256(x)));
}
// {e[1],e[1],e[3],e[3]...}
static inline u256_t dup_odd_32x8(u256_t x)
{
  return from_f32_256(movehdup_ps(to_f32_256(x)));
}

static inline u256_t dup_even_64x4(u256_t x)
{
  return from_f64_256(movedup_pd(to_f64_256(x)));
}


#define blend_64x4(A,B,I)  from_f64_256(blend_pd_256 (to_f64_256(A),to_f64_256(B),I))
#define blendv_64x4(A,B,S) from_f64_256(blendv_pd_256(to_f64_256(A),to_f64_256(B),to_f64_256(S)))
#define blendv_32x8(A,B,S) from_f32_256(blendv_ps_256(to_f32_256(A),to_f32_256(B),to_f64_256(S)))


// broadcast (scalar) 'k' to all elements
static inline u256_t broadcast_8x32 (uint8_t  x) { return set1_8x32 ((int8_t) x); }
static inline u256_t broadcast_16x16(uint16_t x) { return set1_16x16((int16_t)x); }
static inline u256_t broadcast_32x8 (uint32_t x) { return set1_32x8 ((int32_t)x); }
static inline u256_t broadcast_64x4 (uint64_t x) { return set1_64x4 ((int64_t)x); }


// the reversal is unfortunate
#define permutev_32x8   SFH_CAT(AVX2_PREFIX, permutevar8x32_epi32)
#define permutev_pdx4   SFH_CAT(AVX2_PREFIX, permutevar_pd)

// does NOT cross 128-bit lanes (unlike 32x8)
#define permutev_64x4(X,I) from_f64_256(permutev_pdx4(to_f64_256(X),I))

#define permute_ps      SFH_CAT(AVX2_PREFIX, permute_ps)
#define permute_32x8(A,I) from_f32_256(permute_ps(to_f32_256(A),I))
#define permute_64x4    SFH_CAT(AVX2_PREFIX, permute4x64_epi64)
#define permute_128x2   SFH_CAT(AVX2_PREFIX, permute2x128_si256)

#define cast_128_256    SFH_CAT(AVX2_PREFIX, castsi128_si256)
#define cast_256_128    SFH_CAT(AVX2_PREFIX, castsi256_si128)

static inline uint32_t movd_256(u256_t a) { return (uint32_t)SFH_CAT(AVX2_PREFIX, cvtsi256_si32)(a); }
static inline uint64_t movq_256(u256_t a) { return (uint64_t)SFH_CAT(AVX_PREFIX,  cvtsi128_si64)(cast_256_128(a)); }

#define broadcast_lo_8x32_128  SFH_CAT(AVX2_PREFIX, broadcastb_epi8 )
#define broadcast_lo_16x16_128 SFH_CAT(AVX2_PREFIX, broadcastw_epi16)
#define broadcast_lo_32x8_128  SFH_CAT(AVX2_PREFIX, broadcastd_epi32)
#define broadcast_lo_64x4_128  SFH_CAT(AVX2_PREFIX, broadcastq_epi64)

// low element of 'x' broadcast to all (the 'cast' isn't a real operation)
static inline u256_t broadcast_lo_8x32 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastb_epi8 )(cast_256_128(x)); }
static inline u256_t broadcast_lo_16x16(u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastw_epi16)(cast_256_128(x)); }
static inline u256_t broadcast_lo_32x8 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastd_epi32)(cast_256_128(x)); }
static inline u256_t broadcast_lo_64x4 (u256_t x) { return SFH_CAT(AVX2_PREFIX, broadcastq_epi64)(cast_256_128(x)); }


//------------------------------------------------------------------------------
// start of composed functions

// return (scalar) true/false if all elements of a & b are equal
static inline bool cmp_equal_256(u256_t a, u256_t b) { return movemask_8x32(cmpeq_8x32(a,b)) == 0xFFFFFFFF; }

#if 1
// range-remap to get unsigned compares
static inline u256_t ucmpgt_8x32(u256_t a, u256_t b)
{
  u256_t s = broadcast_8x32(0x80);
  return cmpgt_8x32(xor_256(a,s), xor_256(b,s));
}

static inline u256_t ucmpgt_16x16(u256_t a, u256_t b)
{
  u256_t s = broadcast_16x16(0x8000);
  return cmpgt_16x16(xor_256(a,s), xor_256(b,s));
}

static inline u256_t ucmpgt_32x8(u256_t a, u256_t b)
{
  u256_t s = broadcast_32x8(0x80000000);
  return cmpgt_32x8(xor_256(a,s), xor_256(b,s));
}

static inline u256_t ucmpgt_64x4(u256_t a, u256_t b)
{
  u256_t s = broadcast_64x4(0x8000000000000000);
  return cmpgt_64x4(xor_256(a,s), xor_256(b,s));
}
#else
// yeah..avx512
#endif


// attempted to hint to the complier not to promote a computation
// with 'v' into a constant load. use with care and only at point
// of the computation with 'v' to not potentially break constant
// propogation.
//   other_value = op(no_const_fold_256(v))

#if defined(__GNUC__)
#pragma GCC diagnostic push
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif
static inline u256_t hint_no_const_fold_256(u256_t v)
{
  asm volatile ("" : "+x" (v));
  return v;  
}
#pragma GCC diagnostic pop
#else
static inline u256_t hint_no_const_fold_256(u256_t v) { return v; }
#endif

// some 128-bit permutation helpers
//   clear upper/lower 128 bits
//   right shift by 128 bits. aka top to bottom and zero top
//   left  shift by 128 bits. aka bottom to top and zero bottom
static inline u256_t clear_lo_128_256(u256_t x) { return permute_128x2(x,x,0x18); }
static inline u256_t clear_hi_128_256(u256_t x) { return permute_128x2(x,x,0x80); }
static inline u256_t shr_128_256(u256_t x)      { return permute_128x2(x,x,0x83); }
static inline u256_t shl_128_256(u256_t x)      { return permute_128x2(x,x,0x08); }


// broadcast element 'i' to all
static inline u256_t broadcastv_32x8(u256_t x, uint32_t i)
{
  return permutev_32x8(x, broadcast_32x8(i));
}

// 8,16,64 shoehorn 32x8
static inline u256_t broadcastv_8x32(u256_t x, uint32_t i)
{
  u256_t id = broadcast_32x8(i>>2);
  u256_t r0 = permutev_32x8(x, id);
  u256_t r  = pshufb_128x2(r0,broadcast_8x32(i&3));
  return r;
}

static inline u256_t broadcastv_16x16(u256_t x, uint32_t i)
{
  u256_t i0 = broadcast_32x8(i>>1);
  u256_t r0 = permutev_32x8(x, i0);

  i = (i & 1) ? 0x0302 : 0x0100;

  u256_t i1 = broadcast_16x16((uint16_t)i);
  u256_t r  = pshufb_128x2(r0,i1);
  return r;
}

static inline u256_t broadcastv_64x4(u256_t x, uint32_t i)
{
  u256_t id = add_32x8(broadcast_32x8(i<<1), broadcast_64x4(0x100000000));
  u256_t r  = permutev_32x8(x, id);
  return r;
}

#if 1
static inline u256_t umin_64x4(u256_t a, u256_t b)
{
  return blendv_64x4(a,b,ucmpgt_64x4(a,b));
}

static inline u256_t umax_64x4(u256_t a, u256_t b)
{
  return blendv_64x4(b,a,ucmpgt_64x4(a,b));
}

static inline u256_t smin_64x4(u256_t a, u256_t b)
{
  return blendv_8x32(b,a, cmpgt_64x4(b,a));
}

static inline u256_t smax_64x4(u256_t a, u256_t b)
{
  return blendv_8x32(b,a, cmpgt_64x4(a,b));
}

#else
// AVX512VL
#define umin_64x4     SFH_CAT(AVX2_PREFIX, min_epu64)
#define umax_64x4     SFH_CAT(AVX2_PREFIX, max_epu64)
#define smin_64x4     SFH_CAT(AVX2_PREFIX, min_epi64)
#define smax_64x4     SFH_CAT(AVX2_PREFIX, max_epi64)
#endif


// returns true if all elements are the same value
static inline bool   is_rep_8x32 (u256_t x) { return cmp_equal_256(x, broadcast_lo_8x32 (x)); }
static inline bool   is_rep_16x16(u256_t x) { return cmp_equal_256(x, broadcast_lo_16x16(x)); }
static inline bool   is_rep_32x8 (u256_t x) { return cmp_equal_256(x, broadcast_lo_32x8 (x)); }
static inline bool   is_rep_64x6 (u256_t x) { return cmp_equal_256(x, broadcast_lo_64x4 (x)); }


// all bits set : should lower to vpcmpeqd ymmR,ymmN,ymmN (ymmN is any)
static inline u256_t ones_256(void) { u256_t t = zero_256(); return cmpeq_8x32(t,t); }

// ~x
static inline u256_t not_256(u256_t x) { return xor_256(x,ones_256()); }

// each element -1
static inline u256_t dec_8x32 (u256_t x) { return add_8x32 (x,ones_256()); }
static inline u256_t dec_16x16(u256_t x) { return add_16x16(x,ones_256()); }
static inline u256_t dec_32x8 (u256_t x) { return add_32x8 (x,ones_256()); }
static inline u256_t dec_64x4 (u256_t x) { return add_64x4 (x,ones_256()); }

// each element +1
static inline u256_t inc_8x32 (u256_t x) { return sub_8x32 (x,ones_256()); }
static inline u256_t inc_16x16(u256_t x) { return sub_16x16(x,ones_256()); }
static inline u256_t inc_32x8 (u256_t x) { return sub_32x8 (x,ones_256()); }
static inline u256_t inc_64x4 (u256_t x) { return sub_64x4 (x,ones_256()); }

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
#define byte_barrel_shift_128x2 alignr_128x2
#define byte_rotate_128x2(X,A)  alignr_128x2(X,X,A)


static inline u256_t shr_1_8x32(u256_t x)
{
  u256_t z = zero_256();
  u256_t a = avg_8x32(x,z);
  u256_t r = sub_8x32(x,a);
  return r;
}

// alias for action clarity
static inline u256_t unpacklo_128x2(u256_t a, u256_t b) { return permute_128x2(a,b, (2<<4)|0); }
static inline u256_t unpackhi_128x2(u256_t a, u256_t b) { return permute_128x2(a,b, (3<<4)|1); }


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
static inline u256_t bit_lowest_clear_8x32 (u256_t x) { return and_256(inc_8x32 (x),not_256(x)); }
static inline u256_t bit_lowest_clear_16x16(u256_t x) { return and_256(inc_16x16(x),not_256(x)); }
static inline u256_t bit_lowest_clear_32x8 (u256_t x) { return and_256(inc_32x8 (x),not_256(x)); }
static inline u256_t bit_lowest_clear_64x4 (u256_t x) { return and_256(inc_64x4 (x),not_256(x)); }

// isolate lowest bit that differs from the LSB per element (bit set in location. all others clear)
static inline u256_t bit_lowest_changed_8x32 (u256_t x) { return and_256(inc_8x32 (x),negate_8x32 (x)); }
static inline u256_t bit_lowest_changed_16x16(u256_t x) { return and_256(inc_16x16(x),negate_16x16(x)); }
static inline u256_t bit_lowest_changed_32x8 (u256_t x) { return and_256(inc_32x8 (x),negate_32x8 (x)); }
static inline u256_t bit_lowest_changed_64x4 (u256_t x) { return and_256(inc_64x4 (x),negate_64x4 (x)); }


//-------------------------------------------------------------------------------------------------------------


#define set_table_8x4(A,B,C,D) (int8_t)(A),(int8_t)(B),(int8_t)(C),(int8_t)(D)
#define set_table_8x8(A,B,C,D,E,F,G,H) set_table_8x4(A,B,C,D),set_table_8x4(E,F,G,H)

#define set_table_8x32(L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,LA,LB,LC,LD,LE,LF,H0,H1,H2,H3,H4,H5,H6,H7,H8,H9,HA,HB,HC,HD,HE,HF)         \
  setr_8x32(set_table_8x8(L0,L1,L2,L3,L4,L5,L6,L7), \
            set_table_8x8(L8,L9,LA,LB,LC,LD,LE,LF), \
            set_table_8x8(H0,H1,H2,H3,H4,H5,H6,H7), \
            set_table_8x8(H8,H9,HA,HB,HC,HD,HE,HF)) \


#if !defined(AVX2_MACRO_KOP)
// table entries are duplicated per 128 bit lane
static inline u256_t
pshufb_table_128x2(uint8_t B0,uint8_t B1,uint8_t B2,uint8_t B3,
		   uint8_t B4,uint8_t B5,uint8_t B6,uint8_t B7,
		   uint8_t B8,uint8_t B9,uint8_t BA,uint8_t BB,
		   uint8_t BC,uint8_t BD,uint8_t BE,uint8_t BF)
{
  return SFH_CAT(AVX2_PREFIX, setr_epi8)((int8_t)B0,(int8_t)B1,(int8_t)B2,(int8_t)B3,(int8_t)B4,(int8_t)B5,(int8_t)B6,(int8_t)B7,(int8_t)B8,(int8_t)B9,(int8_t)BA,(int8_t)BB,(int8_t)BC,(int8_t)BD,(int8_t)BE,(int8_t)BF,
		      (int8_t)B0,(int8_t)B1,(int8_t)B2,(int8_t)B3,(int8_t)B4,(int8_t)B5,(int8_t)B6,(int8_t)B7,(int8_t)B8,(int8_t)B9,(int8_t)BA,(int8_t)BB,(int8_t)BC,(int8_t)BD,(int8_t)BE,(int8_t)BF);
}
#else
#define pshufb_table_128x2(B0,B1,B2,B3,B4,B5,B6,B7, \
                           B8,B9,BA,BB,BC,BD,BE,BF) \
  SFH_CAT(AVX2_PREFIX, setr_epi8)((int8_t)B0,(int8_t)B1,(int8_t)B2,(int8_t)B3,(int8_t)B4,(int8_t)B5,(int8_t)B6,(int8_t)B7,(int8_t)B8,(int8_t)B9,(int8_t)BA,(int8_t)BB,(int8_t)BC,(int8_t)BD,(int8_t)BE,(int8_t)BF,(int8_t)B0,(int8_t)B1,(int8_t)B2,(int8_t)B3,(int8_t)B4,(int8_t)B5,(int8_t)B6,(int8_t)B7,(int8_t)B8,(int8_t)B9,(int8_t)BA,(int8_t)BB,(int8_t)BC,(int8_t)BD,(int8_t)BE,(int8_t)BF)
#endif

#define pshufb_rtable_128x2(B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF) pshufb_table_128x2(BF,BE,BD,BC,BB,BA,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0)



// register with only bit 'n' set: 256 bit: (1 << n)
// modification of: https://stackoverflow.com/a/39595704/3635927
static inline u256_t bit_n_256(uint32_t n)
{
//u256_t one   = broadcast_32x8(hint_no_const_fold_32(1));
  u256_t one   = broadcast_32x8(1);
  u256_t cnts  = set_32x8(224,192,160,128,96,64,32,0);
  u256_t bn    = broadcast_32x8(n);
  u256_t shift = sub_32x8(bn, cnts);
  u256_t r     = sllv_32x8(one, shift);
  
  return r;
}

// minor mod of: Peter Cordes (https://stackoverflow.com/a/24242696/3635927)
// uiCA predicts 5.0 cycles/iteration throughput (skylake)
static inline u256_t movemask_inverse_256(uint32_t x)
{
  // mapping of bit in 32-bit mask to corresponding byte
  // 8 byte 0, 8 byte 1, etc
  u256_t p = setr_64x4(0x0000000000000000,
		       0x0101010101010101,
		       0x0202020202020202,
		       0x0303030303030303);

  // bit position in mapped byte
  u256_t a  = broadcast_64x4(0x8040201008040201);
  
  u256_t r;

  r = broadcast_32x8(x);          // dup input 4x
  r = pshufb_128x2(r,p);          // rearrange bytes
  r = and_256(r,a);               // isoloate bit of this element
  r = cmpeq_8x32(r,a);            // fill element
  
  return r;
}


// meh
static inline u256_t byte_shuffle_128x2(u256_t x, u256_t table) { return SFH_CAT(AVX2_PREFIX, shuffle_epi8)(x,table); }
static inline u256_t pshufb_256(u256_t x, u256_t table) { return byte_shuffle_128x2(x,table); }

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
static inline u256_t swap_128x2(u256_t x) { return permute_64x4(x, AVX_SHUFFLE_4(1,0,3,2)); }

// reverse the bytes in each element
static inline u256_t byte_reverse_16x16(u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14)); }
static inline u256_t byte_reverse_32x8 (u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12)); }
static inline u256_t byte_reverse_64x4 (u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8)); }
static inline u256_t byte_reverse_128x2(u256_t x) { return pshufb_128x2(x, pshufb_table_128x2(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)); }

static inline u256_t byte_reverse_256(u256_t x)
{
  return swap_128x2(byte_reverse_128x2(x));
}

// reverse n-bit elements (across the 128-bit lanes)
static inline u256_t u32_swap_256(u256_t x) { return swap_128x2(shuffle_32x8(x, AVX_SHUFFLE_4(0,1,2,3))); }
static inline u256_t u64_swap_256(u256_t x) { return permute_64x4(x, AVX_SHUFFLE_4(0,1,2,3)); }

static inline u256_t u16_swap_256(u256_t x)
{
  x = shuffle_lo_16x16(x, AVX_SHUFFLE_4(0,1,2,3));
  x = shuffle_hi_16x16(x, AVX_SHUFFLE_4(0,1,2,3));
  return u64_swap_256(x);
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


// computes the population count of each element

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




// computes the parity of each element width. result is LSB in element
static inline u256_t bit_parity_8x32(u256_t x)
{
  u256_t k  = pshufb_table_128x2(0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0);

  // add nibble pairs and lookup
  x = and_256(xor_256(x,srli_32x8(x,4)), broadcast_8x32(0xf));
  x = pshufb_128x2(k,x);
  return x;
}

static inline u256_t bit_parity_mask_8x32(u256_t x)
{
  u256_t k  = pshufb_table_128x2(0,0xff,0xff,0,0xff,0,0,0xff,0xff,0,0,0xff,0,0xff,0xff,0);

  // add nibble pairs and lookup
  x = and_256(xor_256(x,srli_32x8(x,4)), broadcast_8x32(0xf));
  x = pshufb_128x2(k,x);
  return x;
}

static inline uint32_t bit_parity_packed_8x32(u256_t x)
{
  u256_t k  = pshufb_table_128x2(0,0xff,0xff,0,0xff,0,0,0xff,0xff,0,0,0xff,0,0xff,0xff,0);

  // add nibble pairs and lookup
  x = and_256(xor_256(x,srli_32x8(x,4)), broadcast_8x32(0xf));
  x = pshufb_128x2(k,x);
  return movemask_8x32(x);
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

// leading/trailing byte count across whole register
static inline uint32_t clz_u8_256(u256_t x)
{
  uint32_t r = movemask_8x32(cmpeq_8x32(x, zero_256()));

  return clz_32(r ^ UINT32_C(~0));
}

static inline uint32_t ctz_u8_256(u256_t x)
{
  uint32_t r = movemask_8x32(cmpeq_8x32(x, zero_256()));

  return ctz_32(r ^ UINT32_C(~0));
}

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

static inline u256_t byte_unzip_256(u256_t x)
{
  x = byte_unzip_128x2(x);
  x = permute_64x4(x, AVX_SHUFFLE_4(3,1,2,0));
  return x;
}

// per 128-bit lane (unriffle 2x16)
static inline u256_t u16_unzip_128x2(u256_t x)
{
  u256_t m = pshufb_table_128x2(0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15);
  
  return pshufb_128x2(x,m);
}

static inline u256_t u16_zip_128x2(u256_t x)
{
  u256_t m = pshufb_table_128x2(0,1,8,9,2,3,10,11,4,5,12,13,6,7,14,15);
  
  return pshufb_128x2(x,m);
}

static inline u256_t u64_unzip_256(u256_t x)
{
  return permute_64x4(x, AVX_SHUFFLE_4(3,1,2,0));
}

// macro version
#define DELTA_SWAP2_64x4(X,Y,M,S) { u256_t t; t = and_256(xor_256(X, srli_64x4(Y,S)),M); X = xor_256(X, t); Y = xor_256(Y, slli_64x4(t,S));}

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



// number of selected is: pop_32(movemask_8x32(m)) >> 2
// uiCA predicts 18.0 cycles/iteration throughput (skylake)
static inline u256_t gather_32x8(u256_t x, u256_t mask)
{
  const uint64_t id = 0x0706050403020100;   // element indices
  const uint64_t n  = 0x0f0f0f0f0f0f0f0f;   // scatter nibble selector

  uint32_t mm = movemask_8x32(mask);
  
  // m: mask is elements all set or clear. movemask gives nibble/elem,
  //    scatter to bytes, (input to r) fills in byte.
  // r: gather from 'id' to get selected indices
  // p: expand 'r' to 32-bit elements
  uint64_t m = bit_scatter_64(mm, n);
  uint64_t r = bit_gather_64(id, m^(m<<4));
  u256_t   p = uext_8x16_32x8(set_lo_64x2(r));

  // move the selected but doesn't clear out (any) upper portion
  return permutev_32x8(x,p);
}

// the same as 32x8 except count is obviously different:
// pop_32(movemask_8x32(m)) >> 3
static inline u256_t gather_64x4(u256_t x, u256_t mask)
{
  return gather_32x8(x,mask);
}

#if !defined (SWAR_AVX2_IMPLEMENTATION)

extern u256_t sort_64x4(u256_t x);
extern u256_t sort_32x8(u256_t x);

#else

// sort the 4 64-bit unsigned integers
u256_t sort_64x4(u256_t x)
{
  // bitonic sorting network of 4 elements.
  // (well it's an odd-even mergesort)
  // attempted not using software min/max but was
  // turning out slightly worse.
  // uiCA spitballs at 30.67 cycles. needing to
  // software umin/umax adds 2 xors per pair.
  //
  // alternate stage 1:
  //  t = permute_64x4(x, AVX_SHUFFLE_4(2,3,0,1));
  //  c = dup_even_64x4(ucmpgt_64x4(x,t));
  //  x = blendv_64x4(x,t,c);
  // stages 2 & 3 didn't think of a cheap scheme
  // to modify the selector 'c' like the dup_even

  u256_t t,min,max;

  // sort (0,1) & (2,3) : ascending
  t   = permute_64x4(x, AVX_SHUFFLE_4(2,3,0,1));
  min = umin_64x4(x,t);
  max = umax_64x4(x,t);
  x   = blend_32x8(min, max, 0xcc);

  // sort (0,2) and (1,3) : ascending
  t   = permute_64x4(x, AVX_SHUFFLE_4(1,0,3,2));
  min = umin_64x4(x,t);
  max = umax_64x4(x,t);
  x   = blend_32x8(min, max, 0xf0);

  // sort (1,2)
  t   = permute_64x4(x, AVX_SHUFFLE_4(3,1,2,0));
  min = umin_64x4(x, t);
  max = umax_64x4(x, t);
  x   = blend_32x8(min, max, 0x30);

  return x;
}

// sort the 8 32-bit unsigned integers
u256_t sort_32x8(u256_t x)
{
  // bitonic mergesort
  // uiCA splitballs at 20.18 cycles
  u256_t t,lo,hi;

  // stage 1 (1 step)
  t  = permute_32x8(x, AVX_SHUFFLE_4(2,3,0,1));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xaa); // 0b10101010

  // stage 2 (2 steps)
  t  = permute_32x8(x, AVX_SHUFFLE_4(0,1,2,3));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xcc); // 0b11001100

  t  = permute_32x8(x, AVX_SHUFFLE_4(2,3,0,1));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xaa); // 0b10101010

  // stage 3 (3 steps)
  u256_t s = swap_128x2(x);
  
  t  = permute_32x8(s, AVX_SHUFFLE_4(0,1,2,3));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xf0);

  t  = permute_32x8(x, AVX_SHUFFLE_4(1,0,3,2));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xcc);

  t  = permute_32x8(x, AVX_SHUFFLE_4(2,3,0,1));
  lo = umin_32x8(x,t);
  hi = umax_32x8(x,t);
  x  = blend_32x8(lo, hi, 0xaa);
  
  return x;
}

#endif


#endif
