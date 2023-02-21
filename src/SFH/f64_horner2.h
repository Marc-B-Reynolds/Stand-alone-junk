// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2015-2023

// evalulate n^th degree polynomial in Horner's form. Coefficient arrays are from high to low (c_n,... c_0)

// WARNING: hacked..revalidate. also clean-up (odd/even pairing?)

#ifndef F64_HORNER2
#define F64_HORNER2

// second order versions (layout compat with standard)

// odd
static inline double f64_horner2_3(const double x, const double c[static 4])
{
  double x2 = x*x;
  double e,o;
  e =          c[1];  o =          c[0];
  e = fma(e,x2,c[3]); o = fma(o,x2,c[2]);

  return fma(x,o,e);
}

static inline double f64_horner2_5(const double x, const double c[static 6])
{
  double x2 = x*x;
  double e,o;
  e =          c[1];  o =          c[0];
  e = fma(e,x2,c[3]); o = fma(o,x2,c[2]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);

  return fma(x,o,e);
}

static inline double f64_horner2_7(const double x, const double c[static 8])
{
  double x2 = x*x;
  double e,o;
  e =          c[1];  o =          c[0];
  e = fma(e,x2,c[3]); o = fma(o,x2,c[2]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);
  e = fma(e,x2,c[6]); o = fma(o,x2,c[7]);

  return fma(x,o,e);
}

static inline double f64_horner2_9(const double x, const double c[static 10])
{
  double x2 = x*x;
  double e,o;
  e =          c[1];  o =          c[0];
  e = fma(e,x2,c[3]); o = fma(o,x2,c[2]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);
  e = fma(e,x2,c[6]); o = fma(o,x2,c[7]);
  e = fma(e,x2,c[8]); o = fma(o,x2,c[9]);

  return fma(x,o,e);
}

static inline double f64_horner2_11(const double x, const double c[static 12])
{
  double x2 = x*x;
  double e,o;
  e =          c[ 1];  o =          c[ 0];
  e = fma(e,x2,c[ 3]); o = fma(o,x2,c[ 2]);
  e = fma(e,x2,c[ 4]); o = fma(o,x2,c[ 5]);
  e = fma(e,x2,c[ 6]); o = fma(o,x2,c[ 7]);
  e = fma(e,x2,c[ 8]); o = fma(o,x2,c[ 9]);
  e = fma(e,x2,c[10]); o = fma(o,x2,c[11]);

  return fma(x,o,e);
}

// even

static inline double f64_horner2_4(const double x, const double c[static 5])
{
  double x2 = x*x;
  double e,o;
  e =          c[0];  o =          c[1];
  e = fma(e,x2,c[2]); o = fma(o,x2,c[3]);
  e = fma(e,x2,c[4]); 

  return fma(x,o,e);
}

static inline double f64_horner2_6(const double x, const double c[static 7])
{
  double x2 = x*x;
  double e,o;
  e =          c[0];  o =          c[1];
  e = fma(e,x2,c[2]); o = fma(o,x2,c[3]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);
  e = fma(e,x2,c[6]); 

  return fma(x,o,e);
}

static inline double f64_horner2_8(const double x, const double c[static 9])
{
  double x2 = x*x;
  double e,o;
  e =          c[0];  o =          c[1];
  e = fma(e,x2,c[2]); o = fma(o,x2,c[3]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);
  e = fma(e,x2,c[6]); o = fma(o,x2,c[7]);
  e = fma(e,x2,c[8]); 

  return fma(x,o,e);
}

static inline double f64_horner2_10(const double x, const double c[static 11])
{
  double x2 = x*x;
  double e,o;
  e =          c[0];  o =          c[1];
  e = fma(e,x2,c[2]); o = fma(o,x2,c[3]);
  e = fma(e,x2,c[4]); o = fma(o,x2,c[5]);
  e = fma(e,x2,c[6]); o = fma(o,x2,c[7]);
  e = fma(e,x2,c[8]); o = fma(o,x2,c[9]);
  e = fma(e,x2,c[10]); 

  return fma(x,o,e);
}
 

#endif

