// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"
#include <string.h>

/// ## bmat_zero_*n*(m)
///
/// Sets all the elements of matrix $m$ to zero.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_zero_8 (bmat_param_8 (m))
/// void bmat_zero_16(bmat_param_16(m))
/// void bmat_zero_32(bmat_param_32(m))
/// void bmat_zero_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_zero_8 (bmat_param_8 (m)) { memset(m,0, bmat_sizeof_8 ); }
void bmat_zero_16(bmat_param_16(m)) { memset(m,0, bmat_sizeof_16); }
void bmat_zero_32(bmat_param_32(m)) { memset(m,0, bmat_sizeof_32); }
void bmat_zero_64(bmat_param_64(m)) { memset(m,0, bmat_sizeof_64); }

// fill n 64-bit data chucks with k: verbatium, shifted/rotated left/right by 's' per chunk
static inline void bmat_fill_k_n  (uint64_t* m, uint64_t k, uint32_t n)             { for_range(i,0,n) { m[i] = k; } }
static inline void bmat_fill_ksl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] = k; k <<= s;} }
static inline void bmat_fill_ksr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] = k; k >>= s;} }
static inline void bmat_fill_krl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] = k; k = rol_64(k,s);} }
static inline void bmat_fill_krr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] = k; k = ror_64(k,s);} }

// add n 64-bit data chucks with k: verbatium, shifted/rotated left/right by 's' per chunk
static inline void bmat_add_k_n  (uint64_t* m, uint64_t k, uint32_t n)             { for_range(i,0,n) { m[i] ^= k; } }
static inline void bmat_add_ksl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] ^= k; k <<= s;} }
static inline void bmat_add_ksr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] ^= k; k >>= s;} }
static inline void bmat_add_krl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] ^= k; k = rol_64(k,s);} }
static inline void bmat_add_krr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] ^= k; k = ror_64(k,s);} }

// Hadamard product n 64-bit data chucks with k: verbatium, shifted/rotated left/right by 's' per chunk
static inline void bmat_hmul_k_n  (uint64_t* m, uint64_t k, uint32_t n)             { for_range(i,0,n) { m[i] &= k; } }
static inline void bmat_hmul_ksl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] &= k; k <<= s;} }
static inline void bmat_hmul_ksr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] &= k; k >>= s;} }
static inline void bmat_hmul_krl_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] &= k; k = rol_64(k,s);} }
static inline void bmat_hmul_krr_n(uint64_t* m, uint64_t k, uint32_t s, uint32_t n) { for_range(i,0,n) { m[i] &= k; k = ror_64(k,s);} }




// temp hack: kill
bool bmat_ra_is_unit_8 (bmat_array_8 (m)) { uint8_t  n=1,s=0; for(int32_t i=0; i< 8; i++) { s|=(m[i]^n); n <<= 1; } return s==0; }
bool bmat_ra_is_unit_16(bmat_array_16(m)) { uint16_t n=1,s=0; for(int32_t i=0; i<16; i++) { s|=(m[i]^n); n <<= 1; } return s==0; }
bool bmat_ra_is_unit_32(bmat_array_32(m)) { uint32_t n=1,s=0; for(int32_t i=0; i<32; i++) { s|=(m[i]^n); n <<= 1; } return s==0; }
bool bmat_ra_is_unit_64(bmat_array_64(m)) { uint64_t n=1,s=0; for(int32_t i=0; i<64; i++) { s|=(m[i]^n); n <<= 1; } return s==0; }


/// ## bmat_is_unit_*n*(m)
///
/// Returns *true* if $m$ is the identity matrix
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_is_unit_8 (bmat_param_8 (m))
/// void bmat_is_unit_16(bmat_param_16(m))
/// void bmat_is_unit_32(bmat_param_32(m))
/// void bmat_is_unit_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

bool bmat_is_unit_8 (bmat_param_8 (m)) { uint8_t  a[ 8]; bmat_to_array_8 (a,m); return bmat_ra_is_unit_8 (a); }
bool bmat_is_unit_16(bmat_param_16(m)) { uint16_t a[16]; bmat_to_array_16(a,m); return bmat_ra_is_unit_16(a); }
bool bmat_is_unit_32(bmat_param_32(m)) { uint32_t a[32]; bmat_to_array_32(a,m); return bmat_ra_is_unit_32(a); }
bool bmat_is_unit_64(bmat_param_64(m)) { uint64_t a[64]; bmat_to_array_64(a,m); return bmat_ra_is_unit_64(a); }

/// ## bmat_set_unit_*n*(m)
///
/// Sets $m$ to the identity matrix $\left(I\right)$.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_unit_8 (bmat_param_8 (m))
/// void bmat_set_unit_16(bmat_param_16(m))
/// void bmat_set_unit_32(bmat_param_32(m))
/// void bmat_set_unit_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// set to identity matrix
void bmat_set_unit_8 (bmat_param_8 (m)) { m[0] = bmat_main_diagonal_mask_8; }
void bmat_set_unit_16(bmat_param_16(m)) { bmat_fill_ksl_n(m, bmat_main_diagonal_mask_16, 4, 4);  }
void bmat_set_unit_32(bmat_param_32(m)) { bmat_fill_ksl_n(m, bmat_main_diagonal_mask_32, 2, 16); }
void bmat_set_unit_64(bmat_param_64(m)) { uint64_t n=1; for_range(i,0,64) { m[i] = n; n <<= 1; } }

///----------
///
/// ## bmat_set_exchange_*n*(m)
///
/// Initializes matrix $m$ to $J$ (the [exchange matrix](https://en.wikipedia.org/wiki/Exchange_matrix)). $Jv$ performs a bit reversal of $v$ (and so does $vJ$ for nerdy row vectors). Pre and post multiplication by $J$ have specialized functions:
///
/// * $JM$ link row exchange
/// * $MJ$ link column exchange
/// 
/// $$
///   \left[ {\begin{array}{cccc}
///     0 & 0 & 0 & 1 \				\
///    \vdots & \vdots & \ddots & \vdots\\
///    0 & 1 & \cdots & 0\	 \
///    1 & 0 & \cdots & 0 \\
///  \end{array} } \right]
/// $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_exchange_8 ((bmat_param_8 (m))
/// void bmat_set_exchange_16((bmat_param_16(m))
/// void bmat_set_exchange_32((bmat_param_32(m))
/// void bmat_set_exchange_64((bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// set to exchange matrix (a.k.a: J, bitreverse) TEMP HACK
void bmat_set_exchange_8 (bmat_param_8 (m)) {uint8_t  a[ 8]; uint8_t  n=1; for(int32_t i= 7; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_8 (m,a); }
void bmat_set_exchange_16(bmat_param_16(m)) {uint16_t a[16]; uint16_t n=1; for(int32_t i=15; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_16(m,a); }
void bmat_set_exchange_32(bmat_param_32(m)) {uint32_t a[32]; uint32_t n=1; for(int32_t i=31; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_32(m,a); }
void bmat_set_exchange_64(bmat_param_64(m)) {uint64_t a[64]; uint64_t n=1; for(int32_t i=63; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_64(m,a); }


uint8_t bmat_from_ufunc_8(bmat_param_8(a), uint8_t (*f)(uint8_t))
{
  uint8_t m[8];

  uint8_t t = f(0);
  uint8_t p = 1;

  for(uint32_t i=0; i<8; i++) { m[i]=0; }
    
  do {
    uint8_t r = f(p) ^ t;
    uint8_t b = 1;

    for(int j=0; j<8; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_8(a,m);
  
  return t;
}

uint16_t bmat_from_ufunc_16(bmat_param_16(a), uint16_t (*f)(uint16_t))
{
  uint16_t m[16];

  uint16_t t = f(0);
  uint16_t p = 1;

  for(uint32_t i=0; i<16; i++) { m[i]=0; }
    
  do {
    uint16_t r = f(p) ^ t;
    uint16_t b = 1;

    for(int j=0; j<16; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_16(a,m);
  
  return t;
}



// assumes 'f' can represented by a 32x32 matrix M:
//   y=f(x) -> y = Mx
// or y = Mx+t
//   where 't' is the returned result
uint32_t bmat_from_ufunc_32(bmat_param_32(a), uint32_t (*f)(uint32_t))
{
  uint32_t m[32];

  // an input of zero should yield a zero result. If not
  // work on the assumption that it's 'f' is actually
  // affine.
  uint32_t t = f(0);
  uint32_t p = 1;

  for(uint32_t i=0; i<32; i++) { m[i]=0; }
    
  // build up matrix using single bit inputs
  do {
    uint32_t r = f(p) ^ t;
    uint32_t b = 1;

    for(int j=0; j<32; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_32(a,m);
  
  return t;
}

// allow the logical unary function 'f' be a f(x,k) for input constant 'k'
uint32_t bmat_from_ufunc_p_32(bmat_param_32(a), uint32_t (*f)(uint32_t,uint32_t), uint32_t k)
{
  uint32_t m[32];

  uint32_t t = f(0,k);
  uint32_t p = 1;

  for(uint32_t i=0; i<32; i++) { m[i]=0; }
    
  do {
    uint32_t r = f(p,k) ^ t;
    uint32_t b = 1;

    for(int j=0; j<32; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_32(a,m);

  return t;
}


uint64_t bmat_from_ufunc_64(bmat_param_64(a), uint64_t (*f)(uint64_t))
{
  uint64_t m[64];

  uint64_t t = f(0);
  uint64_t p = 1;

  for(uint32_t i=0; i<64; i++) { m[i]=0; }

  do {
    uint64_t r = f(p) ^ t;
    uint64_t b = 1;

    for(int j=0; j<64; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_64(a,m);
  
  return t;
}

uint64_t bmat_from_ufunc_p_64(bmat_param_64(a), uint64_t (*f)(uint64_t,uint64_t), uint64_t k)
{
  uint64_t m[64];

  uint64_t t = f(0,k);
  uint64_t p = 1;

  for(uint32_t i=0; i<64; i++) { m[i]=0; }

  do {
    uint64_t r = f(p,k) ^ t;
    uint64_t b = 1;

    for(int j=0; j<64; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_64(a,m);

  return t;
}
