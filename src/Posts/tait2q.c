// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/math/2017/04/18/TaitEuler.html

// ex. requirements:
// * quaternion header
// * Sobol seq header (optional, SEE: USE_SOBOL)

// Testing for branch point selection
// 0) Use the optional sobol sampling is a good idea
// 1) SYM_TRIALS should be highish (say 0x07FFFFFF)
// 2) Probablity of in common case measures:
//    * define UNIFORM_ONLY for assumption of uniform input
// 3) Max error measures:
//    * UNIFORM_ONLY not defined
//    * ONLY_METHOD_0 for common case
//    * ONLY_NOT_METHOD_0 for other case(s)
//    * neither ONLY_* defined for overall
//
// As a part of error testing the conversion routines which
// follow multiple paths return a number to indicate the
// computation performed for the result.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// config section

// run sobol sequence tests (requires the header)
#define USE_SOBOL

// number of trials to run per test
//#define SYM_TRIALS (0x07FFFFFF<<2)
//#define SYM_TRIALS 0x07FFFFFF
#define SYM_TRIALS 0x00FFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// number of entries in histogram
#define HLEN 100

// histogram to build
// 0: by rotation angle
// 1: by magnitude of z component of x'
// 2: by magnitude of z component of x' on [.99,1]
// ?: expand HIST_FUNC macro
#define HISTO_TYPE 1

// if defined use doubles for euler/tait to quaternion conversion
//#define DOUBLE2Q

// should expand to value on [0,1]
//#define HIST_FUNC() whatever

//#define UNIFORM_ONLY
//#define ONLY_METHOD_0
//#define ONLY_NOT_METHOD_0

// choose between atan or acos based error measuring
//#define ACOS_MEASURE

// If define Euler ZXZ otherwise Tait-Bryan YZX
//#define ZXZ

#define MAX_METHODS 6

// end config

#define XSTR(X) STR(X)
#define STR(X)  #X

#include "../SFH/quat.h"

#if defined(RANDOMIZE)
#include <x86intrin.h>
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

inline uint64_t rotl(const uint64_t v, int i)
{
  return (v << i)|(v >> (64-i));
}

inline uint64_t rng_u64(void)
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
#define PI      3.1415927410125732421875f
#define RPI     0.3183098733425140380859375f
#define SQRT2   1.41421353816986083984375f
#define ISQRT3  0.57735025882720947265625f
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#else
#define TO_FP32        0x1p-24f
#define SQRT2          0x1.6a09e6p0f  // sqrt(2)
#define ISQRT3         0x9.3cd3ap-4f  // 1/sqrt(3)
#define PI             0x3.243f6cp0f
#define RPI            0x5.17cc18p-4f
#define F32_MIN_NORMAL 0x1p-126f
#endif

#define EPS F32_MIN_NORMAL

inline float rng_f32(void)
{
  return (rng_u64() >> 40)*TO_FP32;
}

inline float rng_f32_s(void)
{
  return 2.f*rng_f32()-1.f;
}

inline float  sgn(float x)   { return copysignf(1.f,x); }
inline double sgnd(double x) { return copysign(1.0, x); }

double quat_mag(quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;
  return sqrt(x*x+y*y+z*z+w*w);
}


// explict list of quaternions to test

quat_t test_vector_data[] =
{
#if !defined(_MSC_VER)
  {{ -0x1.001014p-1, -0x1.001014p-1, -0x1.001014p-1,  0x1.ff9f8cp-2 }},
  {{  0x1.7c5f1p-4,   0x1.66e72cp-1, -0x1.794f58p-4,  0x1.66f418p-1 }},
  {{ -0x1.11334cp-1, -0x1.db1d66p-2, -0x1.11333p-1,   0x1.db1da6p-2 }},
  {{  0x1.21fdb8p-4,  0x1.683828p-1, -0x1.21fdb8p-4,  0x1.683828p-1 }},
  {{ -0x1.21fdcp-4,  -0x1.683828p-1, -0x1.21fdcp-4,   0x1.683828p-1 }},
  {{  0x1.55b24ep-2,  0x1.3f30ep-1,  -0x1.55b24ep-2,  0x1.3f30ep-1  }},
  {{ -0x1.00005cp-1, -0x1.00005cp-1, -0x1.00005cp-1,  0x1.fffdcp-2  }},
  {{ -0x1.2237cp-4,   0x1.683772p-1,  0x1.2401ap-4,   0x1.6831aap-1 }},
  {{ -0x1.5b76e4p-1, -0x1.96c07ep-3, -0x1.5b76e4p-1,  0x1.96c07ep-3 }},
  {{ -0x1.148212p-1, -0x1.148212p-1, -0x1.148212p-1,  0x1.6a122p-2  }},
  {{ -0x1.0d2abp-1,  -0x1.0d2abp-1,  -0x1.0d2abp-1,   0x1.a74804p-2 }},
#endif 
  {{-.5f, -.5f, -.5f, .5f}},
  {{ 0.f,  .5f*SQRT2,  0.f, .5f*SQRT2}},
  {{ .5f,  .5f,  .5f, .5f}},
  {{0.533773f, -0.463778f,  0.533769f,  0.463774f}},
  {{-.5f,  .5f,  .5f, .5f}}
 
};

uint32_t test_vector_pos;

#define LOAD_Q(Q)  float  x=Q->x, y=Q->y, z=Q->z, w=Q->w;
#define LOAD_QD(Q) double x=Q->x, y=Q->y, z=Q->z, w=Q->w;


void normalize_hack(quat_t* q)
{
  LOAD_QD(q);
  double s = 1.0/sqrt(x*x+y*y+z*z+w*w);
  quat_set(q, (float)(s*x),(float)(s*y),(float)(s*z),(float)(s*w));
}

void test_vector(quat_t* q)
{
  quat_dup(q, test_vector_data + test_vector_pos);
  test_vector_pos++;
}

// reset state data of PRNG & Sobol
void reset_generators(uint64_t s0, uint64_t s1)
{
#ifdef USE_SOBOL
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0>>32));
  sobol_1d_init(&srng, (uint32_t)s0);
#endif
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
  test_vector_pos = 0;
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
  float  s1 = 1.f/sqrtf(d1);
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
  float d = uniform_hdisc(p);
  float s = 1.f/sqrtf(d+(CBIAS*CBIAS)); 
  p->x += CBIAS;                    
  p->x *= s;
  p->y *= s;
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

void uniform_quat_trouble(quat_t* q)
{
#if 0
  // too tight to be useful
  float t = 2.f*PI*rng_f32();
  float s = 0.5f*SQRT2*sinf(t);
  float c = 0.5f*SQRT2*cosf(t);
  
  quat_set(q, s,-c,s,c);
#else
  // sloppy and lazy
  float a  = 2.f*PI*rng_f32();
  float b  = .25f*PI*(1.f+(ULP1*256.f)*(rng_f32()-0.5f));
  float sa = sinf(a);
  float ca = cosf(a);
  float sb = sinf(b);
  float cb = cosf(b);

  if (rng_u64() & 1)
    quat_set(q, sa*sb,-ca*sb, cb*sa, ca*cb);
  else
    quat_set(q, -sa*sb, ca*sb, -cb*sa, ca*cb);

  if (q->w >= 0.f) return;
  quat_neg(q,q);
#endif     
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
  float s1 = 1.f/sqrtf(d1);
  float d0 = sobol_uniform_hd1(&qrng, p0);
  float s0 = sqrtf(1.f-d0);
  float s  = s0*s1;

  quat_set(q, p0[1], s*p1[0], s*p1[1], p0[0]);
}


void sobol_yaw_roll(vec3_t* v)
{
  float    p[2];
//float    pitch;
//uint64_t u = rng_u64();

  // should be computing a reasonable pitch w both signs
  v->x = 0.5f*PI;     // TODO: this could be better
  
  sobol_2d_next_f32(&qrng, p);
  v->z = 2.f*PI*p[0]; // yaw
  v->y = 2.f*PI*p[1]; // roll
}

#if defined(ZXZ)    
#define DECODE zxz_to_quat
#else
#define DECODE zyx_to_quat
#endif    

void zyx_to_quat(quat_t* q, vec3_t* v);
void zxz_to_quat(quat_t* q, vec3_t* v);

void sobol_quat_trouble(quat_t* q)
{
  vec3_t zyx;
  sobol_yaw_roll(&zyx);
  DECODE(q, &zyx);
}

#endif


// H in binary64 for error measures
typedef struct { double x,y,z,w; } quatd_t;

static inline void quat_to_d(quatd_t* r, quat_t* q)
{
  r->x = q->x; r->y = q->y; r->z = q->z; r->w = q->w;
}

static inline void quatd_set(quatd_t* r, double x, double y, double z, double w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

static inline void quatd_conj(quatd_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

static inline void quatd_mul(quatd_t* r, quatd_t* a, quatd_t* b)
{
  double x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  double y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  double z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  double w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quatd_set(r,x,y,z,w);
}

// returned angle is in degrees and as R^3 and not H.
// use doubles to compute relative quaternion BA*,
// and atan2 to determine angle between A & B. This
// measure isn't effected by errors in magnitude..
// better or worse than acos measure..it depends.
// (well, really you can choose which measure)
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
  double t = atan2(y,x);
  float  r = (float)(1.1459156036376953125e2*t);

  if (r <= 180.f) return r;

  return 180.f-r;
#else
  double ax=a->x, ay=a->y, az=a->z, aw=a->w;
  double bx=b->x, by=b->y, bz=a->z, bw=b->w;
  double d = fabs(ax*bx + ay*by + az*bz + aw*bw);
  double t = d <= 1.0 ? acos(d) : 0;
  return 1.1459156036376953125e2*t;
#endif  
}

double m_error(quat_t* a)
{
  double ax=a->x, ay=a->y, az=a->z, aw=a->w;
  return sqrt(ax*ax+ay*ay+az*az+aw*aw);
}

// ab+cd
inline float f32_mma(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b, t);
  return f+e;
}

// ab-cd
inline float f32_mms(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b,-t);
  return f-e;
}



// Tait-Bryan: yaw-pitch-roll (zyx)
// pitch = theta = asin(-2(m11-wy))
// yaw   = psi   = atan2(xy+wz, 1/2-(yy+zz))
// roll  = phi   = atan2(yz+wx, 1/2-(m00+yy))

#define YPR_GIMBAL_LOCK (0.5f*(1.f-5.f*ULP1))

// what I had to start with (orig threshold of 1-5*ulp(1))
uint32_t original(vec3_t* v, quat_t* q)
{
  LOAD_Q(q);
  uint32_t r = 0;
  
  // half z-component of x' (negated)
  float xz = w*y-x*z;

  v->z = atan2f(x*y+w*z, .5f-(y*y+z*z));    // yaw
  v->y = atanf(xz/sqrtf(0.25f-xz*xz));      // pitch
  
  if (fabsf(xz) < YPR_GIMBAL_LOCK)
    v->x = atan2f(y*z+w*x, .5f-(x*x+y*y));
  else {
    v->x = 2.f*atan2f(x,w) + sgn(xz)*v->z;
    r = 1;
  }
  
  return r;
}

uint32_t original_fma(vec3_t* v, quat_t* q)
{
  LOAD_Q(q);
  uint32_t r = 0;
  
  // half z-component of x' (negated)
  float xz = f32_mms(w,y,x,z);              // wy-xz
  float t0 = f32_mma(x,y,w,z);              // xy+wz
  float t1 = f32_mma(y,y,z,z);              // yy+zz

  v->z = atan2f(t0, .5f-t1);                // yaw
  v->y = atanf(xz/sqrtf(0.25f-xz*xz));      // pitch
  
  if (fabsf(xz) < YPR_GIMBAL_LOCK) {
    float t2 = f32_mma(y,z,w,x);
    float t3 = f32_mma(x,x,y,y);
    v->x = atan2f(t2, 0.5f-t3);
  }
  else {
    v->x = 2.f*atan2f(x,w) + sgn(xz)*v->z;
    r = 1;
  }
  
  return r;
}



// to show just promoting doesn't help
uint32_t original_d(vec3_t* v, quat_t* q)
{
  LOAD_QD(q);
  uint32_t r = 0;

  double xz = w*y-x*z;

  v->z = (float)atan2(x*y+w*z, .5-(y*y+z*z)); 
  v->y = (float)atan(xz/sqrt(0.25-xz*xz));
  
  if (fabs(xz) < YPR_GIMBAL_LOCK) {
    v->x = (float)atan2(y*z+w*x, .5-(x*x+y*y));
  }
  else {
    v->x = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
    r = 1;
  }
  
  return r;
}

// 1) removed simplifications based on |Q|= 1
// 2) change test of approaching degenerate
//    to approaching zero expression: x^2 + y^2
//    instead of approching one: z, where
//    (x,y,z) = x'

uint32_t revision_1(vec3_t* v, quat_t* q)
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
  v->y = (float)atan(xz/sqrt(t));

  if (t != 0) {
    v->x = (float)atan2(yz, t1-t0);
    return 0;
  }

  v->x = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
  return 1;
}    

// ab-cd
inline double f64_mms(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b,-t);
  return f-e;
}

inline double f64_mma(double a, double b, double c, double d)
{
  double t = c*d;
  double e = fma(c,d,-t);
  double f = fma(a,b, t);
  return f+e;
}

// xformed to use fma
uint32_t revision_1_fma(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;
  double t0 = (x+z)*(x-z);
  double t1 = (w+y)*(w-y);
  double xx = 0.5*(t0+t1);
  double xy = f64_mma(x,y,w,z);
  double xz = f64_mms(w,y,x,z);
  double yz = 2.0*(f64_mma(y,z,w,x));
  double t  = f64_mma(xx,xx,xy,xy);

  v->z = (float)atan2(xy, xx);
  v->y = (float)atan(xz/sqrt(t));

  if (t != 0) {
    v->x = (float)atan2(yz, t1-t0);
    return 0;
  }

  v->x = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
  return 1;
}    


// dropped down to binary32
uint32_t revision_1_fma_s(vec3_t* v, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;
  
  float t0 = (x+z)*(x-z);
  float t1 = (w+y)*(w-y);
  float xx = 0.5f*(t0+t1);
  float xy = f32_mma(x,y,w,z);
  float xz = f32_mms(w,y,x,z);
  float yz = 2.f*(f32_mma(y,z,w,x));
  float t  = xx*xx+xy*xy;

  v->z = atan2f(xy, xx);
  v->y = atanf(xz/sqrtf(t));

  if (t != 0) {
    v->x = atan2f(yz, t1-t0);
    return 0;
  }

  v->x = 2.f*atan2f(x,w) - sgn(xz)*v->z;
  return 1;
}    


// could trade some products/additions
uint32_t revision_1x(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;

  double a = x-z, b = x+z;
  double c = w-y, d = w+y;
  double t0 = a*b;              // x^2-z^2
  double t1 = c*d;              // w^2-y^2
  double xx = 0.5*(t0+t1);
  double xy = x*y+w*z;
  double xz = w*y-x*z;
  double t  = xx*xx+xy*xy;
  double yz = 2.0*(y*z+w*x);

  v->z = (float)atan2(xy, xx);
  v->y = (float)atan(xz/sqrt(t));

  if (t != 0) {
    v->x = (float)atan2(yz, t1-t0);
    return 0;
  }

  v->x = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
  return 1;
}    



// moving 'revision_1' back to single.
// Exactly zero too tight, move back.
// The degenerate case explodes here...
// ignore til next revision.
// common case wrt branch point:
// n*ulp   : maxerror : prob in common (measured)
//       1 : 0.005778 : 0.99999994
//      10 : 0.002416 : 0.9999994
//     100 : 0.000785 : 0.999994
//    1000 : 0.000265 : 0.999942
//    5000 : 0.000121 : 0.999704
//   10000 : 0.000089 : 0.999367
//  100000 : 0.000049 : 0.990194
// 1000000 : 0.000039 : 0.888527

uint32_t revision_1s(vec3_t* v, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;

  float t0 = (x+z)*(x-z);
  float t1 = (w+y)*(w-y);
  float xx = 0.5f*(t0+t1);
  float xy = x*y+w*z;
  float xz = w*y-x*z;
  float yz = 2.f*(y*z+w*x);
  float t  = xx*xx+xy*xy;

  v->z = atan2f(xy, xx);
  v->y = atanf(xz/sqrtf(t));

  // result is only good as per table above
  if (fabsf(t) >= 1000.f*ULP1) {
    v->x = atan2f(yz, t1-t0);
    return 0;
  }

  // this is a garbage result in binary32
  v->x = 2.f*atan2f(x,w) - sgn(xz)*v->z;
  return 1;
}    

// punt: use previous for common case and
// otherwise perform promote to double as
// per the double version of revision_1.
uint32_t revision_2s(vec3_t* v, quat_t* q)
{
  {
    float x=q->x, y=q->y, z=q->z, w=q->w;
    
    float t0 = (x+z)*(x-z);   // x^2-z^2
    float t1 = (w+y)*(w-y);   // w^2-y^2
    float xx = 0.5f*(t0+t1);  //     x'_x
    float xy = x*y+w*z;       // 1/2 x'_y
    float xz = w*y-x*z;       // 1/2 x'_z
    float t  = xx*xx+xy*xy;
    
    if (fabsf(t) >= 250000.f*ULP1) {
      float yz = 2.f*(y*z+w*x);
      v->z = atan2f(xy, xx);
      v->y = atanf(xz/sqrtf(t));
      v->x = atan2f(yz, t1-t0);
      return 0;
    }
  }

  double x=q->x, y=q->y, z=q->z, w=q->w;
  double a = x-z, b = x+z;
  double c = w-y, d = w+y;
  double t0 = a*b;
  double t1 = c*d;
  double xx = 0.5*(t0+t1);
  double xy = x*y+w*z;
  double xz = w*y-x*z;
  double yz = 2.0*(y*z+w*x);
  double t  = xx*xx+xy*xy;

  v->z = (float)atan2(xy, xx);
  v->y = (float)atan(xz/sqrt(t));

  if (t != 0) {
    v->x = (float)atan2(yz, t1-t0);
    return 1;
  }

  v->x = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
  return 2;
}    


// goofying with "fast path" version that attempts
// to avoid using fma.
uint32_t revision_3s(vec3_t* v, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;
  
  float t0 = (x+z)*(x-z);   // x^2-z^2
  float t1 = (w+y)*(w-y);   // w^2-y^2
  float xx = 0.5f*(t0+t1);  //     x'_x
  float xy = x*y+w*z;       // 1/2 x'_y
  float xz = w*y-x*z;       // 1/2 x'_z
  float t  = xx*xx+xy*xy;
  float yz;
  
  if (fabsf(t) >= 0x1p-6f) {
    // taken: ~72,79.6,83.9%  (1/16,1/32,1/64)
    yz = 2.f*(y*z+w*x);
    v->z = atan2f(xy, xx);
    v->y = atanf(xz/sqrtf(t));
    v->x = atan2f(yz, t1-t0);
    return 0;
  }

  // repeating the above computation using fma.
  // need to check compiler is reusing the subexpressions
  xy = f32_mma(x,y,w,z);
  xz = f32_mms(w,y,x,z);
  yz = 2.f*(f32_mma(y,z,w,x));
  t  = xx*xx+xy*xy;

  v->z = atan2f(xy, xx);
  v->y = atanf(xz/sqrtf(t));

  if (t != 0) {
    // taken: ~27.9,20.1,16.7% (1/16,1/32,1/64)
    v->x = atan2f(yz, t1-t0);
    return 1;
  }

  v->x = 2.f*atan2f(x,w) - sgn(xz)*v->z;
  return 2;
}    






//**** ZXZ
// can't be bothered to play with this since it's a
// very uncommon format

uint32_t quat_to_zxz(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;

  double s2 = x*x+y*y;      // sin(beta)^2
  double c2 = w*w+z*z;      // cos(beta)^2
  double s  = atan(z/w);    // (gamma+alpha)/2
  double d  = atan2(y,x);   // (gamma-alpha)/2
  v->x = (float)(s-d);      // alpha
  v->z = (float)(s+d);      // gamma

  if (c2 != 0.0) {
    v->y = (float)(2.0*atan(sqrt(s2/c2)));
    return 0;
  }

  v->y = (0.5 > s2) ? 0 : PI;

  return 1; 
}


// pointer to quaternion generator
typedef void (*qgen_t)(quat_t*);

// pointer to quaternion to ZYX method
typedef uint32_t (*to_zyx_t)(vec3_t*, quat_t*);

typedef struct {
  qgen_t   f;
  uint32_t t;
  char*    name;
} gens_t;

#define DEF(S)    {&S, SYM_TRIALS, XSTR(S)}
#define DEFX(S,T) {&S, T, XSTR(S) }

gens_t gens[] =
{
#if !defined(UNIFORM_ONLY)  
  DEFX(test_vector, sizeof(test_vector_data)/sizeof(quat_t)),

#if defined(USE_SOBOL)  
  DEF(sobol_quat_trouble),
  DEF(sobol_quat_about_diag),
  //DEF(sobol_quat_about_x),
  //DEF(sobol_quat_about_xy),
#endif  

  DEF(uniform_quat_trouble),
  DEF(uniform_quat_about_diag),
  DEF(uniform_quat_from_z),
  //DEF(uniform_quat_about_x),
  //DEF(uniform_quat_about_xy),
#else  
#if defined(USE_SOBOL)  
  DEF(sobol_quat),
#endif
#endif  
  DEF(uniform_quat)
};

#undef  DEF
#undef  DEFX
#define NUM_GENS (sizeof(gens)/sizeof(gens[0]))


typedef struct {
  to_zyx_t f;
  to_zyx_t fd;
  char*    name;
} maps_t;

#define DEF(S)    {&S, (void*)0, XSTR(S)}
#define DEFX(S,D) {&S, &D,       XSTR(S D)}

// defines a table of quat->XYZ conversions, DEF for single method DEFX to
// additionally report the error of the second method.
maps_t maps[] =
{
#if !defined(ZXZ)
  //DEF(original)
  //DEF(original_d)
  //DEF(original_fma)
  //DEF(revision_1),
  //DEF(revision_1_fma)
  //DEF(revision_1_fma_s)
  //DEF(revision_1x)
  //DEF(revision_1s)
  //DEF(revision_2s)
  DEF(revision_3s)
#else
   DEF(quat_to_zxz)
#endif  
};

#undef DEF
#undef DEFX
#define NUM_FUNCS (sizeof(maps)/sizeof(maps[0]))


// Tait-Bryan ZYX to quaternion
#if !defined(DOUBLE2Q)
void zyx_to_quat(quat_t* q, vec3_t* v)
{
  float hy = 0.5f*v->z;
  float hp = 0.5f*v->y;
  float hr = 0.5f*v->x;
  float ys = sinf(hy), yc = cosf(hy);
  float ps = sinf(hp), pc = cosf(hp);
  float rs = sinf(hr), rc = cosf(hr);

  q->w = rc*pc*yc + rs*ps*ys;
  q->x = rs*pc*yc - rc*ps*ys;
  q->y = rc*ps*yc + rs*pc*ys;
  q->z = rc*pc*ys - rs*ps*yc;

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}
#else
void zyx_to_quat(quat_t* q, vec3_t* v)
{
  double hy = 0.5*v->z;
  double hp = 0.5*v->y;
  double hr = 0.5*v->x;
  double ys = sin(hy), yc = cos(hy);
  double ps = sin(hp), pc = cos(hp);
  double rs = sin(hr), rc = cos(hr);

  q->w = (float)(rc*pc*yc + rs*ps*ys);
  q->x = (float)(rs*pc*yc - rc*ps*ys);
  q->y = (float)(rc*ps*yc + rs*pc*ys);
  q->z = (float)(rc*pc*ys - rs*ps*yc);

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}
#endif


#if !defined(DOUBLE2Q)
// Euler ZXZ to quaternion
void zxz_to_quat(quat_t* q, vec3_t* v)
{
  float a = 0.5f*v->x;
  float b = 0.5f*v->y;
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
#else
void zxz_to_quat(quat_t* q, vec3_t* v)
{
  double a = 0.5f*v->x;
  double b = 0.5f*v->y;
  double c = 0.5f*v->z;

  double scma = sin(c-a), ccma = cos(c-a);
  double scpa = sin(c+a), ccpa = cos(c+a);
  double sb   = sin(b),   cb   = cos(b);

  q->w = cb*ccpa;
  q->x = sb*ccma;
  q->y = sb*scma;
  q->z = cb*scpa;

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}
#endif


typedef struct {
  quat_t s;      // untransformed src
  double max;    // max error in bin
} e_histo_t;


typedef struct {
  e_histo_t a[HLEN];
} a_error_t;

a_error_t a_errors[NUM_FUNCS];

void error_init()
{
  memset(a_errors, 0, sizeof(a_errors));
}

double error_add(quat_t* s, quat_t* r, a_error_t* e)
{
  double   ae  = a_error(s,r);
  double   t   = fabs(ae);
  uint32_t hid;

  // hiso

#if !defined(HISTO_TYPE) || (HISTO_TYPE == 0)  
  hid = (uint32_t)(((2.f*HLEN)/PI)*acosf(s->w));
#elif (HISTO_TYPE == 1)
#if !defined(ZXZ)  
  hid = (uint32_t)((HLEN)*(2.f*fabsf(s->x*s->z-s->w*s->y)));
#else
  hid = (uint32_t)((HLEN)*(2.0*fabsf(1.0-2.0*(s->x*s->x+s->z*s->z))));
#endif  
#elif (HISTO_TYPE == 2)
  {
    float t = 2.f*fabsf(s->x*s->z-s->w*s->y);
    t = t > .99f ? (t-.99f)/.01f : 0.f;
    hid = (uint32_t)((HLEN)*t);
  }
#else
  hid = (uint32_t)(HLEN*HIST_FUNC());
#endif  

  if (hid >= HLEN) hid=HLEN-1;

  e_histo_t* h = &e->a[hid]; 
  
  if (ae > h->max) {
    h->max = ae;
    quat_dup(&h->s, s);
  }

  return t;
}

void dump_xform(quat_t* q)
{
  vec3_t x,y,z;
  quat_to_local(&x,&y,&z,q);
  printf("  x':"); vec3_print(&x); vec3_printa(&x); ln(); 
  printf("  y':"); vec3_print(&y); vec3_printa(&y); ln(); 
  printf("  z':"); vec3_print(&z); vec3_printa(&z); ln(); 
}

// dump out simple error reports per tested method
void error_dump()
{
  uint32_t s;

  for(s=0; s<NUM_FUNCS; s++) {
    a_error_t* e  = &a_errors[s];
    double     m  = 0;
    uint32_t   mi = 0;

    printf("%8s:\n", maps[s].name);

    printf("max: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].max); if (e->a[i].max > m) { m = e->a[i].max; mi = i; } } ln();

    // dump out global max error info
    to_zyx_t f  = maps[s].f;
    quat_t*  q  = &e->a[mi].s;
    quat_t   r;
    vec3_t   v;

    f(&v, q);
    DECODE(&r, &v);

    printf("global max: %e, mag-error: %e ",m, fabs(1.0-m_error(q)));
    
    printf("\n  orig   roll/pitch/yaw  recon\n");
    quat_print(q); vec3_print(&v); quat_print(&r);
    printf("\n  ");
    quat_printa(q); vec3_printa(&v); quat_printa(&r); ln();

    printf("orig: %e\n", 1.0-quat_mag(q));  dump_xform(q);
    printf("recon:\n"); dump_xform(&r);
    ln();
  }
}


uint64_t method_count[MAX_METHODS];

void rt_test(uint64_t s0, uint64_t s1)
{
  // walk through all the conversion functions
  for(uint32_t ci=0; ci<NUM_FUNCS; ci++) {
    to_zyx_t   f  = maps[ci].f;
    a_error_t* et = &a_errors[ci];

    printf("%s\n", maps[ci].name);
    memset(method_count, 0, sizeof(method_count));

    for(uint32_t gi=0; gi<NUM_GENS; gi++) {
      qgen_t   gen = gens[gi].f;
      uint32_t len = gens[gi].t;
      quat_t   q,r;
      vec3_t   zyx;
      uint32_t s;
      double   e,me = 0;
      
      reset_generators(s0,s1);
      printf("  %s\n", gens[gi].name);
      
      // generate 'len' test values
      for(uint32_t i=0; i<len; i++) {
	gen(&q);                  // generate test quaternion (Q)
	s = f(&zyx, &q);          // use method to convert to ZYX/ZXZ
	e = 0;
	method_count[s]++;        // track prob

#if defined(ONLY_METHOD_0)	
        if (s == 0)
#elif defined(ONLY_NOT_METHOD_0)	
        if (s != 0)
#endif	
	{
	  DECODE(&r, &zyx);         // round-trip back to quaternion (Q')
	  e = error_add(&q,&r,et);  // measure and track the error
	}
	
	// spew out peak error per method as seen
	if (e > me) {
  	  me = e;
	  if (maps[ci].fd != (void*)0) {
	    // same computation by second method if specified
	    vec3_t vd;
	    quat_t rd;
	    uint32_t s2 = maps[ci].fd(&vd, &q);
	    DECODE(&rd, &vd);
	    double ed = a_error(&q, &rd);

	    printf("%08x (%d/%d): ", i,s,s2);
	    
	    if (me > ed) {
	      vec3_t vdiff;
	      vec3_sub(&vdiff, &vd, &zyx);
	      printf("!");

	      if (me > 0.1) {
		vec3_print(&vd); vec3_print(&vdiff);
		printf("\n                ");
	      }
	    }
	    else {
	      if (me < ed)
		printf("^");
	      else
		printf(" ");
	    }
	    
	    printf(" %f %f ", me,ed);
	  }
	  else {
	    printf("%08x (%d): %f ", i,s, me);
	  }
	  
	  quat_print(&q);
	  vec3_print(&zyx);
	  quat_print(&r);

	  //vec3_t xp;
	  //quat_local_x(&xp,&q);
	  //vec3_print(&xp);
	  printf("\n");
	}
      }
    }

    for(uint32_t i=0; i<MAX_METHODS; i++) {
      printf("%d ", (uint32_t)method_count[i]);
    }
    
    ln();
  }
}

// a simple round-trip test for debugging dev of mapping function
#define TLEN 0x7FFFFFF

void rt_xform_test(uint64_t s0, uint64_t s1)
{
  double me = 0.0;
  quat_t p,r,diff;
  vec3_t v;
  float xmin= 1.f,ymin= 1.f,zmin= 1.f;
  float xmax=-1.f,ymax=-1.f,zmax=-1.f;
  
  reset_generators(s0,s1);

  for(uint32_t i=0; i<TLEN; i++) {
    //uniform_quat_about_diag(&p);
    uniform_quat(&p);
    p.w = fabsf(p.w);

    // test generalize or explict
    uint32_t s = original(&v,&p);
    DECODE(&r,&v);

    if (v.x > xmax) { xmax = v.x; } else if (v.x < xmin) { xmin = v.x; }
    if (v.y > ymax) { ymax = v.y; } else if (v.y < ymin) { ymin = v.y; }
    if (v.z > zmax) { zmax = v.z; } else if (v.z < zmin) { zmin = v.z; }
    quat_sub(&diff, &p, &r);

    double e = a_error(&p,&r);
    
    if (e > me || e > 1.f) {
      vec3_t vd;
      quat_t rd;
      s = original_d(&vd,&p);
      DECODE(&rd,&vd);
      double ed = a_error(&p, &rd);
      
      me = e;
      printf("%08x: %d %f %f",i,s, me,ed);
      printf(" -- % a -- ", 0.5f-(p.x*p.z-p.y*p.w));
      quat_print(&p);
      vec3_print(&v);
      quat_print(&r);
      quat_printa(&diff);
      printf("\n");
    }
  }

  printf("x:(%f,%f) y:(%f,%f) z:(%f,%f)\n", xmin,xmax,ymin,ymax,zmin,zmax);
}

int main()
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

  reset_generators(s0,s1);
    
  rt_test(s0,s1);
  error_dump();
  ln();

  return 0;
}
