// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#ifndef F32_QUANT_H
#define F32_QUANT_H

// scalar uniform quantization helpers

//**** helper functions:

typedef struct { float h,l; } suq_scale_t;

// local version of generating a pair of constants
// to compute 1/n for correctly rounded decodes.
// SEE: https://marc-b-reynolds.github.io/math/2019/03/12/FpDiv.html
static inline suq_scale_t suq_scale(uint32_t n)
{
  const float y  = (float)n;
  const float rh = 1.f/y;
  const float rl = -fmaf(rh, y, -1.f)/y;

  return (suq_scale_t){.h = rh, .l = rl};
}

//**** scalar uniform encoding funcs

// floor quantization
static inline uint32_t suq_encode_f(float x , uint32_t n)
{
  return (uint32_t)(x * (float)n);
}

// rounded quantization
static inline uint32_t suq_encode_r(float x , uint32_t n)
{
  return (uint32_t)fmaf(x,(float)(n-1), 0.5f);
}


//**** scalar uniform decoding funcs

// center reconstruction
static inline float suq_decode_c(uint32_t u, uint32_t n)
{
  float s = (1.f/(float)n);

  return fmaf((float)u, s, 0.5f*s);
}

// center reconstruction: correctly rounded
// (for properly handling non power-of-two 'n')
static inline float suq_decode_c_cr(uint32_t u, uint32_t n)
{
  const suq_scale_t s = suq_scale(n);

  float f = (float)u + 0.5f;

  return fmaf(f, s.h, f*s.l);
}

// left reconstruction
static inline float suq_decode_l(uint32_t u, uint32_t n)
{
  return (float)u * (1.f/(float)(n-1));
}

// left reconstruction: correctly rounded
// (for properly handling when n-1 isn't a power of two)
static inline float suq_decode_l_cr(uint32_t u, uint32_t n)
{ 
  const suq_scale_t s = suq_scale(n-1);

  float f = (float)u;
  float r = fmaf(f, s.h, f*s.l);

  return r;
}

//**** encode/decode pairs helpers: (assumes n is power-of-two)

// output point set excludes 0 & 1. point distance = 1/n {floor/center}
//   x on [0,1)   -> [0,n-1]
//   u on [0,n-1] -> (0,1)
static inline uint32_t suq_encode_fc(float x ,   uint32_t n) { return suq_encode_f(x,n); }
static inline float    suq_decode_fc(uint32_t u, uint32_t n) { return suq_decode_c(u,n); }


// output point set includes 0 & 1. point distance = 1/(n-1)  {rounded/left}
//   x on [0,1]   -> [0,n-1]
//   u on [0,n-1] -> [0,1]
static inline uint32_t suq_encode_rl(float x,    uint32_t n) { return suq_encode_r(x,n); }
static inline float    suq_decode_rl(uint32_t u, uint32_t n) { return suq_decode_l_cr(u,n); }


// wrapped quantization (assumes n is power-of-two).
// domain of input is x mod 1
// Zero is member of output set (1 is equivalent to 0)
// point distance = 1/n 
//   x on [0,1)   -> [0,n-1]   {well input is mod 1}
//   u on [0,n-1] -> [0,1) 
static inline uint32_t suq_encode_w(float x , uint32_t n)
{
  // not doing anything clever here since n is assumed POT
  return ((uint32_t)fmaf(x,(float)(n), 0.5f)) % n;
}

// wrapped dequantization (assume n is power-of-two)
static inline float suq_decode_w(uint32_t u, uint32_t n)
{
  return (float)u * (1.f/(float)(n));
}

// wrapped reconstruction: correctly rounded
// for when n isn't a power of two. a bit useless ATM
// since there's no special case encode for this
static inline float suq_decode_w_cr(uint32_t u, uint32_t n)
{
  // I'm assuming this will be optimized away into
  // precomputed constants.
  const suq_scale_t s = suq_scale(n);

  float f = (float)u;
  float r = fmaf(f, s.h, f*s.l);

  return r;
}

// SEE: https://fgiesen.wordpress.com/2024/11/06/exact-unorm8-to-float/
static inline float unorm8_decode_cr(uint32_t u)
{
  static const float k = 1.f/(255.f*3.f);

  float f = (float)(3*u);
  float r = k*f;

  return r;
}


#endif

