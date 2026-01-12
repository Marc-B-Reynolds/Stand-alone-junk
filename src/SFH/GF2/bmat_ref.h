// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

// internal file for reference versions (self testing). unless I forget the
// reference version should be good on big-endian. Specifically they should
// all be non SWAR and be using on the primitive integer type that corresponds
// to 'n' (e.g. bmat_func_8 works on uint8_t)

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

extern uint8_t  bmat_vmul_8_ref (uint8_t,  bmat_param_8 (m));
extern uint16_t bmat_vmul_16_ref(uint16_t, bmat_param_16(m));
extern uint32_t bmat_vmul_32_ref(uint32_t, bmat_param_32(m));
extern uint64_t bmat_vmul_64_ref(uint64_t, bmat_param_64(m));

extern uint8_t  bmat_mulv_8_ref (bmat_param_8 (m), uint8_t);
extern uint16_t bmat_mulv_16_ref(bmat_param_16(m), uint16_t);
extern uint32_t bmat_mulv_32_ref(bmat_param_32(m), uint32_t);
extern uint64_t bmat_mulv_64_ref(bmat_param_64(m), uint64_t);

extern void bmat_row_lshift_8_ref (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_lshift_16_ref(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_lshift_32_ref(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_lshift_64_ref(bmat_param_64(d), bmat_param_64(m), uint32_t s);

extern void bmat_row_rshift_8_ref (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_rshift_16_ref(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_rshift_32_ref(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_rshift_64_ref(bmat_param_64(d), bmat_param_64(m), uint32_t s);
