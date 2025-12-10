// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2022-2026
// Public Domain under http://unlicense.org, see link for details.

#pragma once

// TL;DR: endgame is AZO (approaching zero overhead) generic methods in C for
// some ordering related operations (like searching) without macro spamming.
// "kernels" of operations are written as functions which take a pointer
// to an ordering (& memory layout) structure & calls functions to retrieve
// and re-order the "key" into a canonical form. This only "works" as
// type specialization (~AZO) if the compiler can recognize the constant
// data and inlines the set of functions. GCC & clang can handle this,
// the Microsoft compiler can't. (this code still limits itself to features
// that allow compiling under MSC though)

// Canonicalize integer & floating-point types (64-bit wide and narrower) to signed
// 64-bit integers. All mappings are reversible (aka error free, bijections, etc)
// ∙ The source type's ordering is preserved. For floating point the native
//   partial ordering is transformed into totally ordered (-0 < 0 and NaNs are
//   order by their sign bit and payload)
// ∙ integer types also maintain distance measures. float point distance measures
//   become relative instead of absolute (the number of ULPs between values)
//
// The point of the exercise is constructing limited type specialization macros
// which are implemented as function calls instead of spamming macros. And they
// will expand as macros (at least in GCC and clang). The compiler in question
// needs to be able to recognize that function pointers are constant which gives
// monomorphic call-sites and inline the functions. This kind of thing is just
// "batteries included" in most modern statically typed language so the 
// structure will be of limited interest in those cases.
//
// Example use-cases are searching and sorting. Basically we need to perform
// three logical operations when fetching an element (datum):
//
// 1. compute the datum address: What bit-width? flat array, element of a
//    structure, etc. Any "etc" functionality needs to be user provided.
//    (say some interleaved storage, AoSoA, ...)
// 2. load the datum "as if" it were a signed integer of its bit-width
//    and promote to 64-bit (if not already).
// 3. given its actual type apply a map that converts the ordering to that
//    of a signed integer.
//
// If the compiler's successes at flatting a given method then the only
// overhead (vs direct per type) is the unsigned case which have an
// additional xor (and probably constant load). Well floating point is
// a small handful of operations (pair of shifts and xor) if the
// direct method isn't using a total ordering.
//
// Obviously all of these operations collapse to a very small number of
// ISA operations when "inlined".  All of verbage in this file is
// C language impedence matching and internal/external helpers
//
// VERY IMPORTANT: the order structure need to be static const.
// SEE: populate new_order macros (below)

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

//***************************************************************************************

// defines a compile time baking (or runtime configurable) method of searching
// ∙ get   : fetches raw data      SEE: new_order_get (methods) 
// ∙ order : data order transfomer SEE: new_order_order (methods)
typedef struct new_order_s new_order_t;

// NEW_ORDER_USER allows baking in addtional side information for
// computing addresses for a base pointer to an index beyond the
// interally supported flat arrays and arrays of structures.
#ifndef NEW_ORDER_USER
#define NEW_ORDER_USER uint32_t dummy
#endif

// For type specialization this is really a dummy structure that
// tells the compiler how to expansion the function.
struct new_order_s
{
  // SEE: new_order_addr, new_order_get & new_order_order (methods) respectively
  // (address computation, datum read, map ordering)
  char*   (*addr)(const new_order_t* const, const void* const, uint64_t);
  int64_t (*get) (const char* const);
  int64_t (*order)(int64_t);
  
  uint32_t slen;
  uint32_t soff;

  // any auxiliary data needed for address computations
  union {
    NEW_ORDER_USER;
  };
};


// composed: compute address, fetch and re-order
static inline int64_t new_order_get(const new_order_t* const def, const void* const a, uint64_t n)
{
  return def->order(def->get(def->addr(def,a,n)));
}


//***************************************************************************************
// new_order_addr (methods): computes the address of the nth datum

// primitive array or array of structures
static inline char* new_order_addr(const new_order_t* const def, const void* const a, uint64_t n)
{
  return (char*)a + (def->slen * n) + def->soff;
}

// array of pointers. 
static inline char* new_order_iaddr(const new_order_t* const def, const void* const a, uint64_t n)
{
#if 1
  void* const* p = (void* const *)a;
  void* e = p[n];
  return (char*)e + def->soff;
#else
  //((const char* const)a)[n] + def->soff;
#endif  
}


//***************************************************************************************
// new_order_get (methods): the kernels all operate on signed 64-bit integers (int64_t)
// so these perform two logical operations:
// ∙ load the item is as a signed integer of that width (again has 16/32/64)
//   (avoids UB for the actual load)
// ∙ promote any narrower bit-widths up to int64_t.
// handling elements which aren't signed integers are handled by: new_order_order (methods)

// macros for less of an eye-sore
// the parameters each function takes
#define new_order_get_params const char* const addr

// flat array & AoS versions
static inline int64_t new_order_get_16(new_order_get_params) { int16_t t; memcpy(&t, addr, 2); return t; }
static inline int64_t new_order_get_32(new_order_get_params) { int32_t t; memcpy(&t, addr, 4); return t; }
static inline int64_t new_order_get_64(new_order_get_params) { int64_t t; memcpy(&t, addr, 8); return t; }

#undef new_order_get_params

//***************************************************************************************
// new_order_order (methods): the kernels all operate on signed 64-bit integers (int64_t) and are
// loaded by 'new_order_get' as if that's actual type of data that's being operated on.
// These routines handle mapping the actual type ordering to that of a int64_t.
// ∙ unsigned integer: toggle top bit
// ∙ floating point:   XOR the sign bits with all other bits (which is a total ordering)
//
// all functions (except 'new_order_rev_order_fp') are involutions. noteworthy for
// constructing testing functionality. An involution as the property: f(f(x))=x
// for all 'x' so we can get back to the original value by applying 'order' a
// second time.
static inline int64_t new_order_order_si(int64_t x) { return x; }
static inline int64_t new_order_order_ui(int64_t x) { return x ^ (INT64_C(1)<<63); }
static inline int64_t new_order_order_fp(int64_t x) { return x ^ (int64_t)((uint64_t)(x >> 63)>>1); }

// data is sorted largest to smallest: bit not reverses the ordering which toggles
// between signed and unsigned integer ordering (in a UB free manner in languages
// where that matters...like C)
static inline int64_t new_order_rev_order_si(int64_t x) { return ~new_order_order_si(x); }
static inline int64_t new_order_rev_order_ui(int64_t x) { return ~new_order_order_ui(x); }
static inline int64_t new_order_rev_order_fp(int64_t x) { return ~new_order_order_fp(x); }  // not an involution


//***************************************************************************************
// "convenience" macro (new_order_value) to manually convert a type to the canonicalized

// helper to prepare an input value to fed to a kernel.
// For integer types this is merely a verbose way to
// cast to int64_t. For floating point types we also
// need to type pun.
#define new_order_value_(V)        \
  _Generic((V),                    \
    int16_t:  new_order_value_i16, \
    uint16_t: new_order_value_u16, \
    int32_t:  new_order_value_i32, \
    uint32_t: new_order_value_u32, \
    int64_t:  new_order_value_i64, \
    uint64_t: new_order_value_u64, \
    float:    new_order_value_f32, \
    double:   new_order_value_f64, \
    default: (void*)0)(V)          \


// all of these are just impedance matching for the generic (new_order_value_)
static inline int64_t new_order_value_i16(int32_t  x) { return (int64_t)x; }
static inline int64_t new_order_value_u16(uint32_t x) { return (int64_t)x; }
static inline int64_t new_order_value_i32(int32_t  x) { return (int64_t)x; }
static inline int64_t new_order_value_u32(uint32_t x) { return (int64_t)x; }
static inline int64_t new_order_value_i64(int64_t  x) { return x; }
static inline int64_t new_order_value_u64(uint64_t x) { return (int64_t)x; }

// floats additionally need to handle type-punning
static inline int64_t new_order_value_f32(float x) { int32_t u; memcpy(&u, &x, 4); return (int64_t)u; }
static inline int64_t new_order_value_f64(double x){ int64_t u; memcpy(&u, &x, 8); return u; }

// the above handles the part equivalent to the "get" steps and
// to complete the operation we need to perform any reordering
// which simply uses the order function from a new_order_t (D)
#define new_order_value(D,V) ((D)->order(new_order_value_(V)))


//***************************************************************************************
// "convenience" macros to fill the .order & .get fields
//
// IntelliSense bug current emits error code:
// 'E0028: expression must have a constant value'
// for baking these in a struct.

// values for the .order field from some variable V
#define new_order_type_order(V)   \
  _Generic((V),                   \
    uint16_t: new_order_order_ui, \
    uint32_t: new_order_order_ui, \
    uint64_t: new_order_order_ui, \
    float:    new_order_order_fp, \
    double:   new_order_order_fp, \
    default:  new_order_order_si)


// values for the .get field from some variable V
// (simple array)
#define new_order_type_get(V)     \
  _Generic((V),                   \
    int16_t:  new_order_get_16,   \
    uint16_t: new_order_get_16,   \
    int32_t:  new_order_get_32,   \
    uint32_t: new_order_get_32,   \
    float:    new_order_get_32,   \
    default:  new_order_get_64)


//***************************************************************************************
// populate new_order macros:
//
// To get type specialization macro behavior the 'new_order_t' structure
// must be const. 
//
// trival macros to populate a new_order_t structure with the functions given a
// variable V to identify the type (and datum element name for AoS)

// flat arrays: feed a variable V of desired type:
//   e.g. foo(float* a) { new_order_t o = new_order(a[0]); ... }
//        bar(float  f) { new_order_t o = new_order(f);    ... }
#define new_order(V)                 \
{                                    \
  .addr  = new_order_addr,           \
  .get   = new_order_type_get(V),    \
  .order = new_order_type_order(V),  \
  .slen  = sizeof(typeof(V)),        \
  .soff  = 0                         \
}

// array of structures: V is variable of the struct and E is datum element.
//   e.g. foo(baz_t* a) { new_order_t o = new_order_aos(a[0],value); ... }
//   if 'value' is the datum element of baz_t
#define new_order_aos(V,E)                \
{                                         \
  .addr  = new_order_addr,                \
  .get   = new_order_type_get((V).E),     \
  .order = new_order_type_order((V).E),   \
  .slen  = sizeof(typeof(V)),             \
  .soff  = offsetof(typeof(V),E)          \
}

// array of pointers to type T with datum element E
// not done and no
#define new_order_aop(T,E)                \
{                                         \
  .addr  = new_order_iaddr,               \
  .get   = new_order_type_get((T v).E),   \
  .order = new_order_type_order((T v).E), \
  .slen  = sizeof(T),                     \
  .soff  = offsetof(T,E)                  \
}


//***************************************************************************************
