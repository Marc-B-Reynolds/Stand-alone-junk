// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// versions designed for AVX2 (but might be compiled for others via
// SIMDe or similar)

#pragma once

extern void bmat_transpose_16_avx2(bmat_rparam_16(d), bmat_param_16(m));
extern void bmat_transpose_32_avx2(bmat_rparam_32(d), bmat_param_32(s));

// bad me. should be here.
//extern void bmat_transpose_64(bmat_rparam_64(d), bmat_param_64(m));  
