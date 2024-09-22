// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// internal file for generic 64-bit hardware versions. These
// should use SWAR (except for 64x64..no extra space)

#pragma once

extern void bmat_transpose_8_gen (bmat_rparam_8 (d), bmat_param_8 (m));
extern void bmat_transpose_16_gen(bmat_rparam_16(d), bmat_param_16(m));
extern void bmat_transpose_32_gen(bmat_rparam_32(d), bmat_param_32(m))
extern void bmat_transpose_64_gen(bmat_rparam_64(d), bmat_param_64(m))
