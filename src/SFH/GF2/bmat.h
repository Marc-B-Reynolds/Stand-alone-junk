// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <stdint.h>
#include <string.h>

#ifndef  PRNG_SMALL_H
#include "prng_small.h" // temp hack
#endif

// hide your eyes

// number of 64-bit words
#define BMAT_WSIZE_8   1
#define BMAT_WSIZE_16  4
#define BMAT_WSIZE_32 16
#define BMAT_WSIZE_64 64

#define bmat_def_8(X)  uint64_t X[ 1]
#define bmat_def_16(X) uint64_t X[ 4]
#define bmat_def_32(X) uint64_t X[16]
#define bmat_def_64(X) uint64_t X[64]

#if !defined(_MSC_VER)
#define bmat_rparam_n(X)  uint64_t* restrict X

#define bmat_rparam_8(X)  uint64_t X[restrict static  1]
#define bmat_rparam_16(X) uint64_t X[restrict static  4]
#define bmat_rparam_32(X) uint64_t X[restrict static 16]
#define bmat_rparam_64(X) uint64_t X[restrict static 64]

#define bmat_param_8(X)  uint64_t X[static  1]
#define bmat_param_16(X) uint64_t X[static  4]
#define bmat_param_32(X) uint64_t X[static 16]
#define bmat_param_64(X) uint64_t X[static 64]

#define bmat_array_8(X)  uint8_t  X[static  8]
#define bmat_array_16(X) uint16_t X[static 16]
#define bmat_array_32(X) uint32_t X[static 32]
#define bmat_array_64(X) uint64_t X[static 64]

#else
#define bmat_rparam_n(X)  uint64_t* X

#define bmat_rparam_8(X)  uint64_t* X
#define bmat_rparam_16(X) uint64_t* X
#define bmat_rparam_32(X) uint64_t* X
#define bmat_rparam_64(X) uint64_t* X

#define bmat_param_8(X)  uint64_t* X
#define bmat_param_16(X) uint64_t* X
#define bmat_param_32(X) uint64_t* X
#define bmat_param_64(X) uint64_t* X

#define bmat_array_8(X)  uint8_t*  X
#define bmat_array_16(X) uint16_t* X
#define bmat_array_32(X) uint32_t* X
#define bmat_array_64(X) uint64_t* X
#endif

static inline void bmat_dup_8      (bmat_param_8(a),  bmat_param_8(m))  { memcpy(a, m,   8); }
static inline void bmat_dup_16     (bmat_param_16(a), bmat_param_16(m)) { memcpy(a, m,  32); }
static inline void bmat_dup_32     (bmat_param_32(a), bmat_param_32(m)) { memcpy(a, m, 128); }
static inline void bmat_dup_64     (bmat_param_64(a), bmat_param_64(m)) { memcpy(a, m, 512); }

// bmat_to_array_n/array_to_bmat_n :
// * for type-punning between array types (for strict aliasing)
// * helpful for locating where endian issues will occur (see documentation..which I haven't written yet)
static inline void bmat_to_array_8 (bmat_array_8 (a), bmat_param_8 (m)) { memcpy(a, m,   8); }
static inline void bmat_to_array_16(bmat_array_16(a), bmat_param_16(m)) { memcpy(a, m,  32); }
static inline void bmat_to_array_32(bmat_array_32(a), bmat_param_32(m)) { memcpy(a, m, 128); }
static inline void bmat_to_array_64(bmat_array_64(a), bmat_param_64(m)) { memcpy(a, m, 512); }

static inline void array_to_bmat_8 (bmat_param_8 (m), bmat_array_8 (a)) { memcpy(m, a,   8); }
static inline void array_to_bmat_16(bmat_param_16(m), bmat_array_16(a)) { memcpy(m, a,  32); }
static inline void array_to_bmat_32(bmat_param_32(m), bmat_array_32(a)) { memcpy(m, a, 128); }
static inline void array_to_bmat_64(bmat_param_64(m), bmat_array_64(a)) { memcpy(m, a, 512); }

// direct 8x8 as uint64_t
static inline uint64_t bmat_add_8_i  (uint64_t a, uint64_t b) { return (a^b);    }
static inline bool     bmat_equal_8_i(uint64_t a, uint64_t b) { return (a^b)==0; }

// bmat_basics.c
extern bool bmat_equal_8 (bmat_param_8 (a), bmat_param_8 (b));
extern bool bmat_equal_16(bmat_param_16(a), bmat_param_16(b));
extern bool bmat_equal_32(bmat_param_32(a), bmat_param_32(b));
extern bool bmat_equal_64(bmat_param_64(a), bmat_param_64(b));

extern bool bmat_is_zero_8 (bmat_param_8(m));
extern bool bmat_is_zero_16(bmat_param_16(m));
extern bool bmat_is_zero_32(bmat_param_32(m));
extern bool bmat_is_zero_64(bmat_param_64(m));

extern void bmat_add_8 (bmat_rparam_8 (d), bmat_param_8 (a), bmat_param_8 (b));
extern void bmat_add_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b));
extern void bmat_add_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b));
extern void bmat_add_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b));

extern void bmat_sum_8 (bmat_param_8 (d), bmat_param_8 (a));
extern void bmat_sum_16(bmat_param_16(d), bmat_param_16(a));
extern void bmat_sum_32(bmat_param_32(d), bmat_param_32(a));
extern void bmat_sum_64(bmat_param_64(d), bmat_param_64(a));

extern uint32_t bmat_md_sum_8 (bmat_param_8 (m));
extern uint32_t bmat_md_sum_16(bmat_param_16(m));
extern uint32_t bmat_md_sum_32(bmat_param_32(m));

extern uint32_t bmat_trace_8 (bmat_param_8 (m));
extern uint32_t bmat_trace_16(bmat_param_16(m));
extern uint32_t bmat_trace_32(bmat_param_32(m));

extern void bmat_flip_h_8 (bmat_param_8 (d), bmat_param_8 (m));
extern void bmat_flip_h_16(bmat_param_16(d), bmat_param_16(m));
extern void bmat_flip_h_32(bmat_param_32(d), bmat_param_32(m));
extern void bmat_flip_h_64(bmat_param_64(d), bmat_param_64(m));

extern void bmat_flip_v_8 (bmat_param_8 (d), bmat_param_8 (m));
extern void bmat_flip_v_16(bmat_param_16(d), bmat_param_16(m));
extern void bmat_flip_v_32(bmat_param_32(d), bmat_param_32(m));
extern void bmat_flip_v_64(bmat_param_64(d), bmat_param_64(m));

// bmat_set.c
extern void bmat_zero_8 (bmat_param_8 (m));
extern void bmat_zero_16(bmat_param_16(m));
extern void bmat_zero_32(bmat_param_32(m));
extern void bmat_zero_64(bmat_param_64(m));

extern void bmat_set_unit_8 (bmat_param_8 (d));
extern void bmat_set_unit_16(bmat_param_16(d));
extern void bmat_set_unit_32(bmat_param_32(d));
extern void bmat_set_unit_64(bmat_param_64(d));

extern void bmat_set_exchange_8 (bmat_param_8 (m));
extern void bmat_set_exchange_16(bmat_param_16(m));
extern void bmat_set_exchange_32(bmat_param_32(m));
extern void bmat_set_exchange_64(bmat_param_64(m));

// bmat_mul.c
extern void bmat_mul_8 (bmat_rparam_8 (c),bmat_param_8 (a),bmat_param_8 (b));
extern void bmat_mul_16(bmat_rparam_16(c),bmat_param_16(a),bmat_param_16(b));
extern void bmat_mul_32(bmat_rparam_32(c),bmat_param_32(a),bmat_param_32(b));
extern void bmat_mul_64(bmat_rparam_64(c),bmat_param_64(a),bmat_param_64(b));

extern void bmat_mult_8 (bmat_rparam_8 (c),bmat_param_8 (a),bmat_param_8 (b));
extern void bmat_mult_16(bmat_rparam_16(c),bmat_param_16(a),bmat_param_16(b));
extern void bmat_mult_32(bmat_rparam_32(c),bmat_param_32(a),bmat_param_32(b));
extern void bmat_mult_64(bmat_rparam_64(c),bmat_param_64(a),bmat_param_64(b));

extern uint8_t  bmat_vmul_8 (uint8_t  v, bmat_param_8 (m));
extern uint16_t bmat_vmul_16(uint16_t v, bmat_param_16(m));
extern uint32_t bmat_vmul_32(uint32_t v, bmat_param_32(m));
extern uint64_t bmat_vmul_64(uint64_t v, bmat_param_64(m));

extern uint8_t  bmat_mulv_8 (bmat_param_8 (m), uint8_t  v);
extern uint16_t bmat_mulv_16(bmat_param_16(m), uint16_t v);
extern uint32_t bmat_mulv_32(bmat_param_32(m), uint32_t v);
extern uint64_t bmat_mulv_64(bmat_param_64(m), uint64_t v);


// bmat_gauss.c
extern uint32_t bmat_rank_8 (bmat_param_8 (m));
extern uint32_t bmat_rank_16(bmat_param_16(m));
extern uint32_t bmat_rank_32(bmat_param_32(m));
extern uint32_t bmat_rank_64(bmat_param_64(m));

extern bool bmat_is_full_rank_8 (bmat_param_8 (m));
extern bool bmat_is_full_rank_16(bmat_param_16(m));
extern bool bmat_is_full_rank_32(bmat_param_32(m));
extern bool bmat_is_full_rank_64(bmat_param_64(m));

extern void bmat_transpose_8 (bmat_rparam_8 (a), bmat_param_8 (b));
extern void bmat_transpose_16(bmat_rparam_16(a), bmat_param_16(b));
extern void bmat_transpose_32(bmat_rparam_32(a), bmat_param_32(b));
extern void bmat_transpose_64(bmat_rparam_64(a), bmat_param_64(b));

// bmat_random.c
extern void bmat_random_8 (bmat_param_8 (m), prng_t*);
extern void bmat_random_16(bmat_param_16(m), prng_t*);
extern void bmat_random_32(bmat_param_32(m), prng_t*);
extern void bmat_random_64(bmat_param_64(m), prng_t*);

extern void bmat_block_xform_16(bmat_param_16(d), bmat_param_16(s));

//
extern void bmat_print_8 (char* prefix, bmat_param_8 (m));
extern void bmat_print_16(char* prefix, bmat_param_16(m));
extern void bmat_print_32(char* prefix, bmat_param_32(m));
extern void bmat_print_64(char* prefix, bmat_param_64(m));

extern uint32_t f2_ufunc_validate_32_i   (uint32_t (*)(uint32_t), uint32_t (*)(uint32_t));
extern uint64_t f2_ufunc_validate_64_i   (uint64_t (*)(uint64_t), uint64_t (*)(uint64_t));
extern uint32_t f2_ufunc_p_validate_32_i (uint32_t (*)(uint32_t), uint32_t (*)(uint32_t,uint32_t), uint32_t);
extern uint32_t f2_ufunc_p_validate_64_i (uint64_t (*)(uint64_t), uint64_t (*)(uint64_t,uint64_t), uint64_t);
extern uint32_t f2_ufunc_p2_validate_32_i(uint32_t (*)(uint32_t,uint32_t), uint32_t (*)(uint32_t, uint32_t), uint32_t);
extern uint64_t f2_ufunc_p2_validate_64_i(uint64_t (*)(uint64_t,uint64_t), uint64_t (*)(uint64_t, uint64_t), uint64_t);

// F_2 unary & binary function types (kill these)
typedef uint32_t (f2_ufunc_32_t)(uint32_t);
typedef uint64_t (f2_ufunc_64_t)(uint64_t);
typedef uint32_t (f2_bfunc_32_t)(uint32_t, uint32_t);
typedef uint64_t (f2_bfunc_64_t)(uint64_t, uint64_t);

extern uint32_t bmat_from_ufunc_32  (bmat_param_32(m), f2_ufunc_32_t* f);
extern uint64_t bmat_from_ufunc_64  (bmat_param_64(m), f2_ufunc_64_t* f);
extern uint32_t bmat_from_ufunc_p_32(bmat_param_32(m), f2_bfunc_32_t* f, uint32_t k);
extern uint64_t bmat_from_ufunc_p_64(bmat_param_64(m), f2_bfunc_64_t* f, uint64_t k);
