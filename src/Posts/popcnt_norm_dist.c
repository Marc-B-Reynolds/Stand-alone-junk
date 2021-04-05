// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/distribution/2021/03/18/CheapGaussianApprox.html

// Mathematica notebook: to be added (sigh)

// NOTE:
// 0) I need to update the post for clarity
// 1) The code in this and the post are intended as "proof of concept". An actually
//    efficient implementation is going to be ISA/usage specific. One main point is the
//    method allows for vectorization. As has been noted on tweeter and post comments
//    even with hardware population count..a software implementation can win on
//    specific hardware.
// 2) As noted the versions that draw two 64-bit uniform integers can draw from
//    different sequences. The bits of these can be partitioned in different ways
//    than these examples. Also how the bits are used can allow for weaker generators
//    to be used. The 2-draw example implmementation implementations draw all the pop
//    count from one (so these needs to perform well at bit independence test) and
//    the triange distribution from the other (standard linear tests). Also given
//    the density errors produced neither generator needs to pass BigCrush testing.
//    Crush is certainly sufficient for both (SmallCrush is really good enough)
// 3) All testing and scaling constant generation is based on empirical testing.
//    However we know that the cut binomial distribution will be faithfully
//    produced (assuming sufficient base generator) and that the convolution
//    with the triangle distribution will produce a linear interpolation on
//    each interval between these. So an optimal scaling constant and exact
//    error bound can be computed.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// having a hickup with odd (sigh). stick to POT
//#define BINS    128  // used for post plots
#define BINS    4096 // used for constant search and error bounds.

// extent of histogram sampling: [-SAMPLESX,SIMPLESX]
// more or less hardcoded. POT is nice for speed &
// accuracy w/o working hard and covers erf(2 Sqrt[2])
// of the function. (~99.9937% of samples on range)
#define SAMPLESX 4

// width of each histogram bin (and it's inverse)
#define BINW    (2.0*(double)SAMPLESX/(double)BINS)
#define IBINW   (0.5*(double)BINS/(double)SAMPLESX)


//----------- kill
// default sampling length (POT is good)

//#define LEN 0x10000000    // for quick hacking & initial searches..yeah lazy
#define LEN 0x80000000      // used for constant search and error bounds

// if defined random sequences differ per run (otherwise same every
// time) only setup for intel (uses RDTSC)
//#define RANDOMIZE

// if define uses two generators for uniform random draws
#define TWO_SEQUENCES

#define REPORT(FLAGS, X) { if (FLAGS & REPORTS_FLAGS) {X;} }

// helper functions
inline float sgn(float x) { return copysignf(1.f,x); }

inline uint32_t lzc64(uint64_t x) { return (uint32_t)__builtin_clzl(x); }
inline uint32_t lzc32(uint32_t x) { return (uint32_t)__builtin_clz(x); }
inline uint32_t pop32(uint32_t x) { return (uint32_t)__builtin_popcount(x); }
inline uint32_t pop64(uint64_t x) { return (uint32_t)__builtin_popcountl(x); }

inline uint32_t f32_to_bits(float x)      { uint32_t u; memcpy(&u, &x, 4); return u; }
inline float    f32_from_bits(uint32_t x) { float u;    memcpy(&u, &x, 4); return u; }

// move 'i' ulp steps away from (for positive) or toward (for negative) zero.  Doesn't deal
// with crossing the zero, entering/leaving NaNs or infinities.
inline float f32_add_ulp(float a, int32_t i) { return f32_from_bits(f32_to_bits(a)+(uint32_t)i); }

void f32_array_print_fmt(float* a, uint32_t len, char* fmt)
{
  printf("{");
  for(uint32_t i=0; i<len; i++) {
    printf(fmt, a[i]);
    if (i != len-1) printf(",");
  }
  printf("}\n");
}

void f32_array_print_f(float* a, uint32_t len) { f32_array_print_fmt(a,len,"%f");}
void f32_array_print_e(float* a, uint32_t len) { f32_array_print_fmt(a,len,"%e");}
void f32_array_print_a(float* a, uint32_t len) { f32_array_print_fmt(a,len,"%a");}

// Welford's method for streaming mean/variance/stdev
typedef struct { double n,m,s; } seq_stats_t;

inline void seq_stats_init(seq_stats_t* d)
{
  memset(d,0,sizeof(seq_stats_t));
}

void seq_stats_add(seq_stats_t* d, float v)
{
  double x = (double)v;
  
  d->n += 1.0;

  double m  = d->m;
  double s  = d->s;
  double dm = x-m;
  
  d->m = m + dm/d->n;
  d->s = s + dm*(x-d->m);
}

inline double seq_stats_mean(seq_stats_t* d)     { return d->m; }
inline double seq_stats_variance(seq_stats_t* d) { return d->s/(d->n-1.0); }
inline double seq_stats_stddev(seq_stats_t* d)   { return sqrt(seq_stats_variance(d)); }

void seq_stats_print(seq_stats_t* d)
{
  printf("mean=%f,variance=%f,std-dev=%f, count=%u\n",
	 d->m,
	 seq_stats_variance(d),
	 seq_stats_stddev(d),
	 (uint32_t)d->n
	 );
}

// external code: xoroshiro128+
// used for generating 'u0' so needs to perform well at bit independence
// at all expected sample sizes per problem (and it does up to BigCrush)

uint64_t u0_state[2];

#define TO_FP32 (1.f/16777216.f)
#define ULP1 TO_FP32
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#define EPS (F32_MIN_NORMAL)

inline uint64_t rotl(const uint64_t v, int i)
{
  return (v << i)|(v >> (64-i));
}

inline uint64_t rng_u0(void)
{
  uint64_t s0 = u0_state[0];
  uint64_t s1 = u0_state[1];
  uint64_t r  = s0 + s1;
  
  s1 ^= s0;
  u0_state[0] = rotl(s0,55) ^ s1 ^ (s1<<14);
  u0_state[1] = rotl(s1,36);
  
  return r;
}
// end: xoroshiro128+


// generator for 'u1' which is used for sum of uniforms. Needs to
// peform well at standard uniformity tests for approp sample
// sizes/problem.
#ifndef TWO_SEQUENCES

inline uint64_t rng_u1(void)
{
  return rng_u0();
}

inline void rng_u1_reset() {}

#else

// just using an xoroshiro128+ style generator ATM
// with a different state update function. Hack..no
// good for vectorized as an example.

uint64_t u1_state[2];

inline uint64_t rng_u1(void)
{
  uint64_t s0 = u1_state[0];
  uint64_t s1 = u1_state[1];
  uint64_t r  = s0 + s1;
  
  s1 ^= s0;
  u1_state[0] = rotl(s0,49) ^ s1 ^ (s1<<21);
  u1_state[1] = rotl(s1,28);
  
  return r;
}

inline void rng_u1_reset()
{
  u1_state[0] = u0_state[0];
  u1_state[1] = u0_state[1];
  rng_u1();
}

#endif

void reset_generators(uint64_t s0, uint64_t s1)
{
  u0_state[0] = s0;
  u0_state[1] = s1;
  rng_u0();
  rng_u1_reset();
}

 float  rng_f32(void) { return (float) (rng_u0() >> (64-24))* 0x1p-24f; }
inline double rng_f64(void) { return (double)(rng_u0() >> (64-53))* 0x1p-53;  }

//--------------------

// correctly rounded: 1/sqrt(2) = sqrt(2)/2
const double f64_sqrt2_2 = 0x1.6a09e667f3bcdp-1;

float  ref_data[BINS];
double ref_expected[BINS];

inline int32_t sample_id(float x);

// Build the reference PDF approximation (to match histogram bins)
// 1) Integrate PDF over the interval of the bin to get probability
//    that a sample falls inside it:
//    (erf(b/sqrt(2))-erf(a/sqrt(2)))/2
// 2) Divide by the bin width to convert probability into density
void ref_build()
{
#if ((BINS & 1) == 0)
  double x  = 0.0;
#else
  double x  = -0.5*BINW*f64_sqrt2_2;
#endif  
  double dx = BINW*f64_sqrt2_2;
  int    i1 = BINS>>1;
  int    i0 = i1 - ((BINS & 1)^1);
  double va = erf(x);
  double vb;

  do {
    // change to product (fma for odd) based instead of addition
    x += dx;  
    vb = erf(x);
  //double e = 0.0;
    // this will get wanky (too much cancellation) for large
    // bin sizes...haven't look at this.
    float  v = (float)((0.5*IBINW)*(vb-va));
    ref_data[i0--] = ref_data[i1++] = v;
    va = vb;
  } while (i0 >= 0);

  if (i0 != -1)   printf("opps: %i\n", i0);
  if (i1 != BINS) printf("opps: %i\n", i1);

  //f32_array_print_e(ref_data,BINS);
  //f32_array_print_a(ref_data,BINS);
}


// Estimate the scaling constant by sampling the
// function and computing the standard deviation.
// This is a black box method. 
float estimate_K(float (*f)(float), float K0)
{
  seq_stats_t stats;
  seq_stats_init(&stats);

  // the guess shouldn't really matter much
  if (K0 == 0.f) K0 = 0x1.0p-32f;
  
  printf("  estimate K: ");
  fflush(stdout);

  // was thinking about adaptive...but meh.
  do {
    for(uint32_t i=0; i<LEN; i++) {
      float x = f(K0);
      seq_stats_add(&stats,x);
    }
  } while(0);
  
  float K = (float)((double)K0/seq_stats_stddev(&stats));
  
  printf("%a (~%e)\n", K,K);

  return (float)K;
}

// (1/sqrt(2pi)) / (binomal(B,B>>1) 2^-B - B)
const double popcnt_k[] = {0.7978845608, 0.7978845608, 1.063846081, 1.063846081, 1.276615297, 1.276615297, 1.458988911, 1.458988911, 1.621098790, 1.621098790, 1.768471407, 1.768471407, 1.904507670, 1.904507670, 2.031474848, 2.031474848, 2.150973368, 2.150973368, 2.264182493, 2.264182493, 2.372000707, 2.372000707, 2.475131172, 2.475131172, 2.574136419, 2.574136419, 2.669474805, 2.669474805, 2.761525660, 2.761525660, 2.850607133, 2.850607133, 2.936989167, 2.936989167, 3.020903144, 3.020903144, 3.102549175, 3.102549175, 3.182101718, 3.182101718, 3.259713955, 3.259713955, 3.335521256, 3.335521256, 3.409643950, 3.409643950, 3.482189566, 3.482189566, 3.553254660, 3.553254660, 3.622926320, 3.622926320, 3.691283420, 3.691283420, 3.758397664, 3.758397664, 3.824334465, 3.824334465, 3.889153693, 3.889153693, 3.952910311, 3.952910311, 4.015654919, 4.015654919};

// given the number of bits of the population count and how many bit
// positions it's left shifted: compute the scaling constant that
// maps 'zero' to the proper density for a standard normal distribution.
// This will be too small for a minimax constant.
float estimate_popcnt_K(uint32_t popbits, uint32_t fractionbits)
{
  uint64_t fb = UINT64_C(1) << fractionbits;

  return (float)(((double)fb)*popcnt_k[popbits-1]);
}

// return histogram index of 'x'. yeah I could
// clean this up & nuke the floor...meh.
inline int32_t sample_id(float x)
{
  double  f = (x * 1.0/(2.0*SAMPLESX)) + 0.5;
  return (int32_t)floor(((double)BINS*f));
}

inline void sample_add(uint32_t* h, float x)
{
  int32_t i = sample_id(x);
  
  if ((uint32_t)i < BINS)
    h[i]++;
}


void sample_dist(float (*f)())
{
  uint32_t h[BINS] = {0};

  seq_stats_t stats;
  seq_stats_init(&stats);

  // sample function, compute streaming stats & add to histo
  for(uint32_t i=0; i<LEN; i++) {
    float x = f();
    seq_stats_add(&stats,x);
    sample_add(h,x);
  }

  double score = 0.0,min=2.0,max=-min;
  float* ref = ref_data;

  // scale histogram values to estimate the PDF
  double scale = IBINW/((double)LEN);
  
  //printf("{");
  for(uint32_t i=0; i<BINS; i++) {
    double v = (double)h[i] * scale;
    double d = v - (double)(*ref++);

    if (d<min) min = d;
    if (d>max) max = d;

    score += fabs(d);
    
    //printf("%e", v);
    //if (i != BINS-1) printf(",");
  }
  //printf("}\n");

  seq_stats_print(&stats);

  printf("score: %e [%e,%e] %+e\n", score,min,max,max+min);
}

float dist_tri_ref()
{
  uint64_t u0 = rng_u0();
  int64_t  a  = (int64_t)(u0 & 0xffffff);
  int64_t  b  = (int64_t)(u0 >> (64-24));
  int64_t  td = a-b;
  return (float)td * 0x1.0p-24f;
}

// half box-muller
float dist_normal_ref()
{
  double u0 = rng_f64();
  double u1 = sqrt(-2.0*log(rng_f64()));
  
  return (float)(u1*cos(2.0*M_PI*u0));
}

//********************** configurable constant helpers

// implemenations here are intended as "proof of concept"
// name d{n} n = number of uniform draws
//      p{n} n = number of bits in population count
//      u{n} n = number of uniforms added to population count

typedef float (dist_func_k_t)(float);

float dist_normal_d2u4_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  a  = (int64_t)(u0 & 0xffffffff);
  uint64_t u1 = rng_u1();
  int64_t  c  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u0 >> 32);
  int64_t  d  = (int64_t)(u1 >> 32);

  return k*((a+b)-(c+d));
}

float dist_normal_d2p08u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xff)-4;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d2p12u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfff)-6;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p12u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfff)-6;
  int64_t  a  = (int64_t)((u0 >> 12) & 0x3ffffff);
  int64_t  b  = (int64_t)((u0 >> 38));
  int64_t  td = a-b;
  float    r  = (float)((bd<<26) + td);

  return k*r;
}

float dist_normal_d2p16u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xffff)-8;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p16u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xffff)-8;
  int64_t  a  = (int64_t)((u0 >> 16) & 0xffffff);
  int64_t  b  = (int64_t)((u0 >> 40));
  int64_t  td = a-b;
  float    r  = (float)((bd<<24) + td);

  return k*r;
}

float dist_normal_d2p20u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfffff)-10;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p20u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfffff)-10;
  int64_t  a  = (int64_t)((u0 >> 20) & 0x3fffff);
  int64_t  b  = (int64_t)((u0 >> 42));
  int64_t  td = a-b;
  float    r  = (float)((bd<<22) + td);

  return k*r;
}

float dist_normal_d2p24u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xffffff)-12;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p24u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xffffff)-12;
  int64_t  a  = (int64_t)((u0 >> 24) & 0xfffff);
  int64_t  b  = (int64_t)((u0 >> 44));
  int64_t  td = a-b;
  float    r  = (float)((bd<<20) + td);

  return k*r;
}

float dist_normal_d2p28u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfffffff)-14;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p28u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0 & 0xfffffff)-14;
  int64_t  a  = (int64_t)((u0 >> 28) & 0x3ffff);
  int64_t  b  = (int64_t)((u0 >> 46));
  int64_t  td = a-b;
  float    r  = (float)((bd<<18) + td);

  return k*r;
}

float dist_normal_d2p32u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0)-16;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d1p32u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop32((uint32_t)u0)-16;
  int64_t  a  = (int64_t)((u0 >> 32) & 0xffff);
  int64_t  b  = (int64_t)((u0 >> 48));
  int64_t  td = a-b;
  float    r  = (float)((bd<<16) + td);

  return k*r;
}

float dist_normal_d2p64u2_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop64(u0)-32;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)(u1 & 0xffffffff);
  int64_t  b  = (int64_t)(u1 >> 32);
  int64_t  td = a-b;
  float    r  = (float)((bd<<32) + td);

  return k*r;
}

float dist_normal_d2p64u3_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop64(u0)-32;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)((u1    ) & 0x1fffff);
  int64_t  b  = (int64_t)((u1>>21) & 0x1fffff);
  int64_t  c  = (int64_t)u1 >> 42;
  int64_t  td = a-b+c;
  float    r  = (float)((bd<<21) + td);

  return k*r;
}

#if 0
// half & double width kernels perform worse
float dist_normal_d2p64u3_h_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop64(u0)-32;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)((u1    ) & 0x1fffff);
  int64_t  b  = (int64_t)((u1>>21) & 0x1fffff);
  int64_t  c  = (int64_t)u1 >> 42;
  int64_t  td = a-b+c;
  float    r  = (float)((bd<<20) + td);

  return k*r;
}

float dist_normal_d2p64u3_d_(float k)
{
  uint64_t u0 = rng_u0();
  int64_t  bd = (int64_t)pop64(u0)-32;
  uint64_t u1 = rng_u1();
  int64_t  a  = (int64_t)((u1    ) & 0x1fffff);
  int64_t  b  = (int64_t)((u1>>21) & 0x1fffff);
  int64_t  c  = (int64_t)u1 >> 42;
  int64_t  td = a-b+c;
  float    r  = (float)((bd<<22) + td);

  return k*r;
}
#endif


// BINS = 4096 LEN = 0x80000000                 abs-sum err   min/max error               score: max+min
const float d2u4_scale    = 0x1.b5041ep-32f; // 1.504352e+01 [-9.327114e-03,9.325549e-03] -1.564622e-06
const float d2p08u2_scale = 0x1.566130p-33f; // 1.270367e+01 [-1.008368e-02,1.008248e-02] -1.192093e-06  <- underperforms
const float d1p12u2_scale = 0x1.1c54b0p-27f; // 8.425666e+00 [-7.370114e-03,7.370442e-03] +3.278255e-07
const float d2p12u2_scale = 0x1.1c5844p-33f; // 8.425876e+00 [-7.352948e-03,7.353038e-03] +8.940697e-08
const float d1p16u2_scale = 0x1.f0cca8p-26f; // 6.224682e+00 [-5.798340e-03,5.799830e-03] +1.490116e-06
const float d2p16u2_scale = 0x1.f0f7acp-34f; // 6.239918e+00 [-5.942076e-03,5.942583e-03] +5.066395e-07
const float d1p20u2_scale = 0x1.bf4a60p-24f; // 5.125237e+00 [-5.242139e-03,5.238980e-03] -3.159046e-06
const float d2p20u2_scale = 0x1.beff2ap-34f; // 5.092392e+00 [-4.845381e-03,4.847020e-03] +1.639128e-06 
const float d1p24u2_scale = 0x1.999f8cp-22f; // 4.293938e+00 [-4.261523e-03,4.263222e-03] +1.698732e-06 
const float d2p24u2_scale = 0x1.99e7ecp-34f; // 4.348634e+00 [-4.503250e-03,4.503548e-03] +2.980232e-07
const float d1p28u2_scale = 0x1.7c4a3cp-20f; // 3.708573e+00 [-4.098207e-03,4.043311e-03] -5.489588e-05
const float d2p28u2_scale = 0x1.7c54f6p-34f; // 3.706938e+00 [-3.681988e-03,3.682405e-03] +4.172325e-07
const float d1p32u2_scale = 0x1.6435e2p-18f; // 3.242814e+00 [-3.551185e-03,3.545761e-03] -5.424023e-06
const float d2p32u2_scale = 0x1.647e50p-34f; // 3.238784e+00 [-3.302395e-03,3.303111e-03] +7.152557e-07
const float d2p64u2_scale = 0x1.fbdcaap-35f; // 1.677512e+00 [-1.978755e-03,1.977921e-03] -8.344650e-07
const float d2p64u3_scale = 0x1.f6eb98p-24f; // 1.253976e+00 [-1.253814e-03,1.258880e-03] +5.066395e-06

//********************** 

#define DEF_FUNC(X) float dist_normal_ ## X () { return dist_normal_ ## X ##_ (X ## _scale); }

// expand direct versions with optimized constants
DEF_FUNC(d2u4);
DEF_FUNC(d2p08u2);
DEF_FUNC(d1p12u2);
DEF_FUNC(d2p12u2);
DEF_FUNC(d1p16u2);
DEF_FUNC(d2p16u2);
DEF_FUNC(d1p20u2);
DEF_FUNC(d2p20u2);
DEF_FUNC(d1p24u2);
DEF_FUNC(d2p24u2);
DEF_FUNC(d1p28u2);
DEF_FUNC(d2p28u2);
DEF_FUNC(d1p32u2);
DEF_FUNC(d2p32u2);
DEF_FUNC(d2p64u2);
DEF_FUNC(d2p64u3);

double optimize_0(dist_func_k_t* f, float K)
{
  uint32_t h[BINS] = {0};

  // build histogram
  for(uint32_t i=0; i<LEN; i++) {
    float x = f(K);
    sample_add(h,x);
  }

  // compute the score of the current constant
  double min=2.0,max=-min;
  float* ref = ref_data;

  // scale histogram values to estimate the PDF
  double scale = IBINW/((double)LEN);
  
  for(uint32_t i=0; i<BINS; i++) {
    double v = (double)h[i] * scale;
    double d = v - (double)(*ref++);

    if (d<min) min = d;
    if (d>max) max = d;
  }

  return max+min;
}

typedef struct {
  dist_func_k_t* f;
  char*          name;

  // optimation func will create these: copy to struct after when adding
  float          Kl;
  float          Kh;
  float          K;      // optimized constant
  uint32_t       fbits;  // number of bits in fixed point fraction (defaults to 32)
} optim_func_t;

// I'd original intended to automate optimization...then I got lazy.
#if 1
optim_func_t optim_funcs[] =
{

#if 1
  {.f=&dist_normal_d2u4_,    .name="draw 2  4 uniform (32 bit)",                .Kl=0x1.b5d0bcp-32f,.Kh=0x1.b5d0d2p-32f, .K=d2u4_scale},
  {.f=&dist_normal_d2p08u2_, .name="draw 2  8-bit popcnt + 2 uniform (32 bit)", .K=d2p08u2_scale},
  {.f=&dist_normal_d1p12u2_, .name="draw 1 12-bit popcnt + 2 uniform (26 bit)", .K=d1p12u2_scale, .fbits=26},
  {.f=&dist_normal_d2p12u2_, .name="draw 2 12-bit popcnt + 2 uniform (32 bit)", .K=d2p12u2_scale},
  {.f=&dist_normal_d1p16u2_, .name="draw 1 16-bit popcnt + 2 uniform (24 bit)", .K=d1p16u2_scale, .fbits=24},
  {.f=&dist_normal_d2p16u2_, .name="draw 2 16-bit popcnt + 2 uniform (32 bit)", .K=d2p16u2_scale},
  {.f=&dist_normal_d1p20u2_, .name="draw 1 20-bit popcnt + 2 uniform (22 bit)", .K=d1p20u2_scale, .fbits=22},
  {.f=&dist_normal_d2p20u2_, .name="draw 2 20-bit popcnt + 2 uniform (32 bit)", .K=d2p20u2_scale},
  {.f=&dist_normal_d1p24u2_, .name="draw 1 24-bit popcnt + 2 uniform (20 bit)", .K=d1p24u2_scale, .fbits=20},
  {.f=&dist_normal_d2p24u2_, .name="draw 2 24-bit popcnt + 2 uniform (32 bit)", .K=d2p24u2_scale},
  {.f=&dist_normal_d1p28u2_, .name="draw 1 28-bit popcnt + 2 uniform (18 bit)", .K=d1p28u2_scale, .fbits=18},
  {.f=&dist_normal_d2p28u2_, .name="draw 2 28-bit popcnt + 2 uniform (32 bit)", .K=d2p28u2_scale},
  {.f=&dist_normal_d1p32u2_, .name="draw 1 32-bit popcnt + 2 uniform (16 bit)", .K=d1p32u2_scale, .fbits=16},
  {.f=&dist_normal_d2p32u2_, .name="draw 2 32-bit popcnt + 2 uniform (32 bit)", .K=d2p32u2_scale},
  {.f=&dist_normal_d2p64u2_, .name="draw 2 64-bit popcnt + 2 uniform (32 bit)", .K=d2p64u2_scale},
  {.f=&dist_normal_d2p64u3_, .name="draw 2 64-bit popcnt + 3 uniform (21 bit)", .K=d2p64u3_scale, .fbits=21},
#endif
  // half & double width kernels: worse error
  //{.f=&dist_normal_p64u3_h_, .name="64-bit popcnt + 3 uniform (21 bit)h", .fbits=21},
  //{.f=&dist_normal_p64u3_d_, .name="64-bit popcnt + 3 uniform (21 bit)d", .fbits=21},
};
#else
// for clean run at optimization
optim_func_t optim_funcs[] =
{
  {.f=&dist_normal_d2u4_,    .name="draw 2  4 uniform (32 bit)",                .Kl=0x1.b5d0bcp-32f,.Kh=0x1.b5d0d2p-32f, .K=d2u4_scale},
  {.f=&dist_normal_d2p08u2_, .name="draw 2  8-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p12u2_, .name="draw 1 12-bit popcnt + 2 uniform (26 bit)", .fbits=26},
  {.f=&dist_normal_d2p12u2_, .name="draw 2 12-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p16u2_, .name="draw 1 16-bit popcnt + 2 uniform (24 bit)", .fbits=24},
  {.f=&dist_normal_d2p16u2_, .name="draw 2 16-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p20u2_, .name="draw 1 20-bit popcnt + 2 uniform (22 bit)", .fbits=22},
  {.f=&dist_normal_d2p20u2_, .name="draw 2 20-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p24u2_, .name="draw 1 24-bit popcnt + 2 uniform (20 bit)", .fbits=20},
  {.f=&dist_normal_d2p24u2_, .name="draw 2 24-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p28u2_, .name="draw 1 28-bit popcnt + 2 uniform (18 bit)", .fbits=18},
  {.f=&dist_normal_d2p28u2_, .name="draw 2 28-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d1p32u2_, .name="draw 1 32-bit popcnt + 2 uniform (16 bit)", .fbits=16},
  {.f=&dist_normal_d2p32u2_, .name="draw 2 32-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d2p64u2_, .name="draw 2 64-bit popcnt + 2 uniform (32 bit)"},
  {.f=&dist_normal_d2p64u3_, .name="draw 2 64-bit popcnt + 3 uniform (21 bit)", .fbits=21},
};
#endif

void optimize_func(uint64_t S0, uint64_t S1, optim_func_t* f)
{
  if (f->fbits == 0) f->fbits = 32;

  // only run if the struct doesn't specify an optimal consant
  if (f->K == 0.f) {
    printf("--------------------\nrunning: %s\n", f->name);

    float  lo,hi, xmin;
    double sl,sh, smin;

    // find an initial estimate of K if not supplied
    if (f->Kl == 0.f) { f->Kl = estimate_K(f->f, 0.f); };
    
    reset_generators(S0,S1);
    xmin = lo = f->Kl;
    sl   = optimize_0(f->f,lo);
    smin = fabs(sl);

    // all very hacky..yes
    if (sl > 0.0) {
      if (f->Kh != 0.0) hi = f->Kh; else hi = 1.125f*lo;
      reset_generators(S0,S1);
      sh = optimize_0(f->f,hi);
      if (sh > 0.0) { printf("fixme: sh %a\n",sh); return; }
      if (-sh < smin) { smin = -sh; xmin = hi; } 
    }
    else {
      hi = lo; sh = sl; lo = .9f*hi;
      reset_generators(S0,S1);
      sl = optimize_0(f->f,lo);
      if (sl < 0.0) { printf("fixme: sl %a\n",sl); return; }
      if (sl < smin) { smin = sl; xmin = lo; } 
    }
    
    for(int i=0; i <= 24; i++) {
      reset_generators(S0,S1);
      
      float  m = (float)(((double)hi+(double)lo)*0.5);
      double v = optimize_0(f->f,m);

      if (fabs(v) < smin) { smin = fabs(v); xmin = m; } 
      
      if (v >= 0.0) {
	if (lo==m) break;
	lo = m; sl=v; 
      }
      else {
	if (hi==m) break;
	hi = m; sh=v; 
      }

      printf("%14a %13e : [.Kl=%14af,.Kh=%14af]->[%13e,%13e]]\n",m,v,lo,hi,sl,sh);
      
      if (hi == lo) break;
    }

    f->K = xmin;
 
    printf(".K=%af : %13e\n",xmin,smin);
  }
}

void optimize_all(uint64_t s0, uint64_t s1)
{
  printf("optimizations:\n");

  for(uint64_t i=0; i<sizeof(optim_funcs)/sizeof(optim_func_t); i++) {
    optimize_func(s0,s1,optim_funcs+i);
  }

  printf("optimizations: done\n");
}

dist_func_k_t* test_func;
float test_K;

float test_func_hack() { return test_func(test_K); };

void optimize_sample(uint64_t s0, uint64_t s1)
{
  printf("--------------------\nsampling optimized:\n");

  for(uint64_t i=0; i<sizeof(optim_funcs)/sizeof(optim_func_t); i++) {
    printf("running: %s\n", optim_funcs[i].name);
    test_func = optim_funcs[i].f;
    test_K    = optim_funcs[i].K;
    reset_generators(s0,s1);
    sample_dist(test_func_hack);
    printf("\n");
  }

  printf("sampling: done\n");
}


//******************************************************

#ifdef RANDOMIZE
#include "xmmintrin.h"
#endif

int main(void)
{
  // choose initial state values for the PRNG
  uint64_t s0 = UINT64_C(0xbf58476d1ce4e5b9);
  uint64_t s1 = UINT64_C(0x61c8864680b583eb);
  
#ifdef RANDOMIZE
  s0 ^= __rdtsc()*UINT64_C(0x7fb5d329728ea185);
  s1 += __rdtsc();
#endif

  reset_generators(s0,s1);
  ref_build();

  optimize_all(s0,s1);
  optimize_sample(s0,s1);

  //sample_dist(dist_normal_ref);
  //sample_dist(&dist_tri_ref);
  //sample_dist(dist_normal_p64u2);
}
