// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2020-2023


#ifndef F32_SINCOSPI_H
#define F32_SINCOSPI_H

// http://marc-b-reynolds.github.io/math/2020/03/11/SinCosPi.html
// original post version with reworked expression. one less constant
// load vs. f32_sinpi_k6
static inline float f32_sinpi_o6(float a)
{
  // blog post ordering of coefficients
  static const float C[] =
    { 0x1.921fb6p1f,  -0x1.4abbecp2f, 0x1.466b2p1f,  -0x1.2f5992p-1f };

  float r,a2,a3;
  
  a2 = a*a; a3 = a2*a;
  r = fmaf(C[3], a2, C[2]);
  r = fmaf(r,    a2, C[1]); 
  r = a3 * r;
  r = fmaf(C[0], a,  r);

  return r;
}

//************* sinpi relative error

static inline float f32_sinpi_k5(float x)
{
  static const float C[] = {0x1.3f3dep1f, -0x1.4aa60ap2f};

  float s = x*x;
  float r;

  r = f32_horner_1(s,C);
  r = fmaf(r,s, -0x1.db31b8p-25f);
  r = fmaf(x, 0x1.921f8ep1f, x*r);

  return r;
}

static inline float f32_sinpi_k6(float x)
{
  static const float C[] = {-0x1.2d9e5ap-1f, 0x1.465edcp1f, -0x1.4abbbap2f};

  float s = x*x;
  float r;

  r = f32_horner_2(s,C);
  r = fmaf(r,s, -0x1.9e5ee4p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}

static inline float f32_sinpi_k7(float x)
{
  static const float C[] = {0x1.48208cp-4f, -0x1.32babap-1f, 0x1.466b8ep1f, -0x1.4abbcep2f};

  float s = x*x;
  float r;

  r = f32_horner_3(s,C);
  r = fmaf(r,s, -0x1.7883b8p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}

// one more term version only as tiny effect
static inline float f32_sinpi_k8(float x)
{
  static const float C[] = {0x1.f095fp-6f, 0x1.36acd8p-4f, -0x1.32a156p-1f, 0x1.466b76p1f, -0x1.4abbcep2f};

  float s = x*x;
  float r;

  r = f32_horner_4(s,C);
  r = fmaf(r,s, -0x1.783058p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}

static inline float f32_sinpi_d6(float v)
{
  static const double C[] = {0x1.4bc25574ce357p-4, -0x1.32ca854cad10ep-1, 0x1.466bba8bfcp1, -0x1.4abbce564cd85p2, 0x1.921fb5443af5fp1};

  double x = (double)v;
  double s = x*x;

  return (float)(x*f64_horner_4(s,C));
}


//************* sinpi absolute error

static inline float f32_sinpi_a5(float x)
{
  static const float C[] = {0x1.3e2042p1f, -0x1.4a9ac4p2f};

  float s = x*x;
  float r;

  r = f32_horner_1(s,C);
  r = fmaf(r,s, -0x1.12767p-24f);
  r = fmaf(x, 0x1.921f32p1f, x*r);

  return r;
}

static inline float f32_sinpi_a6(float x)
{
  static const float C[] = {-0x1.2cf9fep-1f, 0x1.465a5cp1f, -0x1.4abba8p2f};

  float s = x*x;
  float r;

  r = f32_horner_2(s,C);
  r = fmaf(r,s, 0x1.d2c0fp-24f);
  r = fmaf(x, 0x1.921fb4p1f, x*r);

  return r;
}

//************* cospi

// kernel codomain on [sqrt(2)/2, 1] and optimizing for abs outperforms wrt
// relative error as well.

// not exact at f(0)
static inline float f32_cospi_k3(float x)
{
  static const float C[] = {0x1.f7b478p1f, -0x1.3ba49ep2f, 0x1.fffeb2p-1f};

  return f32_horner_2(x*x,C);
}

static inline float f32_cospi_k4(float x)
{
  static const float C[] = {-0x1.4ea1aep0f, 0x1.03b132p2f, -0x1.3bd3ap2f, 1.f};

  return f32_horner_3(x*x,C);
}

static inline float f32_cospi_k5(float x)
{
  static const float C[] = {0x1.d61e28p-3f, -0x1.55b684p0f, 0x1.03c1b6p2f, -0x1.3bd3ccp2f, 1.f};

  return f32_horner_4(x*x,C);
}

// promote to double kernel (exact at f(0) due to rounding)
static inline float f32_cospi_d5(float x)
{
  static const double C[] = {0x1.d9c364eac5b26p-3, -0x1.55c57b06db5e8p0, 0x1.03c1dc1bafc62p2, -0x1.3bd3cc7323531p2, 0x1.ffffffff97c47p-1};

  double d = (double)x;
  double s = d*d;

  // using second order doesn't change error
  return (float)f64_horner2_4(s,C);
}

#endif
