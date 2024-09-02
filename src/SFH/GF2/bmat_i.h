// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bmat.h"
#include "carryless.h"
#include "bitops_small.h"
#include "swar.h"

#if defined(BITOPS_INTEL)
#include "swar_avx2.h"
#endif

// for constant defs
typedef union {
  uint8_t  u8[32];
  uint16_t u16[16];
  uint32_t u32[ 8];
  uint64_t u64[ 4];
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







