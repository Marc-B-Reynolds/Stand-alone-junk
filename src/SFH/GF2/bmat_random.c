// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

// TODO: The PRNG (probably) should be pulled into here instead of being a single header file include.


/// Random matrices
///==============================================================
///


// TODO:
// * need version w/o scatter/gather (or bit_group) hardware support
// * think about full-rank generation

// n = number of 64-bit chunks
static inline void bmat_random_n(uint64_t* m, prng_t* rng, uint32_t n)
{
  for(uint32_t i=0; i<n; i++) m[i] = prng_u64(rng);
}

/// ## bmat_random_*n*(m, prng)
///
/// Sets each element of $m$ to $(0,1)$ with equal probability. The number of invertiable elements of $ n\times n $ :
///
/// $$
/// \text{GL}_n\left(\mathbb{F}_2\right) = \prod\limits_{i=0}^{n-1} \left(2^n-2^i\right)
/// $$
/// 
/// where the left hand side is the [hoity toity](https://en.wikipedia.org/wiki/General_linear_group#Over_finite_fields) way to call that number.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// bmat_random_8 (bmat_param_8 (m), prng_t*)
/// bmat_random_16(bmat_param_16(m), prng_t*)
/// bmat_random_32(bmat_param_32(m), prng_t*)
/// bmat_random_64(bmat_param_64(m), prng_t*)
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_random_8 (bmat_param_8 (m), prng_t* prng) { bmat_random_n(m,prng, 1); }
void bmat_random_16(bmat_param_16(m), prng_t* prng) { bmat_random_n(m,prng, 4); }
void bmat_random_32(bmat_param_32(m), prng_t* prng) { bmat_random_n(m,prng,16); }
void bmat_random_64(bmat_param_64(m), prng_t* prng) { bmat_random_n(m,prng,64); }


/// ----------
///
/// ## bmat_random_sut_*n*(m,prng)
///
/// Generates a random [strictly upper triangular](https://en.wikipedia.org/wiki/Triangular_matrix#Special_forms) matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_random_sut_8 (bmat_param_8 (m), prng_t* prng)
/// void bmat_random_sut_16(bmat_param_16(m), prng_t* prng)
/// void bmat_random_sut_32(bmat_param_32(m), prng_t* prng)
/// void bmat_random_sut_64(bmat_param_64(m), prng_t* prng)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
///  </details>

void bmat_random_sut_8(bmat_param_8(m), prng_t* prng)
{
  uint64_t mask = UINT64_C(0x0080c0e0f0f8fcfe);
  uint64_t t    = prng_u64(prng);
  m[0] = t & mask;
}

void bmat_random_sut_16(bmat_param_16(m), prng_t* prng)
{
  // can be done in 3 random draws but the extra work
  // doesn't make it worthwhile.
  m[0] = prng_u64(prng) & UINT64_C(0xfff0fff8fffcfffe);
  m[1] = prng_u64(prng) & UINT64_C(0xff00ff80ffc0ffe0);
  m[2] = prng_u64(prng) & UINT64_C(0xf000f800fc00fe00);
  m[3] = prng_u64(prng) & UINT64_C(0x00008000c000e000);
}

void bmat_random_sut_32(bmat_param_32(m), prng_t* prng)
{
  uint64_t t;
  uint64_t mask = UINT64_C(0xfffffffcfffffffe);

  for(uint32_t i=0; i<8; i++)  {
    t       = prng_u64(prng);
    m[i]    = (t &  mask);
    m[15-i] = (bit_reverse_64(t & ~mask) << 1) & mask;
    mask    = (mask << 2) & UINT64_C(0xfffffffcfffffffc);
  }
}

void bmat_random_sut_64(bmat_param_64(m), prng_t* prng)
{
  uint64_t t;
  uint64_t mask = UINT64_C(~0)^1;

  for(uint32_t i=0; i<31; i++)  {
    t       = prng_u64(prng);
    m[i]    = t & mask;
    m[62-i] = t << (63-i);
    mask  <<= 1;
  }
  
  m[31] = prng_u64(prng) & mask;
  m[63] = 0;
}



/// ----------
///
/// ## bmat_random_uut_*n*(m,prng)
///
/// Generates a random [upper unitriangular](https://en.wikipedia.org/wiki/Triangular_matrix#Special_forms) matrix (the main diagonal is all 1s).
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_random_sut_8 (bmat_param_8 (m), prng_t* prng)
/// void bmat_random_sut_16(bmat_param_16(m), prng_t* prng)
/// void bmat_random_sut_32(bmat_param_32(m), prng_t* prng)
/// void bmat_random_sut_64(bmat_param_64(m), prng_t* prng)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
///  </details>

void bmat_random_uut_8(bmat_param_8(m),prng_t* prng)
{
  uint64_t foo = UINT64_C(0x8040201008040201);
  bmat_random_sut_8(m,prng);
  m[0] ^= foo;
}

void bmat_random_uut_16(bmat_param_16(m),prng_t* prng)
{
  bmat_random_sut_16(m,prng);
  bmat_add_unit_16(m,m);
}

void bmat_random_uut_32(bmat_param_32(m),prng_t* prng)
{
  bmat_random_sut_32(m,prng);
  bmat_add_unit_32(m,m);
}

void bmat_random_uut_64(bmat_param_64(m),prng_t* prng)
{
  bmat_random_sut_64(m,prng);
  bmat_add_unit_64(m,m);
}





/// ----------
///
/// ## bmat_random_fr_*n*(m,prng)
///
/// Generates a full rank random matrix. The current implementation is a rejection method which
/// requires slightly less that 3.5 iterations to complete. So it's slow (but uniform!). There
/// are transform methods et. al that could replace or supplement.
//
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_random_fr_8 (bmat_param_8 (m), prng_t*)
/// void bmat_random_fr_16(bmat_param_16(m), prng_t*)
/// void bmat_random_fr_32(bmat_param_32(m), prng_t*)
/// void bmat_random_fr_64(bmat_param_64(m), prng_t*)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
///  </details>

// set matrix to a random full rank
// naive loop versons (on average) requires slightly less that 3.5 iterations to complete. it's a sadface.
// with an optimized rank computation could build a better version. there's also transform methods. (add note)
void bmat_random_fr_8 (bmat_param_8 (m), prng_t* prng) { do { bmat_random_8 (m,prng); } while(bmat_rank_8 (m) !=  8); }
void bmat_random_fr_16(bmat_param_16(m), prng_t* prng) { do { bmat_random_16(m,prng); } while(bmat_rank_16(m) != 16); }
void bmat_random_fr_32(bmat_param_32(m), prng_t* prng) { do { bmat_random_32(m,prng); } while(bmat_rank_32(m) != 32); }
void bmat_random_fr_64(bmat_param_64(m), prng_t* prng) { do { bmat_random_64(m,prng); } while(bmat_rank_64(m) != 64); }


