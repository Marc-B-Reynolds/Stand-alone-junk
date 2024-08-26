// Marc B. Reynolds, 2023-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef SWAR_AVX2_H
#define SWAR_AVX2_H

static inline __m256i loadu_256 (void* a)            { return _mm256_loadu_si256 ((__m256i*)a);   }
static inline void    storeu_256(void* a, __m256i v) { _mm256_storeu_si256((__m256i*)a,v); }

typedef struct { __m256i lo,hi; } m256_pair_t;

// lo/hi nibble PSHUFB lookup
static inline m256_pair_t avx2_base_lut(__m256i x, __m256i lo, __m256i hi)
{
  __m256i m  = _mm256_set1_epi8(0x0f);
  __m256i t;

  // nibble split
  t = _mm256_andnot_si256(m,x);
  x = _mm256_and_si256(m,x);
  t = _mm256_srli_epi16(t,4);

  // look-up and merge
  x = _mm256_shuffle_epi8(lo,x);
  t = _mm256_shuffle_epi8(hi,t);

  return (m256_pair_t){.lo=x, .hi=t};
}


static inline __m256i pop_8x32(__m256i x)
{

  __m256i k  = _mm256_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
				0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);

  m256_pair_t r = avx2_base_lut(x,k,k);

  return _mm256_add_epi8(r.hi,r.lo);
}

static inline __m256i pop_16x16(__m256i x)
{
  __m256i m = _mm256_set1_epi16(0xff);
  __m256i t;

  x = pop_8x32(x);
  t = _mm256_srli_epi16(x,8);
  x = _mm256_add_epi8(x,t);
  
  return _mm256_and_si256(m,x);
}

static inline __m256i pop_32x8(__m256i x)
{
  __m256i m = _mm256_set1_epi32(0xff);
  __m256i t;

  x = pop_8x32(x);
  t = _mm256_srli_epi16(x,8);
  x = _mm256_add_epi8(x,t);
  t = _mm256_srli_epi32(x,16);
  x = _mm256_add_epi8(x,t);
  
  return _mm256_and_si256(m,x);
}

// returns the sum of bytes in each 64-bit element
// * the cleverness of using SAD is from: "Faster Population Counts Using AVX2 Instructions", Mula, Kurz & Lemire
//   (https://arxiv.org/pdf/1611.07612)
static inline __m256i byte_sum_64x4(__m256i x) {return _mm256_sad_epu8(x, _mm256_setzero_si256()); }

static inline __m256i pop_64x4(__m256i x) { return byte_sum_64x4(pop_8x32(x)); }


static inline __m256i bit_parity_8x32(__m256i x)
{
  __m256i k  = _mm256_setr_epi8(0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
				0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0);

  m256_pair_t r = avx2_base_lut(x,k,k);

  return _mm256_xor_si256(r.hi,r.lo);
}

static inline __m256i bit_parity_16x16(__m256i x)
{
  x = bit_parity_8x32(x);
  x = _mm256_xor_si256(x,_mm256_srli_epi16(x,8));

  return _mm256_and_si256(x,_mm256_set1_epi16(1));
}

static inline __m256i bit_parity_32x8(__m256i x)
{
  x = bit_parity_8x32(x);
  x = _mm256_xor_si256(x,_mm256_srli_epi16(x, 8));
  x = _mm256_xor_si256(x,_mm256_srli_epi32(x,16));

  return _mm256_and_si256(x,_mm256_set1_epi32(1));
}

static inline __m256i bit_parity_64x4(__m256i x)
{
  x = bit_parity_8x32(x);
  x = _mm256_xor_si256(x,_mm256_srli_epi16(x, 8));
  x = _mm256_xor_si256(x,_mm256_srli_epi32(x,16));
  x = _mm256_xor_si256(x,_mm256_srli_epi64(x,32));

  return _mm256_and_si256(x,_mm256_set1_epi64x(1));
}


// number of leading zeros in each byte
static inline __m256i clz_8x32(__m256i x)
{
  __m256i lo = _mm256_set_epi8(4,4,4,4,4,4,4,4,5,5,5,5,6,6,7,8,
			       4,4,4,4,4,4,4,4,5,5,5,5,6,6,7,8);
  __m256i hi = _mm256_set_epi8(0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,8,
			       0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,8);

  m256_pair_t r = avx2_base_lut(x,lo,hi);

  return _mm256_min_epu8(r.hi,r.lo);
}

// number of trailing zeros in each byte
static inline __m256i ctz_8x32(__m256i x)
{
  __m256i lo = _mm256_set_epi8(0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,8,
			       0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,8);
  __m256i hi = _mm256_set_epi8(4,5,4,6,4,5,4,7,4,5,4,6,4,5,4,8,
			       4,5,4,6,4,5,4,7,4,5,4,6,4,5,4,8);

  m256_pair_t r = avx2_base_lut(x,lo,hi);

  return _mm256_min_epu8(r.hi,r.lo);
}

static inline __m256i clz_16x16(__m256i x)
{
  __m256i t,s,m;

  x = clz_8x32(x);

  m = _mm256_set1_epi16(0xff);  
  s = _mm256_set1_epi8(8);  
  t = _mm256_srli_epi16(x,8);

  s = _mm256_cmpeq_epi8(t,s);
  x = _mm256_and_si256 (x,s);
  
  x = _mm256_add_epi8(t,x);

  return _mm256_and_si256(x,m);
}

static inline __m256i ctz_16x16(__m256i x)
{
  __m256i t,s,m;

  x = ctz_8x32(x);

  m = _mm256_set1_epi16(0xff);  
  s = _mm256_set1_epi8(8);  
  t = _mm256_srli_epi16(x,8);    

  s = _mm256_cmpeq_epi8(x,s);  
  t = _mm256_and_si256 (t,s);

  x = _mm256_add_epi8(t,x);

  return _mm256_and_si256(x,m);  
}


// per 128 lane (riffle 2x16)
static inline __m256i byte_zip_2x128(__m256i x)
{
  __m256i m = _mm256_setr_epi8(0x00,0x08,0x01,0x09,0x02,0x0a,0x03,0x0b,0x04,0x0c,0x05,0x0d,0x06,0x0e,0x07,0x0f,
			       0x00,0x08,0x01,0x09,0x02,0x0a,0x03,0x0b,0x04,0x0c,0x05,0x0d,0x06,0x0e,0x07,0x0f);
  
  return _mm256_shuffle_epi8(x,m);
}

// per 128-bit lane (unriffle 2x16)
static inline __m256i byte_unzip_2x128(__m256i x)
{
  __m256i m = _mm256_setr_epi8(0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x01,0x03,0x05,0x07,0x09,0x0b,0x0d,0x0f,
			       0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x01,0x03,0x05,0x07,0x09,0x0b,0x0d,0x0f);
  
  return _mm256_shuffle_epi8(x,m);
}


// perform a delta swap in each 64-bit lane: bits indicated by 'm' are swapped with the bits
static inline __m256i bit_permute_step_4x64(__m256i x, __m256i m, const int s)
{
  __m256i t;

  t = _mm256_xor_si256(x, _mm256_srli_epi64(x,s));
  t = _mm256_and_si256(x, m);
  t = _mm256_xor_si256(t , _mm256_slli_epi64(t,s));

  return _mm256_xor_si256(t,x);
}

static inline __m256i bit_permute_step_simple_4x64(__m256i x, __m256i m, const int s)
{
  __m256i t0 = _mm256_slli_epi64(_mm256_and_si256 (x,m),s);
  __m256i t1 = _mm256_and_si256 (_mm256_srli_epi64(x,s),m);

  return _mm256_xor_si256(t0,t1);
}

#endif
