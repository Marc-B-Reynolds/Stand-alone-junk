// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#ifndef F32_ASINPI_SB_H
#define F32_ASINPI_SB_H

// SEE:
// * (put post link here)
// * f32_acospi_sb.h
//
// implementation is simply:
//    asinpi(x) = 1/2 - acospi(x).
//
// which is awful numerically. A better solution would be a cheap
// direct form but a good expression is alluding me. An example is
// that RN(acospi(x)) = 1/2 on [-0x1.921fb4p-24, 0x1.921fb4p-25]
// so the asinpi approximation on this range will be zero instead
// of ~x/pi. But spitballing and abs error (shrug)!

// given f = P(x) expand approximation restricted to positive input.
// return NaN for negative inputs.
// just hacked: now test blockhead
static inline float f32_asinpi_sb_xp(float (*f)(float), float x)
{
  float t = f32_sqrt(1.f-x);
  float p = f(x);

  return -fmaf(t,p,-0.5f);
}

// given f = P(x) expand full range approximation:
//    asinpi(x) = sign(x)(1/2 - sqrt(1-|x|)) P(|x|)
static inline float f32_asinpi_sb_xf(float (*f)(float), float x)
{
  // currently only hybrid reduction: (SEE: f32_odd_reduce)
  // as it generally works well with current Intel & ARM arches

  float a  = fabsf(x);
  float sx = f32_xor(x,    a);
  float c  = f32_xor(0.5f, sx);   // (x>=0) ? .5 : -.5
  float t  = -f32_sqrt(1.f-a);
  float p  = f(a);
  
  // combine the subresults: here the sign is being applied the polynomial
  // result 'p' but could instead be applied to sqrt result 't'. ideally
  // it would applied to the sub-result that's expected to complete first.
  // roughly very low degree P apply to 'p', otherwise to 't' (as below)

  return fmaf(t, f32_xor(p,sx), c);
}


// longer polynomial version (see note above)
// just hacked: now test blockhead
static inline float f32_asinpi_sb_xf_l(float (*f)(float), float x)
{
  float a  = fabsf(x);
  float sx = f32_xor(x,    a);
  float c  = f32_xor(0.5f, sx);
  float t  = -f32_sqrt(1.f-a);
  float p  = f(a);

  return fmaf(f32_xor(t,sx), p, c);
}



#endif
