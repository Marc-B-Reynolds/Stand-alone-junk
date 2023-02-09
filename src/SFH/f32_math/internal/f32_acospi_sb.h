
#ifndef __F32_ACOSPI_SB_H__
#define __F32_ACOSPI_SB_H__

// SEE:
//
// * defines two sets of polynomial approximations P(x)
//     f32_acospi_sb_p{n}  = n^th degree where P(0) ~= 1/2 
//     f32_acospi_sbe_p{n} = n^th degree where P(0)  = 1/2
//   the former has slightly lower error with the one more
//   degree of freedom and the latter is exact for inputs
//   at (and near) zero.
// * defines two functions for expanding the P(x) into usable
//   functions:
//     f32_acospi_sb_pp = creates an approximation limited to positive inputs
//     f32_acospi_sb_p  = creates an approximation for the full range
// * if the macro ARG_REDUCE_STDFUNCS is defined then the
//   two previous expansion functions use fabsf & copysignf. Otherwise
//   bit manipulation is performed.
//
// REQUIRES:
//  * if ARG_REDUCE_STDFUNCS isn't defined: f32_to_bits, f32_from_bits
//    and f32_mulsign (SEE: )
//  * f32_horner_{n} 

//**********************************************************************
// f(0) relaxed spitball polynomials: P(x)
// not forcing result of f(0) to 1/2 gives an additional degree of
// freedom and produces slightly more accurate result.

static inline float f32_acospi_sb_p1(float x)
{
  static const float C[] = {-0x1.b6bfa6p-5f, 0x1.fef466p-2f };
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sb_p2(float x)
{
  static const float C[] = { 0x1.0c014ep-6f, -0x1.0bed48p-4f, 0x1.ffe4cap-2f };
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sb_p3(float x)
{
  static const float C[] = { -0x1.b3d094p-8f, 0x1.9107f6p-6f, -0x1.158bb6p-4f, 0x1.fffcd4p-2f };
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sb_p4(float x)
{
  static const float C[] = { 0x1.961c7cp-9f, -0x1.88640ap-7f, 0x1.bea05ep-6f, -0x1.1760fcp-4f, 0x1.ffff9ap-2f };
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_p5(float x)
{
  static const float C[] = {
    -0x1.9989cep-10f, 0x1.adfc26p-8f, -0x1.def6dep-7f,
     0x1.cbd07cp-6f, -0x1.17b844p-4f, 0x1.fffff2p-2f };
  
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sb_p6(float x)
{
  static const float C[] = {
    0x1.b354bcp-11f, -0x1.f4673p-9f, 0x1.2176c4p-7f, -0x1.008af8p-6f,
    0x1.cf49aap-6f, -0x1.17c876p-4, 0x1.fffffep-2f };
  
  return f32_horner_6(x,C);
}


//**********************************************************************
// f(0) exact spitball polynomials: P(x)

static inline float f32_acospi_sbe_p1(float x)
{
  static const float C[] = {-0x1.c95b42p-5f, 0.5f};
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sbe_p2(float x)
{
  static const float C[] = { 0x1.16bf4p-6f, -0x1.0e77f6p-4f, 0.5f};
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sbe_p3(float x)
{
  static const float C[] = {-0x1.c374b8p-8f, 0x1.96b008p-6f, -0x1.161a08p-4f, 0.5f};
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sbe_p4(float x)
{
  static const float C[] = {0x1.a30de4p-9f,-0x1.8eb0fp-7f,0x1.c09deap-6f,-0x1.177d92p-4f, 0.5f};
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sbe_p5(float x)
{
  static const float C[] = {
    -0x1.a58d5ep-10f, 0x1.b5572ep-8f, -0x1.e22fap-7f, 0x1.cc6afp-6f, -0x1.17bdf6p-4f, 0.5f
  };
  
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sbe_p6(float x)
{
  static const float C[] = {
    0x1.bf2db4p-11f, -0x1.fd24cp-9f, 0x1.23eabp-7f, -0x1.0131b8p-6f, 0x1.cf73dep-6f, -0x1.17c992p-4f, 0.5f
  };
  return f32_horner_6(x,C);
}

//**********************************************************************

// given f = P(x) expand approximation restricted to positive input
static inline float f32_acospi_sb_pp(float (*f)(float), float x)
{
  float t = f32_sqrt(1.f-x);
  float p = f(x);

  return t*p;
}

// remember signed zero needs to be handled properly
static inline float f32_acospi_c(uint32_t sx)
{
  uint32_t m = (uint32_t)(((int32_t)sx)>>31);
  return f32_from_bits(f32_to_bits(1.f) & m);
}

// remember signed zero needs to be handled properly
static inline float f32_acospi_cf(float x)
{
  uint32_t m = (uint32_t)(((int32_t)f32_to_bits(x)) >> 31);
  return f32_from_bits(f32_to_bits(1.f) & m);
}


// given f = P(x) expand full range approximation
static inline float f32_acospi_sb_p(float (*f)(float), float x)
{
#if defined(ARG_REDUCE_STDFUNCS)
  // using standard functions
  float a = fabsf(x);
  float c = f32_acospi_cf(x);
  float t = f32_sqrt(1.f-a);
  float p = f(a);
  
  return fmaf(t, copysignf(p,x), c);  // see sign application comment below
#else
  uint32_t ix = f32_to_bits(x);       // IEEE bit pattern of x
  uint32_t ax = ix & 0x7fffffff;      // IEEE bit pattern of |x|
  uint32_t sx = ix ^ ax;              // sign bit isolated
  float    a  = f32_from_bits(ax);    // fabsf(x)
  float    t  = f32_sqrt(1.f-a);      // the transform
  float    c  = f32_acospi_c(ix);     // the constant
  float    p  = f(a);                 // expand the polynomial P(x)

  // combine the subresults: here the sign is being applied the polynomial
  // result 'p' but could instead be applied to sqrt result 't'. ideally
  // it would applied to the sub-result that's expected to complete first.
  // roughly very low degree P apply to 'p', otherwise to 't'
  
  return fmaf(t, f32_mulsign(p, sx), c);
#endif  
}


#endif
