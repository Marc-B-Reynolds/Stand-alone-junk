// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"
#include <string.h>

/// Stuff
///==============================================================
///

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
void bmat_set_unit_8 (bmat_param_8 (m)) { bmat_set_lsk_8 (m, BMAT_MAIN_DIAGONAL_MASK_8);  }
void bmat_set_unit_16(bmat_param_16(m)) { bmat_set_lsk_16(m, BMAT_MAIN_DIAGONAL_MASK_16); }
void bmat_set_unit_32(bmat_param_32(m)) { bmat_set_lsk_32(m, BMAT_MAIN_DIAGONAL_MASK_32); }
void bmat_set_unit_64(bmat_param_64(m)) { bmat_set_lsk_64(m, 1); }

/// ## bmat_set_ones_*n*(m)
///
/// Sets $m$ to the [ones matrix]()
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_ones_8 (bmat_param_8 (m))
/// void bmat_set_ones_16(bmat_param_16(m))
/// void bmat_set_ones_32(bmat_param_32(m))
/// void bmat_set_ones_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// set to identity matrix
void bmat_set_ones_8 (bmat_param_8 (m)) { memset(m,0xff, bmat_sizeof_8 ); }
void bmat_set_ones_16(bmat_param_16(m)) { memset(m,0xff, bmat_sizeof_16); }
void bmat_set_ones_32(bmat_param_32(m)) { memset(m,0xff, bmat_sizeof_32); }
void bmat_set_ones_64(bmat_param_64(m)) { memset(m,0xff, bmat_sizeof_64); }

///----------
///
/// ## bmat_set_exchange_*n*(m)
///
/// Initializes matrix $M$ to $J$ (the [exchange matrix](https://en.wikipedia.org/wiki/Exchange_matrix)).
///
/// $$
///   \left[ {\begin{array}{cccc}
///     0 & 0 & 0 & 1 \newline
///    \vdots & \vdots & \ddots & \vdots \newline
///    0 & 1 & \cdots & 0                \newline
///    1 & 0 & \cdots & 0                \newline
///  \end{array} } \right]
/// $$
/// 
///  $Jv$ performs a bit reversal of $v$ (and so does $vJ$ for nerdy row vectors). Pre and post multiplication by $J$ have specialized functions:
/// <details markdown="1"><summary>function list:</summary>
/// 
/// * $JM$ link row exchange
/// * $MJ$ link column exchange
/// 
/// 
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_exchange_8 ((bmat_param_8 (m))
/// void bmat_set_exchange_16((bmat_param_16(m))
/// void bmat_set_exchange_32((bmat_param_32(m))
/// void bmat_set_exchange_64((bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// set to exchange matrix (a.k.a: J, bitreverse). compile to const-load/store
void bmat_set_exchange_8 (bmat_param_8 (m)) {uint8_t  a[ 8]; uint8_t  n=1; for(int32_t i= 7; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_8 (m,a); }
void bmat_set_exchange_16(bmat_param_16(m)) {uint16_t a[16]; uint16_t n=1; for(int32_t i=15; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_16(m,a); }
void bmat_set_exchange_32(bmat_param_32(m)) {uint32_t a[32]; uint32_t n=1; for(int32_t i=31; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_32(m,a); }
void bmat_set_exchange_64(bmat_param_64(m)) {uint64_t a[64]; uint64_t n=1; for(int32_t i=63; i>=0; i--) { a[i] = n; n <<= 1; } array_to_bmat_64(m,a); }


///----------
///
/// ## bmat_set_companion_*n*(m,n)
///
/// Initializes matrix $M$ to the [Frobenius companion matrix](https://en.wikipedia.org/wiki/Exchange_matrix))
/// NO:
/// 
/// * blah 
/// 
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_companion_8 ((bmat_param_8 (m), uint64_t n)
/// void bmat_set_companion_16((bmat_param_16(m), uint64_t n)
/// void bmat_set_companion_32((bmat_param_32(m), uint64_t n)
/// void bmat_set_companion_64((bmat_param_64(m), uint64_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

#if 0
// todo: add_col
void bmat_set_companion_8 (bmat_param_8 (m), uint64_t n) { bmat_set_lshift_8 (m,1); bmat_set_col_8 (m, 7,n); }
void bmat_set_companion_16(bmat_param_16(m), uint64_t n) { bmat_set_lshift_16(m,1); bmat_set_col_16(m,15,n); }
void bmat_set_companion_32(bmat_param_32(m), uint64_t n) { bmat_set_lshift_32(m,1); bmat_set_col_32(m,31,n); }
void bmat_set_companion_64(bmat_param_64(m), uint64_t n) { bmat_set_lshift_64(m,1); bmat_set_col_64(m,63,n); }
#endif


///----------
///
/// ## bmat_set_linear_rs_*n*(m,n)
///
/// Initializes matrix $M$ to the [Frobenius companion matrix](https://en.wikipedia.org/wiki/Exchange_matrix)) 
/// NO:
/// * blah 
/// $$
///   \left[ {\begin{array}{cccc}
///     0 & 0 & 0 & 1  \newline
///    \vdots & \vdots & \ddots & \vdots \newline
///    0 & 1 & \cdots & 0 \newline
///    1 & 0 & \cdots & 0 \newline
///  \end{array} } \right]
/// $$
/// 
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_set_linear_rs_8 ((bmat_param_8 (m), uint64_t n)
/// void bmat_set_linear_rs_16((bmat_param_16(m), uint64_t n)
/// void bmat_set_linear_rs_32((bmat_param_32(m), uint64_t n)
/// void bmat_set_linear_rs_64((bmat_param_64(m), uint64_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

#if 0
// todo: add_row
void bmat_set_linear_rs_8 (bmat_param_8 (m), uint64_t n) { bmat_set_rshift_8 (m,1); bmat_set_row_8 (m, 7,n); }
void bmat_set_linear_rs_16(bmat_param_16(m), uint64_t n) { bmat_set_rshift_16(m,1); bmat_set_row_16(m,15,n); }
void bmat_set_linear_rs_32(bmat_param_32(m), uint64_t n) { bmat_set_rshift_32(m,1); bmat_set_row_32(m,31,n); }
void bmat_set_linear_rs_64(bmat_param_64(m), uint64_t n) { bmat_set_rshift_64(m,1); bmat_set_row_64(m,63,n); }
#endif

