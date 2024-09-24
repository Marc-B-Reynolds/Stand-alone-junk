// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

#include <stdio.h> // TEMP HACK

/// Stuff
///==============================================================
///

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
///  $$ Ax=0 $$ 
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

  // bits
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

	// exit and move to next row (outter loop)
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
	
	cc ^= t;

	// flip to branch free?
	if ((M[id] & bi)==0) continue;
	
	v ^= UINT64_C(1) << mark[id];
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

  bmat_adup_8(a,m);

  for (uint32_t i=0; i<8; i++)  M[i] = a[i];

  uint32_t r = bmat_kernel_w(M,K,8);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint8_t)K[i];

  return r;
}

uint32_t
bmat_kernel_16(bmat_rparam_16(m), bmat_array_16(k))
{
  uint64_t M[16];
  uint64_t K[16];

  bmat_adup_16(a,m);

  for (uint32_t i=0; i<16; i++)  M[i] = a[i];

  uint32_t r = bmat_kernel_w(M,K,16);

  for (uint32_t i=0; i<r; i++)  k[i] = (uint16_t)K[i];

  return r;
}

uint32_t
bmat_kernel_32(bmat_rparam_32(m), bmat_array_32(k))
{
  uint64_t M[32];
  uint64_t K[32];

  bmat_adup_32(a,m);

  for (uint32_t i=0; i<32; i++)  M[i] = a[i];

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


/// doc disabled
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

#if 0
uint32_t bmat_cokernel_8 (bmat_param_8 (m), bmat_param_8 (v)) { bmat_def_8 (t); bmat_transpose_8(t,m);  return bmat_kernel_8 (t,v); }
uint32_t bmat_cokernel_16(bmat_param_16(m), bmat_param_16(v)) { bmat_def_16(t); bmat_transpose_16(t,m); return bmat_kernel_16(t,v); }
uint32_t bmat_cokernel_32(bmat_param_32(m), bmat_param_32(v)) { bmat_def_32(t); bmat_transpose_32(t,m); return bmat_kernel_32(t,v); }
uint32_t bmat_cokernel_64(bmat_param_64(m), bmat_param_64(v)) { bmat_def_64(t); bmat_transpose_64(t,m); return bmat_kernel_64(t,v); }
#endif


/// doc disabled
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
