// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2016-2026
// Public Domain under http://unlicense.org, see link for details.

// evalulate n^th degree polynomial in Horner's form. Coefficient arrays are from high to low (c_n,... c_0)

#pragma once
#define F32_HORNER

#include <math.h>

#if !defined(_MSC_VER)
#define f32_horner_coeff(N) const float C[static N]
#else
#define f32_horner_coeff(N) const float* C
#endif

typedef float (f32_horner_t)(float, const float * const);

static inline float f32_horner_1(float x, f32_horner_coeff(2))
{
  return fmaf(x,C[0],C[1]);
}

static inline float f32_horner_2 (float x, f32_horner_coeff( 3)) { return fmaf(x,f32_horner_1 (x,C),C[ 2]); }
static inline float f32_horner_3 (float x, f32_horner_coeff( 4)) { return fmaf(x,f32_horner_2 (x,C),C[ 3]); }
static inline float f32_horner_4 (float x, f32_horner_coeff( 5)) { return fmaf(x,f32_horner_3 (x,C),C[ 4]); }
static inline float f32_horner_5 (float x, f32_horner_coeff( 6)) { return fmaf(x,f32_horner_4 (x,C),C[ 5]); }
static inline float f32_horner_6 (float x, f32_horner_coeff( 7)) { return fmaf(x,f32_horner_5 (x,C),C[ 6]); }
static inline float f32_horner_7 (float x, f32_horner_coeff( 8)) { return fmaf(x,f32_horner_6 (x,C),C[ 7]); }
static inline float f32_horner_8 (float x, f32_horner_coeff( 9)) { return fmaf(x,f32_horner_7 (x,C),C[ 8]); }
static inline float f32_horner_9 (float x, f32_horner_coeff(10)) { return fmaf(x,f32_horner_8 (x,C),C[ 9]); }
static inline float f32_horner_10(float x, f32_horner_coeff(11)) { return fmaf(x,f32_horner_9 (x,C),C[10]); }
static inline float f32_horner_11(float x, f32_horner_coeff(12)) { return fmaf(x,f32_horner_10(x,C),C[11]); }
static inline float f32_horner_12(float x, f32_horner_coeff(13)) { return fmaf(x,f32_horner_11(x,C),C[12]); }
static inline float f32_horner_13(float x, f32_horner_coeff(14)) { return fmaf(x,f32_horner_12(x,C),C[13]); }
static inline float f32_horner_14(float x, f32_horner_coeff(15)) { return fmaf(x,f32_horner_13(x,C),C[14]); }
static inline float f32_horner_15(float x, f32_horner_coeff(16)) { return fmaf(x,f32_horner_14(x,C),C[15]); }

