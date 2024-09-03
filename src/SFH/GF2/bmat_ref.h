// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// internal file for reference versions (self testing). unless I forget the
// reference version should be good on big-endian.

#pragma once

extern void bmat_mul_8_ref (uint8_t  C[static  8], uint8_t  A[static  8], uint8_t  B[static  8])
extern void bmat_mul_16_ref(uint16_t C[static 16], uint16_t A[static 16], uint64_t B[static 16])
extern void bmat_mul_32_ref(uint32_t C[static 32], uint32_t A[static 32], uint64_t B[static 32])
extern void bmat_mul_64_ref(uint64_t C[static 64], uint64_t A[static 64], uint64_t B[static 64])

extern void bmat_transpose_8_ref (uint8_t  d[static  8], uint8_t  s[static  8]);
extern void bmat_transpose_16_ref(uint16_t d[static 16], uint16_t s[static 16]);
extern void bmat_transpose_32_ref(uint32_t d[static 32], uint32_t s[static 32]);
extern void bmat_transpose_64_ref(uint64_t d[static 64], uint64_t s[static 64]);

extern void bmat_rot_cw_8_ref  (uint8_t  d[static  8], uint8_t  s[static  8]);
extern void bmat_rot_cw_16_ref (uint16_t d[static 16], uint16_t s[static 16]);
extern void bmat_rot_cw_32_ref (uint32_t d[static 32], uint32_t s[static 32]);
extern void bmat_rot_cw_64_ref (uint64_t d[static 64], uint64_t s[static 64]);
extern void bmat_rot_ccw_8_ref (uint8_t  d[static  8], uint8_t  s[static  8]);
extern void bmat_rot_ccw_16_ref(uint16_t d[static 16], uint16_t s[static 16]);
extern void bmat_rot_ccw_32_ref(uint32_t d[static 32], uint32_t s[static 32]);
extern void bmat_rot_ccw_64_ref(uint64_t d[static 64], uint64_t s[static 64]);
