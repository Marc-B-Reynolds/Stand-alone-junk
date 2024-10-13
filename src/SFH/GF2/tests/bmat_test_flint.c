// Marc B. Reynolds, 2017-2024
// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <stdbool.h>

#define  BMAT_FLINT_ENABLE
#include "../bmat_everything.h"
#include "bmat_test.h"


//******************************************************************************
// just hardcoded test..meh

uint32_t test_roundtrip(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;
  
  bmat_def_64(m0);
  bmat_def_64(m1);

  test_banner("matrix round-trip");

  nmod_mat_t* fm8  = flint_gf2m_alloc_8();
  nmod_mat_t* fm16 = flint_gf2m_alloc_16();
  nmod_mat_t* fm32 = flint_gf2m_alloc_32();
  nmod_mat_t* fm64 = flint_gf2m_alloc_64();

  {
    test_header_8();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_8(m0, prng);
      bmat_to_flint_8(fm8[0],m0);
      bmat_from_flint_8(m1,fm8[0]);
      if (bmat_equal_8(m0,m1)) continue;
      test_fail();
      errors++;
      goto test_16;
    }
    test_pass();
  }
  
 test_16: {
    test_header_16();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_16(m0, prng);
      bmat_to_flint_16(fm16[0],m0);
      bmat_from_flint_16(m1,fm16[0]);
      if (bmat_equal_16(m0,m1)) continue;
      test_fail();
      errors++;
      goto test_32;
    }
    test_pass();
  }

 test_32: {
    test_header_32();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_32(m0, prng);
      bmat_to_flint_32(fm32[0],m0);
      bmat_from_flint_32(m1,fm32[0]);
      if (bmat_equal_32(m0,m1)) continue;
      test_fail();
      errors++;
      goto test_64;
    }
    test_pass();
  }

 test_64: {
    test_header_64();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_64(m0, prng);
      bmat_to_flint_64(fm64[0],m0);
      bmat_from_flint_64(m1,fm64[0]);
      if (bmat_equal_64(m0,m1)) continue;
      test_fail();
      errors++;
      goto test_done;
    }
    test_pass();
  }
  
 test_done:
  flint_gf2m_free(fm8);
  flint_gf2m_free(fm16);
  flint_gf2m_free(fm32);
  flint_gf2m_free(fm64);

  return errors;
}

//******************************************************************************

uint32_t test_charpoly(prng_t* prng, uint32_t trials)
{
  uint32_t errors = 0;

  bmat_def_64(m);

  test_banner("bmat_charpoly_n");

  {
    test_header_8();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_8(m, prng);

      uint64_t r0 = flint_wrap_charpoly_8 (m);
      uint64_t r1 = bmat_charpoly_8(m);
      
      if (r0 == r1) continue;

      printf("%lx %lx : %u\n", r0,r1,i);
      test_fail();
      errors++;
      goto test_16;
    }
    test_pass();
  }

 test_16:
  {
    test_header_16();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_16(m, prng);
      
      uint64_t r0 = flint_wrap_charpoly_16 (m);
      uint64_t r1 = bmat_charpoly_16(m);
      
      if (r0 == r1) continue;
      test_fail();
      errors++;
      goto test_32;
    }
    test_pass();
  }

 test_32:
  {
    test_header_32();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_32(m, prng);
      
      uint64_t r0 = flint_wrap_charpoly_32 (m);
      uint64_t r1 = bmat_charpoly_32(m);
      
      if (r0 == r1) continue;
      test_fail();
      errors++;
      goto test_64;
    }
    test_pass();
  }

 test_64:
  {
    test_header_64();
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_64(m, prng);
      
      uint64_t r0 = flint_wrap_charpoly_64 (m);
      uint64_t r1 = bmat_charpoly_64(m);
      
      if (r0 == r1) continue;
      test_fail();
      errors++;
      goto test_done;
    }
    test_pass();
  }

 test_done:
  return errors;
}


//******************************************************************************



uint32_t test_flint(prng_t* prng, uint32_t trials)
{
  printf(HEADER "FLINT TESTING:\n" ENDC);

  uint32_t errors = test_roundtrip(prng,trials);

  if (!errors) {
    errors += test_charpoly(prng,trials);
  }

  return errors;
}


//******************************************************************************
// start of hack of berkowitz

void
_nmod_mat_charpoly_berkowitz_x(mp_ptr cp, const nmod_mat_t mat, nmod_t mod)
{
  const slong n = mat->r;

  if (mod.n == 1)  {
    printf("here mod.n\n");
    _nmod_vec_zero(cp, n + 1);
  }
  else if (n == 0)  {
  printf("here 0\n");
    cp[0] = 1;
  }
  else if (n == 1)  {
  printf("here 1\n");
    cp[0] = nmod_neg(nmod_mat_entry(mat, 0, 0), mod);
    cp[1] = 1;
  }
  else if (n == 2)  {
    printf("here 2\n");
    cp[0] = nmod_sub(nmod_mul(nmod_mat_entry(mat, 0, 0), nmod_mat_entry(mat, 1, 1), mod),
		     nmod_mul(nmod_mat_entry(mat, 0, 1), nmod_mat_entry(mat, 1, 0), mod), mod);
    cp[1] = nmod_add(nmod_mat_entry(mat, 0, 0), nmod_mat_entry(mat, 1, 1), mod);
    cp[1] = nmod_neg(cp[1], mod);
    cp[2] = 1;
  }
  else {
    slong i, k, t;
    mp_ptr a, A, s;
    int nlimbs;
    //TMP_INIT;
    //TMP_START;

    static int foo = 0;

    if (foo != n) {
    }

#if 0
    a = TMP_ALLOC(sizeof(mp_limb_t) * (unsigned long)(n*n));
    A = a + (n-1)*n;
#else
    mp_limb_t data1[n*n];
    mp_limb_t data2[n];
    
    a = data1;
  //A = a + (n-1)*n;
    A = data2;
  #endif    
    
    nlimbs = _nmod_vec_dot_bound_limbs(n, mod);
    
    _nmod_vec_zero(cp, n+1);
    
  //cp[0] = nmod_neg(nmod_mat_entry(mat, 0, 0), mod);
    cp[0] = nmod_mat_entry(mat, 0, 0); // mbr -x = x
    
    for (t=1; t<n; t++) {
      for (i=0; i<=t; i++) {
	a[i] = nmod_mat_entry(mat, i, t);
      }
      
      A[0] = nmod_mat_entry(mat, t, t);
      
      for (k=1; k<t; k++) {
	for (i=0; i <= t; i++) {
	  s    = a + k * n + i;
	  s[0] = _nmod_vec_dot(mat->rows[i], a + (k - 1) * n, t + 1, mod, nlimbs);
	}
	
	A[k] = a[k*n + t];
      }
      
      A[t] = _nmod_vec_dot(mat->rows[t], a + (t - 1) * n, t + 1, mod, nlimbs);

      for (k = 0; k <= t; k++)  {
 	cp[k] ^= A[k] ^ _nmod_vec_dot_rev(A, cp, k, mod, nlimbs);
      }
    }
    
    /* Shift all coefficients up by one */
    for (i = n; i > 0; i--)
      cp[i] = cp[i-1];
    
    cp[0] = 1;
    _nmod_poly_reverse(cp, cp, n+1, n+1);
    
    //TMP_END;
  }
}

void nmod_mat_charpoly_berkowitz_x(nmod_poly_t cp, const nmod_mat_t mat)
{
  nmod_poly_fit_length(cp, mat->r + 1);
  _nmod_poly_set_length(cp, mat->r + 1);
  _nmod_mat_charpoly_berkowitz_x(cp->coeffs, mat, mat->mod);
}

//******************************************************************************

uint64_t flint_wrap_charpoly_n_(nmod_mat_t* m)
{
  nmod_poly_t* p = flint_gf2p_alloc();

  nmod_mat_charpoly_berkowitz_x(p[0],m[0]);

  flint_gf2m_free(m);

  uint64_t r = flint_cp2num(p[0]);
  flint_gf2p_free(p);

  return r;
}

// 64-bit will have top (implied) bit chopped
uint64_t flint_wrap_charpoly_8_ (bmat_param_8 (m)) { return flint_wrap_charpoly_n_(bmat_make_flint_8 (m)); }
uint64_t flint_wrap_charpoly_16_(bmat_param_16(m)) { return flint_wrap_charpoly_n_(bmat_make_flint_16(m)); }
uint64_t flint_wrap_charpoly_32_(bmat_param_32(m)) { return flint_wrap_charpoly_n_(bmat_make_flint_32(m)); }
uint64_t flint_wrap_charpoly_64_(bmat_param_64(m)) { return flint_wrap_charpoly_n_(bmat_make_flint_64(m)); }


//******************************************************************************


void baz(void)
{
}


int main(void)
{
  prng_t prng;

  prng.state[0] = __rdtsc();
  prng.state[1] = UINT64_C(0x3ba0d900b9aaf028);
  prng.state[2] = UINT64_C(0xab7837b9aa423d86);
  prng_u64(&prng);

  bmat_def_64(m);

  test_flint(&prng,0xffff); return 0;
  
  return 0;
}
