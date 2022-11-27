// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/2017/05/02/QuatQuantPart1.html
// http://marc-b-reynolds.github.io/quaternions/

// This performs sloppy empirical testing, the code is very likely to be broken
// and is intended for hacking. AKA needs proper testing before any usage.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include <math.h>


// config section

// run sobol sequence tests
#define USE_SOBOL

#define USE_PRNG

// number of trials to run per test
//#define SYM_TRIALS 0x7FFFFFF
#define SYM_TRIALS 0xFFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// number of quant bits. hack to give
// imperfect view of how errors drop given
// an arbitrary number of bits
#ifndef BITS
#define BITS 32
#endif

// choose sub-set of test: see 'maps', 'xmaps'
//#define CM_TESTS   // composed methods
#define EX_TESTS   // explicit methods

// rotation angle buckets
#define HLEN 80

// Dump out average and/or RMS values. This auto-disables
// most sampling strategies.
#define DUMP_RMS
//#define DUMP_AVE

// report new peak errors as seen to console
#define TRACE_PEAK

// debug aid. normalize input prior to transform
//#define NORMALIZE_SAMPLES

// choose between atan or acos based error measuring
//#define ACOS_MEASURE

// end config

#define XSTR(X) STR(X)
#define STR(X)  #X

#include "../SFH/quat.h"

#if defined(DUMP_RMS)||defined(DUMP_AVE)
#define UNIFORM_ONLY
#endif

#if defined USE_SOBOL
#define SOBOL_EXTRAS
#define SOBOL_IMPLEMENTATION
#include "../SFH/Sobol.h"

sobol_2d_t qrng;
sobol_1d_t srng;
#endif

// external code: xoroshiro128+

uint64_t rng_state[2];

#define ULP1 TO_FP32

static inline uint64_t rotl(const uint64_t v, int i)
{
  return (v << i)|(v >> (64-i));
}

static inline uint64_t rng_u64(void)
{
  uint64_t s0 = rng_state[0];
  uint64_t s1 = rng_state[1];
  uint64_t r  = s0 + s1;
  
  s1 ^= s0;
  rng_state[0] = rotl(s0,55) ^ s1 ^ (s1<<14);
  rng_state[1] = rotl(s1,36);
  
  return r;
}
// end: xoroshiro128+

#if defined(_MSC_VER)
#define TO_FP32 (1.f/16777216.f)
#define SQRT2   1.41421353816986083984375f
#define SQRT2M1 0.4142135679721832275390625f // sqrt(2)-1 = 1/(sqrt(2)+1)
#define SQRT2P1 2.414213657379150390625f     // sqrt(2)+1 = 1/(sqrt(2)-1)
#define ISQRT3  0.57735025882720947265625f   // 1/sqrt(3)
#define PI      3.1415927410125732421875f
#define RPI     0.3183098733425140380859375f
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#else
#define TO_FP32        0x1p-24f
#define SQRT2          0x1.6a09e6p0f  // sqrt(2)
#define SQRT2M1        0x6.a09e68p-4f // sqrt(2)-1 = 1/(sqrt(2)+1)
#define SQRT2P1        0x2.6a09e8p0f  // sqrt(2)+1 = 1/(sqrt(2)-1)
#define ISQRT3         0x9.3cd3ap-4f  // 1/sqrt(3)
#define PI             0x3.243f6cp0f
#define RPI            0x5.17cc18p-4f
#define F32_MIN_NORMAL 0x1p-126f
#endif

#define EPS (F32_MIN_NORMAL)

 inline float rng_f32(void)
{
  return (rng_u64() >> 40)*TO_FP32;
}

// for quickly/first-pass hacking in arch specific junk
// !!!!!! the recip hurts performance w current gcc/clang..
//        actually does the 1/x and product...sigh.
 inline float sgn(float x) { return copysignf(1.f,x); }
 inline float mulsgn(float x, float v) { return sgn(x)*v; }
 inline float rsqrt(float v) { return sqrtf(1.f/v); }
 inline float recip(float v) { return 1.f/v; }
 inline float rnd(float v)   { return floorf(v+0.5f); }
 inline double sgnd(double x) { return copysign(1.0,x); }

 inline float pmax(float a, float b)   { return a >= b ? a : b; }
 inline float pmin(float a, float b)   { return a <= b ? a : b; }


 inline float clamp(float v, float lo, float hi)
{
  if (v >= lo) {
    if (v <= hi)
      return v;
    else return hi;
  }
  return lo;
}


// reset state data of PRNG & Sobol
void reset_generators(uint64_t s0, uint64_t s1, uint32_t len)
{
#ifdef USE_SOBOL
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0>>32));
  sobol_1d_init(&srng, (uint32_t)s0);
#endif
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}


void vec2_print(vec2_t* v)
{
  printf("{% f, % f} ",v->x,v->y);
}


void vec3_printa(vec3_t* v)
{
  printf("{% a, % a, % a} ",v->x,v->y,v->z);
}

void vec3_print(vec3_t* v)
{
  printf("{% f, % f, % f} ",v->x,v->y,v->z);
}

void quat_printa(quat_t* v)
{
  printf("{% a, % a, % a, % a} ",v->x,v->y,v->z,v->w);
}

void quat_print(quat_t* v)
{
  printf("{% f, % f, % f, % f} ",v->x,v->y,v->z,v->w);
}

void ln()
{
  printf("\n");
}


//static inline uint32_t f32_to_bits(float v)      { uint32_t r; memcpy(&r,&v,4); return r; }
//static inline float    f32_from_bits(uint32_t v) { float r;    memcpy(&r,&v,4); return r; }

float uniform_disc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*TO_FP32;
    y = (v & 0xFFFFFF)*TO_FP32;
    x = 2.f*x-1.f; d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

float uniform_hdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*TO_FP32;
    y = (v & 0xFFFFFF)*TO_FP32;
    d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

float uniform_qdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x  = (v >> 40)*TO_FP32;
    y  = (v & 0xFFFFFF)*TO_FP32;
    d  = x*x;
    d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

void uniform_s2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_disc(&v);  
  s = 2.f*sqrtf(1.f-d);
  p->x = s*v.x;
  p->y = s*v.y;
  p->z = 1.f-2.f*d;
}

// all positive (for sym)
void uniform_ps2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_qdisc(&v);  
  s = 2.f*sqrtf(1.f-d);
  p->x = s*v.x;
  p->y = s*v.y;
  p->z = 1.f-2.f*d;
}

void uniform_quat(quat_t* q)
{
  vec2_t p0,p1;
  float  d1 = uniform_disc(&p1) + EPS;
  float  s1 = rsqrt(d1);
  float  d0 = uniform_hdisc(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
}

void uniform_quat_from_z(quat_t* q)
{
  vec2_t p;
  float  d = uniform_disc(&p);
  float  s = sqrtf(1.f-d);
  quat_set(q, p.x, p.y, 0.f, s);
}


#define CBIAS (1.f/68719476736.f) // 0x1p-36f

void uniform_s1_px(vec2_t* p)
{
#if 1
  float d = uniform_hdisc(p);
  float s = rsqrt(d+(CBIAS*CBIAS)); 
  p->x += CBIAS;                    
  p->x *= s;
  p->y *= s;
#else
  float y = sinf(PI*(rng_f32()-0.5f));
  p->y = y;
  p->x = sqrtf(1.f-y*y);
#endif  
}


vec3_t test_dir = {.x=1.f, .y=0.f, .z=0.f};

void uniform_quat_about_dir(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  quat_set(q,
	   p.y*test_dir.x,
	   p.y*test_dir.y,
	   p.y*test_dir.z, fabsf(p.x)); // silly...should direct half-circle instead
}

void uniform_quat_about_x(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  quat_set(q, p.y, 0.f, 0.f, fabsf(p.x)); // silly...should direct half-circle instead
}

void uniform_quat_about_xy(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  float b = p.y * 0.5f*SQRT2;
  quat_set(q, b, b, 0.f, fabsf(p.x)); // silly...should direct half-circle instead
}

void uniform_quat_about_diag(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  float b = p.y * ISQRT3;
  quat_set(q, b,b,b, fabsf(p.x)); // silly...should direct half-circle instead
}

#ifdef USE_SOBOL

void sobol_quat_about_x(quat_t* q)
{
  float b = sinf(PI*(sobol_1d_next_f32(&srng)-0.5f));
  quat_set(q, b, 0.f, 0.f, sqrtf(1.f-b*b));
}

void sobol_quat_about_xy(quat_t* q)
{
  float t = sinf(PI*(sobol_1d_next_f32(&srng)-0.5f));
  float b = t * 0.5f*SQRT2;
  quat_set(q, b, b, 0.f, sqrtf(1.f-t*t));
}

void sobol_quat_about_diag(quat_t* q)
{
  float a = PI*(sobol_1d_next_f32(&srng)-0.5f);
  float t = sinf(a);
  float b = t * ISQRT3;
  float w = sqrtf(1.f-t*t);
  quat_set(q, b,b,b, w);
}

void sobol_quat(quat_t* q)
{
  float p0[2];
  float p1[2];
  float d1 = sobol_uniform_d1(&qrng, p1) + EPS;
  float s1 = rsqrt(d1);
  float d0 = sobol_uniform_hd1(&qrng, p0);
  float s0 = sqrtf(1.f-d0);
  float s  = s0*s1;

  quat_set(q, p0[1], s*p1[0], s*p1[1], p0[0]);
}
#endif


// H in binary64 for error measures
typedef struct { double x,y,z,w; } quatd_t;

inline void quat_to_d(quatd_t* r, quat_t* q)
{
  r->x = q->x; r->y = q->y; r->z = q->z; r->w = q->w;
}

inline void quatd_set(quatd_t* r, double x, double y, double z, double w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

inline void quatd_conj(quatd_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

inline double quatd_dot(quatd_t* a, quatd_t* b)
{
  return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}

inline double quatd_norm(quatd_t* a)
{
  return quatd_dot(a,a);
}

inline void quatd_mul(quatd_t* r, quatd_t* a, quatd_t* b)
{
  double x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  double y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  double z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  double w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quatd_set(r,x,y,z,w);
}

// returned angle is in degrees and R^3 and not H.
// use doubles to compute relative quaternion BA*,
// and atan2 to determine angle between A & B.

#define TO_R3_DEGREES 114.59156036376953125  // 2(180/pi), the 2 for H to R3 angle measure

float a_error(quat_t* a, quat_t* b)
{
#if !defined(ACOS_MEASURE)  
  quatd_t R,A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  quatd_conj(&A);
  quatd_mul(&R,&B,&A);

  double x = fabs(R.w);
  double y = sqrt(R.x*R.x+R.y*R.y+R.z*R.z);
  double t = atan(y/x);
  return (float)(TO_R3_DEGREES*t);
#else
  quatd_t A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  double d = fabs(quatd_dot(&A,&B));
  double t = d < 1.0 ? acos(d) : 0;
  return TO_R3_DEGREES*t;
#endif  
}


//****** ball/cube/cylinder stuff 

// hacky macros to fill coordinate
#define LOAD_XYZ(S)  float x=S->x, y=S->y, z=S->z;
#define LOAD_XYZ2(S) float x=S->x, y=S->y, z=S->z, x2=x*x, y2=y*y, z2=z*z;

// cyl hacko
void map_aeaz_cb(vec3_t* d, vec3_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;

  d->z = s->z;

  if (x2 > y2) {
    float m = sgn(x);
    d->y = 0.5f*SQRT2*y;
    d->x = m*sqrtf(x2-0.5f*y2);
  } else {
    float m = sgn(y);
    d->x = 0.5f*SQRT2*x;
    d->y = m*sqrtf(y2-0.5f*x2);
  }
}

void map_aeaz_bc(vec3_t* d, vec3_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = sqrtf(x2+y2);

  d->z = s->z;

  if (x2 > y2) {
    d->y = SQRT2*y;
    d->x = mulsgn(x, m);
  } else {
    d->x = SQRT2*x;
    d->y = mulsgn(y, m);
  }
}



// radial stretch
void map_rs_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float m = x2 > y2 ? x : y; m = m*m > z2 ? m : z;
  float s = fabsf(m)*rsqrt(x2+y2+z2+EPS);

  vec3_set(D, s*x, s*y, s*z);
}

void map_rs_bc(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float m = x2 > y2 ? x : y; m = m*m > z2 ? m : z;
  float s = sqrtf(x2+y2+z2)/(fabsf(m)+2.f*EPS);

  vec3_set(D, s*x, s*y, s*z);
}

// volume preserving: cylinder+concentric
void map_vp_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float a,b,t,d,s;

  // square->disc
  if (x2 > y2) {
    float r = recip(x);
    t = sinf(0.25f*PI*(y*r));
    a = x*sqrtf(1.f-t*t);
    b = x*t;
    d = x2;
  }
  else {
    float r = recip(y+EPS);
    t = sinf(0.25f*PI*(x*r));
    a = y*t;
    b = y*sqrtf(1.f-t*t);
    d = y2;
  }

  // cylinder-> ball
  if (z2 > d) {
    float iz = 1.f/(3.f*z);
    float dz = d*iz;
    s = sqrtf((2.f/3.f)-dz*iz);
    z = z - dz;
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(D,s*a,s*b,z);
}

void map_vp_bc(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float pd = x2+y2;
  float d  = pd+z2;
  float t  = sqrtf(d);
  float rd = rsqrt(pd+EPS);
  float s  = rd*pd;

  if (pd < 5.f/4.f*z2) {
    s *= sqrtf(3.f*t/(t+fabsf(z)));
    D->z = sgn(z)*t;
  }
  else {
    s *= t*rd;
    D->z = 1.5f*z;
  }

  if (x2 > y2) {
    float r = recip(fabsf(x));
    D->x = mulsgn(x, s);
    D->y = s*(4*RPI)*atanf(y*r);
  }
  else {
    float r = recip((fabsf(y)+EPS));
    D->x = s*(4*RPI)*atanf(x*r);
    D->y = mulsgn(y, s);
  }
}


// radial stretch+cylinder
void map_rsc_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float a,b;

  // square->disc
  float pd = x2+y2;
  float m  = x2 > y2 ? x : y;
  m = fabsf(m)*rsqrt(pd+EPS);
  a = m*x;
  b = m*y;

  // cylinder->ball
  float d = a*a+b*b; // m^2 pd
  float s;

  if (z2 > d) {
    float iz = 1.f/(3.f*z);
    float dz = d*iz;
    s = sqrtf((2.f/3.f)-dz*iz);
    z = z - dz;
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(D,s*a,s*b,z);
}

void map_rsc_bc(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float pd = x2+y2;
  float id = rsqrt(pd);
  float d  = pd+z2;
  float t  = sqrtf(d);
  float m = x2 > y2 ? x : y;
  float r = recip(fabsf(m)+EPS);
  float s = id*pd*r;

  if (pd < 5.f/4.f*z2) {
    s   *= sqrtf(3.f*t/(t+fabsf(z)));
    D->z = sgn(z)*t;
  }
  else {
    s   *= t*id;
    D->z = 1.5f*z;
  }

  D->x = s*x;
  D->y = s*y;
}

// approx equal volume = approx equal area+cylinder
void map_aev_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float a,b,d;

  // square->disc
  if (x2 > y2) {
    float m = sgn(x);
    b = 0.5f*SQRT2*y;
    a = m*sqrtf(x2-0.5f*y2);
    d = a*a+b*b;
  } else {
    float m = sgn(y);
    a = 0.5f*SQRT2*x;
    b = m*sqrtf(y2-0.5f*x2);
    d = a*a+b*b;
  }

  // cylinder->ball
  float s;

  if (z2 > d) {
    float iz = 1.f/(3.f*z);
    float dz = d*iz;
    s = sqrtf((2.f/3.f)-dz*iz);
    z = z - dz;
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(D,s*a,s*b,z);
}

void map_aev_bc(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);
  float pd = x2+y2;
  float d  = pd+z2;
  float t  = sqrtf(d);
  float ip = rsqrt(pd);
  float s,m;

  // ball->cylinder
 if (pd < 5.f/4.f*z2) {
    s = sqrtf(3.f*t/(t+fabsf(z)));
    m = s*ip*pd;
    D->z = sgn(z)*t;
  }
  else {
    s = t*ip;
    m = t;
    D->z = 1.5f*z;
  }

  // disc->square  
  s *= SQRT2;

  if (x2 > y2) {
    D->x = mulsgn(x, m);
    D->y = s*y;
  } else {
    D->x = s*x;
    D->y = mulsgn(y, m);
  }
}

void map_ell_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float a = x*sqrtf(1.f-0.5f*y2);
  float b = y*sqrtf(1.f-0.5f*x2);
  float a2 = a*a;
  float b2 = b*b;
  float d  = a2+b2; 
  float s;

  if (z2 > d) {
    float iz = 1.f/(3.f*z);
    float dz = d*iz;
    s = sqrtf((2.f/3.f)-dz*iz);
    z = z - dz;
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(D,s*a,s*b,z);
}

void map_ell_bc(vec3_t* D, vec3_t* S)
{
  float a  = S->x;
  float b  = S->y;
  float z  = S->z;
  float a2 = a*a;
  float b2 = b*b;
  float z2 = z*z;
  float dp = a2+b2;
  float d  = dp+z2;
  float t  = sqrtf(d);
  float s;

  // ball->cylinder
  if (dp < 5.f/4.f*z2) {
    s  = sqrtf(3.f*t/(t+fabsf(z)));
    D->z = sgn(z)*t;
  }
  else {
    s  = t*rsqrt(dp);
    D->z = 1.5f*z;
  }

  // disc->square
  float x  = 0.5f*s*a;
  float y  = 0.5f*s*b;
  float x2 = x*x;
  float y2 = y*y;
  float T  = x2-y2;
  float a0 = sqrtf(0.5f + T + SQRT2*x);
  float b0 = sqrtf(0.5f + T - SQRT2*x);
  float c0 = sqrtf(0.5f - T + SQRT2*y);
  float d0 = sqrtf(0.5f - T - SQRT2*y);

  D->x = a0-b0;
  D->y = c0-d0;
}

void map_id_bc(vec3_t* D, vec3_t* S) {vec3_dup(D,S);}
void map_id_cb(vec3_t* D, vec3_t* S) {vec3_dup(D,S);}

//********************************************************
// basic xforms rescaled to unit-ball given no limit on
// max implied angle

// hacky macro 
#define LOAD_Q(Q)  float x=Q->x, y=Q->y, z=Q->z, w=Q->w;

// discard/restore 'w'
void fdw(vec3_t* v, quat_t* q)
{
  quat_put_bv(v,q,1.f);
}

void idw(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  quat_bv_set_scale(q,v,1.f);
  q->w = sqrtf(1.f-pmin(d,1.f));
}


// half-angle
void fha(vec3_t* v, quat_t* q)
{
  float d = 1.f + q->w;
  float s = rsqrt(d);
  quat_put_bv(v,q,s);
}

void iha(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float s = sqrtf(2.f-d); 
  quat_bv_set_scale(q,v,s);
  q->w = 1.f-d;
}

// Cayley transform
void fct(vec3_t* v, quat_t* q)
{
  float s = 1.f/(1.f+q->w);
  quat_put_bv(v,q,s);
}

void ict(quat_t* q, vec3_t* v)
{
  float s = 2.f/(1.f+vec3_norm(v));
  quat_bv_set_scale(q,v,s);
  q->w = s-1.f;
}

// reversed Cayley transform
void frct(vec3_t* v, quat_t* q)
{
  float s = 1.f/(1.f-q->w);
  quat_put_bv(v,q,s);
}

void irct(quat_t* q, vec3_t* v)
{
  float s = 2.f/(vec3_norm(v)+1.f);
  quat_bv_set_scale(q,v,s);
  q->w = 1.f-s;
}

// harmonic mean
void fhm(vec3_t* v, quat_t* q)
{
#if 0  
  quat_t t;
  quat_dup(&t,q);
  quat_usqrt(&t);
  quat_fct(v,&t);
  vec3_scale(v, SQRT2P1);
#else
  float w = q->w;
  float s = SQRT2P1/(1.f+w+sqrtf(2+w+w));
  quat_put_bv(v,q,s);
#endif  
}

#define Khm 0.17157287895679473876953125f // 3-2sqrt(2)

void ihm(quat_t* q, vec3_t* v)
{
#if 0
  vec3_t t;
  vec3_set_scale(&t, v, SQRT2M1);
  quat_ict(q, &t);
  quat_upow2(q);
#else
  float d = Khm*vec3_norm(v);
  float a = (1.f+d);         
  float b = (1.f-d)*(SQRT2M1*4.f);         
  float c = 1.f/(a*a);     

  quat_bv_set_scale(q, v, b*c);
  q->w = (1.f+d*(d-6.f))*c;
#endif
}

// log(q) & exp(b)

void fem(vec3_t* v, quat_t* q)
{
  // for noisy input with w=1 & x,y,z!= 0, don't compute a as 1-w^2
  // for higher bit packages. explodes error near identity
  float w = q->w;
//float a = 1.f-w*w;
  float a = quat_bnorm(q);
  float b = rsqrt(a+EPS);
  float k = a*b;
  float s = (2.f*RPI)*atanf(k/w)*b;
  quat_put_bv(v,q,s);
}

void iem(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float t = rsqrt(d+EPS);
  float a = (PI/2.f)*t*d;
  float s = sinf(a);
  float k = s*t;
  quat_bv_set_scale(q,v,k);
  q->w = cosf(a);
}

// proc pointer to quaternion generator
typedef void (*qsrc)(quat_t*);

// unit quaternion to ball and inverse
typedef void (*s3_to_b3)(vec3_t*, quat_t*);
typedef void (*b3_to_s3)(quat_t*, vec3_t*);

// maps between cube and ball (both directions)
typedef void (*cb_map)(vec3_t*, vec3_t*);

typedef struct {
  s3_to_b3 f;
  b3_to_s3 g;
  cb_map   bc;
  cb_map   cb;
  char*    name;
} maps_t;

// define maps
typedef void (*map_func_t)(vec3_t*, vec3_t*);

#define DEF(S,X) {&f ## S, &i ## S, &map_ ## X ## _bc, &map_ ## X ## _cb, XSTR(S##X)}

maps_t maps[] =
{
//DEF(dw,id),  // no transform (for post comp with smallest three)
#if 0
  // nothing to see here. ;)
  DEF(em,vp),
  DEF(hm,vp),
  DEF(ha,vp),
  DEF(ct,vp),

  DEF(em,aev),
  DEF(hm,aev),
  DEF(ha,aev),
  DEF(ct,aev),
  
  DEF(em,rsc),
  DEF(hm,rsc),
  DEF(ha,rsc),
  DEF(ct,rsc),
  
  DEF(em,rs),
  DEF(hm,rs),
  DEF(ha,rs),
  DEF(ct,rs),
#endif

  DEF(ha,rs),

#if 0
  DEF(em,id),  // exp-map
  DEF(hm,id),  // harmonic mean
  DEF(ha,id),  // half-angle
  DEF(ha,rs),  // half-angle + radial stretch
  DEF(ct,id)   // Cayley
#endif  
};

#define NUM_FUNCS (sizeof(maps)/sizeof(maps[0]))

#undef DEF

// (this is sloppy)
#define QSCALE_(X) ((float)(1<<(X)))
#define QSCALE(X)   QSCALE_(X)
#define DSCALE(X)   (1.f/QSCALE_(X))

#define BITS_E ((BITS-2)/3)
#define BITS_M ((1<<BITS_E)-1)

#define BITS_X  (BITS/3)
#define BITS_Y  ((BITS-BITS_X)/2)
#define BITS_Z  (BITS-BITS_X-BITS_Y)
#define BITS_MX ((1<<BITS_X)-1)
#define BITS_MY ((1<<BITS_Y)-1)
#define BITS_MZ ((1<<BITS_Z)-1)

static inline uint64_t quant(float f, float s)
{
  uint64_t i = (uint64_t)(f*s);
  if (i > s-1.f) i = (uint64_t)(s-1.f);
  return i;
}

static inline float dequant(uint64_t b, float s) { return (b+0.5f)*s; }

void vsdecode(vec3_t* v, uint64_t b)
{
  v->x = 2.f*dequant(b & BITS_M, DSCALE(BITS_E))-1.f; b >>= BITS_E;
  v->y = 2.f*dequant(b & BITS_M, DSCALE(BITS_E))-1.f; b >>= BITS_E;
  v->z = 2.f*dequant(b & BITS_M, DSCALE(BITS_E))-1.f;
}

uint64_t vsencode(vec3_t* v)
{
  uint64_t b;
  b  = quant(0.5f*v->x+0.5f, QSCALE(BITS_E)); 
  b |= quant(0.5f*v->y+0.5f, QSCALE(BITS_E)) << BITS_E; 
  b |= quant(0.5f*v->z+0.5f, QSCALE(BITS_E)) << (BITS_E+BITS_E);
  return b;
}

void vdecodex(vec3_t* v, uint64_t b, uint32_t bx, uint32_t by, uint32_t bz)
{
  v->x = 2.f*dequant(b&((1<<(bx))-1), DSCALE(bx))-1.f; b >>= bx;
  v->y = 2.f*dequant(b&((1<<(by))-1), DSCALE(by))-1.f; b >>= by;
  v->z = 2.f*dequant(b&((1<<(bz))-1), DSCALE(bz))-1.f;
}

uint64_t vencodex(vec3_t* v, uint32_t bx, uint32_t by, uint32_t bz)
{
  uint64_t b;
  b  = quant(0.5f*v->x+0.5f, QSCALE(bx)); 
  b |= quant(0.5f*v->y+0.5f, QSCALE(by)) << bx; 
  b |= quant(0.5f*v->z+0.5f, QSCALE(bz)) << (bx+by);
  return b;
}

void vdecode(vec3_t* v, uint64_t b)
{
  v->x = 2.f*dequant(b & BITS_MX, DSCALE(BITS_X))-1.f; b >>= BITS_X;
  v->y = 2.f*dequant(b & BITS_MY, DSCALE(BITS_Y))-1.f; b >>= BITS_Y;
  v->z = 2.f*dequant(b & BITS_MZ, DSCALE(BITS_Z))-1.f;
}

uint64_t vencode(vec3_t* v)
{
  uint64_t b;
  b  = quant(0.5f*v->x+0.5f, QSCALE(BITS_X)); 
  b |= quant(0.5f*v->y+0.5f, QSCALE(BITS_Y)) << BITS_X; 
  b |= quant(0.5f*v->z+0.5f, QSCALE(BITS_Z)) << (BITS_X+BITS_Y);
  return b;
}

//********
// explicit methods


#define QUANT_S(V,B)   (uint64_t)((V+1.0f)*(1<<(B-1)))
#define QUANT_E(V,B)   (uint64_t)((0.5f*V+.5f)*((1<<B))-1)

#define DEQUANT_S(I,B) ((I)+0.5f)*(1.f/(1<<(B-1)))-1.f;
#define DEQUANT_E(I,B) ((I)+1.0f)*(1.f/(1<<(B-1)))-1.f;

// number of bits per component with 'n' sideline bits
#define X1_BITS    (BITS-1)
#define X1_BITS_X  (X1_BITS/3)
#define X1_BITS_Y  ((X1_BITS-X1_BITS_X)/2)
#define X1_BITS_Z  (X1_BITS-X1_BITS_X-X1_BITS_Y)

#define X2_BITS    (BITS-2)
#define X2_BITS_X  (X2_BITS/3)
#define X2_BITS_Y  ((X2_BITS-X2_BITS_X)/2)
#define X2_BITS_Z  (X2_BITS-X2_BITS_X-X2_BITS_Y)

#define X3_BITS    (BITS-3)
#define X3_BITS_X  (X3_BITS/3)
#define X3_BITS_Y  ((X3_BITS-X3_BITS_X)/2)
#define X3_BITS_Z  (X3_BITS-X3_BITS_X-X3_BITS_Y)


// smallest-three
uint64_t fsot(quat_t* q)
{
  float    m  = fabsf(q->w);
  float    ax = fabsf(q->x);
  float    ay = fabsf(q->y);
  float    az = fabsf(q->z);
  uint32_t id = 3;
  uint64_t r;
  vec3_t   t;

  if (ax > m) { id=0; m=ax; }
  if (ay > m) { id=1; m=ay; }
  if (az > m) { id=2; }

  r = id; r <<= 62;

  float s = SQRT2*sgn(q->f[id]);

  id = (id+1)&3; t.x = s*q->f[id];
  id = (id+1)&3; t.y = s*q->f[id];
  id = (id+1)&3; t.z = s*q->f[id];

  return r|vencodex(&t, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
}

void isot(quat_t* q, uint64_t b)
{
  uint32_t id = (uint32_t)(b >> 62);
  vec3_t v;

  vdecodex(&v, b, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);

  q->f[id] = sqrtf(1.f-0.5f*vec3_norm(&v));
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.x; 
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.y; 
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.z;

  // this is just for compat with other tests.
  // insure that 'w' is positive
  if (q->f[3] >= 0.f) return;
  q->f[0] = -q->f[0];
  q->f[1] = -q->f[1];
  q->f[2] = -q->f[2];
  q->f[3] = -q->f[3];
}

// peak error from 9-bit
// 36.210361, 33.331524, 30.582666,
// 18.190594, 16.793215, 15.458775,
//  9.200575,  8.443863,  7.802176,
//  4.568894,  4.204691,  3.887760,
//  2.291203,  2.119163,  1.965465,
//  1.155557,  1.053847,  0.985540,
//  0.576990,  0.527061,  0.492343,
//  0.288413,  0.263893,  0.244946,
//  0.143265,  0.131212,  0.122809
// expanded: half-angle + radial stretch, 0 side-line bits
uint64_t fhars0(quat_t* q)
{
  vec3_t v;

  // denom in & out of the root. can lose bias in
  // this specific quant case (no zeroes)
#if 0
  float x  = q->x, y = q->y, z = q->z;
  float x2 = x*x,  y2=y*y,   z2=z*z;
  float m  = x2 > y2 ? x2 : y2; m = m > z2 ? m : z2;
  float s  = sqrtf((1.f-q->w)/(m+2.f*EPS));
#else
  float x  = q->x,     y  = q->y,     z  = q->z;
  float ax = fabsf(x), ay = fabsf(y), az = fabsf(q->z);
  float m  = ax > ay ? ax : ay; m = m > az ? m : az;
  float s  = sqrtf(1.f-q->w)/(m+2.f*EPS);
#endif  
  
  vec3_set(&v, s*x, s*y, s*z);
  
  return vencode(&v);
}

void ihars0(quat_t* q, uint64_t b)
{
  vec3_t v;

  vdecode(&v,b);

  float x = v.x,y=v.y,z=v.z;
  float x2=x*x,y2=y*y,z2=z*z;
  float d = x2 > y2 ? x2 : y2; d = d > z2 ? d : z2;
  float s = sqrtf(d*(2.f-d)/(x2+y2+z2+EPS));  // bias can be lost (see above)

  q->x = s*x;
  q->y = s*y;
  q->z = s*z;
  q->w = 1.f-d;
}


#define RK0 0x5.336f98p-4f // 1/sqrt(2(3+sqrt(3)))
#define RK1 0xe.358cfp-4f  // sqrt((3+sqrt(3))/6)

static quat_t rot1 = {.x=0,   .y=RK0, .z=RK0, .w=RK1 };
static quat_t rot2 = {.x=RK0, .y=0,   .z=RK0, .w=RK1 };
static quat_t rot3 = {.x=RK0, .y=RK0, .z=0,   .w=RK1 };


// expanded: half-angle + radial stretch, 1 side-line bit (TODO)
uint64_t fhars1(quat_t* q)
{
  vec3_t   v,r;
  vec3_t   d0,d1;
  uint64_t b0,b1;
  float    e0,e1;

  fha(&v,q);                 // map to ball
  quat_rot(&r, &rot2, &v);   // rotated ball
  map_rs_bc(&v,&v);          // ball2cube: unrotated
  map_rs_bc(&r,&r);          // ball2cube: rotated

  // unrotated radial-stretch
  b0 = vencodex(&v, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  vdecodex(&d0, b0, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  e0 = vec3_dot(&v, &d0);

  // rotated radial-stretch
  b1 = vencodex(&r, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  vdecodex(&d1, b1, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  e1 = vec3_dot(&r, &d1);

  if (e0 > e1) return b0;

  return (1L<<62)|b1;
}

void ihars1(quat_t* q, uint64_t b)
{
  uint32_t id = (uint32_t)(b >> 62);
  vec3_t v;

  vdecodex(&v, b, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  map_rs_cb(&v,&v);

  // temp hack
  if (id) {
    quat_t r;
    vec3_t v0;
    quat_dup(&r, &rot2);
    quat_conj(&r);
    quat_rot(&v0,&r,&v);
    vec3_dup(&v,&v0);
  }
  
  iha(q,&v);
}

#define K0 0x3.61963p-4f   // 1/(3+sqrt(3))
#define K1 0x9.3cd3ap-4f   // 1/sqrt(3)
#define K2 0x1.bb67aep0f   // sqrt(3)
#define K3 0x2.bb67bp0f    // 1+sqrt(3)
#define K4 0x3.bb67bp0f    // 2+sqrt(3) {plus one is correctly rounded}


// expanded: half-angle + radial stretch, 2 side-line bits (TODO)
uint64_t fhars2(quat_t* q)
{
  vec3_t   r0,r1,r2,r3,d;
  uint64_t b,t;
  float    e0,e1;
  uint64_t id = 0;

  fha(&r0,q);

  quat_rot(&r1, &rot1, &r0);
  quat_rot(&r2, &rot2, &r0);
  quat_rot(&r3, &rot3, &r0);

  map_rs_bc(&r0, &r0);
  map_rs_bc(&r1, &r1);
  map_rs_bc(&r2, &r2);
  map_rs_bc(&r3, &r3);

  // unrotated radial-stretch
  b = vencodex(&r0, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  vdecodex(&d, b, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  e0 = vec3_dot(&r0, &d);

  t = vencodex(&r1, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  vdecodex(&d, t, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  e1 = vec3_dot(&r1, &d);

  if (e1 > e0) { b = t; e0 = e1; id=1; }

  t = vencodex(&r2, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  vdecodex(&d, t, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  e1 = vec3_dot(&r2, &d);

  if (e1 > e0) { b = t; e0 = e1; id=2; }

  t = vencodex(&r3, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  vdecodex(&d, t, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  e1 = vec3_dot(&r3, &d);
  
  if (e1 > e0) { b = t; e0 = e1; id=3;}

  return (id<<61)|b; // temp hack
}

void ihars2(quat_t* q, uint64_t b)
{
  uint32_t id = (uint32_t)(b >> 61);
  vec3_t c,v;

  vdecodex(&c, b, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
  map_rs_cb(&v,&c);

  // temp hack
  if (id != 0) {
    quat_t r;
    switch(id) {
    case 1: quat_dup(&r, &rot1); break;
    case 2: quat_dup(&r, &rot2); break;
    case 3: quat_dup(&r, &rot3); break;
    }

    quat_conj(&r);
    quat_rot(&v,&r,&v);
  }
  iha(q,&v);
}

#if 0

// ordered-scaled (TODO: nope)
//  0 000 {x, y, z}  
//  1 001 {x, z, y}  {a,b,c} -> {a,c,b} : 1st bit = flip last two
//  2 010 {y, x, z}  {a,b,c} -> {b,a,c} : 2nd bit = flip first two
//  3 011 {y, z, x}
//  4 100 {z, y, x}  {a,b,c} -> {c,b,a} : 3rd bit = flip first/last
//  5 101 {z, x, y}
//  6 110 --
//  7 111 --
uint64_t fos(quat_t* q)
{
  quat_t h;
  quat_dup(&h,q);
  quat_usqrt(&h);

  // w in [sqrt(2)/2, 1]
  float    a  = h.x, b = h.y, c = h.z;
  uint32_t id = 0;

  // ordering into: |a| >= |b| >= |c|
  if (fabsf(b) > fabsf(c)) { float t=b; b=c; c=t; id =1; }
  if (fabsf(a) > fabsf(b)) { float t=a; a=b; b=t; id|=2; }
  if (fabsf(a) > fabsf(c)) { float t=a; a=c; c=t; id|=4; }

  //------------------------------------------------------
  float s = SQRT2*sgn(q->f[id]);

  id = (id+1)&3; t.x = s*q->f[id];
  id = (id+1)&3; t.y = s*q->f[id];
  id = (id+1)&3; t.z = s*q->f[id];

  return r|vencodex(&t, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);
}

void ios(quat_t* q, uint64_t b)
{
  uint32_t id = (uint32_t)(b >> 62);
  vec3_t v;

  vdecodex(&v, b, X2_BITS_X, X2_BITS_Y, X2_BITS_Z);

  q->f[id] = sqrtf(1.f-0.5f*vec3_norm(&v));
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.x; 
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.y; 
  id = (id+1)&3; q->f[id] = 0.5f*SQRT2*v.z;

  // this is just for compat with other tests.
  // insure that 'w' is positive
  if (q->f[3] >= 0.f) return;
  q->f[0] = -q->f[0];
  q->f[1] = -q->f[1];
  q->f[2] = -q->f[2];
  q->f[3] = -q->f[3];
}

#endif


// all temp hacks

// Tait-Bryan: yaw-pitch-roll (ZYX)
// storage of values in struct swap x&y given bit allocation
// strategy used in this file.

void quat_to_zyx(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;
  
  double t0 = (x+z)*(x-z);
  double t1 = (w+y)*(w-y);
  double xx = 0.5*(t0+t1);
  double xy = x*y+w*z;
  double xz = w*y-x*z;
  double yz = 2.0*(y*z+w*x);
  double t  = xx*xx+xy*xy;

  v->z = (float)atan2(xy, xx);
  v->x = (float)atan(xz/sqrt(t));

  if (t != 0)
    v->y = (float)atan2(yz, t1-t0);
  else 
    v->y = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
}

void zyx_to_quat(quat_t* q, vec3_t* v)
{
  float hy = 0.5f*v->z;
  float hp = 0.5f*v->x;
  float hr = 0.5f*v->y;
  float ys = sinf(hy), yc = cosf(hy);
  float ps = sinf(hp), pc = cosf(hp);
  float rs = sinf(hr), rc = cosf(hr);

  q->w = rc*pc*yc + rs*ps*ys;
  q->x = rs*pc*yc - rc*ps*ys;
  q->y = rc*ps*yc + rs*pc*ys;
  q->z = rc*pc*ys - rs*ps*yc;

  // below here temp hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}

uint64_t fzyx(quat_t* q)
{
  vec3_t v;
  quat_to_zyx(&v, q);
  v.x *= 2.f*RPI;
  v.y *= RPI;
  v.z *= RPI;
  return vencode(&v);
}

void izyx(quat_t* q, uint64_t b)
{
  vec3_t v;
  vdecode(&v, b);
  v.x *= 0.5f*PI;
  v.y *= PI;
  v.z *= PI;
  zyx_to_quat(q, &v);
}

void quat_to_zxz(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;

  double s2 = x*x+y*y;      // sin(beta)^2
  double c2 = w*w+z*z;      // cos(beta)^2
  double s  = atan(z/w);    // (gamma+alpha)/2
  double d  = atan2(y,x);   // (gamma-alpha)/2
  v->y = s-d;               // alpha
  v->z = s+d;               // gamma

  // this is a hack I can't be bothered to fix.
  // if 'x' is negative we don't have a min range
  // for gamma or alpha. The proper fix is to
  // convert computing 'd' by single parameter
  // atan and pull-out the arg reduction and
  // constants. Could fold with 's' arg reduction
  // as well.
  if (x < 0) {
    if (fabs(v->y) > PI) v->y -= sgn(v->y)*2.f*PI;
    if (fabs(v->z) > PI) v->z -= sgn(v->z)*2.f*PI;
  }
  
  if (c2 != 0.0)
    v->x = 2.0*atan(sqrt(s2/c2));
  else
    v->x = (0.5 > s2) ? 0 : PI;
}

void zxz_to_quat(quat_t* q, vec3_t* v)
{
  float a = 0.5f*v->y;
  float b = 0.5f*v->x;
  float c = 0.5f*v->z;

  float scma = sinf(c-a), ccma = cosf(c-a);
  float scpa = sinf(c+a), ccpa = cosf(c+a);
  float sb   = sinf(b),   cb   = cosf(b);

  q->w = cb*ccpa;
  q->x = sb*ccma;
  q->y = sb*scma;
  q->z = cb*scpa;

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}

uint64_t fzxz(quat_t* q)
{
  vec3_t v;
  quat_to_zxz(&v, q);

  v.x = v.x*RPI;
  v.y = v.y*RPI/2.f+.5f;
  v.z = v.z*RPI/2.f+.5f;

  uint64_t b;

  b  = quant(v.x, QSCALE(BITS_X));
  b |= quant(v.y, QSCALE(BITS_Y)) << BITS_X;
  b |= quant(v.z, QSCALE(BITS_Z)) << (BITS_Y+BITS_X);
  
  return b;
}

void izxz(quat_t* q, uint64_t b)
{
  vec3_t v;

  v.x = dequant(b & BITS_MX, DSCALE(BITS_X)); b >>= BITS_X;
  v.y = dequant(b & BITS_MY, DSCALE(BITS_Y)); b >>= BITS_Y;
  v.z = dequant(b & BITS_MZ, DSCALE(BITS_Z));

  v.x = PI*v.x;
  v.y = 2.f*PI*(v.y-0.5f);
  v.z = 2.f*PI*(v.z-0.5f);

  zxz_to_quat(q, &v);
}

// Hopf coordinates
// http://marc-b-reynolds.github.io/quaternions/2017/05/12/HopfCoordConvert.html

void quat_to_hopf(vec3_t* v, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;

  float gamma = 2.f*atan2f(z, w);
  float alpha = 2.f*atan2f(sqrtf(x*x+y*y), sqrtf(w*w+z*z));
  float beta  = atan2f(y*z-w*x, w*y+x*z);

  v->x = alpha;
  v->y = beta;
  v->z = gamma;
}

void hopf_to_quat(quat_t* q, vec3_t* v)
{
  float ha    = 0.5f*v->x;
  float b     = v->y;
  float hg    = 0.5f*v->z;
  float sinHa = sinf(ha);
  float cosHa = cosf(ha);
  q->x = sinHa*sinf(hg-b);
  q->y = sinHa*cosf(hg-b);
  q->z = cosHa*sinf(hg);
  q->w = cosHa*cosf(hg);
}

// hopf coordinate
uint64_t fhopfc(quat_t* q)
{
  vec3_t v;
  quat_to_hopf(&v, q);

  v.x = v.x*RPI;
  v.y = v.y*RPI/2.f+.5f;
  v.z = v.z*RPI/2.f+.5f;

  uint64_t b;

  b  = quant(v.x, QSCALE(BITS_X));
  b |= quant(v.y, QSCALE(BITS_Y)) << BITS_X;
  b |= quant(v.z, QSCALE(BITS_Z)) << (BITS_Y+BITS_X);
  
  return b;
}

void ihopfc(quat_t* q, uint64_t b)
{
  vec3_t v;

  v.x = dequant(b & BITS_MX, DSCALE(BITS_X)); b >>= BITS_X;
  v.y = dequant(b & BITS_MY, DSCALE(BITS_Y)); b >>= BITS_Y;
  v.z = dequant(b & BITS_MZ, DSCALE(BITS_Z));

  v.x = PI*v.x;
  v.y = 2.f*PI*(v.y-0.5f);
  v.z = 2.f*PI*(v.z-0.5f);

  hopf_to_quat(q, &v);
}


//============================================================

// "normal" encoding routines

// "On floating-point normal vectors" via "A Survey of Efficient Representations for Independent Unit Vectors"
// 'oct' is full sphere, 'hoct' is half-sphere (positive Z)

void oct_encode(vec2_t* p, vec3_t* v)
{
  float s = 1.f/(fabsf(v->x)+fabsf(v->y)+fabsf(v->z));
  float x = s*v->x;
  float y = s*v->y;
  
  if (v->z < 0.0f) {
    p->x = (1.f - fabsf(y))*sgn(x);
    p->y = (1.f - fabsf(x))*sgn(y);
  } else {
    p->x = x;
    p->y = y;
  }
}

void oct_decode(vec3_t* v, vec2_t* p)
{
  v->x = p->x;
  v->y = p->y;
  v->z = 1.f - (fabsf(v->x)+fabs(v->y));
  
  if (v->z <= 0.f) {
    float x = v->x;
    v->x = (1.f - fabsf(v->y)) * sgn(x);
    v->y = (1.f - fabsf(x))    * sgn(v->y);
  }

  float s = 1.f/sqrtf(vec3_dot(v,v));
  v->x *= s;
  v->y *= s;
  v->z *= s;
}

void hoct_encode(vec2_t* p, vec3_t* v)
{
  float s = 1.f/(fabsf(v->x)+fabsf(v->y)+fabsf(v->z));
  float x = s*v->x;
  float y = s*v->y;

  p->x = x+y;
  p->y = x-y;
}

void hoct_decode(vec3_t* v, vec2_t* p)
{
  float x = (p->x+p->y)*0.5f;
  float y = (p->x-p->y)*0.5f;

  v->x = x;
  v->y = y;
  v->z = 1.0-fabsf(x)-fabsf(y);
  
  float s = 1.f/sqrtf(vec3_dot(v,v));
  
  v->x *= s;
  v->y *= s;
  v->z *= s;
}


// Hopf map related factorization.
// http://marc-b-reynolds.github.io/quaternions/2017/05/12/HopfCoordConvert.html
typedef struct {
  vec3_t m;   // about axis in 'xy'
  vec2_t t;   // about 'z' twist
} tmtwist_t;

#define THRESHOLD (ULP1*ULP1)

// factorize with respect to positive Z
void quat_to_tmtwist(tmtwist_t* d, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;
  float t=w*w+z*z;

  if (t > THRESHOLD) {
    float s = 1.f/sqrtf(t);
    // point on 2-sphere
    d->m.x = s*(w*x-y*z);
    d->m.y = s*(w*y+x*z);
    d->m.z = s*t;

    // point on 1-sphere
    d->t.x = s*z;
    d->t.y = s*w;
  }
  else {
    d->m.x = x;
    d->m.y = y;
    d->m.z = 0;
    d->t.x = 0;
    d->t.y = 1.f;
  }
}

void tmtwist_to_quat(quat_t* q, tmtwist_t* s)
{
  q->x = s->m.x*s->t.y + s->m.y*s->t.x;
  q->y = s->m.y*s->t.y - s->m.x*s->t.x;
  q->z = s->m.z*s->t.x;
  q->w = s->m.z*s->t.y;
}

#undef THRESHOLD


uint64_t fhopf(quat_t* q)
{
  tmtwist_t qf;
  vec2_t   s;
  
  quat_to_tmtwist(&qf, q);
  hoct_encode(&s, &qf.m);
  float a = RPI*atan2f(qf.t.x, qf.t.y)+0.5f;
  uint64_t b;

  b  = quant(0.5f*s.x+0.5f, QSCALE(BITS_X));
  b |= quant(0.5f*s.y+0.5f, QSCALE(BITS_Y)) << BITS_X;
  b |= quant(a, QSCALE(BITS_Z)) << (BITS_X+BITS_Y);
  
  return b;
}

void ihopf(quat_t* q, uint64_t b)
{
  tmtwist_t qf;
  vec2_t    s;
  float     a;
  
  s.x = 2.f*dequant(b & BITS_MX, DSCALE(BITS_X))-1.f; b >>= BITS_X;
  s.y = 2.f*dequant(b & BITS_MY, DSCALE(BITS_Y))-1.f; b >>= BITS_Y;
  a   = PI*(dequant(b & BITS_MZ, DSCALE(BITS_Z))-0.5f);
  qf.t.x = sinf(a);
  qf.t.y = sqrtf(1.f-qf.t.x*qf.t.x);
  
  hoct_decode(&qf.m, &s);
  
  tmtwist_to_quat(q, &qf);
}

#if 0
uint64_t fhahopf(quat_t* q)
{
  tmtwist_t qf;
  vec2_t   s;
  quat_t m;
  
  quat_to_tmtwist(&qf, q);

  m.x = qf.m.x;
  m.y = qf.m.y;
  m.z = 0.f;
  m.w = qf.m.z;

  float a = RPI*atan2f(qf.t.x, qf.t.y)+0.5f;
  uint64_t b;

  b  = quant(0.5f*s.x+0.5f, QSCALE(BITS_X));
  b |= quant(0.5f*s.y+0.5f, QSCALE(BITS_Y)) << BITS_X;
  b |= quant(a, QSCALE(BITS_Z)) << (BITS_X+BITS_Y);
  
  return b;
}

void ihahopf(quat_t* q, uint64_t b)
{
  tmtwist_t qf;
  vec2_t    s;
  float     a;
  
  s.x = 2.f*dequant(b & BITS_MX, DSCALE(BITS_X))-1.f; b >>= BITS_X;
  s.y = 2.f*dequant(b & BITS_MY, DSCALE(BITS_Y))-1.f; b >>= BITS_Y;
  a   = PI*(dequant(b & BITS_MZ, DSCALE(BITS_Z))-0.5f);
  qf.t.x = sinf(a);
  qf.t.y = sqrtf(1.f-qf.t.x*qf.t.x);
  
  hoct_decode(&qf.m, &s);
  
  tmtwist_to_quat(q, &qf);
}
#endif





//***********************************************************

// sanity test for roundtripping between a point on
// unit sphere (3D) and point in 2D.
void s2_xform_test(uint64_t s0, uint64_t s1)
{
  printf("performing S2 round-trip test\n");
  double min = 1.f;
  
  reset_generators(s0,s1,SYM_TRIALS);

  for(uint32_t i=0; i<SYM_TRIALS; i++) {
    vec3_t p,r;
    vec2_t c;

    uniform_s2(&p);
    oct_encode(&c,&p);
    oct_decode(&r,&c);
    float d = vec3_dot(&p,&r);
    
    if (d < min) {
      min = d;
      printf("%08x: %f ",i, (180.f*RPI)*acosf(min));
      vec3_print(&p);
      vec3_print(&r);
      vec2_print(&c);
      printf("\n");
    }
  }
}


void hf_xform_test(uint64_t s0, uint64_t s1)
{
  float min = 1.f;
  printf("performing Hopf factorization round-trip test\n");

  reset_generators(s0,s1,SYM_TRIALS);

  for(uint32_t i=0; i<SYM_TRIALS; i++) {
    quat_t    q,r;
    tmtwist_t qf;
    vec2_t    s;
    float     d;

    uniform_quat(&q);         // random unit quat
    quat_to_tmtwist(&qf,&q);  // factorize
    
    hoct_encode(&s,    &qf.m);

    uint64_t b;

#if 1
    b  = quant(0.5f*s.x+0.5f, QSCALE(BITS_X));
    b |= quant(0.5f*s.y+0.5f, QSCALE(BITS_Y)) << BITS_X;

    s.x = 2.f*dequant(b & BITS_MX, DSCALE(BITS_X))-1.f; b >>= BITS_X;
    s.y = 2.f*dequant(b & BITS_MY, DSCALE(BITS_Y))-1.f; b >>= BITS_Y;
#endif

    qf.m.z = sqrtf(1.f-qf.m.x*qf.m.x-qf.m.y*qf.m.y);

#if 1
    float a = RPI*atan2f(qf.t.x, qf.t.y)+0.5f;
    b = quant(a, QSCALE(BITS_X));
    a = PI*(dequant(b & BITS_MX, DSCALE(BITS_X))-0.5f);
    qf.t.x = sinf(a);
#endif    
    qf.t.y = sqrtf(1.f-qf.t.x*qf.t.x);
    
    hoct_decode(&qf.m, &s);

    tmtwist_to_quat(&r, &qf);  // compose rotations

    d = fabs(quat_dot(&q,&r));

    if (d < min) {
      min = d;
      printf("%08x: %f ",i, (360.f*RPI)*acosf(min));
      quat_print(&q);
      quat_print(&r);
      printf("\n");
    }
  }
}


// ** Explict quantization methods

typedef uint64_t (*q_to_bits)(quat_t*);
typedef void     (*bits_to_q)(quat_t*, uint64_t);

typedef struct {
  q_to_bits encode;
  bits_to_q decode;
  char*     name;
} x_maps_t;

#define DEF(S) {&f ## S, &i ## S, XSTR(S)}

x_maps_t xmaps[] =
{
  //DEF(sot),
  //DEF(zyx),
  //DEF(zxz),
  //DEF(hopfc),
  DEF(hopf),
  //DEF(hars0),
  //DEF(hars1),
  //DEF(hars2),
};

#undef DEF

#define NUM_XFUNCS (sizeof(xmaps)/sizeof(xmaps[0]))

typedef struct {
  quat_t   s;      // untransformed src
  uint32_t cnt;    // bin count
  double   max;    // max error in bin
  double   rms;    // rms sum
  double   ave;    // ave sum
} e_histo_t;


typedef struct {
  e_histo_t a[HLEN];  // rotation angle histo
} a_error_t;

a_error_t a_errors[NUM_FUNCS+NUM_XFUNCS];

void error_init()
{
  memset(a_errors,0, sizeof(a_errors));
}

double error_add(quat_t* s, quat_t* r, a_error_t* e)
{
  double     ae  = a_error(s,r);
  double     t   = fabs(ae);
  uint32_t   hid = (uint32_t)(((2.f*HLEN)/PI)*acosf(s->w));

  if (hid >= HLEN) hid=HLEN-1;

  e_histo_t* h   = &e->a[hid]; 
  
  h->ave += t;
  h->rms += ae*ae;
  h->cnt++;

  if (ae > h->max) {
    h->max = ae;
    quat_dup(&h->s, s);
  }

  return t;
}

// return non-zero if any samples have been collected...
// for a little less macro flipping mess.
uint32_t error_has_samples(a_error_t* e)
{
  for(uint32_t i=0; i<HLEN; i++)
    if (e->a[i].cnt != 0) return e->a[i].cnt;

  return 0;
}

// dump out simple error reports per tested method
void error_dump()
{
  uint32_t s;

  for(s=0; s<NUM_FUNCS+NUM_XFUNCS; s++) {
    a_error_t* e  = &a_errors[s];
    double     m  = 0;
    uint32_t   mi = 0;

    if (error_has_samples(e) == 0) continue;

    if (s < NUM_FUNCS)
      printf("%8s:\n", maps[s].name);
    else 
      printf("%8s:\n", xmaps[s-NUM_FUNCS].name);

    printf("max: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].max); if (e->a[i].max > m) { m = e->a[i].max; mi = i; } } ln();

#if defined(DUMP_RMS)
    printf("rms: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", sqrt(e->a[i].rms/e->a[i].cnt)); } ln();
#endif    
#if defined(DUMP_AVE)
    printf("ave: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].ave/e->a[i].cnt); } ln();
#endif    
#if 0
    printf("src: "); for(uint32_t i=0; i<HLEN; i++) { quat_print(&e->a[i].s); printf(", "); } ln();
#endif    


    // dump out global max
    if (s < NUM_FUNCS) {
      s3_to_b3   f  = maps[s].f;
      b3_to_s3   g  = maps[s].g;
      map_func_t bc = maps[s].bc;
      map_func_t cb = maps[s].cb;
      quat_t*    q  = &e->a[mi].s;
      quat_t     r;// , mq;
      vec3_t     b,rb;
      vec3_t     c,rc;
      uint64_t   bits;

      f(&b, q);  
      bc(&c, &b);
      bits = vencode(&c);
      vdecode(&rc, bits);
      cb(&rb, &rc);
      g(&r, &rb);
      printf("global max: %f: ",m);

#if 1
      printf("\n  ");
      quat_print(q); vec3_print(&b); vec3_print(&c); vec3_print(&rc); vec3_print(&rb); quat_print(&r);
      printf("\n  ");
      quat_printa(q); vec3_printa(&b); vec3_printa(&c); vec3_printa(&rc); vec3_printa(&rb); quat_printa(&r);
      ln();
#else
      vec3_print(&b); quat_print(q); quat_print(&r);
      quat_sub(&r,q,&r); quat_printa(&r); quat_printa(q);

      ln();
#endif      
    } else {
      q_to_bits  encode = xmaps[s-NUM_FUNCS].encode;
      bits_to_q  decode = xmaps[s-NUM_FUNCS].decode;
      quat_t*    q      = &e->a[mi].s;
      uint64_t   bits   = encode(q);
      quat_t     r;
      decode(&r, bits);
      printf("global max: %f: ",m);
      quat_print(q); quat_print(&r);
      quat_sub(&r,q,&r); quat_printa(&r);
      ln();
    }
  }
}

#if defined(NORMALIZE_SAMPLES)
inline void NORMALIZE(quat_t* q)
{
  double x=q->x,y=q->y,z=q->z,w=q->w;
  double s = 1.0/sqrt(x*x+y*y+z*z+w*w);
  q->x=s*x;
  q->y=s*y;
  q->z=s*z;
  q->w=s*w;;
}
#else
#define NORMALIZE(X)
#endif


// measure round-trip errors using distribution of 'gen'
// using direct quat routines
void rt_test_x(uint64_t s0, uint64_t s1, qsrc gen, uint32_t len)
{
  for(uint32_t s=0; s<NUM_XFUNCS; s++) {
    q_to_bits  encode = xmaps[s].encode;
    bits_to_q  decode = xmaps[s].decode;
    a_error_t* e = &a_errors[s+NUM_FUNCS];
    quat_t     q,r,mq;
    uint64_t   bits;
    double     maxe = 0;

    // reset base generators
    reset_generators(s0,s1,len);
    quat_set(&mq,0,0,0,1);
    printf("\n%s\n", xmaps[s].name);
    
    for(uint32_t i=0; i<len; i++) {
      double ae;
      gen(&q);                        // H in distribution
      NORMALIZE(&q);                  // normalize sample if NORMALIZE_SAMPLES defined
      bits = encode(&q);              // quantize
      decode(&r, bits);               // dequantize
      ae = error_add(&q,&r,e);
      
      if (ae > maxe) {
	maxe = ae;
#if defined(TRACE_PEAK)	
	printf("%08x: %f ",i, ae);
	quat_print(&q);	 quat_print(&r);
      //quat_printa(&q); quat_printa(&r);
	printf("\n");
#endif	
      }
    }
  }
  ln();
}

// measure round-trip errors using distribution of 'gen'. two-stage
// map H to ball, ball-to-cube (or id) and inverse
// (see rt_test_x)
void rt_test(uint64_t s0, uint64_t s1, qsrc gen, uint32_t len)
{
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    s3_to_b3   f  = maps[s].f;
    b3_to_s3   g  = maps[s].g;
    map_func_t bc = maps[s].bc;
    map_func_t cb = maps[s].cb;
    a_error_t* e  = &a_errors[s];
    quat_t     q,r,mq;
    vec3_t     b,rb;
    vec3_t     c,rc;
    uint64_t   bits;
    double     maxe=0;

    printf("\n%s\n", maps[s].name);
    reset_generators(s0,s1,len);
    quat_set(&mq,0,0,0,1);

    for(uint32_t i=0; i<len; i++) {
      double ae;
      gen(&q);
      NORMALIZE(&q);
      f(&b, &q);  
      bc(&c, &b);
      bits = vencode(&c);
      vdecode(&rc, bits);
      cb(&rb, &rc);
      
      g(&r, &rb);
      //g(&r, &b); // hack for no-quantization
      ae = error_add(&q,&r,e);
      if (ae > maxe) {
	maxe = ae;
#if defined(TRACE_PEAK)	
	printf("%08x: %f ",i, ae);
	quat_print(&q);	 quat_print(&r);
      //quat_printa(&q); quat_printa(&r);
	printf("\n");
#endif	
      }
    }
  }
  
  ln();
}

// a simple round-trip test for debugging dev of mapping function
#define TLEN 0x7FFFFFF
#define TEX

void rt_xform_test(uint64_t s0, uint64_t s1)
{
  double e = 0.0;
  //double rms = 0.0;
  //double rm = 0.5f;
  quat_t p,r,diff;
  uint32_t h[HLEN];
#if !defined(TEX)  
  vec3_t v;
#endif
  float xmin= 1.f,ymin= 1.f,zmin= 1.f;
  float xmax=-1.f,ymax=-1.f,zmax=-1.f;
  
  reset_generators(s0,s1,TLEN);

  memset(h,  0,  4*HLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    //uniform_quat_about_diag(&p);
    uniform_quat(&p);
    p.w = fabsf(p.w);

    // test generalize or explict
#if !defined(TEX)
    q2os(&v,&p);
    os2q(&r,&v);
#else
    uint64_t bits = fhopfc(&p);
    ihopfc(&r,bits);
#endif    
    
    quat_sub(&diff, &p, &r);

    double ae = a_error(&p,&r);
    //rms += ae*ae;
    
    if (ae > e /*|| ae > 1.f*/) {
      e = ae;
      printf("%08x: %f ",i, ae);
    //printf(" -- % a -- ", 0.5f-(p.x*p.z-p.y*p.w));
      quat_print(&p);
#if !defined(TEX)      
      vec3_print(&v);
#endif      
      quat_print(&r);
      quat_printa(&diff);
      printf("\n");
    }
  }

#if !defined(TEX)  
  printf("x:(%f,%f) y:(%f,%f) z:(%f,%f)\n", xmin,xmax,ymin,ymax,zmin,zmax);
#endif  
}

// helpers to build histograms in specific bivector directions
// choose the direction to compute the histogram
//#define MTYPE(T) T ## _quat_about_x
//#define MTYPE(T) T ## _quat_about_xy
#define MTYPE(T) T ## _quat_about_diag

void angle_histo_x(uint64_t s0, uint64_t s1)
{
#ifdef USE_SOBOL
  rt_test_x(s0,s1, &MTYPE(sobol), SYM_TRIALS);
#endif
#ifdef USE_PRNG  
  rt_test_x(s0,s1, &MTYPE(uniform), SYM_TRIALS);
#endif  
  error_dump();
}

void angle_histo(uint64_t s0, uint64_t s1)
{
#ifdef USE_SOBOL
  rt_test(s0,s1, &MTYPE(sobol), SYM_TRIALS);
#endif
#ifdef USE_PRNG  
  rt_test(s0,s1, &MTYPE(uniform), SYM_TRIALS);
#endif  
  error_dump();
}
#undef MTYPE

#if 1
#include <stdio.h>

typedef struct {float v,r,g,b;} gradient_t;

gradient_t tempmap[] = {
  {0.0f,  .18f, .31f, .93f},
  {0.5f,  .98f, .99f, .91f},
  {0.75f, .96f, .86f, .28f},
  {1.0f,  .82f, .13f, .16f},
#if 0  
  {0.0f,  0.f, 0.f, 0.f},
  {0.25f, 1.f, 0.f, 0.f},
  {0.75f, 0.f, 1.f, 0.f},
  {1.0f,  0.f, 0.f, 1.f},
#endif  
};

#define TEMPMAP_LEN (sizeof(tempmap)/sizeof(gradient_t))

// plot a sphere with point being max error in that direction.
// this is really quite dumb. Orthographic projection in an
// axial direction ATM.

// dimensions of plot to create
#define DIM 256

#define FIG_GMAP

// scale density map to errors for this plot, otherwise
// use macros EMIN & EMAX for range
#define ERROR_TRACK

// explict error range for density plot
#define EMIN .23
#define EMAX .46

void fig_sphere(uint64_t s0, uint64_t s1)
{
  FILE* file;
  char* name;

#if defined(FIG_GMAP)
  name = maps[FIG_GMAP].name;
#else
  name = "foo.ppm";
#endif  

  file = fopen(name, "r");
  
  // don't bother if file already exists
  if (file == 0) {
    uint8_t buffer[DIM*DIM*3];
    float   data[DIM*DIM];
    printf("making\n");

    // build the error
    float* eb = data;
    float  emax = 0.f;
    float  emin = 1.f;

#if defined(FIG_GMAP)
    // all the explict transforms
    s3_to_b3 fx = maps[FIG_GMAP].f;
    b3_to_s3 ix = maps[FIG_GMAP].g;
    cb_map   fm = maps[FIG_GMAP].bc;
    cb_map   im = maps[FIG_GMAP].cb;
#endif    
    
    for(uint32_t j=0; j<DIM; j++) {
      for(uint32_t i=0; i<DIM; i++) {
	float x = i*(2.1f/DIM)-1.05f;
	float y = j*(2.1f/DIM)-1.05f;
	float z = sqrtf(1.f-(x*x+y*y));

	if (x*x+y*y <= 1) {
	  double e  = 0.f;
	  vec3_set(&test_dir, z, x, y);
	  reset_generators(s0,s1,TLEN);

#if 1
	  for(uint32_t k=0; k<1000; k++) {
	    quat_t p,r;
	    
	    uniform_quat_about_dir(&p);

#if 1
	    vec3_t v,rv;
	    fem(&v,&p);
	    uint64_t bits = vencode(&v);
	    vdecode(&rv, bits);
	    iem(&r,&rv);
#else
	    //uint64_t bits = fsot(&p);
	    //isot(&r,bits);
	    //uint64_t bits = fhars0(&p);
	    //ihars0(&r,bits);
	    //uint64_t bits = fzxz(&p);
	    //izxz(&r,bits);
	    uint64_t bits = fhopfc(&p);
	    ihopfc(&r,bits);
	    //uint64_t bits = fhopf(&p);
	    //ihopf(&r,bits);
#endif    
	    double ae = a_error(&p,&r);
	    
	    if (ae > e) { e = ae; }
	  }
#else
	  e = 2.f*y+1.f;
	  //e = z;
#endif	  

	  if (e > emax) { emax=e; }
	  if (e < emin) { emin=e; }
	  *eb = e;
	}
	else {
	  *eb = -1000.f;
	}
	eb++;
      }
    }

    printf("{%f,%f}\n",emin,emax);
    
#if !defined(ERROR_TRACK)	  
    emax = EMAX;
    emin = EMIN;
#endif    
 
    // convert to density map
    uint8_t* p = buffer;
    eb = data;
    float erange = 1.f/(emax-emin);

    printf("{%f,%f}\n",(emin-emin)*erange,(emax-emin)*erange);
    
    for(uint32_t j=0; j<DIM; j++) {
      for(uint32_t i=0; i<DIM; i++) {
	float r,g,b;
	float x = i*(2.1f/DIM)-1.05f;
	float y = j*(2.1f/DIM)-1.05f;
	float z = sqrtf(1.f-(x*x+y*y));
	float e = *eb++;

	if (e > -1000.f) {
	  e = (e-emin)*erange;

	  if (e<0.f) e = 0.f;
	  else if (e>1.f) e = 1.f;
	  
	  uint32_t gi;

	  for(gi=0; gi<TEMPMAP_LEN; gi++) {
	    if (tempmap[gi].v >= e) 
	      break;
	  }

	  if (gi != 0) {
	    float p0 = tempmap[gi-1].v;
	    float p1 = tempmap[gi].v;
	    float f  = (e-p0)/(p1-p0);
	    
	    r = f*tempmap[gi].r + (1.f-f)*tempmap[gi-1].r; 
	    g = f*tempmap[gi].g + (1.f-f)*tempmap[gi-1].g; 
	    b = f*tempmap[gi].b + (1.f-f)*tempmap[gi-1].b; 
	  }
	  else {
	    r = tempmap[0].r;
	    g = tempmap[0].g;
	    b = tempmap[0].b;
	  }

	  r = r >= 0 ? r : 0;
	  g = g >= 0 ? g : 0;
	  b = b >= 0 ? b : 0;
	  r = r <= 1 ? r : 1;
	  g = g <= 1 ? g : 1;
	  b = b <= 1 ? b : 1;

#if 0	  
	  float occ = 0.6f-0.4f*y;
	  r = sqrtf(r*r*occ);
	  g = sqrtf(g*g*occ);
	  b = sqrtf(b*b*occ);
#endif	  
	  
	} else { r=b=g=1;}

	*p++ = (uint8_t)(255.f*r);
	*p++ = (uint8_t)(255.f*g);
	*p++ = (uint8_t)(255.f*b);
      }
    }
    
    file = fopen("foo.ppm", "w");

    fprintf(file, "P6\n%d %d\n255\n",DIM,DIM);
    fwrite(buffer, DIM*DIM*3, 1, file);
    fclose(file);
  }
}

#endif


int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;

  error_init();

#ifdef RANDOMIZE
  s0 = _rdtsc();
#else  
  s0 = 0x77535345;
#endif  
  s1 = 0x1234567;
  reset_generators(s0,s1,SYM_TRIALS);
  
  printf("bits: %d/%d/%d - 2+%d/%d/%d\n",
	 BITS_X,BITS_Y,BITS_Z,
	 X2_BITS_X,X2_BITS_Y,X2_BITS_Z);

    
  //short-circuits for specialized testing
  //fig_sphere(s0,s1); return 0;        // max error per direction figure
  //s2_xform_test(s0,s1); return 0;   // 
  //hf_xform_test(s0,s1); return 0;   // 
  //rt_xform_test(s0,s1); return 0;   // method developement debug routine
  //angle_histo(s0,s1); return 0;     // limited directional sampling 4 post plots
  //angle_histo_x(s0,s1); return 0;     // limited directional sampling 4 post plots

#ifdef USE_SOBOL
  printf("sobol rt\n");
#if defined(CM_TESTS)
  rt_test  (s0,s1, &sobol_quat, SYM_TRIALS);
#endif
#if defined(EX_TESTS)
  rt_test_x(s0,s1, &sobol_quat, SYM_TRIALS);
#endif
#endif  
  printf("prng rt\n");

#if defined(CM_TESTS)
  rt_test  (s0,s1, &uniform_quat,            SYM_TRIALS);
#if !defined(UNIFORM_ONLY)  
  rt_test  (s0,s1, &uniform_quat_from_z,     SYM_TRIALS);
  rt_test  (s0,s1, &uniform_quat_about_x,    SYM_TRIALS);
  rt_test  (s0,s1, &uniform_quat_about_diag, SYM_TRIALS);
#endif  
#endif

#if defined(EX_TESTS)
  rt_test_x(s0,s1, &uniform_quat,            SYM_TRIALS);
#if !defined(UNIFORM_ONLY)  
  rt_test_x(s0,s1, &uniform_quat_from_z,     SYM_TRIALS);
  rt_test_x(s0,s1, &uniform_quat_about_x,    SYM_TRIALS);
  rt_test_x(s0,s1, &uniform_quat_about_diag, SYM_TRIALS);
#endif  
#endif

  error_dump();

  return 0;
}
