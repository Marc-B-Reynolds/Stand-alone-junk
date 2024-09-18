#ifndef COMPILER_HINTS_H
#define COMPILER_HINTS_H

// compiler hints: must assume that the hint will be ignored

// hint_result_barrier
// This is a hacky attempt at a lightweight code motion barrier:
// 1) It shouldn't work but it seems to (on current versions of GCC & clang)
// 2) I haven't seen it "break" other optimizations but it increases the
//    register pressure by one between computed 'v' and usage so it
//    can increase uop count which is generally a loss. 
// 3) If it ends up doing nothing..then no big deal.
//
// Roughly attempting to compute some primative value 'v' (using a hardware op
// with longish latency) which will be used later in the total computation.
//
//     v = some_computation();
//         do other work here { wanting to hide the latency of 'v' }
//     do something with 'v' here
//
#if defined(__GNUC__)
#define hint_result_barrier(X) __asm__ __volatile__("" : "+r"(X) : "r"(X));
//                             __asm__ __volatile__("" : "+r"(X) : );
#else
#define hint_result_barrier(X)
#endif

// attempted to hint to the complier not to promote a computation
// with 'v' into a constant load. use with care and only at point
// of the computation with 'v' to not potentially break constant
// propogation.
//   other_value = op(no_const_fold_n(v))

#if defined(__GNUC__)
#pragma GCC diagnostic push
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif
static inline uint32_t hint_no_const_fold_32(uint32_t v)
{
  asm volatile ("" : "+x" (v));
  return v;  
}

static inline uint64_t hint_no_const_fold_64(uint64_t v)
{
  asm volatile ("" : "+x" (v));
  return v;  
}
#pragma GCC diagnostic pop
#else
static inline uint32_t hint_no_const_fold_32(uint32_t v) { return v; }
static inline uint64_t hint_no_const_fold_64(uint64_t v) { return v; }
#endif



// hint_rw_barrier() : compiler read/write barrier

#if   defined(__GNUC__)
#define hint_rw_barrier()       __asm__ __volatile__("": : :"memory")
#elif defined(_MSC_VER)
#define hint_rw_barrier()       _ReadWriteBarrier()  // deprecated
#else
#include <stdatomic.h>
#define hint_rw_barrier()       atomic_signal_fence(memory_order_acq_rel)
#endif

// because clang loses it's mind about loop unrolling
#if defined(__clang__)
#define hint_no_unroll _Pragma("clang loop unroll(disable)")
#else 
#define hint_no_unroll
#endif 

#if defined(__clang__)
#define hint_assume(expression) __builtin_assume(expression)
#else
#define hint_assume(expression) do { if (!(expression)) __builtin_unreachable(); } while (0)
#endif

#if defined(__GNUC__)
#define hint_expect(expression) __builtin_expect(expression)
#define hint_unpredictable()    __builtin_unpredictable()
#define hint_unreachable()      __builtin_unreachable()
#elif defined(_MSC_VER)
#define hint_expect(expression) 
#define hint_unpredictable()
#define hint_unreachable()      __assume(0)
#else
#define hint_expect(expression) 
#define hint_unpredictable()
#define hint_unreachable()
#endif


#if defined(__GNUC__)
#define hint_no_inline          __attribute__((__noinline__))
#define hint_pure_func          __attribute__((__pure__))
#define hint_const_func         __attribute__((__const__))
#elif defined(_MSC_VER)
#define hint_no_inline          __declspec(noinline)
#define hint_pure_func               
#define hint_const_func
#else
#define hint_no_inline
#define hint_pure_fun
#define hint_const_func
#endif




#endif
