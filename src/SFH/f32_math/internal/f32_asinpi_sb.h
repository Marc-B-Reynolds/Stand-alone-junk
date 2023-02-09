
#ifndef __F32_ASINPI_SB_H__
#define __F32_ASINPI_SB_H__

// less than ideal numerically. simply converts acospi to asinpi by
// asinpi(x) = 1/2 - acospi(x)
// but we're spitballing kids!
static inline float f32_asinpi_sb_p(float (*f)(float), float x)
{
#if defined(ARG_REDUCE_STDFUNCS)
  // using standard functions

  float a = fabsf(x);
  float c = copysignf(0.5f, x);
  float t = -sqrtf(1.f-a);
  float p = f(a);
  
  return fmaf(t, copysignf(p,x), c);
  
#else
  uint32_t ix = f32_to_bits(x);        // IEEE bit pattern of x
  uint32_t ax = ix & 0x7fffffff;       // IEEE bit pattern of |x|
  uint32_t sx = ix ^ ax;               // sign bit isolated
  float    a  = f32_from_bits(ax);     // fabsf(x)
  float    t  = -sqrtf(1.f-a);         // the transform
  float    c  = f32_mulsign(0.5f, sx); // the constant
  float    p  = f(a);                  // expand the polynomial P(x)

  // combine the subresults: here the sign is being applied the polynomial
  // result 'p' but could instead be applied to sqrt result 't'. ideally
  // it would applied to the sub-result that's expected to complete first.
  // roughly very low degree P apply to 'p', otherwise to 't'
  
  return fmaf(t, f32_mulsign(p, sx), c);
#endif  
}

#endif
