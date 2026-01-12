// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

/// FLINT bridge functions
///==============================================================
///
///
///
///

#include "bmat_i.h"
#include "bmat_flint.h"

// mod 2 constant
const mp_limb_t flint_n = 2;


//******************************************************************************
// matrix alloc/free

nmod_mat_t* flint_gf2m_alloc_n(slong n)
{
  nmod_mat_t* m = (nmod_mat_t*)flint_malloc(sizeof(nmod_mat_t));
  
  nmod_mat_init(m[0],n,n,flint_n);
  
  return m;
}

nmod_poly_t* flint_gf2p_alloc(void)
{
  nmod_poly_t* p = (nmod_poly_t*)flint_malloc(sizeof(nmod_poly_t));
  
  nmod_poly_init(p[0],flint_n);
  
  return p;
}

void flint_gf2m_free(nmod_mat_t* m)
{
  if (m != NULL) {
    nmod_mat_clear(m[0]);
    flint_free(m);
  }
}

void flint_gf2p_free(nmod_poly_t* p)
{
  if (p != NULL) {
    nmod_poly_clear(p[0]);
    flint_free(p);
  }
}


/// ----------
/// ## flint_alloc_*n*(void)
///
/// allocates a FLINT structure for a $n \times n$ matrix.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// nmod_mat_t* flint_alloc_8 (void)
/// nmod_mat_t* flint_alloc_16(void)
/// nmod_mat_t* flint_alloc_32(void)
/// nmod_mat_t* flint_alloc_64(void)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// change these names
nmod_mat_t* flint_gf2m_alloc_8 (void) { return flint_gf2m_alloc_n(8);  }
nmod_mat_t* flint_gf2m_alloc_16(void) { return flint_gf2m_alloc_n(16); }
nmod_mat_t* flint_gf2m_alloc_32(void) { return flint_gf2m_alloc_n(32); }
nmod_mat_t* flint_gf2m_alloc_64(void) { return flint_gf2m_alloc_n(64); }

//******************************************************************************

#if defined(BMAT_M4RI_ENABLE)

#include <m4ri/m4ri_config.h>
#include <m4ri/m4ri.h>

//******************************************************************************
// move in/out of flint matrices. flint must be proper shape

/// ----------
/// ## m4ri_to_flint_*n*(d,m)
///
/// copies the matrix `m` into the FLINT structure `d`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_to_flint_8 (nmod_mat_t d, bmat_param_8(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


// M4RI convert. must be the same shape
void m4ri_to_flint(nmod_mat_t R, mzd_t* M)
{
  rci_t n = M->nrows;

  for (rci_t x=0; x<n; x++) {
    for (rci_t y=0; y<n; y++) {
      nmod_mat_entry(R,x,y) = (mp_limb_t)mzd_read_bit(M,x,y);
    }
  }
}

void flint_to_m4ri(mzd_t* R, nmod_mat_t M)
{
  rci_t n = (rci_t)nmod_mat_nrows(M);

  for (rci_t x=0; x<n; x++) {
    for (rci_t y=0; y<n; y++) {
      mzd_write_bit(R, x,y, (BIT)nmod_mat_get_entry(M,x,y));
    }
  }
}
#endif


//******************************************************************************
// move in/out of flint matrices. flint must be proper shape

/// ----------
/// ## bmat_to_flint_*n*(d,m)
///
/// copies the matrix `m` into the FLINT structure `d`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_to_flint_8 (nmod_mat_t d, bmat_param_8(m))
/// void bmat_to_flint_16(nmod_mat_t d, bmat_param_16(m))
/// void bmat_to_flint_32(nmod_mat_t d, bmat_param_32(m))
/// void bmat_to_flint_64(nmod_mat_t d, bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


void bmat_to_flint_8(nmod_mat_t R, bmat_param_8(m))
{
  const uint32_t D = 8;

  uint8_t s[8];
  bmat_to_array_8(s,m);

  for(uint32_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(uint32_t c=0; c<D; c++) {
      nmod_mat_entry(R,r,c) = (mp_limb_t)(row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_flint_16(nmod_mat_t R, bmat_param_16(m))
{
  const uint32_t D = 16;

  uint16_t s[16];
  bmat_to_array_16(s,m);

  for(uint32_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(uint32_t c=0; c<D; c++) {
      nmod_mat_entry(R,r,c) = (mp_limb_t)(row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_flint_32(nmod_mat_t R, bmat_param_32(m))
{
  const uint32_t D = 32;

  uint32_t s[32];
  bmat_to_array_32(s,m);

  for(uint32_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(uint32_t c=0; c<D; c++) {
      nmod_mat_entry(R,r,c) = (mp_limb_t)(row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_flint_64(nmod_mat_t R, bmat_param_32(m))
{
  const uint32_t D = 64;

  for(uint32_t r=0; r<D; r++) {
    uint64_t row = m[r];
    for(uint32_t c=0; c<D; c++) {
      nmod_mat_entry(R,r,c) = (mp_limb_t)(row & 1);
      row >>= 1;
    }
  }
}


/// ----------
/// ## bmat_from_flint_*n*(d,m)
///
/// copies the matrix `m` into the FLINT structure `d`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_from_flint_8 (bmat_param_8(m)), nmod_mat_t d)
/// void bmat_from_flint_16(bmat_param_16(m), nmod_mat_t d)
/// void bmat_from_flint_32(bmat_param_32(m), nmod_mat_t d)
/// void bmat_from_flint_64(bmat_param_64(m), nmod_mat_t d)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_from_flint_8(bmat_param_8(m), nmod_mat_t M)
{
  const uint32_t D = 8;

  uint8_t d[8];
  
  for (uint32_t r=0; r<D; r++) {
    uint32_t row = 0;

    for (uint32_t c=0; c<D; c++) {
      row ^= (nmod_mat_get_entry(M,r,c) << c);
    }

    d[r] = (uint8_t)row;
  }
  
  array_to_bmat_8(m,d);
}

void bmat_from_flint_16(bmat_param_16(m), nmod_mat_t M)
{
  const uint32_t D = 16;

  uint16_t d[16];
  
  for (uint32_t r=0; r<D; r++) {
    uint32_t row = 0;

    for (uint32_t c=0; c<D; c++) {
      row ^= (nmod_mat_get_entry(M,r,c) << c);
    }

    d[r] = (uint16_t)row;
  }
  
  array_to_bmat_16(m,d);
}

void bmat_from_flint_32(bmat_param_32(m), nmod_mat_t M)
{
  const uint32_t D = 32;

  uint32_t d[32];
  
  for (uint32_t r=0; r<D; r++) {
    uint32_t row = 0;

    for (uint32_t c=0; c<D; c++) {
      row ^= (nmod_mat_get_entry(M,r,c) << c);
    }

    d[r] = row;
  }
  
  array_to_bmat_32(m,d);
}

void bmat_from_flint_64(bmat_param_8(m), nmod_mat_t M)
{
  const uint32_t D = 64;

  for (uint32_t r=0; r<D; r++) {
    uint64_t row = 0;

    for (uint32_t c=0; c<D; c++) {
      row ^= ((uint64_t)nmod_mat_get_entry(M,r,c) << c);
    }

    m[r] = row;
  }
}


//******************************************************************************
// allocate/convert/return helpers

/// ----------
/// ## flint_make_*n*(m)
///
/// allocates a FLINT structure for a $n \times n$ matrix and
/// sets it to `m`.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// nmod_mat_t* flint_make_8 (bmat_param_8 (m))
/// nmod_mat_t* flint_make_16(bmat_param_16(m))
/// nmod_mat_t* flint_make_32(bmat_param_32(m))
/// nmod_mat_t* flint_make_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

nmod_mat_t* bmat_make_flint_8(bmat_param_8(m))
{
  nmod_mat_t* M = flint_gf2m_alloc_8();
  bmat_to_flint_8(M[0],m);
  return M;
}

nmod_mat_t* bmat_make_flint_16(bmat_param_16(m))
{
  nmod_mat_t* M = flint_gf2m_alloc_16();
  bmat_to_flint_16(M[0],m);
  return M;
}

nmod_mat_t* bmat_make_flint_32(bmat_param_32(m))
{
  nmod_mat_t* M = flint_gf2m_alloc_32();
  bmat_to_flint_32(M[0],m);
  return M;
}

nmod_mat_t* bmat_make_flint_64(bmat_param_64(m))
{
  nmod_mat_t* M = flint_gf2m_alloc_64();
  bmat_to_flint_64(M[0],m);
  return M;
}


//******************************************************************************

// looses any bits beyond the first 64 (obviously) so the charpoly of
// a 64x64 will not include the final (implied) term x^64.
// could add a version that fills an array. meh.
uint64_t flint_cp2num(nmod_poly_t p)
{
  slong    e = nmod_poly_length(p);
  uint64_t r = 0;
  uint64_t b = 1;

  // the bit shift automatically handles dropping
  // the x^64 term for 64x64.
  
  for(uint32_t i=0; i<e; i++) {
    r  ^= nmod_poly_get_coeff_ui(p,i) ? b : 0;
    b <<= 1;
  }
  
  return r;
}

uint64_t flint_wrap_charpoly_n(nmod_mat_t* m)
{
  nmod_poly_t* p = flint_gf2p_alloc();
  nmod_mat_charpoly(p[0],m[0]);
  flint_gf2m_free(m);

  uint64_t r = flint_cp2num(p[0]);
  flint_gf2p_free(p);

  return r;
}

// 64-bit will have top (implied) bit chopped
uint64_t flint_wrap_charpoly_8 (bmat_param_8 (m)) { return flint_wrap_charpoly_n(bmat_make_flint_8 (m)); }
uint64_t flint_wrap_charpoly_16(bmat_param_16(m)) { return flint_wrap_charpoly_n(bmat_make_flint_16(m)); }
uint64_t flint_wrap_charpoly_32(bmat_param_32(m)) { return flint_wrap_charpoly_n(bmat_make_flint_32(m)); }
uint64_t flint_wrap_charpoly_64(bmat_param_64(m)) { return flint_wrap_charpoly_n(bmat_make_flint_64(m)); }



/// ----------
/// ## flint_alloc_n(n)
///
/// allocates a FLINT structure for a $n \times n$ matrix.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// nmod_mat_t flint_alloc_n(uint32_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>









