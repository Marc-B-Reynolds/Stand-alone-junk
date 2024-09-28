// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

#include <stdio.h> // TEMP HACK

/// Stuff
///==============================================================
///

// ....
// performs elementary row operations to
// get an upper triangular matrix but isn't
// quite row echelon form (REF).
void bmat_reduce_ut_8(bmat_param_8(M))
{
  uint64_t cmask = 0x0101010101010101;
  uint64_t rmask = 0xff;
  uint64_t m     = M[0];

  for(uint32_t i=0; i<7; i++) {
    // select: sets LSB of bytes of rows with column 'i' set
    // shift:  bit distance from row 'i' to the first row
    //         with column 'i' set (for row swap)
    
    uint64_t select = m & cmask;
    uint32_t shift  = ctz_64(select);

    // perform a row swap:
    // 1) if there's a row with column 'i' set then swap
    //    with the first (if this row then NOP is performed)
    // * hint : GCC will introduce a branch to avoid
    //          the delta-swap without it.
    shift = hint_no_const_fold_32((shift != 64) ? shift - 9*i : 0);

    // perform the row swap (NOP if none)
    m = bit_permute_step_64(m, rmask, shift);

    // * clear the low set bit of select and shift
    //   into for position for the row broadcast.
    // * update the column mask for next
    select = (select ^ (-select & select)) >> 9*i;
    cmask  = cmask << 9;

    uint64_t broadcast = select * (m & rmask);
    
    m ^= broadcast;

    rmask <<= 8;
  }

  M[0] = m;
}


/// ## bmat_echelon_*n*(m)
/// 
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_echelon_8 (bmat_param_8 (m))
/// uint32_t bmat_echelon_16(bmat_param_16(m))
/// uint32_t bmat_echelon_32(bmat_param_32(m))
/// uint32_t bmat_echelon_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


// textbook like. could be sped up quit a bit. plus SIMD
// version obviously.
uint32_t bmat_echelon_n(uint64_t* A, uint32_t n, uint32_t m)
{
  uint32_t e    = m <= n ? m : n;
  uint32_t rank = 0;
  uint32_t j    = 0;
  uint64_t bj   = 1;

  while ((j < m) && (rank < e)) {
    
    // pivot search
    for (uint32_t i=rank; i<n; i++) {

      // found and perform elimination
      if (A[i] & bj) {
        uint64_t row = A[i];                // row of pivot
        BIT_SWAP2_64(A[i],A[rank]);         // swap
        rank += 1;

        // forward eliminiation
        for (uint32_t l=rank; l<n; l++) {
          A[l] ^= (A[l] & bj) ? row : 0;
        }

        // pivot was found: exit search loop
        break;
      }
    }

    // move to next column
    j   += 1;
    bj <<= 1;
  }
  return rank;
}

uint32_t bmat_echelon_64(bmat_param_64(m))
{
  return bmat_echelon_n(m,64,64);
}

// versions that wrapper generic
uint32_t bmat_echelon_8(bmat_param_8(m))
{
  uint64_t M[8];
  
  bmat_widen_8(M,m);
  uint32_t r = bmat_echelon_n(M,8,8);
  bmat_narrow_8(m,M);
  return r;
}

uint32_t bmat_echelon_16(bmat_param_16(m))
{
  uint64_t M[16];

  bmat_widen_16(M,m);
  uint32_t r = bmat_echelon_n(M,16,16);
  bmat_narrow_16(m,M);
  return r;
}

uint32_t bmat_echelon_32(bmat_param_32(m))
{
  uint64_t M[32];

  bmat_widen_32(M,m);
  uint32_t r = bmat_echelon_n(M,32,32);
  bmat_narrow_32(m,M);
  return r;
}


/// ## bmat_rref_*n*(m)
/// In place computes the *reduced row echelon form* of `m`.
/// Returns the *rank* of `m`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_rref_8 (bmat_param_8 (m))
/// uint32_t bmat_rref_16(bmat_param_16(m))
/// uint32_t bmat_rref_32(bmat_param_32(m))
/// uint32_t bmat_rref_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// temp hack
#if 0
void bmat_match_row_add_64(uint64_t* m, uint64_t row, uint64_t t, uint32_t n)
{
  for (uint32_t j=0; j<n; j++) {
    m[j] ^= (m[j] & t) ? row : 0;
  }
}
#else
// temp hack. caller should be SIMDified as well. also breaks strict aliasing
// maybe on SIMDe ARM? Needs checking (probably not since common pattern)
void bmat_match_row_add_64(uint64_t* m, uint64_t row, uint64_t test, uint32_t n)
{
  u256_t*  M = (u256_t*)m;
  u256_t   v = broadcast_64x4(row);
  u256_t   s = broadcast_64x4(test);
  uint32_t e = n >> 2;

  for(uint32_t i=0; i<e; i++) {
    u256_t t = cmpeq_64x4(and_256(M[i],s),s);
    M[i] = xor_256(M[i], and_256(t,v));
  }
}
#endif

// get this cleaned-up before making specialized smaller
// & SIMD versions. NOTE: this is passing over the whole
// matrix each time. this is more for "doing it the way"
// the SIMD one will instead of how might be reasonable
// for "this" function to behave. (but then again should
// peek at how this autovectorizes at some point)
uint32_t bmat_rref_n(uint64_t* A, uint32_t n, uint32_t m)
{
  uint32_t e    = m <= n ? m : n;
  uint32_t rank = 0;
  uint32_t j    = 0;
  uint64_t bj   = 1;

  while ((j < m) && (rank < e)) {
    
    // pivot search
    for (uint32_t i=rank; i<n; i++) {

      // found and perform elimination
      if (A[i] & bj) {
        uint64_t row = A[i];                // row of pivot
	A[i] = A[rank];                     // part 1 of logical swap
        bmat_match_row_add_64(A,row,bj,n);  // eliminate on whole matrix
        A[rank] = row;                      // complete the swap
        rank += 1;
        break;                              // exit search loop
      }
    }

    // move to next column
    j   += 1;
    bj <<= 1;
  }

  return rank;
}


uint32_t bmat_rref_64(bmat_rparam_64(m))
{
  return bmat_rref_n(m,64,64);
}

// versions that wrapper generic
uint32_t bmat_rref_8(bmat_param_8(m))
{
  uint64_t M[8];
  
  bmat_widen_8(M,m);
  uint32_t r = bmat_rref_n(M,8,8);
  bmat_narrow_8(m,M);
  return r;
}

uint32_t bmat_rref_16(bmat_rparam_16(m))
{
  uint64_t M[16];

  bmat_widen_16(M,m);
  uint32_t r = bmat_rref_n(M,16,16);
  bmat_narrow_16(m,M);
  return r;
}

uint32_t bmat_rref_32(bmat_rparam_32(m))
{
  uint64_t M[32];

  bmat_widen_32(M,m);
  uint32_t r = bmat_rref_n(M,32,32);
  bmat_narrow_32(m,M);
  return r;
}


/// ## bmat_rank_*n*(m)
///
/// Returns the [rank](https://en.wikipedia.org/wiki/Rank_(linear_algebra)) of `m`
///
/// (add notes about special matrices)
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_rank_8 (bmat_param_8 (m))
/// uint32_t bmat_rank_16(bmat_param_16(m))
/// uint32_t bmat_rank_32(bmat_param_32(m))
/// uint32_t bmat_rank_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// just forward to reference for the moment.
extern uint32_t bmat_rank_8_ref (bmat_param_8 (m));
extern uint32_t bmat_rank_16_ref(bmat_param_16(m));
extern uint32_t bmat_rank_32_ref(bmat_param_32(m));
extern uint32_t bmat_rank_64_ref(bmat_param_64(m));

uint32_t bmat_rank_8 (bmat_param_8 (m)) { return bmat_rank_8_ref(m);  }
uint32_t bmat_rank_16(bmat_param_16(m)) { return bmat_rank_16_ref(m); }
uint32_t bmat_rank_32(bmat_param_32(m)) { return bmat_rank_32_ref(m); }
uint32_t bmat_rank_64(bmat_param_64(m)) { return bmat_rank_64_ref(m); }



/// ## bmat_nullity_*n*(m)
///
/// Returns the *nullity* of `m`.
///
//  Trivially related to the rank: 
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_nullity_8 (bmat_param_8 (m))
/// uint32_t bmat_nullity_16(bmat_param_16(m))
/// uint32_t bmat_nullity_32(bmat_param_32(m))
/// uint32_t bmat_nullity_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

uint32_t bmat_nullity_8 (bmat_param_8(m) ) { return  8-bmat_rank_8(m);  }
uint32_t bmat_nullity_16(bmat_param_16(m)) { return 16-bmat_rank_16(m); }
uint32_t bmat_nullity_32(bmat_param_32(m)) { return 32-bmat_rank_32(m); }
uint32_t bmat_nullity_64(bmat_param_64(m)) { return 64-bmat_rank_64(m); }


/// ## bmat_is_full_rank_*n*(m)
///
/// Returns *true* if `m` is full rank. 
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// bool bmat_is_full_rank_8 (bmat_param_8 (m))
/// bool bmat_is_full_rank_16(bmat_param_16(m))
/// bool bmat_is_full_rank_32(bmat_param_32(m))
/// bool bmat_is_full_rank_64(bmat_param_64(m))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// just forward for the moment. given an optimized rank computation can flip
// to a specialized version that bails when any row becomes zero.
bool bmat_is_full_rank_8 (bmat_param_8(m) ) { return bmat_rank_8(m)  ==  8; }
bool bmat_is_full_rank_16(bmat_param_16(m)) { return bmat_rank_16(m) == 16; }
bool bmat_is_full_rank_32(bmat_param_32(m)) { return bmat_rank_32(m) == 32; }
bool bmat_is_full_rank_64(bmat_param_64(m)) { return bmat_rank_64(m) == 64; }

/// ## bmat_kernel_*n*(m,v)
/// 
/// [kernel](https://en.wikipedia.org/wiki/Kernel_(linear_algebra)) (aka *nullspace*)
///
///  $$ Av=0 $$
///
///  A\left(x+v\right) = Ax + Av = Ax = b
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_kernel_8 (bmat_rparam_8 (m), bmat_rparam_8 (v))
/// uint32_t bmat_kernel_16(bmat_rparam_16(m), bmat_rparam_16(v))
/// uint32_t bmat_kernel_32(bmat_rparam_32(m), bmat_rparam_32(v))
/// uint32_t bmat_kernel_64(bmat_rparam_64(m), bmat_rparam_64(v))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// kernel worker: handles up to 64-bit rows

uint32_t bmat_kernel_w(uint64_t* restrict M, uint64_t* restrict V, uint64_t n)
{
  uint64_t c    = UINT64_C(~0) >> (64-n);
  uint64_t mask = c;
  uint64_t bi   = 1;
  uint32_t nullity = 0;

  // defect row index 
  uint8_t  mark[64];

  // walk the rows
  for (uint32_t i=0; i<n; i++, bi<<=1) {

    uint64_t cc = c;

    while(true) {
      uint64_t t   = -cc & cc;
      uint32_t id  = ctz_64(t);
      uint64_t row = M[id];

      cc ^= t;
      
      if (row & bi) {
        for (uint32_t j=0; j<n; j++) {
          if (j != id)
            if (M[j] & bi) M[j] ^= row;
        }
        
        c ^= t;
        mark[id] = (uint8_t)i;

        // exit and move to next row (outer loop)
        break;
      }
      
      // move to the next candidate if any remain
      if (cc) continue;

      // otherwise the nullity count needs to
      // increase and we generate the vector
      uint64_t v = bi;

      // walk the complement of the flags
      cc = c ^ mask;

      do {
        uint64_t t  = -cc & cc;
        uint32_t id = ctz_64(t);
        uint64_t x  = (M[id] & bi) ? 1 : 0;
        
        cc ^= t;
        v  ^= x << mark[id];
      } while(cc);
      
      V[nullity++] = v;
      break;
    }
  }
  
  return nullity;
}

uint32_t
bmat_kernel_8(bmat_rparam_8(m), bmat_array_8(k))
{
  uint64_t M[8];
  uint64_t K[8];

  bmat_widen_8(M,m);

  uint32_t r = bmat_kernel_w(M,K,8);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint8_t)K[i];

  return r;
}

uint32_t
bmat_kernel_16(bmat_rparam_16(m), bmat_array_16(k))
{
  uint64_t M[16];
  uint64_t K[16];

  bmat_widen_16(M,m);

  uint32_t r = bmat_kernel_w(M,K,16);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint16_t)K[i];

  return r;
}

uint32_t
bmat_kernel_32(bmat_rparam_32(m), bmat_array_32(k))
{
  uint64_t M[32];
  uint64_t K[32];

  bmat_widen_32(M,m);

  uint32_t r = bmat_kernel_w(M,K,32);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint32_t)K[i];

  return r;
}

uint32_t
bmat_kernel_64(bmat_rparam_64(m), bmat_array_64(k))
{
  uint64_t M[64];

  for (uint32_t i=0; i<64; i++)  M[i] = m[i];

  return bmat_kernel_w(M,k,64);
}


/// ## bmat_cokernel_*n*(m,v)
/// 
/// [cokernel](https://en.wikipedia.org/wiki/Cokernel) (aka *left nullspace*)
///  $$ xA=0 $$
///
/// 
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_cokernel_8 (bmat_rparam_8 (m), bmat_rparam_8 (v))
/// uint32_t bmat_cokernel_16(bmat_rparam_16(m), bmat_rparam_16(v))
/// uint32_t bmat_cokernel_32(bmat_rparam_32(m), bmat_rparam_32(v))
/// uint32_t bmat_cokernel_64(bmat_rparam_64(m), bmat_rparam_64(v))
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

uint32_t bmat_cokernel_8 (bmat_rparam_8 (m), bmat_array_8 (v)) { bmat_def_8 (t); bmat_transpose_8(t,m);  return bmat_kernel_8 (t,v); }
uint32_t bmat_cokernel_16(bmat_rparam_16(m), bmat_array_16(v)) { bmat_def_16(t); bmat_transpose_16(t,m); return bmat_kernel_16(t,v); }
uint32_t bmat_cokernel_32(bmat_rparam_32(m), bmat_array_32(v)) { bmat_def_32(t); bmat_transpose_32(t,m); return bmat_kernel_32(t,v); }
uint32_t bmat_cokernel_64(bmat_rparam_64(m), bmat_array_64(v)) { bmat_def_64(t); bmat_transpose_64(t,m); return bmat_kernel_64(t,v); }


/// ## bmat_fixed_points_*n*(m,v)
///
/// $$ Ax=x $$ 
/// 
/// $$ Ax+x=0 \\  
///    \left(A+I\right)x = 0$$ 
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint32_t bmat_fixed_points_8 (bmat_param_8 (m), uint8_t  v[static  8])
/// uint32_t bmat_fixed_points_16(bmat_param_16(m), uint16_t v[static 16])
/// uint32_t bmat_fixed_points_32(bmat_param_32(m), uint32t  v[static 32])
/// uint32_t bmat_fixed_points_64(bmat_param_64(m), uint64t  v[static 64])
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

uint32_t
bmat_fixed_points_8(bmat_rparam_8(m), bmat_array_8(k))
{
  uint64_t M[8];
  uint64_t K[8];
  uint64_t u = 1;

  bmat_adup_8(a,m);
  
  for (uint32_t i=0; i<8; i++, u<<=1)  M[i] = a[i]^u;

  uint32_t r = bmat_kernel_w(M,K,8);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint8_t)K[i];

  return r;
}

uint32_t
bmat_fixed_points_16(bmat_rparam_16(m), bmat_array_16(k))
{
  uint64_t M[16];
  uint64_t K[16];
  uint64_t u = 1;

  bmat_adup_16(a,m);

  for (uint32_t i=0; i<16; i++, u<<=1)  M[i] = a[i]^u;

  uint32_t r = bmat_kernel_w(M,K,16);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint16_t)K[i];

  return r;
}

uint32_t
bmat_fixed_points_32(bmat_rparam_32(m), bmat_array_32(k))
{
  uint64_t M[32];
  uint64_t K[32];
  uint64_t u = 1;

  bmat_adup_32(a,m);

  for (uint32_t i=0; i<32; i++, u<<=1)  M[i] = a[i]^u;

  uint32_t r = bmat_kernel_w(M,K,32);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint32_t)K[i];

  return r;
}

uint32_t
bmat_fixed_points_64(bmat_rparam_64(m), bmat_array_64(k))
{
  uint64_t M[64];
  uint64_t u = 1;

  for (uint32_t i=0; i<64; i++, u<<=1)  M[i] = m[i]^u;

  return bmat_kernel_w(M,k,64);
}
