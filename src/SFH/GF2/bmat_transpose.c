// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

/// Transpose
///==============================================================
///

// the "portable" versions are making non-portable assumptions:
// 1) 64-bit registers (reasonable)
// 2) not using bit_scatter/bit_gather 'cause assuming hardware has AVX2 in that case.
//    (OK..some ARMs have group ops..but then ARM has SIMD as well)
// 3) little-endian (too bad it isn't reasonable) -- that's everywhere in this codebase

// misplaced (should be swar_avx2.h and renamed)
#define BMAT_DELTA_SWAP2_64x4(X,Y,M,S) { __m256i t = (X^_mm256_slli_epi64(Y,S)) & M; X^=t; Y ^= _mm256_slli_epi64(t,S); }


//*******************************************************************
// 8-bit

// transpose 8x8 block stored in 'x'
static inline uint64_t bmat_transpose_8x8_i(uint64_t x)
{
  static const uint64_t m0 = UINT64_C(0x00AA00AA00AA00AA);
  static const uint64_t m1 = UINT64_C(0x0000CCCC0000CCCC);
  static const uint64_t m2 = UINT64_C(0x00000000F0F0F0F0);

  x = bit_permute_step_64(x, m0,  7);
  x = bit_permute_step_64(x, m1, 14);
  x = bit_permute_step_64(x, m2, 28);

  return x;
}

void bmat_transpose_8_gen(bmat_rparam_8(d), bmat_param_8(m))
{
  d[0] = bmat_transpose_8x8_i(m[0]);
}

//*******************************************************************
// 16-bit

#if defined(SWAR_AVX2_H)

// perform 8x8 bit matrix transpose in each 64-bit lane (one matrix per lane)
static inline u256_t bmat_transpose_8x8_x4(u256_t x)
{
  u256_t m0 = _mm256_set1_epi64x(UINT64_C(0x00AA00AA00AA00AA));
  u256_t m1 = _mm256_set1_epi64x(UINT64_C(0x0000CCCC0000CCCC));
  u256_t m2 = _mm256_set1_epi64x(UINT64_C(0x00000000F0F0F0F0));

  x = bit_permute_step_64x4(x, m0,  7);
  x = bit_permute_step_64x4(x, m1, 14);
  x = bit_permute_step_64x4(x, m2, 28);

  return x;
}

// in-register version
static inline u256_t bmat_transpose_16_i(u256_t m)
{
  // divide and conquer: Do the classic break into a 2x2 blocks  {{A,B},{C,D}}
  //  0) take input (displayed below as byte indices)
  //  1) reorder bytes such that each 8x8 block is linear in memory (unzip/unriffle)
  //  2) transpose the blocks: {{A,C},{B,D}}
  //  3) perform 4 8x8 transposes
  //  4) restore the byte ordering (zip/riffle)
  
  // 0) 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f|10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
  // 1) 00 02 04 06 08 0a 0c 0e|01 03 05 07 09 0b 0d 0f|10 12 14 16 18 1a 1c 1e|11 13 15 17 19 1b 1d 1f
  // 2) 00 02 04 06 08 0a 0c 0e|10 12 14 16 18 1a 1c 1e|01 03 05 07 09 0b 0d 0f|11 13 15 17 19 1b 1d 1f
  // 3) -- 4 8x8 transpose step
  // 4) 00 10 02 12 04 14 06 16 08 18 0a 1a 0c 1c 0e 1e|01 11 03 13 05 15 07 17 09 19 0b 1b 0d 1d 0f 1f

  m = byte_unzip_128x2(m);
  m = _mm256_permute4x64_epi64(m, _MM_SHUFFLE(3,1,2,0)); 
  m = bmat_transpose_8x8_x4(m);
  m = byte_zip_128x2(m);

  return m;
}

void bmat_transpose_16_avx2(bmat_rparam_16(d), bmat_param_16(m))
{
  u256_t x= loadu_256((void*)m);

  x = bmat_transpose_16_i(x);
  
  storeu_256((void*)d,x);
}

#endif

void bmat_transpose_16_gen(bmat_rparam_16(d), bmat_param_16(m))
{
  // divide and conquer: hierarchically perform swaps on blocks. For
  // 16-bit we have 4 division steps (16 = 2^4). Need to perform 4
  // 2 register delta swaps and 8 single register delta swaps since
  // we're working SWAR. Non SWAR would require 32 2 register delta
  // swaps (width 8 divided 4 times: 8*4)
  //
  // Splitball: 1/2 register delta swaps require 6/4 basic ops respectively.
  // * 1 reg dswap       : 8*6 = 48
  // * 2 reg dswap       : 4*4 = 16
  // * matrix load/store : 8   =  8
  // * const loads       : 4   =  4
  // so approximately 76 basic ops vs. 128 for non-SWAR.
  // (both ignoring any bookeeping and/or autovectorization) 
  
  // pull in the source matrix
  uint64_t  r0 = m[0];
  uint64_t  r1 = m[1];
  uint64_t  r2 = m[2];
  uint64_t  r3 = m[3];

  uint64_t  M  = UINT64_C(0x00ff00ff00ff00ff);

  // 8-bit swaps (top blocks)
  BMAT_DELTA_SWAP2_64(r2,r0, M,8);
  BMAT_DELTA_SWAP2_64(r3,r1, M,8);

  // 4-bit swaps
  M ^= M << 4;
  BMAT_DELTA_SWAP2_64(r1,r0, M,4);
  BMAT_DELTA_SWAP2_64(r3,r2, M,4);

  // 2-bit swaps
  M  = UINT64_C(0xcccccccc);
  r0 = bit_permute_step_64(r0, M,30);
  r1 = bit_permute_step_64(r1, M,30);
  r2 = bit_permute_step_64(r2, M,30);
  r3 = bit_permute_step_64(r3, M,30);

  // 1-bit swaps
  M  = UINT64_C(0x0000aaaa0000aaaa);
  r0 = bit_permute_step_64(r0, M,15);
  r1 = bit_permute_step_64(r1, M,15);
  r2 = bit_permute_step_64(r2, M,15);
  r3 = bit_permute_step_64(r3, M,15);

  // store the result
  d[0] = r0;
  d[1] = r1;
  d[2] = r2;
  d[3] = r3;
}



//*******************************************************************
// 32-bit

#if defined(SWAR_AVX2_H)

// less than ideal version. simply converts to 16x16 blocks
// perform the block transpose and restores. less bad than
// I would have expected though. half-heartedly goofying
// with some alternate ideas in tests/dev_transpose_32.c
void bmat_transpose_32_avx2(bmat_rparam_32(d), bmat_param_32(s))
{
  u256_t r[4];
  u256_t t[4];

  bmat_load_256x4(r, s);

  // this is copy-n-paste: bmat_block_fxf_32
  r[0] = u16_unzip_128x2(r[0]);
  r[1] = u16_unzip_128x2(r[1]);
  r[2] = u16_unzip_128x2(r[2]);
  r[3] = u16_unzip_128x2(r[3]);
  r[0] = u64_unzip_256(r[0]);  
  r[1] = u64_unzip_256(r[1]);
  r[2] = u64_unzip_256(r[2]);
  r[3] = u64_unzip_256(r[3]);
  t[0] = unpacklo_128x2(r[0],r[1]);     // A
  t[1] = unpackhi_128x2(r[0],r[1]);     // B
  t[2] = unpacklo_128x2(r[2],r[3]);     // C
  t[3] = unpackhi_128x2(r[2],r[3]);     // D

  // perform the block transpose.
  r[0] = bmat_transpose_16_i(t[0]);     // A^T
  r[1] = bmat_transpose_16_i(t[2]);     // C^T
  r[2] = bmat_transpose_16_i(t[1]);     // B^T
  r[3] = bmat_transpose_16_i(t[3]);     // D^T

  // this is copy-n-paste: bmat_block_rxf_32
  t[0] = unpacklo_128x2(r[0],r[1]);
  t[1] = unpackhi_128x2(r[0],r[1]);
  t[2] = unpacklo_128x2(r[2],r[3]);
  t[3] = unpackhi_128x2(r[2],r[3]);
  t[0] = u64_unzip_256(t[0]);
  t[1] = u64_unzip_256(t[1]);
  t[2] = u64_unzip_256(t[2]);
  t[3] = u64_unzip_256(t[3]);
  t[0] = u16_zip_128x2(t[0]);
  t[1] = u16_zip_128x2(t[1]);
  t[2] = u16_zip_128x2(t[2]);
  t[3] = u16_zip_128x2(t[3]);

  bmat_store_256x4(d,t);
}

void bmat_block_transpose_32(bmat_rparam_32(d), bmat_param_32(s))
{
  u256_t r[4];
  
  bmat_load_256x4(r,s);
  r[0] = bmat_transpose_16_i(r[0]);     // A^T
  r[1] = bmat_transpose_16_i(r[2]);     // C^T
  r[2] = bmat_transpose_16_i(r[1]);     // B^T
  r[3] = bmat_transpose_16_i(r[3]);     // D^T
  bmat_store_256x4(d,r);
}

#endif

void bmat_transpose_32_gen(bmat_rparam_32(D), bmat_param_32(S))
{
  uint64_t  M = UINT64_C(0x0000ffff0000ffff);
  
  // 16-bit swaps (top blocks)
  for(uint32_t i=0; i<8; i++) {
    uint64_t t0 = S[i  ];
    uint64_t t1 = S[i+8];
    BMAT_DELTA_SWAP2_64(t1,t0, M,16);
    D[i  ] = t0;
    D[i+8] = t1;
  }

  // 8-bit swaps
  M ^= M << 8;

  for(uint32_t i=0; i<4; i++) {
    BMAT_DELTA_SWAP2_64(D[i+4  ],D[i  ], M,8);
    BMAT_DELTA_SWAP2_64(D[i+4+8],D[i+8], M,8);
  }

  // 4-bit swaps
  M ^= M << 4;

  for(uint32_t i=0; i<2; i++) {
    BMAT_DELTA_SWAP2_64(D[i+2   ],D[i   ], M,4);
    BMAT_DELTA_SWAP2_64(D[i+2+ 4],D[i+ 4], M,4);
    BMAT_DELTA_SWAP2_64(D[i+2+ 8],D[i+ 8], M,4);
    BMAT_DELTA_SWAP2_64(D[i+2+12],D[i+12], M,4);
  }

  // 2-bit swaps
  M ^= M << 2;
  BMAT_DELTA_SWAP2_64(D[ 1],D[ 0], M,2);
  BMAT_DELTA_SWAP2_64(D[ 3],D[ 2], M,2);
  BMAT_DELTA_SWAP2_64(D[ 5],D[ 4], M,2);
  BMAT_DELTA_SWAP2_64(D[ 7],D[ 6], M,2);
  BMAT_DELTA_SWAP2_64(D[ 9],D[ 8], M,2);
  BMAT_DELTA_SWAP2_64(D[11],D[10], M,2);
  BMAT_DELTA_SWAP2_64(D[13],D[12], M,2);
  BMAT_DELTA_SWAP2_64(D[15],D[14], M,2);

  // 1-bit swaps. can be interleaved with the
  // 2-bit swaps above. haven't bothered to
  // look if GCC/clang are doing it for me.
  M  = UINT64_C(0xaaaaaaaa);
  for(uint32_t i=0; i<16; i++) {
    D[i] = bit_permute_step_64(D[i], M,31);
  }
}

#if 0
void bmat_block_transpose_32(bmat_param_32(d), bmat_param_32(s))
{
  // this is disallowing d=s. rework.
  bmat_transpose_16(d,     s);
  bmat_transpose_16(d+3*4, s+3*4);

  // need of copy of src B for step 1 & use for 2
  bmat_transpose_16(d+1*4, s+2*4);
  bmat_transpose_16(d+2*4, s+1*4);
}

#endif




//*******************************************************************
// 64-bit

// 49.00
void bmat_transpose_64_gen(bmat_rparam_64(D), bmat_param_64(S))
{
  uint64_t m = 0xffffffff;
  uint32_t d;

  // 32-bit swaps (top blocks). Just a specialized version of
  // the loop below that additionally handles moving the
  // data to the output matrix.
  for(uint32_t i=0; i<32; i++) {
    uint64_t t0 = S[i   ];
    uint64_t t1 = S[i+32];
    BMAT_DELTA_SWAP2_64(t1,t0, m, 32);
    D[i   ] = t0;
    D[i+32] = t1;
  }

  d  = 16;
  m ^= m << d; 

  // now perform the {16,8,4,2,1}-bit swap (d). There's "code golf"
  // computing the base row (r) of each step: (r+d+1)&(~d)
  // 16: 0 1 2 3 4  5  6  7  8  9 10 11 12 13 14 15 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
  //  8: 0 1 2 3 4  5  6  7 16 17 18 19 20 21 22 23 32 33 34 35 36 37 38 39 48 49 50 51 52 53 54 55
  //  4: 0 1 2 3 8  9 10 11 16 17 18 19 24 25 26 27 32 33 34 35 40 41 42 43 48 49 50 51 56 57 58 59
  //  2: 0 1 4 5 8  9 12 13 16 17 20 21 24 25 28 29 32 33 36 37 40 41 44 45 48 49 52 53 56 57 60 61
  //  1: 0 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62  
  do {
    for (uint32_t r=0; r<64; r = (r+d+1) & ~d) {
      BMAT_DELTA_SWAP2_64(D[r+d],D[r],m,d);
    }
    
    d  = d >> 1;
    m ^= m << d; 
  }
  while (d != 0);
}

//*******************************************************************
// default expansions (all temp hacks)

BMAT_FLATTEN
void bmat_transpose_8 (bmat_rparam_8 (d), bmat_param_8 (m)) { bmat_transpose_8_gen (d,m); }

BMAT_FLATTEN
void bmat_transpose_64(bmat_rparam_64(d), bmat_param_64(m)) { bmat_transpose_64_gen(d,m); }

#if defined(SWAR_AVX2_H)

BMAT_FLATTEN
void bmat_transpose_16(bmat_rparam_16(d), bmat_param_16(m)) { bmat_transpose_16_avx2(d,m); }

BMAT_FLATTEN
void bmat_transpose_32(bmat_rparam_32(d), bmat_param_32(m)) { bmat_transpose_32_avx2(d,m); }

#else

BMAT_FLATTEN
void bmat_transpose_16(bmat_rparam_16(d), bmat_param_16(m)) { bmat_transpose_16_gen(d,m); }

BMAT_FLATTEN
void bmat_transpose_32(bmat_rparam_32(d), bmat_param_32(m)) { bmat_transpose_32_gen(d,m); }
#endif
