// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

/// Powers
///==============================================================
///

// nothing done (as you can see)


//*******************************************************************
/// ----------
///
/// ## bmat_pow2_*n*(m)
///
/// Computes $ M = M^2 $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_pow2_8 (bmat_rparam_8 (m),bmat_param_8 (a))
/// void bmat_pow2_16(bmat_rparam_16(m),bmat_param_16(a))
/// void bmat_pow2_32(bmat_rparam_32(m),bmat_param_16(a))
/// void bmat_pow2_64(bmat_rparam_64(m),bmat_param_16(a))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_pow2_8 (bmat_rparam_8 (m)) { bmat_defdup_8 (a,m); bmat_mul_8 (m,a,a); }
void bmat_pow2_16(bmat_rparam_16(m)) { bmat_defdup_16(a,m); bmat_mul_16(m,a,a); }
void bmat_pow2_32(bmat_rparam_32(m)) { bmat_defdup_32(a,m); bmat_mul_32(m,a,a); }
void bmat_pow2_64(bmat_rparam_64(m)) { bmat_defdup_64(a,m); bmat_mul_64(m,a,a); }



//*******************************************************************
/// ----------
///
/// ## bmat_pow_*n*(m,a,n)
///
/// Computes $ M = A^n $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_pow_8 (bmat_rparam_8 (m), bmat_param_8 (a), uint64_t n)
/// void bmat_pow_16(bmat_rparam_16(m), bmat_param_16(a), uint64_t n)
/// void bmat_pow_32(bmat_rparam_32(m), bmat_param_32(a), uint64_t n)
/// void bmat_pow_64(bmat_rparam_64(m), bmat_param_64(a), uint64_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_pow_8(bmat_rparam_8(m), bmat_param_8(a), uint64_t n)
{
  bmat_def_8(s);
  bmat_def_8(t);
  
  bmat_set_unit_8(m);                   // M=I
  bmat_dup_8(s,a);                      // S=A
  
  while (n != 0) {
    if (n & 1) {
      bmat_dup_8(t,m);			
      bmat_mul_8(m,t,s);                // M=M*S
    }

    bmat_pow2_8(s);			// S=S^2
    
    n >>= 1;
  }
}

void bmat_pow_16(bmat_rparam_16(m), bmat_param_16(a), uint64_t n)
{
  bmat_def_16(s);
  bmat_def_16(t);
  
  bmat_set_unit_16(m);                  // M=I
  bmat_dup_16(s,a);                     // S=A
  
  while (n != 0) {
    if (n & 1) {
      bmat_dup_16(t,m);			
      bmat_mul_16(m,t,s);               // M=M*S
    }

    bmat_pow2_16(s);			// S=S^2
    
    n >>= 1;
  }
}


void bmat_pow_32(bmat_rparam_32(m), bmat_param_32(a), uint64_t n)
{
  bmat_def_32(s);
  bmat_def_32(t);
  
  bmat_set_unit_32(m);                  // M=I
  bmat_dup_32(s,a);                     // S=A
  
  while (n != 0) {
    if (n & 1) {
      bmat_dup_32(t,m);			
      bmat_mul_32(m,t,s);               // M=M*S
    }

    bmat_pow2_32(s);			// S=S^2
    
    n >>= 1;
  }
}

void bmat_pow_64(bmat_rparam_64(m), bmat_param_64(a), uint64_t n)
{
  bmat_def_64(s);
  bmat_def_64(t);
  
  bmat_set_unit_64(m);                  // M=I
  bmat_dup_64(s,a);                     // S=A
  
  while (n != 0) {
    if (n & 1) {
      bmat_dup_64(t,m);			
      bmat_mul_64(m,t,s);               // M=M*S
    }

    bmat_pow2_64(s);			// S=S^2
    
    n >>= 1;
  }
}

