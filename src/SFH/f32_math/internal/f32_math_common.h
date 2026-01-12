// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2026

// common approximation utility functions

#ifndef F32_MATH_COMMON_H
#define F32_MATH_COMMON_H

#ifndef   F32_HORNER2
#include "f32_util.h"
#include "f32_horner.h"
#include "f32_horner2.h"
#endif

#ifndef   F64_HORNER2
#include "f64_util.h"
#include "f64_horner.h"
#include "f64_horner2.h"
#endif

// given a function 'f' defined for postive 'x': extend to it
// negative numbers as an odd function: f(-x) = -f(x)
// into an odd function. Multiple (compile time) variants.
static inline float f32_odd_reduce(float x, float (*f)(float))
{
#if   defined(F32_ODD_REDUCE_BITOPS)  
  // purely bit manipulation.
  
  uint32_t ix = f32_to_bits(x);     // bit pattern of x
  uint32_t sx = ix & 0x80000000;    // sign(x) {bit }
  uint32_t ax = ix ^ sx;            // |x|     {bits}
  float    a  = f32_from_bits(ax);  // |x|
  float    r  = f(a);               // core approximation
  
  return f32_mulsign(r, sx);        // restore sign

#elif defined(F32_ODD_REDUCE_STDFUNCS)
  // standard functions only
  float a     = fabsf(x);
  float r     = f(a);               // core approximation

  return copysignf(r, x);           // restore sign

#else
  // default: hybrid. 
  float    a  = fabsf(x); 
  float    sx = f32_xor(x,a);       // isolated sign bit of 'x'
  float    r  = f(a);               // core approximation

  return f32_xor(r, sx);            // restore sign
#endif  
}


#endif
