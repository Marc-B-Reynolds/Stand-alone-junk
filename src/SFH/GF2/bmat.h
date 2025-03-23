// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <stdint.h>
#include <string.h>

#ifndef  PRNG_SMALL_H
#include "prng_small.h" // temp hack
#endif

// NOTES:
// * a number of functions purposefully drop 'restrict' that is used
//   at the implementation from the forward declarations. This is because
//   identical inputs are fine for the routine but the compiler needs
//   to know that there's no partial overlaps. An example is matrix
//   addition.

// hide your eyes

// number of 64-bit words
#define BMAT_WSIZE_8   1
#define BMAT_WSIZE_16  4
#define BMAT_WSIZE_32 16
#define BMAT_WSIZE_64 64

#if !defined(_MSC_VER)
#define bmat_rparam_n(X)  uint64_t* restrict X

#define BMAT_ALIGN   __attribute__((aligned(32)))
#define BMAT_FLATTEN __attribute__((flatten))

// define paramater macros. mainly to handle visual C not
// supporting array[static n] syntax.
#define bmat_rparam_8(X)  uint64_t X[restrict static  1]
#define bmat_rparam_16(X) uint64_t X[restrict static  4]
#define bmat_rparam_32(X) uint64_t X[restrict static 16]
#define bmat_rparam_64(X) uint64_t X[restrict static 64]

#define bmat_param_8(X)  uint64_t X[static  1]
#define bmat_param_16(X) uint64_t X[static  4]
#define bmat_param_32(X) uint64_t X[static 16]
#define bmat_param_64(X) uint64_t X[static 64]

// these are poorly named and should be changed. For
// reference versions look at the data as "natural"
// width integers.
#define bmat_array_8(X)  uint8_t  X[static  8]
#define bmat_array_16(X) uint16_t X[static 16]
#define bmat_array_32(X) uint32_t X[static 32]
#define bmat_array_64(X) uint64_t X[static 64]

#else

#define BMAT_ALIGN  __declspec(align(32))
#define BMAT_FLATTEN

#define bmat_rparam_n(X)  uint64_t* __restrict X

#define bmat_rparam_8(X)  uint64_t* __restrict X
#define bmat_rparam_16(X) uint64_t* __restrict X
#define bmat_rparam_32(X) uint64_t* __restrict X
#define bmat_rparam_64(X) uint64_t* __restrict X

#define bmat_param_8(X)  uint64_t* X
#define bmat_param_16(X) uint64_t* X
#define bmat_param_32(X) uint64_t* X
#define bmat_param_64(X) uint64_t* X

#define bmat_array_8(X)  uint8_t*  X
#define bmat_array_16(X) uint16_t* X
#define bmat_array_32(X) uint32_t* X
#define bmat_array_64(X) uint64_t* X
#endif


// -Wpedantic enables this
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

// C macro crime (spiting on sidewalk) in progress: 
//   bmat_def_n(name)   -> variable 'name' with space for 1 matrix
//   bmat_def_n(name,N) -> variable 'name' with space for N matrices
#define bmat_def_n(NAME,W,...)  BMAT_ALIGN uint64_t NAME[W __VA_OPT__(* __VA_ARGS__)]
#define bmat_def_8(NAME,...)    bmat_def_n(NAME, 1,__VA_ARGS__)
#define bmat_def_16(NAME,...)   bmat_def_n(NAME, 4,__VA_ARGS__)
#define bmat_def_32(NAME,...)   bmat_def_n(NAME,16,__VA_ARGS__)
#define bmat_def_64(NAME,...)   bmat_def_n(NAME,64,__VA_ARGS__)

static inline void bmat_dup_8 (bmat_param_8(a),  bmat_param_8(m))  { memcpy(a, m,   8); }
static inline void bmat_dup_16(bmat_param_16(a), bmat_param_16(m)) { memcpy(a, m,  32); }
static inline void bmat_dup_32(bmat_param_32(a), bmat_param_32(m)) { memcpy(a, m, 128); }
static inline void bmat_dup_64(bmat_param_64(a), bmat_param_64(m)) { memcpy(a, m, 512); }

#define bmat_defdup_8(M,A)     bmat_def_n(M, 1); bmat_dup_8(M,A);
#define bmat_defdup_16(M,A)    bmat_def_n(M, 4); bmat_dup_16(M,A);
#define bmat_defdup_32(M,A)    bmat_def_n(M,16); bmat_dup_32(M,A);
#define bmat_defdup_64(M,A)    bmat_def_n(M,64); bmat_dup_64(M,A);

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

extern void bmat_sum_8 (bmat_rparam_8 (d), bmat_param_8 (a));
extern void bmat_sum_16(bmat_rparam_16(d), bmat_param_16(a));
extern void bmat_sum_32(bmat_rparam_32(d), bmat_param_32(a));
extern void bmat_sum_64(bmat_rparam_64(d), bmat_param_64(a));

extern void bmat_add_8 (bmat_param_8 (d), bmat_param_8 (a), bmat_param_8 (b));
extern void bmat_add_16(bmat_param_16(d), bmat_param_16(a), bmat_param_16(b));
extern void bmat_add_32(bmat_param_32(d), bmat_param_32(a), bmat_param_32(b));
extern void bmat_add_64(bmat_param_64(d), bmat_param_64(a), bmat_param_64(b));

extern void bmat_add_unit_8 (bmat_param_8 (d), bmat_param_8 (a));
extern void bmat_add_unit_16(bmat_param_16(d), bmat_param_16(a));
extern void bmat_add_unit_32(bmat_param_32(d), bmat_param_32(a));
extern void bmat_add_unit_64(bmat_param_64(d), bmat_param_64(a));

extern uint32_t bmat_md_sum_8 (bmat_param_8 (m));
extern uint32_t bmat_md_sum_16(bmat_param_16(m));
extern uint32_t bmat_md_sum_32(bmat_param_32(m));

extern uint32_t bmat_trace_8 (bmat_param_8 (m));
extern uint32_t bmat_trace_16(bmat_param_16(m));
extern uint32_t bmat_trace_32(bmat_param_32(m));

extern uint64_t bmat_get_row_8 (bmat_param_8 (m), uint32_t n);
extern uint64_t bmat_get_row_16(bmat_param_16(m), uint32_t n);
extern uint64_t bmat_get_row_32(bmat_param_32(m), uint32_t n);
extern uint64_t bmat_get_row_64(bmat_param_64(m), uint32_t n);

extern void bmat_set_row_8 (bmat_param_8 (m), uint32_t r, uint64_t v);
extern void bmat_set_row_16(bmat_param_16(m), uint32_t r, uint64_t v);
extern void bmat_set_row_32(bmat_param_32(m), uint32_t r, uint64_t v);
extern void bmat_set_row_64(bmat_param_64(m), uint32_t r, uint64_t v);

extern void bmat_row_lshift_8 (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_lshift_16(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_lshift_32(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_lshift_64(bmat_param_64(d), bmat_param_64(m), uint32_t s);

extern void bmat_row_rshift_8 (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_rshift_16(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_rshift_32(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_rshift_64(bmat_param_64(d), bmat_param_64(m), uint32_t s);

extern void bmat_row_ushift_8 (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_ushift_16(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_ushift_32(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_ushift_64(bmat_param_64(d), bmat_param_64(m), uint32_t s);

extern void bmat_row_dshift_8 (bmat_param_8 (d), bmat_param_8 (m), uint32_t s);
extern void bmat_row_dshift_16(bmat_param_16(d), bmat_param_16(m), uint32_t s);
extern void bmat_row_dshift_32(bmat_param_32(d), bmat_param_32(m), uint32_t s);
extern void bmat_row_dshift_64(bmat_param_64(d), bmat_param_64(m), uint32_t s);

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

extern void bmat_set_ones_8 (bmat_param_8 (d));
extern void bmat_set_ones_16(bmat_param_16(d));
extern void bmat_set_ones_32(bmat_param_32(d));
extern void bmat_set_ones_64(bmat_param_64(d));

extern void bmat_set_exchange_8 (bmat_param_8 (m));
extern void bmat_set_exchange_16(bmat_param_16(m));
extern void bmat_set_exchange_32(bmat_param_32(m));
extern void bmat_set_exchange_64(bmat_param_64(m));

// bmat_toeplitz.c
extern void bmat_set_ut_toeplitz_8 (bmat_param_8 (m), uint64_t k);
extern void bmat_set_ut_toeplitz_16(bmat_param_16(m), uint64_t k);
extern void bmat_set_ut_toeplitz_32(bmat_param_32(m), uint64_t k);
extern void bmat_set_ut_toeplitz_64(bmat_param_64(m), uint64_t k);

extern void bmat_set_lt_toeplitz_8 (bmat_param_8 (m), uint64_t k);
extern void bmat_set_lt_toeplitz_16(bmat_param_16(m), uint64_t k);
extern void bmat_set_lt_toeplitz_32(bmat_param_32(m), uint64_t k);
extern void bmat_set_lt_toeplitz_64(bmat_param_64(m), uint64_t k);

extern void bmat_set_toeplitz_8 (bmat_param_8 (m), uint8_t  r, uint8_t  c);
extern void bmat_set_toeplitz_16(bmat_param_16(m), uint16_t r, uint16_t c);
extern void bmat_set_toeplitz_32(bmat_param_32(m), uint32_t r, uint32_t c);
extern void bmat_set_toeplitz_64(bmat_param_64(m), uint64_t r, uint64_t c);

extern void bmat_set_cl_mul_8 (bmat_param_8 (m), uint8_t  n);
extern void bmat_set_cl_mul_16(bmat_param_16(m), uint16_t n);
extern void bmat_set_cl_mul_32(bmat_param_32(m), uint32_t n);
extern void bmat_set_cl_mul_64(bmat_param_64(m), uint64_t n);

extern void bmat_set_cr_mul_8 (bmat_param_8 (m), uint8_t  n);
extern void bmat_set_cr_mul_16(bmat_param_16(m), uint16_t n);
extern void bmat_set_cr_mul_32(bmat_param_32(m), uint32_t n);
extern void bmat_set_cr_mul_64(bmat_param_64(m), uint64_t n);

extern void bmat_set_cc_mul_8 (bmat_param_8 (m), uint8_t  n);
extern void bmat_set_cc_mul_16(bmat_param_16(m), uint16_t n);
extern void bmat_set_cc_mul_32(bmat_param_32(m), uint32_t n);
extern void bmat_set_cc_mul_64(bmat_param_64(m), uint64_t n);

// bmat_mul.c
extern uint8_t  bmat_vmul_8 (uint8_t  v, bmat_param_8 (m));
extern uint16_t bmat_vmul_16(uint16_t v, bmat_param_16(m));
extern uint32_t bmat_vmul_32(uint32_t v, bmat_param_32(m));
extern uint64_t bmat_vmul_64(uint64_t v, bmat_param_64(m));

extern uint8_t  bmat_mulv_8 (bmat_param_8 (m), uint8_t  v);
extern uint16_t bmat_mulv_16(bmat_param_16(m), uint16_t v);
extern uint32_t bmat_mulv_32(bmat_param_32(m), uint32_t v);
extern uint64_t bmat_mulv_64(bmat_param_64(m), uint64_t v);

extern void bmat_mul_8 (bmat_rparam_8 (c),bmat_param_8 (a),bmat_param_8 (b));
extern void bmat_mul_16(bmat_rparam_16(c),bmat_param_16(a),bmat_param_16(b));
extern void bmat_mul_32(bmat_rparam_32(c),bmat_param_32(a),bmat_param_32(b));
extern void bmat_mul_64(bmat_rparam_64(c),bmat_param_64(a),bmat_param_64(b));

extern void bmat_mult_8 (bmat_rparam_8 (c),bmat_param_8 (a),bmat_param_8 (b));
extern void bmat_mult_16(bmat_rparam_16(c),bmat_param_16(a),bmat_param_16(b));
extern void bmat_mult_32(bmat_rparam_32(c),bmat_param_32(a),bmat_param_32(b));
extern void bmat_mult_64(bmat_rparam_64(c),bmat_param_64(a),bmat_param_64(b));

extern void bmat_madd_8 (bmat_rparam_8 (d), bmat_param_8 (a), bmat_param_8 (b), bmat_param_8 (c));
extern void bmat_madd_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b), bmat_param_16(c));
extern void bmat_madd_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b), bmat_param_32(c));
extern void bmat_madd_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b), bmat_param_64(c));

extern void bmat_bracket_8 (bmat_rparam_8 (c), bmat_param_8 (a), bmat_param_8 (b));
extern void bmat_bracket_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b));
extern void bmat_bracket_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b));
extern void bmat_bracket_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b));

// bmat_pow.h
extern void bmat_pow2_8 (bmat_rparam_8 (m));
extern void bmat_pow2_16(bmat_rparam_16(m));
extern void bmat_pow2_32(bmat_rparam_32(m));
extern void bmat_pow2_64(bmat_rparam_64(m));

extern void bmat_pow_8 (bmat_rparam_8 (m), bmat_param_8 (a), uint64_t n);
extern void bmat_pow_16(bmat_rparam_16(m), bmat_param_16(a), uint64_t n);
extern void bmat_pow_32(bmat_rparam_32(m), bmat_param_32(a), uint64_t n);
extern void bmat_pow_64(bmat_rparam_64(m), bmat_param_64(a), uint64_t n);

// bmat_gauss.c
extern uint32_t bmat_echelon_8 (bmat_param_8 (m));
extern uint32_t bmat_echelon_16(bmat_param_16(m));
extern uint32_t bmat_echelon_32(bmat_param_32(m));
extern uint32_t bmat_echelon_64(bmat_param_64(m));

extern uint32_t bmat_rref_8 (bmat_param_8 (m));
extern uint32_t bmat_rref_16(bmat_param_16(m));
extern uint32_t bmat_rref_32(bmat_param_32(m));
extern uint32_t bmat_rref_64(bmat_param_64(m));

extern uint32_t bmat_rref2_8 (bmat_param_8 (a), bmat_param_8 (b));
extern uint32_t bmat_rref2_16(bmat_param_16(a), bmat_param_16(b));
extern uint32_t bmat_rref2_32(bmat_param_32(a), bmat_param_32(b));
extern uint32_t bmat_rref2_64(bmat_param_64(a), bmat_param_64(b));

extern uint32_t bmat_rank_8 (bmat_param_8 (m));
extern uint32_t bmat_rank_16(bmat_param_16(m));
extern uint32_t bmat_rank_32(bmat_param_32(m));
extern uint32_t bmat_rank_64(bmat_param_64(m));

extern uint32_t bmat_kernel_8 (bmat_rparam_8 (m), bmat_array_8 (k));
extern uint32_t bmat_kernel_16(bmat_rparam_16(m), bmat_array_16(k));
extern uint32_t bmat_kernel_32(bmat_rparam_32(m), bmat_array_32(k));
extern uint32_t bmat_kernel_64(bmat_rparam_64(m), bmat_array_64(k));

extern uint32_t bmat_cokernel_8 (bmat_rparam_8 (m), bmat_array_8 (k));
extern uint32_t bmat_cokernel_16(bmat_rparam_16(m), bmat_array_16(k));
extern uint32_t bmat_cokernel_32(bmat_rparam_32(m), bmat_array_32(k));
extern uint32_t bmat_cokernel_64(bmat_rparam_64(m), bmat_array_64(k));

extern uint32_t bmat_fixed_points_8 (bmat_rparam_8 (m), bmat_array_8 (k));
extern uint32_t bmat_fixed_points_16(bmat_rparam_16(m), bmat_array_16(k));
extern uint32_t bmat_fixed_points_32(bmat_rparam_32(m), bmat_array_32(k));
extern uint32_t bmat_fixed_points_64(bmat_rparam_64(m), bmat_array_64(k));

extern bool bmat_is_full_rank_8 (bmat_param_8 (m));
extern bool bmat_is_full_rank_16(bmat_param_16(m));
extern bool bmat_is_full_rank_32(bmat_param_32(m));
extern bool bmat_is_full_rank_64(bmat_param_64(m));

extern void bmat_hessenberg_8(bmat_param_8(m));
extern void bmat_hessenberg_16(bmat_param_16(m));
extern void bmat_hessenberg_32(bmat_param_32(m));
extern void bmat_hessenberg_64(bmat_param_64(m));

extern uint64_t bmat_charpoly_8(bmat_param_8(m));
extern uint64_t bmat_charpoly_16(bmat_param_16(m));
extern uint64_t bmat_charpoly_32(bmat_param_32(m));
extern uint64_t bmat_charpoly_64(bmat_param_64(m));

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

extern void bmat_print_list_8 (bmat_param_8 (m));
extern void bmat_print_list_16(bmat_param_16(m));
extern void bmat_print_list_32(bmat_param_32(m));
extern void bmat_print_list_64(bmat_param_64(m));


extern uint32_t f2_ufunc_validate_32_i   (uint32_t (*)(uint32_t), uint32_t (*)(uint32_t));
extern uint64_t f2_ufunc_validate_64_i   (uint64_t (*)(uint64_t), uint64_t (*)(uint64_t));
extern uint32_t f2_ufunc_p_validate_32_i (uint32_t (*)(uint32_t), uint32_t (*)(uint32_t,uint32_t), uint32_t);
extern uint32_t f2_ufunc_p_validate_64_i (uint64_t (*)(uint64_t), uint64_t (*)(uint64_t,uint64_t), uint64_t);
extern uint32_t f2_ufunc_p2_validate_32_i(uint32_t (*)(uint32_t,uint32_t), uint32_t (*)(uint32_t, uint32_t), uint32_t);
extern uint64_t f2_ufunc_p2_validate_64_i(uint64_t (*)(uint64_t,uint64_t), uint64_t (*)(uint64_t, uint64_t), uint64_t);

extern uint8_t  bmat_from_ufunc_8 (bmat_param_8 (a), uint8_t  (*f)(uint8_t));
extern uint16_t bmat_from_ufunc_16(bmat_param_16(a), uint16_t (*f)(uint16_t));
extern uint32_t bmat_from_ufunc_32(bmat_param_32(a), uint32_t (*f)(uint32_t));
extern uint64_t bmat_from_ufunc_64(bmat_param_64(a), uint64_t (*f)(uint64_t));

extern uint32_t bmat_from_ufunc_p_32(bmat_param_32(a), uint32_t (*f)(uint32_t,uint32_t), uint32_t);
extern uint64_t bmat_from_ufunc_p_64(bmat_param_64(a), uint64_t (*f)(uint64_t,uint64_t), uint64_t);
