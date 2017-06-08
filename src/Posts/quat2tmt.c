// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/2017/05/12/HopfCoordConvert.html

// ex. requirements:
// * quaternion header
// * Sobol seq header (optional, SEE: USE_SOBOL)

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// config section

// run sobol sequence tests (requires the header)
#define USE_SOBOL

// number of trials to run per test
//#define SYM_TRIALS (0x07FFFFFF<<2)
#define SYM_TRIALS 0x07FFFFFF
//#define SYM_TRIALS 0x00FFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// number of entries in histogram
#define HLEN 100

// choose between atan or acos based error measuring
//#define ACOS_MEASURE

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
_inline uint32_t lzc64(uint64_t x) { unsigned __int64 r; _BitScanReverse64(&r, (__int64)x); return (uint64_t)r; }

#else
#define TO_FP32        0x1p-24f
#define SQRT2          0x1.6a09e6p0f  // sqrt(2)
#define ISQRT3         0x9.3cd3ap-4f  // 1/sqrt(3)
#define PI             0x3.243f6cp0f
#define RPI            0x5.17cc18p-4f
#define F32_MIN_NORMAL 0x1p-126f
inline uint32_t lzc64(uint64_t x) { return __builtin_clzl(x); }
#endif

#define ULP1 TO_FP32
#define EPS  F32_MIN_NORMAL

// note on [0,1]
inline float rng_f32_x(void)
{
  double u = (rng_u64() >> 11)*0x1p-53f;

  return (float)u;
}

static inline float f32_from_bits(uint32_t v) { float r; memcpy(&r,&v,4); return r; }

// uniform on (0,1)
float rng_dense_f32()
{
  // this is a special case version based on
  // an rng_u64 that does not return zero.
  uint64_t u = rng_u64();
  uint32_t e = lzc64(u);
  uint32_t s;
  
  // are there at least 23 bits left for significand?
  if (e <= 41)
    s = (uint32_t)u;
  else
    s = (uint32_t)rng_u64();
  
  e = 126-e;        // exponent
  s &= 0x7FFFFF;    // 23 bit for significand
  
  return f32_from_bits(e<<23|s);
}


// uniform on (-1,1)
float rng_dense_f32s()
{
  // this is a special case version based on
  // an rng_u64 that does not return zero.
  uint64_t u = rng_u64();
  uint32_t e = lzc64(u);
  uint32_t s;
  
  if (e <= 40)
    s = (uint32_t)u;
  else
    s = (uint32_t)rng_u64();

  // TODO: add sign bit
  
  e = 126-e;
  s &= 0x7FFFFF;
  
  return f32_from_bits(e<<23|s);
}

double hypot(double p, double q)
{
  double r;
  p = fabs(p);
  q = fabs(q);
  
  if (p < q) { r = p; p = q; q = r; }
  r = q/(p + FP32_MIN);
  return p*sqrt(1+r*r);
}


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


#define LOAD_Q(Q)  float  x=Q->x, y=Q->y, z=Q->z, w=Q->w;
#define LOAD_QD(Q) double x=Q->x, y=Q->y, z=Q->z, w=Q->w;


void normalize_hack(quat_t* q)
{
  LOAD_QD(q);
  double s = 1.0/sqrt(x*x+y*y+z*z+w*w);
  quat_set(q, (float)(s*x),(float)(s*y),(float)(s*z),(float)(s*w));
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

void uniform_hopf(quat_t* q)
{
  vec2_t c;
  float u  = 2.f*PI*rng_f32_x();
  float mx = cosf(u);
  float my = sinf(u);
  float tz = 10.f*TO_FP32*rng_f32_s();
  float ms = (float)sqrt(1.0-mx*mx-my*my);
  float ts = (float)sqrt(1.0-tz*tz);
  
  q->x = mx*ts + my*tz;
  q->y = my*ts - mx*tz;
  q->z = ms*tz;
  q->w = ms*ts;
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

//============================================================

typedef struct {
  float mx,my,ms;   // torque minimal wrt positive Z
  float tz,ts;      // twist
} tmtwist_t;

#define THRESHOLD (ULP1*ULP1)

void quat_to_tmtwist(tmtwist_t* d, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;
  float t=w*w+z*z;

  if (t > THRESHOLD) {
    float s = 1.f/sqrtf(t);
    d->mx = s*(w*x-y*z);
    d->my = s*(w*y+x*z);
    d->ms = s*t;
    d->tz = s*z;
    d->ts = s*w;
  }
  else {
    d->mx = x;
    d->my = y;
    d->ms = 0;
    d->tz = 0;
    d->ts = 1.f;
  }
}

void tmtwist_to_quat(quat_t* q, tmtwist_t* s)
{
  q->x = s->mx*s->ts + s->my*s->tz;
  q->y = s->my*s->ts - s->mx*s->tz;
  q->z = s->ms*s->tz;
  q->w = s->ms*s->ts;
}

//============================================================

#define TLEN 0x7FFFFFF

void rt_test(uint64_t s0, uint64_t s1)
{
  double me = -1.0;
  quat_t p,r,diff;
  
  reset_generators(s0,s1,TLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    stmtwist_t tm;

    //uniform_quat_about_diag(&p);
    //sobol_quat(&p);
    uniform_hopf(&p);
    p.w = fabsf(p.w);

    quat_to_stmtwist(&tm,&p);
    stmtwist_to_quat(&r,&tm);
    quat_sub(&diff, &p, &r);

    double e = a_error(&p,&r);
    
    if (e > me) {
      me = e;
      printf("%08x: %f %e ",i, me, p.w*p.w+p.z*p.z);
      //printf(" -- % a -- ", 0.5f-(p.x*p.z-p.y*p.w));
      quat_print(&p);
      //vec3_print(&v);
      quat_print(&r);
      quat_printa(&diff);
      printf("\n");
    }
  }

  //printf("x:(%f,%f) y:(%f,%f) z:(%f,%f)\n", xmin,xmax,ymin,ymax,zmin,zmax);
}

int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;

#ifdef RANDOMIZE
  s0 = _rdtsc();
#else  
  s0 = 0x77535345;
#endif  
  s1 = 0x1234567;

  reset_generators(s0,s1,SYM_TRIALS);
    
  rt_test(s0,s1);
  ln();

  return 0;
}
