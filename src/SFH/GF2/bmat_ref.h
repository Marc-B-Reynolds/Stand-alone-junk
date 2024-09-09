// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// internal file for reference versions (self testing). unless I forget the
// reference version should be good on big-endian.

#pragma once

extern void bmat_set_unit_8_ref (bmat_param_8 (m));
extern void bmat_set_unit_16_ref(bmat_param_16(m));
extern void bmat_set_unit_32_ref(bmat_param_32(m));
extern void bmat_set_unit_64_ref(bmat_param_64(m));

extern void bmat_set_exchange_8_ref (bmat_param_8 (m));
extern void bmat_set_exchange_16_ref(bmat_param_16(m));
extern void bmat_set_exchange_32_ref(bmat_param_32(m));
extern void bmat_set_exchange_64_ref(bmat_param_64(m));

extern uint32_t bmat_rank_8_ref (bmat_param_8 (m));
extern uint32_t bmat_rank_16_ref(bmat_param_16(m));
extern uint32_t bmat_rank_32_ref(bmat_param_32(m));
extern uint32_t bmat_rank_64_ref(bmat_param_64(m));

extern void bmat_transpose_8_ref (bmat_param_8 (a), bmat_param_8 (s));
extern void bmat_transpose_16_ref(bmat_param_16(a), bmat_param_16(s));
extern void bmat_transpose_32_ref(bmat_param_32(a), bmat_param_32(s));
extern void bmat_transpose_64_ref(bmat_param_64(a), bmat_param_64(s));

extern void bmat_mul_8_ref (bmat_param_8 (r), bmat_param_8 (a), bmat_param_8 (b));
extern void bmat_mul_16_ref(bmat_param_16(r), bmat_param_16(a), bmat_param_16(b));
extern void bmat_mul_32_ref(bmat_param_32(r), bmat_param_32(a), bmat_param_32(b));
extern void bmat_mul_64_ref(bmat_param_64(r), bmat_param_64(a), bmat_param_64(b));

extern void bmat_mult_8_ref (bmat_param_8 (c), bmat_param_8 (a), bmat_param_8 (b));
extern void bmat_mult_16_ref(bmat_param_16(c), bmat_param_16(a), bmat_param_16(b));
extern void bmat_mult_32_ref(bmat_param_32(c), bmat_param_32(a), bmat_param_32(b));
extern void bmat_mult_64_ref(bmat_param_64(c), bmat_param_64(a), bmat_param_64(b));

#if 0
extern void bmat_rot_cw_8_ref  (bmat_param_8 (d), bmat_param_8 (s));
extern void bmat_rot_cw_16_ref (bmat_param_16(d), bmat_param_16(s));
extern void bmat_rot_cw_32_ref (bmat_param_32(d), bmat_param_32(s));
extern void bmat_rot_cw_64_ref (bmat_param_64(d), bmat_param_64(s));
extern void bmat_rot_ccw_8_ref (bmat_param_8 (d), bmat_param_8 (s));
extern void bmat_rot_ccw_16_ref(bmat_param_16(d), bmat_param_16(s));
extern void bmat_rot_ccw_32_ref(bmat_param_32(d), bmat_param_32(s));
extern void bmat_rot_ccw_64_ref(bmat_param_64(d), bmat_param_64(s));
#endif
