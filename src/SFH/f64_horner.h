// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2026

// evalulate n^th degree polynomial in Horner's form. Coefficient arrays are from high to low.

#pragma once
#define F64_HORNER

#include <math.h>

#if !defined(_MSC_VER)
#define f64_horner_coeff(N) const double C[static N]
#else
#define f64_horner_coeff(N) const double* C
#endif

static inline double f64_horner_1(double x, f64_horner_coeff(2))
{
  return fma(x,C[0],C[1]);
}


static inline double f64_horner_2 (double x, f64_horner_coeff( 3)) { return fma(x,f64_horner_1 (x,C),C[ 2]); }
static inline double f64_horner_3 (double x, f64_horner_coeff( 4)) { return fma(x,f64_horner_2 (x,C),C[ 3]); }
static inline double f64_horner_4 (double x, f64_horner_coeff( 5)) { return fma(x,f64_horner_3 (x,C),C[ 4]); }
static inline double f64_horner_5 (double x, f64_horner_coeff( 6)) { return fma(x,f64_horner_4 (x,C),C[ 5]); }
static inline double f64_horner_6 (double x, f64_horner_coeff( 7)) { return fma(x,f64_horner_5 (x,C),C[ 6]); }
static inline double f64_horner_7 (double x, f64_horner_coeff( 8)) { return fma(x,f64_horner_6 (x,C),C[ 7]); }
static inline double f64_horner_8 (double x, f64_horner_coeff( 9)) { return fma(x,f64_horner_7 (x,C),C[ 8]); }
static inline double f64_horner_9 (double x, f64_horner_coeff(10)) { return fma(x,f64_horner_8 (x,C),C[ 9]); }
static inline double f64_horner_10(double x, f64_horner_coeff(11)) { return fma(x,f64_horner_9 (x,C),C[10]); }
static inline double f64_horner_11(double x, f64_horner_coeff(12)) { return fma(x,f64_horner_10(x,C),C[11]); }
static inline double f64_horner_12(double x, f64_horner_coeff(13)) { return fma(x,f64_horner_11(x,C),C[12]); }
static inline double f64_horner_13(double x, f64_horner_coeff(14)) { return fma(x,f64_horner_12(x,C),C[13]); }
static inline double f64_horner_14(double x, f64_horner_coeff(15)) { return fma(x,f64_horner_13(x,C),C[14]); }
static inline double f64_horner_15(double x, f64_horner_coeff(16)) { return fma(x,f64_horner_14(x,C),C[15]); }

