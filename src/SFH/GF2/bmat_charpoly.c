// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

#include <stdio.h> // TEMP HACK

/// Characteristic polynomial stuff
///==============================================================
///


// Two refs for the Hessenberg algorithm:
// "A Course in Computational Algebraic Number Theory",
// Henri Cohen. Algorithm 2.2.9
// 
// "Computing Characteristic Polynomials of Matrices of
// Structured Polynomials", Marshall Law, 2017
// Algorithm 3
// (https://www.cecm.sfu.ca/CAG/theses/marshall.pdf)

// bmat_col_{swap,add}_w autovectorize pretty well. changing the mask
// to be passed in to adapt smaller than 64-bit row workers

// requires i>j
static inline void
bmat_col_swap_w(uint64_t* m, uint32_t i, uint32_t j, uint32_t n)
{
  uint64_t mask = (1UL << j);
  uint32_t dist = i-j;

  for (uint32_t k=0; k<n; k++) {
    m[k] = bit_permute_step_64(m[k],mask,dist);
  }
}

// requires i>j
static inline void
bmat_col_add_w(uint64_t* m, uint32_t i, uint32_t j, uint32_t n)
{
  uint64_t mask = (1UL << i);
  uint32_t dist = i-j;

  for (uint32_t k=0; k<n; k++) {
    m[k] ^= (m[k] & mask) >> dist;
  }
}

/// ## bmat_hessenberg_*n*(m)
/// 
/// In place reduces `m` into an upper [Hessenberg matrix](https://en.wikipedia.org/wiki/Hessenberg_matrix).
/// The reduced matrix is *similar* to `m` and therefor has the same
/// [characteristic polynomial](https://en.wikipedia.org/wiki/Characteristic_polynomial).
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_hessenberg_8 (bmat_param_8 (m))
/// void bmat_hessenberg_16(bmat_param_16(m))
/// void bmat_hessenberg_32(bmat_param_32(m))
/// void bmat_hessenberg_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// worker function
static inline void bmat_hessenberg_w(uint64_t* r, uint32_t n)
{
  uint64_t bj = 1;

  // reduce
  for (uint32_t j=1; j<n-1; j++, bj <<= 1) {
    // pivot search
    for (uint32_t i=j+1; i<n; i++) {

      if (r[i] & bj) {
	// pivot found: swap the row & column
	BIT_SWAP2_64(r[i],r[j]);
	bmat_col_swap_w(r,i,j,n);

	// forward reduce
	for (i=j+1; i<n; i++) {
	  if (r[i] & bj) {
	    r[i] ^= r[j];
	    bmat_col_add_w(r,i,j,n);
	  }
	}

	// pivot was found: exit search loop
	break;
      }
    }
  }
}


//******************************************************************************

void bmat_hessenberg_64(bmat_param_64(m))
{
  bmat_hessenberg_w(m,64);
}

// versions that wrapper generic

void bmat_hessenberg_8(bmat_param_8(m))
{
  BMAT_ALIGN uint64_t M[8];
  
  bmat_widen_8(M,m);
  bmat_hessenberg_w(M,8);
  bmat_narrow_8(m,M);
}

void bmat_hessenberg_16(bmat_param_16(m))
{
  BMAT_ALIGN uint64_t M[16];

  bmat_widen_16(M,m);
  bmat_hessenberg_w(M,16);
  bmat_narrow_16(m,M);
}

void bmat_hessenberg_32(bmat_param_32(m))
{
  BMAT_ALIGN uint64_t M[32];

  bmat_widen_32(M,m);
  bmat_hessenberg_w(M,32);
  bmat_narrow_32(m,M);
}


//******************************************************************************

/// ## bmat_charpoly_*n*(m)
/// 
/// 
/// NOTE: The top bit 
/// 
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint64_t bmat_charpoly_8 (bmat_param_8 (m))
/// uint64_t bmat_charpoly_16(bmat_param_16(m))
/// uint64_t bmat_charpoly_32(bmat_param_32(m))
/// uint64_t bmat_charpoly_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


uint64_t
bmat_hessenberg_recurrence_n(const uint64_t* H, uint64_t n)
{
  uint64_t c[65];

  c[0] = 1;                             // c_0 = 1

  uint64_t bm = 1;                      // main diagonal bit

  // compute the recurrence relation:
  for (uint32_t m=0; m<n; m++, bm <<= 1) {
    uint64_t dm = H[m] & bm;            // main diagonal entry
    uint64_t cm = c[m];                 // start with previous entry

    cm = (cm<<1) ^ (dm ? cm : 0);       // c_m = (2+d_m) c_{m-1}

    uint64_t bd = bm >> 1;              // sub-diagonal bit

    // compute the sum term
    for (uint32_t i=1; i<=m; i++, bd >>= 1) {
      uint32_t d = m-i;

      // sum's done when we hit a zero on the sub-diagonal
      if (!(H[d+1] & bd)) break;

      cm ^= (H[d] & bm) ? c[d] : 0;
    }

    c[m+1] = cm;
  }
  
  return c[n];
}


BMAT_FLATTEN
uint64_t bmat_charpoly_64(bmat_param_64(m))
{
  bmat_def_64(h);
  bmat_dup_64(h,m);
  bmat_hessenberg_w(h,64);
  return bmat_hessenberg_recurrence_n(h,64);
}

// versions that wrapper generic

BMAT_FLATTEN
uint64_t bmat_charpoly_8(bmat_param_8(m))
{
  BMAT_ALIGN uint64_t H[8];
  
  bmat_widen_8(H,m);
  bmat_hessenberg_w(H,8);
  return bmat_hessenberg_recurrence_n(H,8);
}

BMAT_FLATTEN
uint64_t bmat_charpoly_16(bmat_param_16(m))
{
  BMAT_ALIGN uint64_t H[16];
  bmat_widen_16(H,m);
  bmat_hessenberg_w(H,16);
  return bmat_hessenberg_recurrence_n(H,16);
}

BMAT_FLATTEN
uint64_t bmat_charpoly_32(bmat_param_32(m))
{
  BMAT_ALIGN uint64_t H[32];
  bmat_widen_32(H,m);
  bmat_hessenberg_w(H,32);
  return bmat_hessenberg_recurrence_n(H,32);
}
