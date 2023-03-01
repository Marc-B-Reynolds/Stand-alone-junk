#ifndef __UTIL_H__
#define __UTIL_H__

static inline uint32_t f32_to_bits(float x)
{
  uint32_t u; memcpy(&u,&x,4); return u;
}

static inline float f32_from_bits(uint32_t x)
{
  float u; memcpy(&u,&x,4); return u;
}

// if 'v' is float and 's' is all clear (except sign bit)
static inline float f32_mulsign(float v, uint32_t s)
{
  return f32_from_bits(f32_to_bits(v)^s);
}

static inline uint32_t u32_abs(uint32_t x)
{
  return (int32_t)x >= 0 ? x : -x;
}

// ulp distance provided a & b are finite
// and have the same sign
static inline uint32_t f32_ulp_dist_ss(float a, float b)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);
  return u32_abs(ua-ub);
}

const double f64_pi      = 0x1.921fb54442d18p1;
const double f64_half_pi = 0.5*f64_pi;

const uint32_t f32_bits_one = 0x3f800000;

const float  f32_pi      = 0x1.921fb6p1f;
const float  f32_half_pi = 0.5f*f32_pi;
const float  PIO2F = 0.5f*0x1.921fb6p1f;

typedef struct { float h,l; } f32_pair_t;

// pi as unevaluated pair
const f32_pair_t f32_up_pi   = {.h=0x1.921fb6p1f, .l=-0x1.777a5cp-24f };
const f32_pair_t f32_up_pio2 = {.h=0x1.921fb6p0f, .l=-0x1.777a5cp-23f };

// pi as multiplicative pair (h*l)
const f32_pair_t f32_mk_pi    = {.h = (float)(61*256661), .l= (float)(13*73*14879)*0x1.0p-46f};
const float      f32_mk_pi_lo = -0x1.1ee59ep-45f;


static inline float f32_add_pi(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x);
}

static inline float f32_add_pi_x(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x + f32_mk_pi_lo);
}

// fma variant of : pi/2 + x
static inline float f32_add_half_pi(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = 0.5f*f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x);
}

static inline float f32_add_half_pi_x(float x)
{
  const float pi_a = f32_mk_pi.h;
  const float pi_b = 0.5f*f32_mk_pi.l;

  return fmaf(pi_a, pi_b, x + (0.5f*f32_mk_pi_lo));
}


// high to low coeffificent ordering
inline float f32_horner_1(float x, const float C[static 2])
{
  return fmaf(x,C[0],C[1]);
}

inline float f32_horner_2(float x, const float C[static 3])
{
  return fmaf(x,fmaf(x,C[0],C[1]),C[2]);
}

inline float f32_horner_3(float x, const float C[static 4])
{
  return fmaf(x,fmaf(x,fmaf(x,C[0],C[1]),C[2]),C[3]);
}

inline float f32_horner_4(float x, const float C[static 5])
{
  return fmaf(x,fmaf(x,fmaf(x,fmaf(x,C[0],C[1]),C[2]),C[3]),C[4]);
}

inline float f32_horner_5(float x, const float C[static 6])
{
  return fmaf(x,fmaf(x,fmaf(x,fmaf(x,fmaf(x,C[0],C[1]),C[2]),C[3]),C[4]),C[5]);
}

inline float f32_horner_6(float x, const float C[static 7])
{
  return fmaf(x,fmaf(x,fmaf(x,fmaf(x,fmaf(x,fmaf(x,C[0],C[1]),C[2]),C[3]),C[4]),C[5]),C[6]);
}

//************************************************************************

inline double f64_horner_1(double x, const double C[static 2])
{
  return fma(x,C[0],C[1]);
}

inline double f64_horner_2(double x, const double C[static 3])
{
  return fma(x,fma(x,C[0],C[1]),C[2]);
}

inline double f64_horner_3(double x, const double C[static 4])
{
  return fma(x,fma(x,fma(x,C[0],C[1]),C[2]),C[3]);
}

inline double f64_horner_4(double x, const double C[static 5])
{
  return fma(x,fma(x,fma(x,fma(x,C[0],C[1]),C[2]),C[3]),C[4]);
}

inline double f64_horner_5(double x, const double C[static 6])
{
  return fma(x,fma(x,fma(x,fma(x,fma(x,C[0],C[1]),C[2]),C[3]),C[4]),C[5]);
}

//************************************************************************
// stuff for the drivers

typedef struct {
  float (*f)(float);
  char*   name;
} func_entry_t;

#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))
#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S
#define ENTRY(X) { .f=&X, .name=STRINGIFY(X) }


#endif
