// Marc B. Reynolds, 2023-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef SWAR_AVX2_H
#define SWAR_AVX2_H

//-------------------------------------------------------------------------------------------------------------
// these a are bunch of trival wrappers. I hate stuff like this too. It's not just you. BUT:
// * copypasta-a-rific. handy for scanning same op across different widths (IMHO)
// * a babystep toward allowing usage of extern libraries like SIMDe less of a pain
// * and they really aren't intended to external usage anywho

// it gets worse...C's macros!
#pragma push_macro("W")
#pragma push_macro("DEF")

#if defined(SIMDE_VERSION)
#define W(X) simde ## _mm256_ ## X
typedef simde__m256i u256_t;
#else
#define W(X) _mm256_ ## X
typedef __m256i u256_t;
#endif

static inline u256_t loadu_256 (void* a)           { return W(loadu_si256) ((u256_t*)a);   }
static inline void   storeu_256(void* a, u256_t v) { W(storeu_si256)((u256_t*)a,v); }

static inline u256_t zero_256(void) { return W(setzero_si256)(); }

static inline u256_t lor_256(u256_t a, u256_t b)   { return W(or_si256) (a,b); }
static inline u256_t xor_256(u256_t a, u256_t b)   { return W(xor_si256)(a,b); }
static inline u256_t and_256(u256_t a, u256_t b)   { return W(and_si256)(a,b); }

static inline u256_t add_8x32 (u256_t a, u256_t b) { return W(add_epi8) (a,b); }
static inline u256_t add_16x16(u256_t a, u256_t b) { return W(add_epi16)(a,b); }
static inline u256_t add_32x8 (u256_t a, u256_t b) { return W(add_epi32)(a,b); }
static inline u256_t add_64x4 (u256_t a, u256_t b) { return W(add_epi64)(a,b); }

static inline u256_t sub_8x32 (u256_t a, u256_t b) { return W(sub_epi8) (a,b); }
static inline u256_t sub_16x16(u256_t a, u256_t b) { return W(sub_epi16)(a,b); }
static inline u256_t sub_32x8 (u256_t a, u256_t b) { return W(sub_epi32)(a,b); }
static inline u256_t sub_64x4 (u256_t a, u256_t b) { return W(sub_epi64)(a,b); }

// min/max of unsigned elements
static inline u256_t umin_8x32 (u256_t a, u256_t b) { return W(min_epu8) (a,b); }
static inline u256_t umin_16x16(u256_t a, u256_t b) { return W(min_epu16)(a,b); }
static inline u256_t umin_32x8 (u256_t a, u256_t b) { return W(min_epu32)(a,b); }
static inline u256_t umax_8x32 (u256_t a, u256_t b) { return W(max_epu8) (a,b); }
static inline u256_t umax_16x16(u256_t a, u256_t b) { return W(max_epu16)(a,b); }
static inline u256_t umax_32x8 (u256_t a, u256_t b) { return W(max_epu32)(a,b); }

// min/max of signed elements
static inline u256_t smin_8x32 (u256_t a, u256_t b) { return W(min_epi8) (a,b); }
static inline u256_t smin_16x16(u256_t a, u256_t b) { return W(min_epi16)(a,b); }
static inline u256_t smin_32x8 (u256_t a, u256_t b) { return W(min_epi32)(a,b); }
static inline u256_t smax_8x32 (u256_t a, u256_t b) { return W(max_epi8) (a,b); }
static inline u256_t smax_16x16(u256_t a, u256_t b) { return W(max_epi16)(a,b); }
static inline u256_t smax_32x8 (u256_t a, u256_t b) { return W(max_epi32)(a,b); }

// compile time constant shifts (matching intrinsic naming scheme)
static inline u256_t srli_16x16(u256_t x, int s) { return W(srli_epi16)(x,s); }
static inline u256_t srli_32x8 (u256_t x, int s) { return W(srli_epi32)(x,s); }
static inline u256_t srli_64x4 (u256_t x, int s) { return W(srli_epi64)(x,s); }

static inline u256_t slli_16x16(u256_t x, int s) { return W(slli_epi16)(x,s); }
static inline u256_t slli_32x8 (u256_t x, int s) { return W(slli_epi32)(x,s); }
static inline u256_t slli_64x4 (u256_t x, int s) { return W(slli_epi64)(x,s); }

// broadcast (scalar) 'k' to all elements
static inline u256_t broadcast_8x32 (int8_t  k)  { return W(set1_epi8 )(k); }
static inline u256_t broadcast_16x16(int16_t k)  { return W(set1_epi16)(k); }
static inline u256_t broadcast_32x8 (int32_t k)  { return W(set1_epi32)(k); }
static inline u256_t broadcast_64x4 (int64_t k)  { return W(set1_epi64x)(k);}

// low element of 'x' broadcast to all (the 'cast' isn't a real operation)
static inline u256_t broadcast_lo_8x32 (u256_t x)  { return W(broadcastb_epi8 )(W(castsi256_si128)(x)); }
static inline u256_t broadcast_lo_16x16(u256_t x)  { return W(broadcastw_epi16)(W(castsi256_si128)(x)); }
static inline u256_t broadcast_lo_32x8 (u256_t x)  { return W(broadcastd_epi32)(W(castsi256_si128)(x)); }
static inline u256_t broadcast_lo_64x4 (u256_t x)  { return W(broadcastq_epi64)(W(castsi256_si128)(x)); }

// (~a) & b
static inline u256_t andnot_256(u256_t a, u256_t b) { return W(andnot_si256)(a,b); }

static inline u256_t unpackhi_8x32 (u256_t a, u256_t b) { return W(unpackhi_epi8) (a,b); }
static inline u256_t unpacklo_8x32 (u256_t a, u256_t b) { return W(unpacklo_epi8) (a,b); }
static inline u256_t unpackhi_16x16(u256_t a, u256_t b) { return W(unpackhi_epi16)(a,b); }
static inline u256_t unpacklo_16x16(u256_t a, u256_t b) { return W(unpackhi_epi16)(a,b); }
static inline u256_t unpackhi_32x8 (u256_t a, u256_t b) { return W(unpackhi_epi32)(a,b); }
static inline u256_t unpacklo_32x8 (u256_t a, u256_t b) { return W(unpackhi_epi32)(a,b); }
static inline u256_t unpackhi_64x4 (u256_t a, u256_t b) { return W(unpackhi_epi64)(a,b); }
static inline u256_t unpacklo_64x4 (u256_t a, u256_t b) { return W(unpackhi_epi64)(a,b); }



//-------------------------------------------------------------------------------------------------------------
// 

static inline bool cmp_equal_256(u256_t a, u256_t b) { return ((uint32_t)W(movemask_epi8)(W(cmpeq_epi8)(a,b))) == 0xFFFFFFFF; }

// all bits set : should lower to vpcmpeqd ymmR,ymmN,ymmN (ymmN is any)
static inline u256_t bit_allset_256(void) { u256_t t = W(setzero_si256)(); return W(cmpeq_epi32)(t,t); }

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
static inline u256_t byte_sum_64x4(u256_t x) {return W(sad_epu8)(x, zero_256()); }

//-------------------------------------------------------------------------------------------------------------

// per element: x ^ (x >> s)
static inline u256_t rxorshift_16x16(u256_t x, const int s) { return xor_256(x,srli_16x16(x,s)); }
static inline u256_t rxorshift_32x8 (u256_t x, const int s) { return xor_256(x,srli_32x8 (x,s)); }
static inline u256_t rxorshift_64x4 (u256_t x, const int s) { return xor_256(x,srli_64x4 (x,s)); }

// per element: x ^ (x << s)
static inline u256_t lxorshift_16x16(u256_t x, const int s) { return xor_256(x,slli_16x16(x,s)); }
static inline u256_t lxorshift_32x8 (u256_t x, const int s) { return xor_256(x,slli_32x8 (x,s)); }
static inline u256_t lxorshift_64x4 (u256_t x, const int s) { return xor_256(x,slli_64x4 (x,s)); }

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

// when table entries are duplicated per 128 bit lane
#define pshufb_table(B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF) W(setr_epi8)(B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,BA,BB,BC,BD,BE,BF)


// pshufb wrapper for byte motion
static inline u256_t byte_shuffle_128x2(u256_t x, u256_t table)
{
  return W(shuffle_epi8)(x,table);
}


typedef struct { u256_t lo,hi; } m256_pair_t;

// lo/hi nibble PSHUFB lookup worker. (r.lo = lo[low nibble], r.hi = hi[high nibble])
static inline m256_pair_t pshufb_lookup(u256_t x, u256_t lo, u256_t hi)
{
  u256_t m  = broadcast_8x32(0x0f);
  u256_t t;

  t = andnot_256(m,x);
  x = and_256   (m,x);
  t = srli_16x16(t,4);
  x = byte_shuffle_128x2(lo,x);
  t = byte_shuffle_128x2(hi,t);

  return (m256_pair_t){.lo=x, .hi=t};
}


// add bit_swap_{1,2,4,8,16,32,64}_256 aliases

static inline u256_t byte_reverse_16x16(u256_t x)
{
  u256_t k = pshufb_table(1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14);

  return byte_shuffle_128x2(x,k);
}

static inline u256_t byte_reverse_32x8(u256_t x)
{
  u256_t k = pshufb_table(3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12);

  return byte_shuffle_128x2(x,k);
}

static inline u256_t byte_reverse_64x4(u256_t x)
{
  u256_t k = pshufb_table(7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8);
  
  return byte_shuffle_128x2(x,k);
}

static inline u256_t bit_reverse_8x32(u256_t x)
{
  u256_t      k = pshufb_table(0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15);
  m256_pair_t r = pshufb_lookup(x,k,k);
  u256_t      t = slli_16x16(r.lo,4);
  
  return xor_256(r.hi,t);
}

static inline u256_t bit_reverse_16x16(u256_t x) {  return byte_reverse_16x16(bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_32x8 (u256_t x) {  return byte_reverse_32x8 (bit_reverse_8x32(x)); }
static inline u256_t bit_reverse_64x4 (u256_t x) {  return byte_reverse_64x4 (bit_reverse_8x32(x)); }


// computes the population count of each data width
static inline u256_t pop_8x32(u256_t x)
{
  u256_t k = pshufb_table(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
  
  m256_pair_t r = pshufb_lookup(x,k,k);
  
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

// computes the parity of each data width. result is LSB in width
static inline u256_t bit_parity_8x32(u256_t x)
{
  u256_t k  = pshufb_table(0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0);

  m256_pair_t r = pshufb_lookup(x,k,k);

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
  u256_t      lo = pshufb_table(8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4);
  u256_t      hi = pshufb_table(8,3,2,2,1,1,1,1,0,0,0,0,0,0,0,0);
  m256_pair_t r  = pshufb_lookup(x,lo,hi);

  return umin_8x32(r.hi,r.lo);
}

// number of trailing zeros in each byte
static inline u256_t ctz_8x32(u256_t x)
{
  u256_t      lo = pshufb_table(8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0);
  u256_t      hi = pshufb_table(8,4,5,4,6,4,5,4,7,4,5,4,6,4,5,4);
  m256_pair_t r  = pshufb_lookup(x,lo,hi);

  return umin_8x32(r.hi,r.lo);
}

static inline u256_t clz_16x16(u256_t x)
{
  u256_t t,s,m;

  x = clz_8x32(x);

  m = broadcast_16x16(0xff);
  s = broadcast_8x32(8);
  t = srli_16x16(x,8);
  
  s = W(cmpeq_epi8)(t,s);
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

  s = W(cmpeq_epi8)(x,s);  
  t = and_256 (t,s);

  x = add_8x32(t,x);

  return and_256(x,m);  
}


// per 128 lane (riffle 2x16)
static inline u256_t byte_zip_2x128(u256_t x)
{
  u256_t m = pshufb_table(0x0,0x8,0x1,0x9,0x2,0xa,0x3,0xb,0x4,0xc,0x5,0xd,0x6,0xe,0x7,0xf);
  
  return byte_shuffle_128x2(x,m);
}

// per 128-bit lane (unriffle 2x16)
static inline u256_t byte_unzip_2x128(u256_t x)
{
  u256_t m = pshufb_table(0x0,0x2,0x4,0x6,0x8,0xa,0xc,0xe,0x1,0x3,0x5,0x7,0x9,0xb,0xd,0xf);
  
  return byte_shuffle_128x2(x,m);
}

// backward named. my bad. fix
#define DELTA_SWAP_4x64(X,Y,M,S) { u256_t t; t = and_256(xor_256(X, srli_64x4(Y,S)),M); X = xor_256(X, t); Y = xor_256(Y, slli_64x4(t,S));}

static inline m256_pair_t delta_swap_4x64(m256_pair_t p, u256_t m, const int s)
{
  u256_t t;
  
  t    = and_256(xor_256(p.lo, srli_64x4(p.hi,s)),m);
  p.lo = xor_256(p.lo, t);
  p.hi = xor_256(p.hi, slli_64x4(t,s));
  
  return p;
}

// perform a delta swap in each 64-bit lane: bits indicated by 'm' are swapped with the bits
static inline u256_t bit_permute_step_64x4(u256_t x, u256_t m, const int s)
{
  u256_t t;
  
  t = rxorshift_64x4(x,s);
  t = and_256 (x,m);
  t = lxorshift_64x4(t,s);
  t = xor_256(t,x);

  return t;
}

static inline u256_t bit_permute_step_simple_64x4(u256_t x, u256_t m, const int s)
{
  u256_t t0 = slli_64x4(and_256 (x,m),s);
  u256_t t1 = and_256 (srli_64x4(x,s),m);
  
  return xor_256(t0,t1);
}

// clean-up
#pragma pop_macro("W")
#pragma pop_macro("DEF")


#endif
