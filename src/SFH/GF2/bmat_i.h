// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bmat.h"

// temp hack 4 godbolt
#if !defined(BITOPS_H)
#include "bitops.h"
#include "carryless.h"
#include "bitops_small.h"
#include "swar.h"
#endif

#if defined(BITOPS_INTEL) && !defined(SWAR_AVX2_H)
#include "swar_avx2.h"
#endif

// for constant defs
typedef union {
  uint8_t  u8[32];
  uint16_t u16[16];
  uint32_t u32[ 8];
  uint64_t u64[ 4];
#if defined(SWAR_AVX2_H)
  u256_t   p;
#endif  
} u256_data_t;

extern const uint64_t bmat_main_diagonal_mask_8;
extern const uint64_t bmat_main_diagonal_mask_16;
extern const uint64_t bmat_main_diagonal_mask_32;

// main diagonal (mask/markers) in 256 bit
extern const u256_data_t bmat_md_256_16;
extern const u256_data_t bmat_md_256_32;
extern const u256_data_t bmat_md_256_64;


// memory chuck sizes in bytes
#define bmat_sizeof_8     (8*8/8)
#define bmat_sizeof_16    (16*16/8)
#define bmat_sizeof_32    (32*32/8)
#define bmat_sizeof_64    (64*64/8)

// memory chunk sizes in 64-bit datum
#define bmat_chunksize_8  (8*8/64)
#define bmat_chunksize_16 (16*16/64)
#define bmat_chunksize_32 (32*32/64)
#define bmat_chinksize_64 (64*64/64)

#define for_range(I,S,E) for(uint32_t I=S; I<E; I++)

#define bmat_adup_8(A,M)  uint8_t  A[ 8]; bmat_to_array_8 (A,M)
#define bmat_adup_16(A,M) uint16_t A[16]; bmat_to_array_16(A,M)
#define bmat_adup_32(A,M) uint32_t A[32]; bmat_to_array_32(A,M)
#define bmat_adup_64(A,M) uint64_t A[64]; bmat_to_array_64(A,M)

// * Guy Steele's shifted bit field swap between two registers. (misplaced..clean up vs. bitops.h)
#define BMAT_DELTA_SWAP2_8(X,Y,M,S)  { uint8_t  t = (X^(Y>>S)) & M; X=(uint8_t) (X^t); Y=(uint8_t) (Y^(t<<S)); }
#define BMAT_DELTA_SWAP2_16(X,Y,M,S) { uint16_t t = (X^(Y>>S)) & M; X=(uint16_t)(X^t); Y=(uint16_t)(Y^(t<<S)); }
#define BMAT_DELTA_SWAP2_32(X,Y,M,S) { uint32_t t = (X^(Y>>S)) & M; X=(uint32_t)(X^t); Y=(uint32_t)(Y^(t<<S)); }
#define BMAT_DELTA_SWAP2_64(X,Y,M,S) { uint64_t t = (X^(Y>>S)) & M; X=(uint64_t)(X^t); Y=(uint64_t)(Y^(t<<S)); }


#define BMAT_REP_SHIFT_4(X,S) {X,X<<S,X<<(2*S),X<<(3*S)}

#if defined(SWAR_AVX2_H)

static inline u256_t bmat_load_256(uint64_t* s)
{
  u256_t* p = (u256_t*)s;
  return loadu_256(p);
}

static inline void bmat_store_256(uint64_t* d, u256_t r)
{
  u256_t* p = (u256_t*)d;
  storeu_256(p, r);
}

static inline void bmat_load_256xn(u256_t* r, uint64_t* s, uint32_t n)
{
  u256_t* p = (u256_t*)s;

  for(uint32_t i=0; i<n; i++)
    r[i] = loadu_256(p+i);
}

static inline void bmat_store_256xn(uint64_t* d, u256_t* s, uint32_t n)
{
  u256_t* p = (u256_t*)d;

  for(uint32_t i=0; i<n; i++)
    storeu_256(p+i, s[i]);
}

static inline void bmat_store_256x4(uint64_t* d, u256_t* s)
{
  bmat_store_256xn(d,s,4);
}

static inline void bmat_load_256x4(u256_t* r, uint64_t* s)
{
  u256_t* p = (u256_t*)s;
  r[0] = loadu_256(p  );
  r[1] = loadu_256(p+1);
  r[2] = loadu_256(p+2);
  r[3] = loadu_256(p+3);
}

static inline void bmat_storex_256x4(uint64_t* d, u256_t r0, u256_t r1, u256_t r2, u256_t r3)
{
  u256_t* p = (u256_t*)d;
  storeu_256(p,   r0);
  storeu_256(p+1, r1);
  storeu_256(p+2, r2);
  storeu_256(p+3, r3);
}

// carryless sum of the packed elements
static inline uint16_t bmat_hsum_8x32(u256_t x)
{
  x = xor_256(x, swap_128x2(x));     // 128
  x = xor_256(x, bsrli_128x2(x,8));  //  64
  x = xor_256(x, bsrli_128x2(x,4));  //  32
  x = xor_256(x, bsrli_128x2(x,2));  //  16
  x = xor_256(x, bsrli_128x2(x,1));  //   8

  return (uint8_t)extract_8_256(x,0);
}

static inline uint16_t bmat_hsum_16x16(u256_t x)
{
  x = xor_256(x, swap_128x2(x));     // 128
  x = xor_256(x, bsrli_128x2(x,8));  //  64
  x = xor_256(x, bsrli_128x2(x,4));  //  32
  x = xor_256(x, bsrli_128x2(x,2));  //  16

  return (uint16_t)extract_16_256(x,0);
}

static inline uint32_t bmat_hsum_32x8(u256_t x)
{
  x = xor_256(x, swap_128x2(x));     // 128
  x = xor_256(x, bsrli_128x2(x,8));  //  64
  x = xor_256(x, bsrli_128x2(x,4));  //  32

  return (uint32_t)extract_32_256(x,0);
}

static inline uint64_t bmat_hsum_64x4(u256_t x)
{
  x = xor_256(x, swap_128x2(x));     // 128
  x = xor_256(x, bsrli_128x2(x,8));  //  64

  return (uint64_t)extract_64_256(x,0);
}

#endif




