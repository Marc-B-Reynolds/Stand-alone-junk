
#ifndef __F32_ACOSPI_SB_H__
#define __F32_ACOSPI_SB_H__

// fast branchfree asinpi targeting minimizing abs error. intended for
// low quality approximations.
// SEE:
// * (put post link here)
//
// Fast & lower quality acospi functions (targeting abs error)
// core approximation on [0,1] using expressions:
//    acospi(x) ~= sqrt(1-x) P(x)
//
// * defines two sets of polynomial approximations P(x)
//     f32_acospi_sb_kr{n} = n^th degree where P(0) ~= 1/2 
//     f32_acospi_sb_ke{n} = n^th degree where P(0)  = 1/2
//   the former has slightly lower error with the one more
//   degree of freedom and the latter is exact for inputs
//   at (and near) zero.
// * defines two functions for expanding the P(x) into usable
//   functions:
//     f32_acospi_sb_xp   = expand an approximation for positive inputs
//     f32_acospi_sb_xf   = expand an approximation for full range
//     f32_acopis_bs_fx_l =   full range (see below)

// NOTES:
//   hybrid (stdfunc/bitops) only reduction. see f32_reduce_odd

//**********************************************************************
// f(0) relaxed spitball polynomials: P(x)
// not forcing result of f(0) to 1/2 gives an additional degree of
// freedom and produces slightly more accurate result.

static inline float f32_acospi_sb_kr1(float x)
{
  static const float C[] = {-0x1.b6bfa6p-5f, 0x1.fef466p-2f };
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sb_kr2(float x)
{
  static const float C[] = { 0x1.0c014ep-6f, -0x1.0bed48p-4f, 0x1.ffe4cap-2f };
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sb_kr3(float x)
{
  static const float C[] = { -0x1.b3d094p-8f, 0x1.9107f6p-6f, -0x1.158bb6p-4f, 0x1.fffcd4p-2f };
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sb_kr4(float x)
{
  static const float C[] = { 0x1.961c7cp-9f, -0x1.88640ap-7f, 0x1.bea05ep-6f, -0x1.1760fcp-4f, 0x1.ffff9ap-2f };
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_kr5(float x)
{
  static const float C[] = {
    -0x1.9989cep-10f, 0x1.adfc26p-8f, -0x1.def6dep-7f,
     0x1.cbd07cp-6f, -0x1.17b844p-4f, 0x1.fffff2p-2f };
  
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sb_kr6(float x)
{
  static const float C[] = {
    0x1.b354bcp-11f, -0x1.f4673p-9f, 0x1.2176c4p-7f, -0x1.008af8p-6f,
    0x1.cf49aap-6f, -0x1.17c876p-4, 0x1.fffffep-2f };
  
  return f32_horner_6(x,C);
}


//**********************************************************************
// f(0) exact spitball polynomials: P(x)

static inline float f32_acospi_sb_ke1(float x)
{
  static const float C[] = {-0x1.c95b42p-5f, 0.5f};
  
  return f32_horner_1(x,C);
}

static inline float f32_acospi_sb_ke2(float x)
{
  static const float C[] = { 0x1.16bf4p-6f, -0x1.0e77f6p-4f, 0.5f};
  
  return f32_horner_2(x,C);
}

static inline float f32_acospi_sb_ke3(float x)
{
  static const float C[] = {-0x1.c374b8p-8f, 0x1.96b008p-6f, -0x1.161a08p-4f, 0.5f};
  
  return f32_horner_3(x,C);
}

static inline float f32_acospi_sb_ke4(float x)
{
  static const float C[] = {0x1.a30de4p-9f,-0x1.8eb0fp-7f,0x1.c09deap-6f,-0x1.177d92p-4f, 0.5f};
  
  return f32_horner_4(x,C);
}

static inline float f32_acospi_sb_ke5(float x)
{
  static const float C[] = {
    -0x1.a58d5ep-10f, 0x1.b5572ep-8f, -0x1.e22fap-7f, 0x1.cc6afp-6f, -0x1.17bdf6p-4f, 0.5f
  };
  
  return f32_horner_5(x,C);
}

static inline float f32_acospi_sb_ke6(float x)
{
  static const float C[] = {
    0x1.bf2db4p-11f, -0x1.fd24cp-9f, 0x1.23eabp-7f, -0x1.0131b8p-6f, 0x1.cf73dep-6f, -0x1.17c992p-4f, 0.5f
  };
  return f32_horner_6(x,C);
}

//**********************************************************************


// given f = P(x) expand approximation restricted to positive input.
// return NaN for negative inputs.
static inline float f32_acospi_sb_xp(float (*f)(float), float x)
{
  float t = f32_sqrt(1.f-x);
  float p = f(x);

  return t*p;
}


// compiler hand holding to keep it branchfree. placeholder for being
// to make it easy to flip a compile time switch if the compiler + arch
// is doing better than the work around.
static inline float f32_acospi_c(float x)
{
#if 1
  return f32_sign_select(1.f, x);
#else
  return (x < 0.f) ? 1.f : 0.f; 
#endif  
}


// given f = P(x) expand full range approximation
static inline float f32_acospi_sb_xf(float (*f)(float), float x)
{
  float a  = fabsf(x);
  float sx = f32_xor(x,a);      // isolate sign bit
  float c  = f32_acospi_c(x);   // (x<0) ? 1 : 0
  float t  = f32_sqrt(1.f-a);
  float p  = f(a);
  
  // combine the subresults: here the sign is being applied the polynomial
  // result 'p' but could instead be applied to sqrt result 't'. ideally
  // it would applied to the sub-result that's expected to complete first.
  // roughly very low degree P apply to 'p', otherwise to 't' (as below)

  return fmaf(t, f32_xor(p,sx), c);  
}

// longer polynomial version (see note above)
static inline float f32_acospi_sb_xf_l(float (*f)(float), float x)
{
  float a  = fabsf(x);
  float sx = f32_xor(x,a);
  float c  = f32_acospi_c(x);
  float t  = f32_sqrt(1.f-a);
  float p  = f(a);
  
  return fmaf(f32_xor(t,sx), p, c);  
}


#endif
