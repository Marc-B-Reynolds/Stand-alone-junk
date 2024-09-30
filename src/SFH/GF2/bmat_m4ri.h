// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <m4ri/m4ri_config.h>
#include <m4ri/m4ri.h>

extern mzd_t* m4ri_alloc_8 (void);
extern mzd_t* m4ri_alloc_16(void);
extern mzd_t* m4ri_alloc_32(void);
extern mzd_t* m4ri_alloc_64(void);

extern void m4ri_free(mzd_t*);

extern void bmat_to_m4ri_8 (mzd_t*, bmat_param_8(m));
extern void bmat_to_m4ri_16(mzd_t*, bmat_param_16(m));
extern void bmat_to_m4ri_32(mzd_t*, bmat_param_32(m));
extern void bmat_to_m4ri_64(mzd_t*, bmat_param_64(m));

extern void bmat_from_m4ri_8 (bmat_param_8(m),  mzd_t*);
extern void bmat_from_m4ri_16(bmat_param_16(m), mzd_t*);
extern void bmat_from_m4ri_32(bmat_param_32(m), mzd_t*);
extern void bmat_from_m4ri_64(bmat_param_64(m), mzd_t*);

extern mzd_t* m4ri_make_8 (bmat_param_8 (s));
extern mzd_t* m4ri_make_16(bmat_param_16(s));
extern mzd_t* m4ri_make_32(bmat_param_32(s));
extern mzd_t* m4ri_make_64(bmat_param_64(s));

extern void m4ri_set_rv_8 (mzd_t* v, uint8_t  x);
extern void m4ri_set_rv_16(mzd_t* v, uint16_t x);
extern void m4ri_set_rv_32(mzd_t* v, uint32_t x);
extern void m4ri_set_rv_64(mzd_t* v, uint64_t x);

extern uint32_t m4ri_get_rv_8 (mzd_t* v);
extern uint32_t m4ri_get_rv_16(mzd_t* v);
extern uint32_t m4ri_get_rv_32(mzd_t* v);
extern uint64_t m4ri_get_rv_64(mzd_t* v);

// heavy weight wrappers
extern void m4ri_wrap_mm_8 (bmat_param_8(C),  bmat_param_8(A),  bmat_param_8(B));
extern void m4ri_wrap_mm_16(bmat_param_16(C), bmat_param_16(A), bmat_param_16(B));
extern void m4ri_wrap_mm_32(bmat_param_32(C), bmat_param_32(A), bmat_param_32(B));
extern void m4ri_wrap_mm_64(bmat_param_64(C), bmat_param_64(A), bmat_param_64(B));
extern void m4ri_wrap_mt_8 (bmat_param_8(C),  bmat_param_8(A),  bmat_param_8(B));
extern void m4ri_wrap_mt_16(bmat_param_16(C), bmat_param_16(A), bmat_param_16(B));
extern void m4ri_wrap_mt_32(bmat_param_32(C), bmat_param_32(A), bmat_param_32(B));
extern void m4ri_wrap_mt_64(bmat_param_64(C), bmat_param_64(A), bmat_param_64(B));

extern uint32_t m4ri_wrap_vm_8 (uint32_t V, bmat_param_8 (M));
extern uint32_t m4ri_wrap_vm_16(uint32_t V, bmat_param_16(M));
extern uint32_t m4ri_wrap_vm_32(uint32_t V, bmat_param_32(M));
extern uint64_t m4ri_wrap_vm_64(uint64_t V, bmat_param_64(M));
extern uint32_t m4ri_wrap_mv_8 (bmat_param_8 (M), uint32_t V);
extern uint32_t m4ri_wrap_mv_16(bmat_param_16(M), uint32_t V);
extern uint32_t m4ri_wrap_mv_32(bmat_param_32(M), uint32_t V);
extern uint64_t m4ri_wrap_mv_64(bmat_param_64(M), uint64_t V);

extern uint32_t m4ri_wrap_rref_8 (bmat_param_8 (m));
extern uint32_t m4ri_wrap_rref_16(bmat_param_16(m));
extern uint32_t m4ri_wrap_rref_32(bmat_param_32(m));
extern uint32_t m4ri_wrap_rref_64(bmat_param_64(m));

extern uint32_t m4ri_wrap_rref2_8 (bmat_param_8 (a),bmat_param_8 (b));
extern uint32_t m4ri_wrap_rref2_16(bmat_param_16(a),bmat_param_16(b));
extern uint32_t m4ri_wrap_rref2_32(bmat_param_32(a),bmat_param_32(b));
extern uint32_t m4ri_wrap_rref2_64(bmat_param_64(a),bmat_param_64(b));

extern bool m4ri_wrap_inverse_8 (bmat_param_8 (m));
extern bool m4ri_wrap_inverse_16(bmat_param_16(m));
extern bool m4ri_wrap_inverse_32(bmat_param_32(m));
extern bool m4ri_wrap_inverse_64(bmat_param_64(m));
