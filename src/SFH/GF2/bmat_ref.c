// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

// a sub-set of reference functions live here (others are with the actual version).



//****************************************************************************
// reference versions of transpose:
// trivially modified variants of Warren (hackers delight)

void bmat_transpose_8_ref(uint8_t d[static 8], uint8_t s[static 8])
{
  uint8_t m = 0xf;

  if (d != s) bmat_dup_8(d,s);

  for (uint32_t j=4; j!=0; j=j>>1, m=(uint8_t)(m^(m << j))) {
    for (uint32_t k=0; k<8; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_8(d[k+j], d[k], m, j);
    }
  }
}

void bmat_transpose_16_ref(uint16_t d[static 16], uint16_t s[static 16])
{
  uint16_t m = 0xff;

  // worker loop progression:
  //   m     j   k                         
  // 0x00ff  8 { 0  1  2  3  4  5  6  7 }  8 <- cumulative steps
  // 0x0f0f  4 { 0  1  2  3  8  9 10 11 } 16
  // 0x3333  2 { 0  1  4  5  8  9 12 13 } 24
  // 0x5555  1 { 0  2  4  6  8 10 12 14 } 32

  // copy source to destination (if not the same)
  if (d != s) bmat_dup_16(d,s);

  // in-place transpose: see comments in the semi-portable
  // version of 'bmat_transpose_64' for code-golf 'k' computation
  //
  // 'j' is the swap size & 'k' is base address of given delta swap

  for (uint32_t j=8; j!=0; j=j>>1, m=(uint16_t)(m^(m << j))) {
    for (uint32_t k=0; k<16; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_16(d[k+j], d[k], m, j);
    }
  }
}

void bmat_transpose_32_ref(uint32_t d[static 32], uint32_t s[static 32])
{
  uint32_t m = 0xffff;

  if (d != s) bmat_dup_32(d,s);
  
  //   m         j   k                         
  // 0x0000ffff 16 { 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 } 16
  // 0x00ff00ff  8 { 0  1  2  3  4  5  6  7 16 17 18 19 20 21 22 23 } 32
  // 0x0f0f0f0f  4 { 0  1  2  3  8  9 10 11 16 17 18 19 24 25 26 27 } 48
  // 0x33333333  2 { 0  1  4  5  8  9 12 13 16 17 20 21 24 25 28 29 } 64
  // 0x55555555  1 { 0  2  4  6  8 10 12 14 16 18 20 22 24 26 28 30 } 80
  
  for (uint32_t j=16; j!=0; j=j>>1, m=(m^(m << j))) {
    for (uint32_t k=0; k<32; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_32(d[k+j], d[k], m, j);
    }

    // dev hacks for arch specific variants
    //if (j==8) return;
    //if (j==4) return;
  }
}

void bmat_transpose_64_ref(uint64_t d[static 64], uint64_t s[static 64])
{
  uint64_t m = 0xffffffff;

  if (d != s) bmat_dup_64(s,d);
  
  for (uint32_t j=32; j!=0; j=j>>1, m=(m^(m << j))) {
    for (uint32_t k=0; k<64; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_64(d[k+j], d[k], m, j);
    }
  }
}

//------------------------------------------------------------------

void bmat_rot_cw_8_ref(uint8_t  d[static  8], uint8_t  s[static  8])
{
  bmat_transpose_8(d,s); bmat_flip_h_8(d);
}

void bmat_rot_cw_16_ref(uint16_t d[static 16], uint16_t s[static 16])
{
  bmat_transpose_16_ref(d,s); bmat_flip_h_16(d);
}

void bmat_rot_cw_32_ref(uint32_t d[static 32], uint32_t s[static 32])
{
  bmat_transpose_32_ref(d,s); bmat_flip_h_32(d);
}

void bmat_rot_cw_64_ref(uint64_t d[static 64], uint64_t s[static 64])
{
  bmat_transpose_64_ref(d,s); bmat_flip_h_64(d);
}

//****************************************************************************
// reference versions of products:

// variants:
// 1) textbook build a row at a time. very sad. performs transpose & n^2 parity
// 2) row broadcast. these can be made reasonable if changed to SWAR.

#if 0

// C = AB^T

// * sucky naming..re-think
void bmat_mult_8_ref(uint8_t C[static 8], uint8_t A[static 8], uint8_t B[static 8])
{

  uint8_t S[8];
  
  for (uint32_t i=0; i<8; i++) {
    uint8_t a = A[i];
    uint8_t r = 0;
    
    for (uint32_t j=0; j<8; j++) {
      uint8_t  b = B[j];
      uint32_t p = bit_parity_32((uint32_t)(a & b));
      r |= (uint8_t)(p << j);
    }

    S[i] = r;
  }
  
  bmat_dup_8(C,S);
}

void bmat_mult_16_ref(uint16_t C[static 16], uint16_t A[static 16], uint16_t B[static 16])
{
  uint16_t S[16];
  
  for (uint32_t i=0; i<16; i++) {
    uint16_t a = A[i];
    uint16_t r = 0;
    
    for (uint32_t j=0; j<16; j++) {
      uint16_t b = B[j];
      uint32_t p = bit_parity_32((uint32_t)(a & b));
      r |= (uint16_t)(p << j);
    }

    S[i] = r;
  }
  
  bmat_dup_16(C,S);
}

void bmat_mult_32_ref(uint32_t C[static 32], uint32_t A[static 32], uint32_t B[static 32])
{
  uint32_t S[32];
  
  for (uint32_t i=0; i<32; i++) {
    uint32_t a = A[i];
    uint32_t r = 0;
    
    for (uint64_t j=0; j<32; j++) {
      uint32_t b = B[j];
      uint32_t p  = bit_parity_32(a & b);
      r |= (p << j);
    }

    S[i] = r;
  }
  
  bmat_dup_32(C,S);
}

void bmat_mult_64_ref(uint64_t C[static 64], uint64_t A[static 64], uint64_t B[static 64])
{
  uint64_t S[64];
  
  for (uint32_t i=0; i<64; i++) {
    uint64_t a = A[i];
    uint64_t r = 0;
    
    for (uint64_t j=0; j<64; j++) {
      uint64_t b = B[j];
      uint64_t p = bit_parity_64(a & b);
      r |= (p << j);
    }

    S[i] = r;
  }
  
  bmat_dup_64(C,S);
}

//-----------------------------------------------------------------------
// C=AB

void bmat_mul_8_ref(uint8_t C[static 8], uint8_t A[static 8], uint8_t B[static 8])
{
  uint8_t T[8];

  bmat_transpose_8(T,B);
  bmat_mult_8_ref(C,A,T);
}


void bmat_mul_16_ref(uint16_t C[static 16], uint16_t A[static 16], uint16_t B[static 16])
{
  uint16_t T[16];

  bmat_transpose_16(T,B);
  bmat_mult_16_ref(C,A,T);
}

void bmat_mul_32_ref(uint32_t C[static 32], uint32_t A[static 32], uint32_t B[static 32])
{
  uint32_t T[32];

  bmat_transpose_32(T,B);
  bmat_mult_32_ref(C,A,T);
}

void bmat_mul_64_ref(uint64_t C[static 64], uint64_t A[static 64], uint64_t B[static 64])
{
  uint64_t T[64];
  bmat_transpose_64(T,B);
  bmat_mult_64_ref(C,A,T);
}

#else

//----------------------------------------------------------------------------------------
// row broadcast variants

void bmat_mul_8_ref(uint8_t C[static 8], uint8_t A[static 8], uint8_t B[static 8])
{
  uint8_t S[8];

  for (uint32_t i=0; i<8; i++) {
    uint8_t a = A[i];
    uint8_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<8; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  bmat_dup_8(C,S);
}

void bmat_mul_16_ref(uint16_t C[static 16], uint16_t A[static 16], uint16_t B[static 16])
{
  uint16_t S[16];

  for (uint32_t i=0; i<16; i++) {
    uint16_t a = A[i];
    uint16_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<16; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  bmat_dup_16(C,S);
}


void bmat_mul_32_ref(uint32_t C[static 32], uint32_t A[static 32], uint32_t B[static 32])
{
  uint32_t S[32];

  for (uint32_t i=0; i<32; i++) {
    uint32_t a = A[i];
    uint32_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<32; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  bmat_dup_32(C,S);
}

void bmat_mul_64_ref(uint64_t C[static 64], uint64_t A[static 64], uint64_t B[static 64])
{
  uint64_t S[64];

  for (uint32_t i=0; i<64; i++) {
    uint64_t a = A[i];
    uint64_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<64; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  bmat_dup_64(C,S);
}


#if 0
// M' = JM : vertical flip (row reversal)
void bmat_flip_v_8 (uint8_t  m[static  8]) { for(int i=0; i< 4; i++) { uint8_t  t=m[i]; m[i]=m[ 7-i]; m[ 7-i]=t; }}
void bmat_flip_v_16(uint16_t m[static 16]) { for(int i=0; i< 8; i++) { uint16_t t=m[i]; m[i]=m[15-i]; m[15-i]=t; }}
void bmat_flip_v_32(uint32_t m[static 32]) { for(int i=0; i<16; i++) { uint32_t t=m[i]; m[i]=m[31-i]; m[31-i]=t; }}
void bmat_flip_v_64(uint64_t m[static 64]) { for(int i=0; i<32; i++) { uint64_t t=m[i]; m[i]=m[63-i]; m[63-i]=t; }}
#endif

#endif

