// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2023

#ifndef __F64_LIBM__
#define __F64_LIBM__

#ifdef __cplusplus
extern "C" {
#ifdef __emacs_hack
}  
#endif  
#endif

#ifndef __BITOPS_H__
#include "bitops.h"
#endif

#ifndef __F64_UTIL_H__
#include "f64_util.h"
#endif


//*************************************************************************
// solve quadratic equation ax^2+bx+c for real roots
// complex roots will yield a standard NaN

// larger magnitude root
static inline double f64_quadratic_max(double a, double b, double c)
{
  double t0 = f64_sqrt(f64_mms(b,b,4.0*a,c));
  double t1 = b+copysign(t0,b);
  return t1/(-2.0*a);
}

// smaller magnitude root
static inline double f64_quadratic_min(double a, double b, double c)
{
  double t0 = f64_sqrt(f64_mms(b,b,4.0*a,c));
  double t1 = b+copysign(t0,b);
  return (-2.0*c)/t1;
}

static inline void f64_quadratic(f64_pair_t* r, double a, double b, double c)
{
  double t0 = f64_sqrt(f64_mms(b,b,4.0*a,c));
  double t1 = b+copysign(t0,b);

  r->h = t1/(-2.0*a);
  r->l = (-2.0*c)/t1;
}



#ifdef __cplusplus
}
#endif

#endif

