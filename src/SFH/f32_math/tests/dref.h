// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2026

#pragma once

// taking double value 'r' to be exact and single value 'a' as
// the approximation: return single ULP distance as a double.
// So a correctly rounded result should return a value < 1/2.
static inline double dref_ulp_dist(double r, float a)
{
  double d   = (double)a;
  double ulp = (double)f64_ulp_dist(r,d);

  if (fabs(a) > f32_min_normal)
    return ulp * 0x1.0p-29;

  // handle denormals & specials (punting ATM since I'm not checking these)
  float fr = (float)r;

  return (double)f32_ulp_dist(fr,a);
}
