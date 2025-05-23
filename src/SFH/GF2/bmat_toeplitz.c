// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

// * don't need "reference" versions since validation can be performed
//   using function to matrix. but have an equivalent here for compile
//   time switch fallback.

/// ## bmat_set_ut_toeplitz_*n*(m,k)
/// Sets `m` to an upper triangluar Toeplitz matrix:
///
/// $$
///   \left[ {\begin{array}{ccccc}
///    k_{0} & k_{1}  & k_{2}  & \cdots & k_{n-1}  \newline
///       0  & k_{0}  & k_{1}  & \cdots & k_{n-2}  \newline
///       0  &    0   & k_{0}  & \cdots & k_{n-3}  \newline
///   \vdots & \vdots & \vdots & \ddots & \vdots   \newline
///       0  &    0   & \cdots & \cdots & k_{0}    \newline
///  \end{array} } \right]
/// $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_ut_toeplitz_8 (bmat_param_8(m),  uint64_t k)
/// void bmat_set_ut_toeplitz_16(bmat_param_16(m), uint64_t k)
/// void bmat_set_ut_toeplitz_32(bmat_param_32(m), uint64_t k)
/// void bmat_set_ut_toeplitz_64(bmat_param_64(m), uint64_t k)
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_set_ut_toeplitz_8(bmat_param_8(m), uint64_t k)
{
  k   &= 0xff;
  k    = BMAT_MAIN_DIAGONAL_MASK_8 * k;
  m[0] = k & UINT64_C(0x80c0e0f0f8fcfeff);
}

void bmat_set_ut_toeplitz_16(bmat_param_16(m), uint64_t k)
{
  // 4 rows/per 64-bit
  uint64_t mask = UINT64_C(0xfff0fff0fff0fff0);

  k &= 0xffff;
  k  = (BMAT_MAIN_DIAGONAL_MASK_16 * k) & UINT64_C(0xfff8fffcfffeffff);

  m[0] = k; k = (k << 4) & mask;
  m[1] = k; k = (k << 4) & mask;
  m[2] = k; k = (k << 4) & mask;
  m[3] = k;
}

void bmat_set_ut_toeplitz_32(bmat_param_32(m), uint64_t k)
{
  // 2 rows/per 64-bit
  uint64_t mask = UINT64_C(0xfffffffcfffffffc);

  k &= 0xffffffff;
  k  = (BMAT_MAIN_DIAGONAL_MASK_32 * k) & UINT64_C(0xfffffffeffffffff);

  for(uint32_t i=0; i<16; i++) {
    m[i] = k; k = (k << 2) & mask;
  }
}

void bmat_set_ut_toeplitz_64(bmat_param_64(m), uint64_t k)
{
  for(uint32_t i=0; i<64; i++) {
    m[i] = k; k <<= 1;
  }
}


/// ## bmat_set_lt_toeplitz_*n*(m,k)
/// Sets `m` to an lower triangluar Toeplitz matrix:
///
/// put figure here
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_lt_toeplitz_8 (bmat_param_8(m),  uint64_t k)
/// void bmat_set_lt_toeplitz_16(bmat_param_16(m), uint64_t k)
/// void bmat_set_lt_toeplitz_32(bmat_param_32(m), uint64_t k)
/// void bmat_set_lt_toeplitz_64(bmat_param_64(m), uint64_t k)
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


// these compile like garbage

void bmat_set_toeplitz_8_i(bmat_param_8(m), uint32_t v)
{
  uint8_t a[8];

  for(uint32_t i=0; i<8; i++) {
    a[7-i] = (uint8_t)v;
    v >>= 1;
  }

  array_to_bmat_8(m,a);
}  

void bmat_set_toeplitz_16_i(bmat_param_16(m), uint32_t v)
{
  uint16_t a[16];

  for(uint32_t i=0; i<16; i++) {
    a[15-i] = (uint16_t)v;
    v >>= 1;
  }

  array_to_bmat_16(m,a);
}  

void bmat_set_toeplitz_32_i(bmat_param_32(m), uint64_t v)
{
  uint32_t a[32];

  for(uint32_t i=0; i<32; i++) {
    a[31-i] = (uint32_t)v;
    v >>= 1;
  }
  
  array_to_bmat_32(m,a);
}  


void bmat_set_toeplitz_8 (bmat_param_8 (m), uint8_t  r, uint8_t  c) { bmat_set_toeplitz_8_i (m, (((uint32_t)r)<< 8)|bit_reverse_8 (c)); }
void bmat_set_toeplitz_16(bmat_param_16(m), uint16_t r, uint16_t c) { bmat_set_toeplitz_16_i(m, (((uint32_t)r)<<16)|bit_reverse_16(c)); }
void bmat_set_toeplitz_32(bmat_param_32(m), uint32_t r, uint32_t c) { bmat_set_toeplitz_32_i(m, (((uint64_t)r)<<32)|bit_reverse_32(c)); }

void bmat_set_toeplitz_64(bmat_param_64(m), uint64_t r, uint64_t c)
{
  c = bit_reverse_64(c);

  for_range(i,0,64) {
    m[63-i] = c;
    c = (c>>1) | (r<<63);
    r >>= 1;
  }
}


// forward to LT
#if 1
void bmat_set_cl_mul_8 (bmat_param_8 (m), uint8_t  n) { bmat_set_toeplitz_8 (m,0,n); }
void bmat_set_cl_mul_16(bmat_param_16(m), uint16_t n) { bmat_set_toeplitz_16(m,0,n); }
void bmat_set_cl_mul_32(bmat_param_32(m), uint32_t n) { bmat_set_toeplitz_32(m,0,n); }
void bmat_set_cl_mul_64(bmat_param_64(m), uint64_t n) { bmat_set_toeplitz_64(m,0,n); }
#else
void bmat_set_cl_mul_8 (bmat_param_8 (m), uint8_t  n) { uint8_t  a[ 8]; n = bit_reverse_8 (n); for_range(i,0, 8) { a[ 7-i] = n; n >>= 1; } array_to_bmat_8 (m,a);}
void bmat_set_cl_mul_16(bmat_param_16(m), uint16_t n) { uint16_t a[16]; n = bit_reverse_16(n); for_range(i,0,16) { a[15-i] = n; n >>= 1; } array_to_bmat_16(m,a);}
void bmat_set_cl_mul_32(bmat_param_32(m), uint32_t n) { uint32_t a[32]; n = bit_reverse_32(n); for_range(i,0,32) { a[31-i] = n; n >>= 1; } array_to_bmat_32(m,a);}
void bmat_set_cl_mul_64(bmat_param_64(m), uint64_t n) { uint64_t a[64]; n = bit_reverse_64(n); for_range(i,0,64) { a[63-i] = n; n >>= 1; } array_to_bmat_64(m,a);}
#endif

void bmat_set_cc_mul_8 (bmat_param_8 (m), uint8_t  n) { bmat_set_toeplitz_8 (m,bit_reverse_8 (n),n); }
void bmat_set_cc_mul_16(bmat_param_16(m), uint16_t n) { bmat_set_toeplitz_16(m,bit_reverse_16(n),n); }
void bmat_set_cc_mul_32(bmat_param_32(m), uint32_t n) { bmat_set_toeplitz_32(m,bit_reverse_32(n),n); }
void bmat_set_cc_mul_64(bmat_param_64(m), uint64_t n) { bmat_set_toeplitz_64(m,bit_reverse_64(n),n); }



// change to using ut_toeplitz
#if 1
void bmat_set_cr_mul_8 (bmat_param_8 (m), uint8_t  n) { uint8_t  a[ 8]; n = bit_reverse_8 (n); for_range(i,0, 8) { a[i] = n; n <<= 1; } array_to_bmat_8 (m,a); }
void bmat_set_cr_mul_16(bmat_param_16(m), uint16_t n) { uint16_t a[16]; n = bit_reverse_16(n); for_range(i,0,16) { a[i] = n; n <<= 1; } array_to_bmat_16(m,a); }
void bmat_set_cr_mul_32(bmat_param_32(m), uint32_t n) { uint32_t a[32]; n = bit_reverse_32(n); for_range(i,0,32) { a[i] = n; n <<= 1; } array_to_bmat_32(m,a); }
void bmat_set_cr_mul_64(bmat_param_64(m), uint64_t n) { uint64_t a[64]; n = bit_reverse_64(n); for_range(i,0,64) { a[i] = n; n <<= 1; } array_to_bmat_64(m,a); }
#else
#endif


