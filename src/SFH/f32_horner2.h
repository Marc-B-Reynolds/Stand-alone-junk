// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2015-2026
// Public Domain under http://unlicense.org, see link for details.

// evalulate n^th degree polynomial in Horner's form. Coefficient arrays are from high to low (c_n,... c_0)

// WARNING: hacked..revalidate. also clean-up (odd/even pairing?)

#ifndef F32_HORNER2
#define F32_HORNER2

// second order versions (layout compat with standard)

// odd
static inline float f32_horner2_3(const float x, const float c[static 4])
{
  float x2 = x*x;
  float e,o;
  e =           c[1];  o =           c[0];
  e = fmaf(e,x2,c[3]); o = fmaf(o,x2,c[2]);

  return fmaf(x,o,e);
}

static inline float f32_horner2_5(const float x, const float c[static 6])
{
  float x2 = x*x;
  float e,o;
  e =           c[1];  o =           c[0];
  e = fmaf(e,x2,c[3]); o = fmaf(o,x2,c[2]);
  e = fmaf(e,x2,c[5]); o = fmaf(o,x2,c[4]);

  return fmaf(x,o,e);
}

static inline float f32_horner2_7(const float x, const float c[static 8])
{
  float x2 = x*x;
  float e,o;
  e =           c[1];  o =           c[0];
  e = fmaf(e,x2,c[3]); o = fmaf(o,x2,c[2]);
  e = fmaf(e,x2,c[5]); o = fmaf(o,x2,c[4]);
  e = fmaf(e,x2,c[7]); o = fmaf(o,x2,c[6]);

  return fmaf(x,o,e);
}

static inline float f32_horner2_9(const float x, const float c[static 10])
{
  float x2 = x*x;
  float e,o;
  e =           c[1];  o =           c[0];
  e = fmaf(e,x2,c[3]); o = fmaf(o,x2,c[2]);
  e = fmaf(e,x2,c[5]); o = fmaf(o,x2,c[4]);
  e = fmaf(e,x2,c[7]); o = fmaf(o,x2,c[6]);
  e = fmaf(e,x2,c[9]); o = fmaf(o,x2,c[8]);

  return fmaf(x,o,e);
}

static inline float f32_horner2_11(const float x, const float c[static 12])
{
  float x2 = x*x;
  float e,o;
  e =           c[ 1];  o =           c[ 0];
  e = fmaf(e,x2,c[ 3]); o = fmaf(o,x2,c[ 2]);
  e = fmaf(e,x2,c[ 5]); o = fmaf(o,x2,c[ 4]);
  e = fmaf(e,x2,c[ 7]); o = fmaf(o,x2,c[ 6]);
  e = fmaf(e,x2,c[ 9]); o = fmaf(o,x2,c[ 8]);
  e = fmaf(e,x2,c[11]); o = fmaf(o,x2,c[10]);

  return fmaf(x,o,e);
}

// even

static inline float f32_horner2_4(const float x, const float c[static 5])
{
  float x2 = x*x;
  float e,o;
  e =           c[0];  o =           c[1];
  e = fmaf(e,x2,c[2]); o = fmaf(o,x2,c[3]);
  e = fmaf(e,x2,c[4]); 

  return fmaf(x,o,e);
}

static inline float f32_horner2_6(const float x, const float c[static 7])
{
  float x2 = x*x;
  float e,o;
  e =           c[0];  o =           c[1];
  e = fmaf(e,x2,c[2]); o = fmaf(o,x2,c[3]);
  e = fmaf(e,x2,c[4]); o = fmaf(o,x2,c[5]);
  e = fmaf(e,x2,c[6]); 

  return fmaf(x,o,e);
}

static inline float f32_horner2_8(const float x, const float c[static 9])
{
  float x2 = x*x;
  float e,o;
  e =           c[0];  o =           c[1];
  e = fmaf(e,x2,c[2]); o = fmaf(o,x2,c[3]);
  e = fmaf(e,x2,c[4]); o = fmaf(o,x2,c[5]);
  e = fmaf(e,x2,c[6]); o = fmaf(o,x2,c[7]);
  e = fmaf(e,x2,c[8]); 

  return fmaf(x,o,e);
}

static inline float f32_horner2_10(const float x, const float c[static 11])
{
  float x2 = x*x;
  float e,o;
  e =           c[0];  o =           c[1];
  e = fmaf(e,x2,c[2]); o = fmaf(o,x2,c[3]);
  e = fmaf(e,x2,c[4]); o = fmaf(o,x2,c[5]);
  e = fmaf(e,x2,c[6]); o = fmaf(o,x2,c[7]);
  e = fmaf(e,x2,c[8]); o = fmaf(o,x2,c[9]);
  e = fmaf(e,x2,c[10]); 

  return fmaf(x,o,e);
}
 

#endif

