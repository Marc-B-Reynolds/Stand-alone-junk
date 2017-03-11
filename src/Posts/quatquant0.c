// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io

// This performs sloppy empirical testing.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include <x86intrin.h>
#include <math.h>


// config section

// run sobol sequence tests
#define USE_SOBOL

// number of trials to run per test
//#define SYM_TRIALS 0x7FFFFFF
#define SYM_TRIALS 0xFFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// number of quant bits. hacky to give
// impefect view of how errors drop given
// an arbitrary number of bits
#ifndef BITS
#define BITS 32
#endif

#define CM_TESTS   // composed methods
#define EX_TESTS   // explicit methods

// rotation angle buckets
#define HLEN 80

//#define DUMP_RMS
//#define DUMP_AVE

// end config

#define XSTR(X) STR(X)
#define STR(X)  #X

#include "../SFH/quat.h"

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

static inline float rng_f32(void)
{
  return (rng_u64() >> 40)*TO_FP32;
}

// for quickly/first-pass hacking in arch specific junk
// !!!!!! the recip hurts performance w current gcc/clang..
//        actually does the 1/x and product...sigh.
static inline float sgn(float x)   { return x >= 0.f ? 1.f : -1.f; }
static inline float mulsgn(float x, float v) { return x >= 0.f ? v : -v; }
static inline float rsqrt(float v) { return 1.f/sqrtf(v); }
static inline float recip(float v) { return 1.f/v; }
static inline float rnd(float v)   { return floorf(v+0.5f); }

static inline float pmax(float a, float b)   { return a >= b ? a : b; }
static inline float pmin(float a, float b)   { return a <= b ? a : b; }


static inline float clamp(float v, float lo, float hi)
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


//static inline uint32_t f32_to_bits(float v)      { uint32_t r; memcpy(&r,&v,4); return r; }
//static inline float    f32_from_bits(uint32_t v) { float r;    memcpy(&r,&v,4); return r; }

float uniform_disc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*0x1p-24f;
    y = (v & 0xFFFFFF)*0x1p-24f;
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
    x = (v >> 40)*0x1p-24f;
    y = (v & 0xFFFFFF)*0x1p-24f;
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
    d  = x*x;
    y  = (v & 0xFFFFFF)*TO_FP32;
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
  float d = uniform_disc(p);
  float s = rsqrt(d+(CBIAS*CBIAS));  // 1/sqrt approx + X NR steps
  p->x += CBIAS;                     // map (0,0) -> (1,0)
  p->x *= s;
  p->y *= s;
}

void uniform_quat_about_x(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  quat_set(q, p.y, 0.f, 0.f, fabs(p.x)); // silly...direct half-circle
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
  float t = sinf(PI*(sobol_1d_next_f32(&srng)-0.5f));
  float b = t * ISQRT3;
  quat_set(q, b,b,b, sqrtf(1.f-t*t));
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
float a_error(quat_t* a, quat_t* b)
{
  quatd_t R,A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  quatd_conj(&A);
  quatd_mul(&R,&B,&A);

  double x = R.w;
  double y = sqrt(R.x*R.x+R.y*R.y+R.z*R.z);
  double t = atan2(y,x);
  float  r = (float)(1.1459156036376953125e2*t);

  if (r <= 180.f) return r;

  return 180.f-r;
}


//****** ball/cube/cylinder stuff 

// hacky macros to fill coordinate
#define LOAD_XYZ(S)  float x=S->x, y=S->y, z=S->z;
#define LOAD_XYZ2(S) float x=S->x, y=S->y, z=S->z, x2=x*x, y2=y*y, z2=z*z;

// stretch
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
  float s = sqrtf(x2+y2+z2)/(fabsf(m)+EPS);
  //printf("%f", s);

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
#if 1

void fem(vec3_t* v, quat_t* q)
{
  float w = q->w;
  float a = 1.f-w*w;
  float b = rsqrt(a+EPS*EPS);
  float k = a*b;
  float s = (2.f/PI)*atanf(k/w)*b;
  quat_put_bv(v,q,s);
}

void iem(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float t = rsqrt(d+EPS*EPS);
  float a = (PI/2.f)*t*d;
  float s = sinf(a);
  float k = s*t;
  quat_bv_set_scale(q,v,k);
  q->w = cosf(a);
//q->w = sqrtf(1.f-s*s);
}

#else

// testing junk
void fem(vec3_t* v, quat_t* q)
{
  float w = q->w;
  float a = 1.f-w*w;
  float b = rsqrt(a+EPS*EPS);
  float k = a*b;
  float s = (2.f/PI)*atanf(k/w);
  quat_put_bv(v,q, s*b);
}

void iemx(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float t = rsqrt(d+EPS*EPS);
  float a = (PI/2.f)*t*d;
  float s = sinf(a);
  float k = s*t;
  quat_bv_set_scale(q,v,k);
  q->w = cosf(a);
}
#endif

// proc pointer to quaternion generator
typedef void (*qsrc)(quat_t*);

// unit quaternion to ball and inverse
typedef void (*s3_to_b3)(vec3_t*, quat_t*);
typedef void (*b3_to_s3)(quat_t*, vec3_t*);

// maps between cube and ball (both directions)
typedef void (*cb_map)(vec3_t*, vec3_t*);

typedef struct {
  s3_to_b3 f;   //
  b3_to_s3 g;   //
  cb_map   bc;  //
  cb_map   cb;  //
  char*    name;
} maps_t;


// define maps
typedef void (*map_func_t)(vec3_t*, vec3_t*);

#define DEF(S,X) {&f ## S, &i ## S, &map_ ## X ## _bc, &map_ ## X ## _cb, XSTR(S##X)}

maps_t maps[] =
{
#if 0  
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

//DEF(dw,id),  // no transform
//DEF(em,vp),
//DEF(em,rs),
  DEF(em,id),  // exp-map
#if 1
  DEF(hm,id),  // harmonic mean
  DEF(ha,id),  // half-angle
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

#if 1
uint64_t quant(float f, float s)
{
  uint64_t i = (uint64_t)(f*s);
  if (i > s) i = s;
  return i;
}

float dequant(uint64_t b, float s) { return (b+0.5f)*s; }
#else
#endif

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

#define X1_BITS    (BITS-1)
#define X1_BITS_X  (X1_BITS/3)
#define X1_BITS_Y  ((X1_BITS-X1_BITS_X)/2)
#define X1_BITS_Z  (X1_BITS-X1_BITS_X-X1_BITS_Y)

#define X2_BITS    (BITS-2)
#define X2_BITS_X  (X2_BITS/3)
#define X2_BITS_Y  ((X2_BITS-X2_BITS_X)/2)
#define X2_BITS_Z  (X2_BITS-X2_BITS_X-X2_BITS_Y)

// smallest-of-three
uint64_t fsot(quat_t* q)
{
  float    m  = fabs(q->w);
  float    ax = fabs(q->x);
  float    ay = fabs(q->y);
  float    az = fabs(q->z);
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


// all temp hacks

uint64_t fem1(quat_t* q)
{
  vec3_t   v;
  uint64_t r = 0;
  float    w = q->w;
  float    a = 1.f-w*w;
  float    b = rsqrt(a+EPS*EPS);
  float    k = a*b;
  float    s = (2.f/PI)*atanf(k/w)*b;

  if (w > SQRT2) { s *= 2.f; r |= 1L<<63; }

  quat_put_bv(&v,q,s);
  
  return r|vencodex(&v, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
}

void iem1(quat_t* q, uint64_t b)
{
  vec3_t v;

  vdecodex(&v, b, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  if ((int64_t)b < 0) { vec3_scale(&v, 0.5f); }
  
  float d = vec3_norm(&v);
  float t = rsqrt(d+EPS*EPS);
  float a = (PI/2.f)*t*d;
  float s = sinf(a);
  float k = s*t;
  quat_bv_set_scale(q,&v,k);
  q->w = cosf(a);
}

#if 0
#define K0 0.5f*SQRT2
#define K1 SQRT2
#define K2 0.5f*SQRT2
#else
#define K0 0.923880f
#define K1 2.61313f
#define K2 0.382683f
#endif


uint64_t fha1(quat_t* q)
{
  vec3_t   v;
  uint64_t r = 0;
  float    d = 1.f + q->w;
  float    s = QUAT_RSQRT(d);

  if (q->w >= K0) { s *= K1; r |= 1L<<63; }
  
  quat_put_bv(&v,q,s);
  return r|vencodex(&v, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
}

void iha1(quat_t* q, uint64_t b)
{
  vec3_t v;

  vdecodex(&v, b, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
  if ((int64_t)b < 0) { vec3_scale(&v, K2); }

  float d = vec3_norm(&v);
  float s = QUAT_SQRT(2.f-d); 
  quat_bv_set_scale(q,&v,s);
  q->w = 1.f-d;
}

#undef K0
#undef K1
#undef K2

uint64_t ftoy(quat_t* q)
{
  vec3_t   v;
  uint64_t r = 0;
  float    w = q->w;

  if (w >= .5f*SQRT2) {
    quat_put_bv(&v,q, SQRT2);
    //fha(&v,q);
  }
  else {
    r |= 1L<<63;
    fem(&v,q);
    //float d = 1.f + w;
    //float s = QUAT_RSQRT(d);
    //quat_put_bv(&v,q,s);
  }

  return r|vencodex(&v, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);
}

void itoy(quat_t* q, uint64_t b)
{
  vec3_t v;

  vdecodex(&v, b, X1_BITS_X, X1_BITS_Y, X1_BITS_Z);

  if ((int64_t)b >= 0) {
    //iha(q,&v);
    float d = vec3_norm(&v);
    quat_bv_set_scale(q,&v, 0.5f*SQRT2);
    q->w = sqrtf(1.f-0.5f*d);
  }
  else {
    iem(q,&v);
    //float s = QUAT_SQRT(2.f-d); 
    //quat_bv_set_scale(q,&v,s);
    //q->w = 1.f-d;
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
//DEF(em1),
//DEF(ha1),
//DEF(hir),
//DEF(toy),
  DEF(sot)
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


#define HLEN 80

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

#if defined(CM_TESTS)
#define S0 0
#else
#define S0 NUM_FUNCS
#endif

#if defined(EX_TESTS)
#define SE NUM_FUNCS+NUM_XFUNCS
#else
#define SE NUM_FUNCS
#endif

void error_dump()
{
  uint32_t s;

  for(s=S0; s<SE; s++) {
    a_error_t* e  = &a_errors[s];
    double     m  = 0;
    uint32_t   mi = 0;

    if (s < NUM_FUNCS)
      printf("%8s:\n", maps[s].name);
    else 
      printf("%8s:\n", xmaps[s-NUM_FUNCS].name);

    printf("max: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].max); if (e->a[i].max > m) { m = e->a[i].max; mi = i; } } ln();

#if defined(DUMP_RMS)
    printf("rms: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", sqrt(e->a[i].rms/(1.f*e->a[i].cnt))); } ln();
#endif    
#if defined(DUMP_AVE)
    printf("ave: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].ave/e->a[i].cnt); } ln();
#endif    
  //printf("cnt: "); for(uint32_t i=0; i<HLEN; i++) { printf("%d, ", e->a[i].cnt); } ln();

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
      vec3_print(&b); quat_print(q); quat_print(&r);
      quat_sub(&r,q,&r); quat_printa(&r); quat_printa(q);
      ln();
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

// measure round-trip errors using distribution of 'gen'
// using direct quat routines
void rt_test_x(uint64_t s0, uint64_t s1, qsrc gen, uint32_t len)
{
  for(uint32_t s=0; s<NUM_XFUNCS; s++) {
    q_to_bits encode = xmaps[s].encode;
    bits_to_q decode = xmaps[s].decode;
    quat_t    q,r,mq;
    uint64_t  bits;
    double    e   = 0.f;
    double    rms = 0.0;

    // reset base generators
    reset_generators(s0,s1,len);
    quat_set(&mq,0,0,0,1);
    
    for(uint32_t i=0; i<len; i++) {
      gen(&q);                        // H in distribution
      bits = encode(&q);              // quantize
      decode(&r, bits);               // dequantize
      double ae = a_error(&q,&r);     // R3 degrees
      rms += ae*ae;                   // for rms error
      
      // track max error
      if (ae > e) {
	e = ae;
	quat_dup(&mq, &q);
#if 0
	printf("          % 2.4f %s", e, "                                         "); 
	quat_print(&q); quat_print(&r); ln();
#endif      
      }
    }
    
    // reconstruct the found max error
    quat_dup(&q,&mq);
    bits = encode(&q);
    decode(&r, bits);
    
    printf("%8s: ", xmaps[s].name);
    printf("% 2.4f ", e);
    printf("% 2.4f ", sqrt(rms/(1.0*len+1.0)));
  //printf(": % 2.4f ", sqrt(vec3_norm(&b)));
    quat_print(&q); quat_print(&r);
    quat_sub(&r,&q,&r); quat_printa(&r);
    ln();
  }
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
    quat_t     q,r,mq;
    vec3_t     b,rb;
    vec3_t     c,rc;
    uint64_t   bits;
    double     e   = 0.0;
    double     rms = 0.0;

    reset_generators(s0,s1,len);
    quat_set(&mq,0,0,0,1);
    
    for(uint32_t i=0; i<len; i++) {
      gen(&q);
      f(&b, &q);  
      bc(&c, &b);
      bits = vencode(&c);
      vdecode(&rc, bits);
      cb(&rb, &rc);
      g(&r, &rb);
      double ae = a_error(&q,&r);
      rms += ae*ae;

      if (ae > e) {
	e = ae;
	quat_dup(&mq, &q);
	//printf("          %f ",ae); vec3_print(&b); quat_print(&q); quat_print(&r); ln();
      }
    }

    quat_dup(&q,&mq);
    f(&b, &q);  
    bc(&c, &b);
    bits = vencode(&c);
    vdecode(&rc, bits);
    cb(&rb, &rc);
    g(&r, &rb);

    printf("%8s: ", maps[s].name);
    printf("% 2.4f ", e);
    printf("% 2.4f ", sqrt(rms/(1.0*len+1.0)));
  //printf(": % 2.4f ", sqrt(vec3_norm(&b)));
    vec3_print(&b); quat_print(&q); quat_print(&r);
    quat_sub(&r,&q,&r); quat_printa(&r);
    ln();
  }
}

#undef HLEN

// a simple round-trip test for debugging inital mapping function
//   half-angle, exp, etc.
#define TLEN 0x2FFFFFF
#define HLEN 80
#define TEX

void rt_xform_test(uint64_t s0, uint64_t s1)
{
  double e = 0.0;
  double rms = 0.0;
  double rm = 0.5f;
  quat_t p,r,diff;
  uint32_t h[HLEN];
#if !defined(TEX)  
  vec3_t v;
#endif

  reset_generators(s0,s1,TLEN);

  //for(uint32_t x=0; x<TLEN; x++) { h[x]=0; }
  memset(h,  0,  4*HLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    uniform_quat(&p);
    p.w = fabs(p.w);

    // test generalize or explict
#if !defined(TEX)
    frct(&v,&p);
    irct(&r,&v); 
    
    //bar_(&v);

    float r2 = vec3_norm(&v);
    uint32_t id = (uint32_t)((HLEN-1)*r2);
    if (id < HLEN)
      h[id]++;
    else
      printf(".%d.",id);

    if (r2 > rm) { rm = r2; /*printf("R(%f)\n", r2);*/ }
#else
    uint64_t bits = fsot(&p);
    isot(&r,bits);
#endif    
    
    quat_sub(&diff, &p, &r);

    double ae = a_error(&p,&r);
    rms += ae*ae;
    
    if (ae > e) {
      e = ae;
      printf("%08x: %f %f ",i, ae, sqrt(rms/(i+1.0)) );
#if !defined(TEX)      
      vec3_print(&v);
#endif      
      quat_print(&p); quat_print(&r); quat_printa(&diff);
      printf(" %f \n", rm);
    }
  }

#if !defined(TEX)  
  for(uint32_t i=0; i<HLEN; i++) {
    printf("%f, ", h[i]*(HLEN*1.0)/TLEN);
  }
#endif  
}


#if 0
// minimal test round-trip quant/dequant bit patterns
void dq_rt()
{
  printf("dq_rt:\n");
  for(uint32_t i=0; i<(1<<(3*BITS)); i++) {
    vec3_t   v;
    uint64_t r;

    vdecode(&v,i);
    r = vencode(&v);

    if (i != r)
      printf("%08x %08x\n", i,r);
  }
  printf("  done\n");
}
#endif


#if 0
#define TB 10
void test()
{
  vec3_t v;

  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    s3_to_b3   f  = maps[s].f;
    b3_to_s3   g  = maps[s].g;
    map_func_t bc = maps[s].c;
    map_func_t cb = maps[s].s;
    float      e  = 2.f;
    float      t;
    quat_t     q,r,mq;
    vec3_t     b,rb,vec;
    vec3_t     c,rc;
    uint64_t   bits,bitsZ,bitsY;
    double     rms = 0.0;

    // walk positive
    
    for(uint32_t z=0; z<(1<<(TB-1)); z++) {
      bitsZ = z<<(2*BITS);
      
      for(uint32_t y=0; y<(1<<(TB-1)); y++) {
	bitsY = y<<BITS;
	
	for(uint32_t x=0; x<(1<<(TB-1)); x++) {
	  uint32_t bits = (z<<(2*TB))|(y<<(TB))|x;
	  
	  bits |= (1<<(  TB-1));
	  bits |= (1<<(2*TB-1));
	  bits |= (1<<(3*TB-1));

	  vdecode(&vec, bits);
	  //cb(&ball, &vec);
	  //g(&q, &ball);
	  vec3_print(&vec); ln();
	  
	}
	return;
      }
    }
  }
}
#endif

// s2->q wrt z
void s2_rt_test(uint64_t s0, uint64_t s1)
{
  printf("s2_rt_test\n");
  
  reset_generators(s0,s1,TLEN);
  float e = 1.f;

  for(uint32_t i=0; i<TLEN; i++) {
    vec3_t v,t,r;
    float  s,d;

    uniform_s2(&v);
  //v.z = fabs(v.z);
    d   = 1.f+v.z;   // degen as z -> -1
    s   = rsqrt(d+d);
    t.z = d*s;
    t.x = v.x*s;
    t.y = v.y*s;

    d   = t.x*t.x+t.y*t.y;
    s   = 2.f*sqrt(1.f-d);
    r.x = t.x*s;
    r.y = t.y*s;
    r.z = 1.f-2.f*(d);

    d = vec3_dot(&v,&r);

    if (d < e) {
      e=d;
      printf("%f ", e); vec3_print(&v); vec3_print(&r); ln();
    }
  }
  
  printf("s2_rt_test: done\n");
}

// sanity check: shells are function of distance cubed,
// count # points of N points/dim uniform grid inside
// H equal volume shells (and points outside ball)
//  
#define N 1024
#define N3 1.f/(N*N*N)
#define H 50
  
void shell_sanity()
{
  uint32_t h[H];
  uint32_t o = 0;

  for(uint32_t i=0; i<H; i++) h[i]=0;
  
  for(uint32_t iz=0; iz<N; iz++) {
    float z = iz * (1.f/N);
    for(uint32_t iy=0; iy<N; iy++) {
      float y = iy * (1.f/N);
      for(uint32_t ix=0; ix<N; ix++) {
	float x = ix * (1.f/N);
	float d = x*x+y*y+z*z;
	if (d <= 1.f) {
	  d *= sqrtf(d);
	  h[(uint32_t)(H*d)]++;
	}
	else o++;
      }
    }
  }
  // after normalization:
  // ball = 4/3 pi, box=8, outside = 1-pi/6 ~= 0.476401
  // per shell = (pi/6)/H

  printf("estimate: %f/shell and .476401 outside\n", 0.5235987756/H);
  printf("ball: {");
  for(uint32_t i=0; i<H; i++) printf("%f,", h[i]*N3);
  printf("}\n x = %f\n", o*N3);
}
#undef N
#undef N3
#undef H

void ptest()
{
  quat_t q,r,d;
  
  //quat_set(&q, -0x1.6a118ep-13f, 0, 0, 0x1.fffffep-1f);

  //quat_set(&q, 0.5f*SQRT2, 0.f, 0.f, 0.5f*SQRT2); 
  //quat_set(&q, 1.f, 0.f, 0.f, 0.f);
  //float x = rng_f32()+0.5f;
  //quat_set(&q, sinf(x), 0.f, 0.f, cosf(x));
#if 0
  uint64_t bits = fsot(&q);
  isot(&r,bits);
#else
  vec3_t v;
  frct(&v,&q);
  irct(&r,&v);
  vec3_print(&v);
#endif  
  
  quat_sub(&d, &q, &r);

  double ae = a_error(&q,&r);
  printf("%f:", ae);
//printf(" (%08x) ", (uint32_t)bits);
  quat_print(&q); quat_print(&r); quat_printa(&d); ln();
}

int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;

  error_init();

  //dq_rt();
  //foo(); return 0;
  //test(); return 0;

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

#if 0
  {
    quat_t q;
    vec3_t b,c;
    quat_set(&q, -0x1.6a118ep-13f, 0, 0, 0x1.fffffep-1f);
    quat_set(&q, -0x1.69ad06p-13f, 0,0,  0x1p+0f);
    fem(&b, &q);
    map_rs_bc(&c, &b);
    quat_print(&q);
    vec3_print(&b);
    vec3_print(&c);
    return 0;
  }
#endif
  
  //ptest(); return 0;
  //rt_xform_test(s0,s1); return 0;

#if 1
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
  rt_test  (s0,s1, &uniform_quat_from_z,     SYM_TRIALS);
  rt_test  (s0,s1, &uniform_quat,            SYM_TRIALS);
  rt_test  (s0,s1, &uniform_quat_about_x,    SYM_TRIALS);
  rt_test  (s0,s1, &uniform_quat_about_diag, SYM_TRIALS);
#endif

#if defined(EX_TESTS)
  rt_test_x(s0,s1, &uniform_quat_from_z,     SYM_TRIALS);
  rt_test_x(s0,s1, &uniform_quat,            SYM_TRIALS);
  rt_test_x(s0,s1, &uniform_quat_about_x,    SYM_TRIALS);
  rt_test_x(s0,s1, &uniform_quat_about_diag, SYM_TRIALS);
#endif
#else
//rt_test_x(s0,s1, &uniform_quat_about_diag, SYM_TRIALS);
//rt_test  (s0,s1, &uniform_quat_about_x, SYM_TRIALS);
//rt_test  (s0,s1, &sobol_quat_about_xy, SYM_TRIALS);
  rt_test  (s0,s1, &sobol_quat_about_diag, SYM_TRIALS);
#endif

  //error_dump();
  
  return 0;
}
