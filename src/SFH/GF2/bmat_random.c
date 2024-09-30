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
/// ## bmat_random_stu_*n*(m,prng)
///
/// Generates a [strictly upper triangular](https://en.wikipedia.org/wiki/Triangular_matrix) matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_random_stu_64(bmat_param_64(m), prng_t* prng)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
///  </details>

void bmat_random_stu_64(bmat_param_64(m),prng_t* prng)
{
  uint64_t t;

  for(uint32_t i=0; i<32; i++)  {
    t       = prng_u64(prng);
    m[i]    = (t >> (i+1) );
    m[63-i] = (t << (63-i));
  }
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


