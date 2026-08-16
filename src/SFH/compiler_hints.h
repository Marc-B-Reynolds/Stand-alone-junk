// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2016-2026
// Public Domain under http://unlicense.org, see link for details.

#pragma once
#define COMPILER_HINTS_H

#include <stdint.h>

// compiler hints: must assume that the hint will be ignored


#define hint_empty_statement  do { /* empty compound statement */ } while (0)

#if defined(__GNUC__) || defined(__clang__)
#define hint_sequence_int(A,B) do asm ("" : "+r" (A), "+r"(B)); while (0)
#define hint_sequence_fp(A,B)  do asm ("" : "+x" (A), "+x"(B)); while (0)

// force both A and B to be alive in registers at this point
#define hint_sequence(A,B) __builtin_choose_expr(__builtin_classify_type(A) == 1, hint_sequence_int(A,B), hint_sequence_fp(C,A,B))

// attempt at: `foo = hint_barrier(foo);` being a barrier on the preceeding computation of foo
// look like forces a register move in GCC
//#define hint_barrier_fp(X)  ({ typeof(X) __hbv = (X); asm volatile ("" : "+x" (__hbv)); __hbv; })
//#define hint_barrier_int(X) ({ typeof(X) __hbv = (X); asm volatile ("" : "+r" (__hbv)); __hbv; })
//#define hint_barrier(A) __builtin_choose_expr(__builtin_classify_type(A) == 1, hint_barrier_int(A), hint_barrier_fp(A))

#else
#define hint_sequence(A,B) hint_empty_statement
#define hint_barrier(A)    A
#endif

#ifndef __has_builtin
#define __has_builtin(X) 0
#endif

#ifndef __has_c_attribute
#define __has_c_attribute(x) 0
#endif


// attempted to hint to the complier not to promote a computation
// with 'v' into a constant load. use with care and only at point
// of the computation with 'v' to not potentially break constant
// propogation.
//   other_value = op(no_const_fold_n(v))

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif

// force expression 'v' to be register at this point, forget information about 'v' but
// removable if 'v' isn't accessed. Should be renamed.
#define hint_alive_int(V) ({ typeof(V) _v = (V); asm ("" : "+r" (_v)); _v; })
#define hint_alive_fp(V)  ({ typeof(V) _v = (V); asm ("" : "+x" (_v)); _v; })
#define hint_alive(V) __builtin_choose_expr(__builtin_classify_type(V) == 1,  hint_alive_int(V), hint_alive_fp(V))

// as above but w/o forgetting range info
#define hint_weak_alive_int(V) ({ typeof(V) _v = (V); asm ("" : : "r" (_v)); _v; })
#define hint_weak_alive_fp(V)  ({ typeof(V) _v = (V); asm ("" : : "x" (_v)); _v; })
#define hint_weak_alive(V) __builtin_choose_expr(__builtin_classify_type(V) == 1,  hint_weak_alive_int(V), hint_weak_alive_fp(V))

#if defined(__clang__)
#define hint_alive_clang    hint_alive
#define hint_alive_gcc(V)   ({(V)})
#else
#define hint_alive_gcc      hint_alive
#define hint_alive_clang(V) ({(V)})
#endif

#pragma GCC diagnostic pop
#else
#define hint_alive(V)      (V)
#define hint_weak_alive(V) (V)
#endif


// hint_rw_barrier() : compiler read/write barrier

#if   defined(__GNUC__) || defined(__clang__)
#define hint_rw_barrier()  do { asm __volatile__("": : :"memory"); } while(0)
#elif defined(_MSC_VER)
#define hint_rw_barrier()  _ReadWriteBarrier()  // deprecated
#else
#include <stdatomic.h>
#define hint_rw_barrier()  atomic_signal_fence(memory_order_acq_rel)
#endif

#define hint_pragma(X) _Pragma(#X)

#if defined(__clang__)
#define hint_unroll(X) hint_pragma(clang loop unroll_count(X))
#define hint_no_unroll hint_pragma(clang loop unroll(disable))
#elif defined(__GNUC__)
#define hint_unroll(X) hint_pragma(GCC unroll X)
#define hint_no_unroll hint_pragma(GCC unroll 0)
#else
#define hint_unroll(X)
#define hint_no_unroll
#endif


// void hint_unreachable() : tell compiler it can't reach this statement.
// If it can then it's UB
#if   (__has_builtin(__builtin_unreachable))
#define hint_unreachable()        __builtin_unreachable()
#elif defined(_MSC_VER)
#define hint_unreachable()        __assume(0)
#else
#define hint_unreachable()        hint_empty_statement
#endif

// void hint_assume(exp) : tell compiler to assume `exp` is always true.
// if `exp` is ever false UB can be introduced.
#if   (__has_builtin(__builtin_assume))
#define hint_assume(exp)          __builtin_assume(exp)
#elif defined(_MSC_VER)
#define hint_assume(exp)          __assume(exp)
#elif (__has_builtin(__builtin_unreachable))
#define hint_assume(exp)           do { if (!(exp)) __builtin_unreachable(); } while (0)
#else
#define hint_assume(exp)           hint_empty_statement
#endif


// hint_pointer_aligned(P,B) : pointer P is B byte aligned
#if   (__has_builtin(__builtin_assume_aligned))
#define hint_pointer_aligned(P,B) do { P = __builtin_assume_aligned(P,B); } while(0)
#elif (__has_builtin(__builtin_unreachable))
#define hint_pointer_aligned(P,B) do { if ((size_t)P & ((B)-1)) { __builtin_unreachable(); }; } while(0)
#else
#define hint_pointer_aligned(P,B)
#endif

// branch probability related
#if defined(__GNUC__) || defined(__clang__)
#define hint_unlikely(exp)        __builtin_expect(!!(exp),0)
#define hint_expect(exp)          __builtin_expect(!!(exp),1)
#if defined(__clang__)
#define hint_unpredictable(exp)   __builtin_unpredictable(exp)
#else
#define hint_unpredictable(exp)   __builtin_expect_with_probability(exp,1,0.5)
#endif
#elif defined(_MSC_VER)
#define hint_unlikely(exp)        (exp)
#define hint_expect(exp)          (exp)
#define hint_unpredictable(exp)   (X)
#else
#define hint_expect(exp)          (exp)
#define hint_unpredictable(exp)   (X)
#endif

#define hint_likely hint_expect

// function attributes
#if defined(__GNUC__) || defined(__clang__)
#define hint_flatten            __attribute__((__flatten__))
#define hint_no_inline          __attribute__((__noinline__))
#define hint_pure_func          __attribute__((__pure__))
#define hint_const_func         __attribute__((__const__))
#elif defined(_MSC_VER)
#define hint_no_inline          __declspec(noinline)
#define hint_flatten
#define hint_pure_func               
#define hint_const_func
#else
#define hint_flatten
#define hint_no_inline
#define hint_pure_fun
#define hint_const_func
#endif


//────────────────────────────────────────────────────────────────────────────────────
// a sad attempt to convince the compiler to be branch-free (generate cmov/csel)
// 

#if defined(__GNUC__) || defined(__clang__)
#if __has_builtin(__builtin_unpredictable)
#define hint_select_is_generic
#define hint_select(C,A,B) ({__builtin_unpredictable(C) ? A : B;})
#else
// currently only this or `hint_select_is_generic`
#define hint_select_is_int_vs_fp

// using the wrong constraints forces some register moves
#define hint_select_int(C,A,B) ({ int _hsc=C; typeof(A) _hsa=A,_hsb=B,_r; asm("" : "+r"(_hsc), "+r"(_hsa), "+r"(_hsb)); _r = _hsc ? _hsa : _hsb; _r;})
#define hint_select_fp(C,A,B)  ({ int _hsc=C; typeof(A) _hsa=A,_hsb=B,_r; asm("" : "+x"(_hsc), "+x"(_hsa), "+x"(_hsb)); _r = _hsc ? _hsa : _hsb; _r;})

#define hint_select(C,A,B) __builtin_choose_expr(__builtin_classify_type(A) == 1, hint_select_int(C,A,B), hint_select_fp(C,A,B))
#endif
#else
// haven't really looked at MSVC.
#define hint_select_is_generic
#define hint_select(C,A,B) _Generic(C,default: C ? A:B)
#endif

// the large number of macros is an attempt to minimize expansions when a refined type is specified.
// currently clang/MSVC map everything to hint_select & GCC only maps to hint_select_int or hint_select_fp.
// So mostly useless ATM but in-place if type specific versions happen in the future.
#if defined(hint_select_is_generic) || defined(hint_select_is_int_vs_fp)
#if defined(hint_select_is_generic)
#define hint_select_int hint_select
#define hint_select_fp  hint_select
#endif
#define hint_select_u32 hint_select_int
#define hint_select_u64 hint_select_int
#define hint_select_i32 hint_select_int
#define hint_select_i64 hint_select_int
#define hint_select_f32 hint_select_fp
#define hint_select_f64 hint_select_fp
#else
#if (!defined(hint_select_u32) || !defined(hint_select_u64) ||
     !defined(hint_select_i32) || !defined(hint_select_i64) ||
     !defined(hint_select_f32) || !defined(hint_select_f64) )
#error "hint_select_<type> macros need defining"

#endif
#endif

     
#if defined(hint_select_i)
// inlines need work with _Generic
static inline uint32_t hint_select_u32(int c, uint32_t a, uint32_t b) { return hint_select_i(u32x1_t,C,A,B); }
static inline uint64_t hint_select_u64(int c, uint64_t a, uint64_t b) { return hint_select_i(u64x1_t,C,A,B); }
static inline int64_t  hint_select_i32(int c, uint32_t a, uint32_t b) { return hint_select_i(i32x1_t,C,A,B); }
static inline int64_t  hint_select_i64(int c, uint64_t a, uint64_t b) { return hint_select_i(i64x1_t,C,A,B); }
static inline float    hint_select_f32(int c, float    a, float    b) { return hint_select_i(f32x1_t,C,A,B); }
static inline double   hint_select_f64(int c, double   a, double   b) { return hint_select_i(f64x1_t,C,A,B); }
#undef hint_select_t
#endif
     
#ifndef hint_select
#define hint_select(C,A,B) ({ \
  _Generic((A),               \
   uint32_t: hint_select_u32, \
   uint32_t: hint_select_u64, \
   uint32_t: hint_select_i32, \
   uint32_t: hint_select_i64, \
   uint32_t: hint_select_f32, \
   uint32_t: hint_select_f64) \
   (C,A,B);                   \
  })
#endif     
     
     
//────────────────────────────────────────────────────────────────────────────────────
// conditional swap

#define HINT_CSWAP(C,X,Y,S) do { int _swapc = C; typeof(X) _swapx = X, _swapy = Y; X=S(_swapc,_swapy,_swapx); Y=S(_swapc,_swapx,_swapy); } while(0)

#define hint_cswap(C,X,Y)     HINT_CSWAP(C,X,Y,hint_select)
#define hint_cswap_int(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_int)
#define hint_cswap_fp(C,X,Y)  HINT_CSWAP(C,X,Y,hint_select_fp)
#define hint_cswap_u32(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_u32)
#define hint_cswap_u64(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_u64)
#define hint_cswap_i32(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_i32)
#define hint_cswap_i64(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_i64)
#define hint_cswap_f32(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_f32)
#define hint_cswap_f64(C,X,Y) HINT_CSWAP(C,X,Y,hint_select_f64)



//────────────────────────────────────────────────────────────────────────────────────
// temp hack placement in this file (thus not called hint_something)
// register: 'void f(void)' to be called at initialization time
// unless SFH_NO_AUTOINIT is defined in which case it simply 
// expands to 'void f(void)' (not static which auto version is)
// so it can be called here or elsewhere. Having name 'f' allows
// more than one per file in addition to manual. 
//
// register_init_time_function(f)
// {
//   // stuff to do at init time
// }
//
// IMPORTANT: The name 'f' (including any mangling) has to be unique
// in MSVC. Otherwise will produce a link error.

#if !defined(SFH_NO_AUTOINIT)
#ifdef __cplusplus
  #define register_init_time_function(f)                         \
    static void f(void);                                         \
    struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
    static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
  #define register_init_time_msvc(f,p)                       \
    static void f(void);                                     \
    __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
    __pragma(comment(linker,"/include:" p #f "_"))           \
    static void f(void)
 #ifdef _WIN64
    #define register_init_time_function(f) register_init_time_msvc(f,"")
 #else
    #define register_init_time_function(f) register_init_time_msvc(f,"_")
 #endif
#elif defined(__GNUC__)
// don't need __clang__ since clang-cl will have _MSC_VER defined
// and clang defines __GNUC__ everywhere else AFAIK
  #define register_init_time_function(f)              \
    static void f(void) __attribute__((constructor)); \
    static void f(void)
#else
#define register_init_time_function(f) "error: register_init_time_function"
#endif
#else
#define register_init_time_function(f)  void f(void)
#endif
