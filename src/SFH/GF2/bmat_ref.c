// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"
#include "bmat_ref.h"

// a sub-set of reference functions live here (others are with the actual version).

//****************************************************************************


// set to identity matrix
void bmat_set_unit_8_ref (bmat_param_8 (m)) {uint8_t  a[ 8]; uint8_t  n=1; for_range(i,0, 8) { a[i] = n; n <<= 1; } array_to_bmat_8 (m,a); }
void bmat_set_unit_16_ref(bmat_param_16(m)) {uint16_t a[16]; uint16_t n=1; for_range(i,0,16) { a[i] = n; n <<= 1; } array_to_bmat_16(m,a); }
void bmat_set_unit_32_ref(bmat_param_32(m)) {uint32_t a[32]; uint32_t n=1; for_range(i,0,32) { a[i] = n; n <<= 1; } array_to_bmat_32(m,a); }
void bmat_set_unit_64_ref(bmat_param_64(m)) {uint64_t a[64]; uint64_t n=1; for_range(i,0,64) { a[i] = n; n <<= 1; } array_to_bmat_64(m,a); }

// set to exchange matrix (a.k.a: J, bitreverse)
void bmat_set_exchange_8_ref (bmat_param_8 (m)) {uint8_t  a[ 8]; uint8_t  n=1; for(int32_t i= 7; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_8 (m,a); }
void bmat_set_exchange_16_ref(bmat_param_16(m)) {uint16_t a[16]; uint16_t n=1; for(int32_t i=15; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_16(m,a); }
void bmat_set_exchange_32_ref(bmat_param_32(m)) {uint32_t a[32]; uint32_t n=1; for(int32_t i=31; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_32(m,a); }
void bmat_set_exchange_64_ref(bmat_param_64(m)) {uint64_t a[64]; uint64_t n=1; for(int32_t i=63; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_64(m,a); }

// D = SJ : horizontal flip (column reversal/row bit reverse)
void bmat_flip_h_8_ref (bmat_param_8 (d),bmat_param_8 (s)) { bmat_adup_8 (a,s); for_range(i,0, 8) a[i] = bit_reverse_8 (a[i]); array_to_bmat_8 (d,a); }
void bmat_flip_h_16_ref(bmat_param_16(d),bmat_param_16(s)) { bmat_adup_16(a,s); for_range(i,0,16) a[i] = bit_reverse_16(a[i]); array_to_bmat_16(d,a); }
void bmat_flip_h_32_ref(bmat_param_32(d),bmat_param_32(s)) { bmat_adup_32(a,s); for_range(i,0,32) a[i] = bit_reverse_32(a[i]); array_to_bmat_32(d,a); }
void bmat_flip_h_64_ref(bmat_param_64(d),bmat_param_64(s)) { bmat_adup_64(a,s); for_range(i,0,64) a[i] = bit_reverse_64(a[i]); array_to_bmat_64(d,a); }

// D = JS : vertical flip (row reversal) : use a swap2 macro silly rabbit
void bmat_flip_v_8_ref (bmat_param_8 (d),bmat_param_8 (s)) { bmat_adup_8 (a,s); for_range(i,0, 4) { uint8_t  t=a[i]; a[i]=a[ 7-i]; a[ 7-i]=t; } array_to_bmat_8 (d,a); }
void bmat_flip_v_16_ref(bmat_param_16(d),bmat_param_16(s)) { bmat_adup_16(a,s); for_range(i,0, 8) { uint16_t t=a[i]; a[i]=a[15-i]; a[15-i]=t; } array_to_bmat_16(d,a); }
void bmat_flip_v_32_ref(bmat_param_32(d),bmat_param_32(s)) { bmat_adup_32(a,s); for_range(i,0,16) { uint32_t t=a[i]; a[i]=a[31-i]; a[31-i]=t; } array_to_bmat_32(d,a); }
void bmat_flip_v_64_ref(bmat_param_64(d),bmat_param_64(s)) { bmat_adup_64(a,s); for_range(i,0,32) { uint64_t t=a[i]; a[i]=a[63-i]; a[63-i]=t; } array_to_bmat_64(d,a); }

//****************************************************************************

// these are fine for reference. changing to SWAR is probably
// the only thing (maybe) worth bothering with. can broadcast
// the pivot 'p' in the same manner as broadcasting rows when
// performing the product.

uint32_t bmat_rank_8_ref(bmat_param_8(m))
{
  static const uint32_t D = 8;

  uint8_t  a[8];
  uint32_t r = 0;

  bmat_to_array_8(a,m);
  
  for(uint32_t i=0; i<D; i++) {
    uint8_t p = a[i];

    if (p) {
      uint8_t lsb = -p & p;
      for(uint32_t j=i+1; j<D; j++) { if (a[j] & lsb) a[j] ^= p; }
      r++;
    }
  }

  return r;
}

uint32_t bmat_rank_16_ref(bmat_param_16(m))
{
  static const uint32_t D = 16;

  uint16_t a[16];
  uint32_t r = 0;

  bmat_to_array_16(a,m);

  for(uint32_t i=0; i<D; i++) {
    uint16_t p = a[i];

    if (p) {
      uint16_t lsb = -p & p;
      for(uint32_t j=i+1; j<D; j++) { if (a[j] & lsb) a[j] ^= p; }
      r++;
    }
  }

  return r;
}

uint32_t bmat_rank_32_ref(bmat_param_32(m))
{
  static const uint32_t D = 32;

  uint32_t a[32];
  uint32_t r = 0;

  bmat_to_array_32(a,m);

  for(uint32_t i=0; i<D; i++) {
    uint32_t p = a[i];

    if (p) {
      uint32_t lsb = -p & p;
      for(uint32_t j=i+1; j<D; j++) { if (a[j] & lsb) a[j] ^= p; }
      r++;
    }
  }

  return r;
}

uint32_t bmat_rank_64_ref(bmat_param_64(m))
{
  static const uint32_t D = 64;

  uint64_t a[64];
  uint32_t r = 0;

  bmat_to_array_64(a,m);

  for(uint32_t i=0; i<D; i++) {
    uint64_t p = a[i];

    if (p) {
      uint64_t lsb = -p & p;
      for(uint32_t j=i+1; j<D; j++) {if (a[j] & lsb) a[j] ^= p; }
      r++;
    }
  }

  return r;
}


//****************************************************************************
// reference versions of transpose:
// trivially modified variants of Warren (hackers delight)

void bmat_transpose_8_ref(bmat_param_8(d), bmat_param_8(s))
{
  uint8_t m = 0xf;

  bmat_adup_8(a,s);

  for (uint32_t j=4; j!=0; j=j>>1, m=(uint8_t)(m^(m << j))) {
    for (uint32_t k=0; k<8; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_8(a[k+j], a[k], m, j);
    }
  }

  array_to_bmat_8(d,a);
}

void bmat_transpose_16_ref(bmat_param_16(d), bmat_param_16(s))
{
  uint16_t m = 0xff;

  // worker loop progression:
  //   m     j   k                         
  // 0x00ff  8 { 0  1  2  3  4  5  6  7 }  8 <- cumulative steps
  // 0x0f0f  4 { 0  1  2  3  8  9 10 11 } 16
  // 0x3333  2 { 0  1  4  5  8  9 12 13 } 24
  // 0x5555  1 { 0  2  4  6  8 10 12 14 } 32

  bmat_adup_16(a,s);

  // in-place transpose: see comments in the semi-portable
  // version of 'bmat_transpose_64' for code-golf 'k' computation
  //
  // 'j' is the swap size & 'k' is base address of given delta swap

  for (uint32_t j=8; j!=0; j=j>>1, m=(uint16_t)(m^(m << j))) {
    for (uint32_t k=0; k<16; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_16(a[k+j], a[k], m, j);
    }
  }

  array_to_bmat_16(d,a);
}


void bmat_transpose_32_ref(bmat_param_32(d), bmat_param_32(s))
{
  uint32_t m = 0xffff;

  bmat_adup_32(a,s);

  //   m         j   k                         
  // 0x0000ffff 16 { 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 } 16
  // 0x00ff00ff  8 { 0  1  2  3  4  5  6  7 16 17 18 19 20 21 22 23 } 32
  // 0x0f0f0f0f  4 { 0  1  2  3  8  9 10 11 16 17 18 19 24 25 26 27 } 48
  // 0x33333333  2 { 0  1  4  5  8  9 12 13 16 17 20 21 24 25 28 29 } 64
  // 0x55555555  1 { 0  2  4  6  8 10 12 14 16 18 20 22 24 26 28 30 } 80
  
  for (uint32_t j=16; j!=0; j=j>>1, m=(m^(m << j))) {
    for (uint32_t k=0; k<32; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_32(a[k+j], a[k], m, j);
    }

    // dev hacks for arch specific variants
    //if (j==8) return;
    //if (j==4) return;
  }

  array_to_bmat_32(d,a);
}

void bmat_transpose_64_ref(bmat_param_64(d), bmat_param_64(s))
{
  uint64_t m = 0xffffffff;

  bmat_adup_64(a,s);
  
  for (uint32_t j=32; j!=0; j=j>>1, m=(m^(m << j))) {
    for (uint32_t k=0; k<64; k = (k+j+1) & ~j) {
      BMAT_DELTA_SWAP2_64(a[k+j], a[k], m, j);
    }
  }
  array_to_bmat_64(d,a);
}


#if 0
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
#endif


//****************************************************************************
// reference versions of products: row broadcasting. see bmat_mul.c if that's WTF?

void bmat_mul_8_ref(bmat_param_8(C), bmat_param_8(MA), bmat_param_8(MB))
{
  uint8_t S[8];

  bmat_adup_8(A,MA);
  bmat_adup_8(B,MB);

  for (uint32_t i=0; i<8; i++) {
    uint8_t a = A[i];
    uint8_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<8; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  array_to_bmat_8(C,S);
}


void bmat_mul_16_ref(bmat_param_16(C), bmat_param_16(MA), bmat_param_16(MB))
{
  uint16_t S[16];

  bmat_adup_16(A,MA);
  bmat_adup_16(B,MB);

  for (uint32_t i=0; i<16; i++) {
    uint16_t a = A[i];
    uint16_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<16; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  array_to_bmat_16(C,S);
}


void bmat_mul_32_ref(bmat_param_32(C), bmat_param_32(MA), bmat_param_32(MB))
{
  uint32_t S[32];

  bmat_adup_32(A,MA);
  bmat_adup_32(B,MB);

  for (uint32_t i=0; i<32; i++) {
    uint32_t a = A[i];
    uint32_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<32; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  array_to_bmat_32(C,S);
}


void bmat_mul_64_ref(bmat_param_64(C), bmat_param_64(MA), bmat_param_64(MB))
{
  uint64_t S[64];

  bmat_adup_64(A,MA);
  bmat_adup_64(B,MB);

  for (uint32_t i=0; i<64; i++) {
    uint64_t a = A[i];
    uint64_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<64; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }

  array_to_bmat_64(C,S);
}


// naive thing using reference mul and transpose
void bmat_mult_8_ref (bmat_param_8(c),  bmat_param_8(a),  bmat_param_8(b))  { bmat_def_8(t);  bmat_transpose_8_ref(t,b);  bmat_mul_8_ref(c,a,t);  }
void bmat_mult_16_ref(bmat_param_16(c), bmat_param_16(a), bmat_param_16(b)) { bmat_def_16(t); bmat_transpose_16_ref(t,b); bmat_mul_16_ref(c,a,t); }
void bmat_mult_32_ref(bmat_param_32(c), bmat_param_32(a), bmat_param_32(b)) { bmat_def_32(t); bmat_transpose_32_ref(t,b); bmat_mul_32_ref(c,a,t); }
void bmat_mult_64_ref(bmat_param_64(c), bmat_param_64(a), bmat_param_64(b)) { bmat_def_64(t); bmat_transpose_64_ref(t,b); bmat_mul_64_ref(c,a,t); }

//***************************************************************************

uint8_t bmat_vmul_8_ref(uint8_t v, bmat_param_8(M))
{
  bmat_adup_8(m,M);
  uint32_t r = 0;
  for_range(i,0,8) { r ^= (m[i] & (-(v & 1))); v >>= 1; } return (uint8_t)r;
}

uint16_t bmat_vmul_16_ref(uint16_t v, bmat_param_16(M))
{
  bmat_adup_16(m,M);
  uint32_t r = 0;
  for_range(i,0,16) { r ^= (m[i] & (-(v&1))); v >>= 1; } return (uint16_t)r;
}

uint32_t bmat_vmul_32_ref(uint32_t v, bmat_param_32(M))
{
  bmat_adup_32(m,M);
  uint32_t r = 0;
  for_range(i,0,32) { r ^= (m[i] & (-(v&1))); v >>= 1; } return r;
}

uint64_t bmat_vmul_64_ref(uint64_t v, bmat_param_64(M))
{
  bmat_adup_64(m,M);
  uint64_t r = 0;
  for_range(i,0,64) { r ^= (m[i] & (-(v&1)));  v >>= 1; } return r;
}


//***************************************************************************

uint8_t bmat_mulv_8_ref(bmat_param_8(M), uint8_t v)
{
  bmat_adup_8(m,M);

  uint32_t r = 0;
  uint32_t b = 1;
  
  for_range(i,0,8) {
    r ^= b & bit_parity_mask_32(m[i] & v);
    b <<= 1;
  }

  return (uint8_t)r;
}

uint16_t bmat_mulv_16_ref(bmat_param_16(M), uint16_t v)
{
  bmat_adup_16(m,M);

  uint32_t r = 0;
  uint32_t b = 1;
  
  for_range(i,0,16) {
    r ^= b & bit_parity_mask_32(m[i] & v);
    b <<= 1;
  }

  return (uint16_t)r;
}

uint32_t bmat_mulv_32_ref(bmat_param_32(M), uint32_t v)
{
  bmat_adup_32(m,M);

  uint32_t r = 0;
  uint32_t b = 1;
  
  for_range(i,0,32) {
    r ^= b & bit_parity_mask_32(m[i] & v);
    b <<= 1;
  }

  return r;
}

uint64_t bmat_mulv_64_ref(bmat_param_64(M), uint64_t v)
{
  bmat_adup_64(m,M);

  uint64_t r = 0;
  uint64_t b = 1;
  
  for_range(i,0,64) {
    r ^= b & bit_parity_mask_64(m[i] & v);
    b <<= 1;
  }

  return r;
}

//***************************************************************************

// meh..needs work
// performs d[i] = OP working with element sized rows
#define BMAT_TRIVIAL_REF(W,OP)               \
  uint##W##_t a[W];                          \
  bmat_to_array_##W(a,m);                    \
  for_range(i,0,W) a[i] = (uint##W##_t)(OP); \
  array_to_bmat_##W(d,a);

void bmat_row_lshift_8_ref (bmat_param_8 (d), bmat_param_8 (m), uint32_t s) { BMAT_TRIVIAL_REF(8,  a[i]<<s) }
void bmat_row_lshift_16_ref(bmat_param_16(d), bmat_param_16(m), uint32_t s) { BMAT_TRIVIAL_REF(16, a[i]<<s) }
void bmat_row_lshift_32_ref(bmat_param_32(d), bmat_param_32(m), uint32_t s) { BMAT_TRIVIAL_REF(32, a[i]<<s) }
void bmat_row_lshift_64_ref(bmat_param_64(d), bmat_param_64(m), uint32_t s) { BMAT_TRIVIAL_REF(64, a[i]<<s) }

void bmat_row_rshift_8_ref (bmat_param_8 (d), bmat_param_8 (m), uint32_t s) { BMAT_TRIVIAL_REF(8,  a[i]>>s) }
void bmat_row_rshift_16_ref(bmat_param_16(d), bmat_param_16(m), uint32_t s) { BMAT_TRIVIAL_REF(16, a[i]>>s) }
void bmat_row_rshift_32_ref(bmat_param_32(d), bmat_param_32(m), uint32_t s) { BMAT_TRIVIAL_REF(32, a[i]>>s) }
void bmat_row_rshift_64_ref(bmat_param_64(d), bmat_param_64(m), uint32_t s) { BMAT_TRIVIAL_REF(64, a[i]>>s) }

