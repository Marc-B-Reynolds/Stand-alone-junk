// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2020-2023


#ifndef F32_SINCOSPI_H
#define F32_SINCOSPI_H

static inline float f32_sin_pi_k3(float x)
{
  static const float C[] = {-0x1.2f5992p-1f, 0x1.466b2p1f, -0x1.4abbecp2f, 0x1.921fb6p1f};

  return f32_horner_3(x,C);
}


static inline float f32_sin_pi_k4(float x)
{
  static const float C[] = {0x1.eb5482p-3f, -0x1.3e497cp-1f, 0x1.468e6cp1f, -0x1.4abc1cp2f, 0x1.921fb6p1f};

  return f32_horner_4(x,C);
}


static inline float f32_sin_pi_k5(float x)
{
  static const float C[] = {-0x1.054c4ap3f, 0x1.7dd156p0f, -0x1.603544p-1f, 0x1.46bbd8p1f, -0x1.4abc42p2f, 0x1.921fb6p1f};

  return f32_horner_5(x,C);
}


static inline float f32_sin_pi_k6(float x)
{
  static const float C[] = {0x1.b64fb2p8f, -0x1.65c5fp6f, 0x1.c6e31cp2f, -0x1.ba9fd4p-1f, 0x1.470b92p1f, -0x1.4abc7p2f, 0x1.921fb6p1f};

  return f32_horner_6(x,C);
}


static inline float f32_sin_pi_xp_k4(float x)
{
  static const float C[] = {-0x1.2d9e5ap-1f, 0x1.465edcp1f, -0x1.4abbbap2f};

  float s = x*x;
  float r;

  r = f32_horner_2(s,C);
  r = fmaf(r,s, -0x1.9e5ee4p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}


static inline float f32_sin_pi_xp_k5(float x)
{
  static const float C[] = {0x1.48208cp-4f, -0x1.32babap-1f, 0x1.466b8ep1f, -0x1.4abbcep2f};

  float s = x*x;
  float r;

  r = f32_horner_3(s,C);
  r = fmaf(r,s, -0x1.7883b8p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}


static inline float f32_sin_pi_xp_k6(float x)
{
  static const float C[] = {0x1.f095fp-6f, 0x1.36acd8p-4f, -0x1.32a156p-1f, 0x1.466b76p1f, -0x1.4abbcep2f};

  float s = x*x;
  float r;

  r = f32_horner_4(s,C);
  r = fmaf(r,s, -0x1.783058p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}


static inline float f32_sin_pi_xp_k7(float x)
{
  static const float C[] = {-0x1.87e56cp0f, 0x1.3048acp-2f, 0x1.e3e2f2p-5f, -0x1.326692p-1f, 0x1.466b52p1f, -0x1.4abbcep2f};

  float s = x*x;
  float r;

  r = f32_horner_5(s,C);
  r = fmaf(r,s, -0x1.77f8c8p-24f);
  r = fmaf(x, 0x1.921fb6p1f, x*r);

  return r;
}



#endif
