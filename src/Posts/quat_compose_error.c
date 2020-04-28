// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/XXX

// data from post compiled with clang 9.0.1:
//   clang -Wall -Wextra -O3 -ffast-math -fno-math-errno -march=haswell -o bar quat_compose_error.c -lm
// except for 'no-fma' trace loses fast-math, fast-math is IMHO a shoot yourself in the foot option
// included 'cause people will anyway and to allow fp contractions.
// Individual tests require flipping compile time constants and options...sorry!

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "x86intrin.h"

// drastically reduces the number of samples tested
//#define FAST_TEST

// perform long product chain test (in post) otherwise short product chain (not in post)
#define LONG_CHAIN

// use different sequences each execution
//#define RANDOMIZE

// define to test matrices instead of quaternions
//#define MAT_TEST

// quaternion product using TwoProduct (Fast2Mul)
//#define QUAT_FMA

// matrix product using TwoProduct (Fast2Mul)
//#define MAT_FMA

// flush denormals to zero (stress testing)
//#define DAZ

// change rounding mode to toward zero (stress testing)
//#define ROUND_TZ

// 'n' of the post
#define MAX_POW 16

// define one of these to get the limited angle version
// max rotation angle is 2 acos(ANGLE_LIMIT)
//#define ANGLE_LIMIT 0.707107f  // ~pi/2 max angle
//#define ANGLE_LIMIT 0.92388f   // ~pi/4 
//#define ANGLE_LIMIT 0.980785f  // ~pi/8
//#define ANGLE_LIMIT 0.995185f  // ~pi/16



// external code: xoroshiro128+

uint64_t rng_state[2];

#define TO_FP32 (1.f/16777216.f)
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#define EPS (F32_MIN_NORMAL)

// 360/pi
const double ascale = 114.5915590261646457292954437434673309326171875;

// x64 only hack
inline void fp_set_rn() { _mm_setcsr((_mm_getcsr() & ~(3u<<13))); }
inline void fp_set_ru() { _mm_setcsr((_mm_getcsr() & ~(3u<<13))|(1u<<14)); }
inline void fp_set_rd() { _mm_setcsr((_mm_getcsr() & ~(3u<<13))|(1u<<13)); }
inline void fp_set_rz() { _mm_setcsr((_mm_getcsr() & ~(3u<<13))|(3u<<13)); }

// enable/disable denormals-as-zero mode
inline void fp_daz_enable()  { _mm_setcsr(_mm_getcsr() |  (1u<<6)); }
inline void fp_daz_disable() { _mm_setcsr(_mm_getcsr() & ~(1u<<6)); }


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

inline float rsqrtf(float v) { return 1.f/sqrtf(v); }

void reset_generators(uint64_t s0, uint64_t s1)
{
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}

typedef struct { float x,y;     } vec2_t;
typedef struct { float x,y,z;   } vec3_t;
typedef union  { struct{ float  x,y,z,w; }; float  f[4]; } quat_t;
typedef union  { struct{ double x,y,z,w; }; double f[4]; } dquat_t;

inline void vec3_set(vec3_t* v, float x, float y, float z)
{
  v->x=x; v->y=y; v->z=z;
}

inline void quat_set(quat_t* q, float x, float y, float z, float w)
{
  q->x=x; q->y=y; q->z=z; q->w=w;
}

inline void dquat_set(dquat_t* q, double x, double y, double z, double w)
{
  q->x=x; q->y=y; q->z=z; q->w=w;
}

inline void quat_dup(quat_t* d, quat_t* s)
{
  quat_set(d,s->x,s->y,s->z,s->w);
}

inline void quat_scale(quat_t* q, float s)
{
  q->x *= s; q->y *= s; q->z *= s; q->w *= s;
}

inline void quat_set_id(quat_t* q)
{
  q->x = q->y = q->z = 0.f;
  q->w=1.f;
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


// uniform point in postive-x half disc
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

// uniform point in disc (rejection method)
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

// uniform random rotation
void uniform_quat(quat_t* q)
{
  vec2_t p0,p1;
  float  d1 = uniform_disc(&p1) + EPS;
  float  s1 = rsqrtf(d1);
  float  d0 = uniform_hdisc(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
}


void uniform_dquat(dquat_t* q)
{
  // hack about precision. Doesn't matter.
  // SEE range limited section for justification
  vec2_t p0,p1;
  double d1 = (double)uniform_disc(&p1) + EPS;
  double s1 = 1.0/sqrt(d1);
  double d0 = (double)uniform_hdisc(&p0);
  double s0 = sqrt(1.f-d0);
  double s  = s0*s1;

  dquat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
}

// uniform point on 3D sphere
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

#define COS_MAX 0x1.0p-10f;

// 'w' will be on [0,COS_MAX) so in neighborhood 
// of maximum magnitude rotation angle.
void quat_near_max(quat_t* q)
{
  vec3_t v;

  uniform_s2(&v);

  float c = ((rng_u64() >> 40)*TO_FP32)* COS_MAX;
  float s = sqrtf(1.f-c*c);
  
  q->x = s*v.x;
  q->y = s*v.y;
  q->z = s*v.z;
  q->w = c;
}

// near max angle rotations & axis of rotations are
// permuted: if A=(x,y,z) then B=(y,z,x). When mag.
// of rot angles are approaching pi result is dominated
// by cross(A,B) & dot(A,B)
void quat_near_max_ortho(quat_t* a, quat_t* b)
{
  vec3_t v;

  uniform_s2(&v);

  uint64_t u = rng_u64();
  float    x = (u >> 40)*TO_FP32;
  float    y = (u & 0xFFFFFF)*TO_FP32;
  float    c = x * COS_MAX;
  float    s = sqrtf(1.f-c*c);
  
  a->x = s*v.x;
  a->y = s*v.y;
  a->z = s*v.z;
  a->w = c;

  c = y * COS_MAX;
  s = sqrtf(1.f-c*c);

  b->x = s*v.y;
  b->y = s*v.z;
  b->z = s*v.x;
  b->w = c;
}

//
#define OFF 4096

void quat_near_max_near(quat_t* a, quat_t* b)
{
  vec3_t v;

  uniform_s2(&v);

  uint64_t u = rng_u64();
  float    x = (u >> 40)*TO_FP32;
  float    c = x * COS_MAX;
  float    s = sqrtf(1.f-c*c);
  
  a->x = s*v.x;
  a->y = s*v.y;
  a->z = s*v.z;
  a->w = c;

  b->x = v.x * (1+OFF*0x1.0p-23f);
  b->y = v.y * (1+OFF*0x1.0p-23f);
  b->z = v.z * (1+OFF*0x1.0p-23f);

  s = sqrtf((1.f-c*c)*(OFF+0x1.0p23f)*(OFF+0x1.0p23f))*0x1.0p-23f;

  b->x *= s;
  b->y *= s;
  b->z *= s;
  b->w  = c;
}

#if defined(ANGLE_LIMIT)
void quat_limited(quat_t* a)
{
  vec3_t v;

  // uniform axis of rotation
  uniform_s2(&v);

  // not quite uniform angle..meh..spitball
  uint64_t u = rng_u64();
  float    x = (u >> 40)*TO_FP32;
  float    c = x * ANGLE_LIMIT;
  float    s = sqrtf(1.f-c*c);
  
  a->x = s*v.x;
  a->y = s*v.y;
  a->z = s*v.z;
  a->w = c;
}
#endif


inline void quat_conj(quat_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

inline void dquat_conj(dquat_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

inline void quat_nconj(quat_t* a)
{
  a->w = -a->w;
}

inline float quat_dot(quat_t* a, quat_t* b)
{
  return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}

typedef struct { float  h,l; } f32_pair_t;
typedef struct { double h,l; } f64_pair_t;

inline void f32_2mul(f32_pair_t* r, float a, float b)
{
  float t = a*b;
  float e = fmaf(a,b,-t);
  r->h = t;
  r->l = e;
}

// ab+cd
inline float mma(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b, t);
  return f+e;
}

// ab-cd
inline float mms(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b,-t);
  return f-e;
}

// ab+cd+ef
inline float mma3(float a, float b, float c, float d, float e, float f)
{
  f32_pair_t ab,cd,ef;
  f32_2mul(&ab,a,b);
  f32_2mul(&cd,c,d);
  f32_2mul(&ef,e,f);
  float hi = ab.h + cd.h + ef.h;
  float lo = ab.l + cd.l + ef.l;
  return hi+lo;
}

inline void dquat_mul(dquat_t* r, dquat_t* a, dquat_t* b)
{
  double x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  double y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  double z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  double w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  dquat_set(r,x,y,z,w);
}

// hack: promote to double, perform and back to single
// double rounding? i don't care about no stinking double rounding.
inline void quat_mul_pd(quat_t* r, quat_t* a, quat_t* b)
{
  dquat_t ad = {.x=a->x, .y=a->y, .z=a->z, .w=a->w};
  dquat_t bd = {.x=b->x, .y=b->y, .z=b->z, .w=b->w};
  dquat_t rd;
  dquat_mul(&rd,&ad,&bd);
  quat_set(r,(float)rd.x,(float)rd.y,(float)rd.z,(float)rd.w);
}


// flip-this to enable using Kahan ab+cd FMA expressions
#if !defined(QUAT_FMA)

inline void quat_mul(quat_t* r, quat_t* a, quat_t* b)
{
  float x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  float y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  float z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  float w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;

  // hack for normalize after each product trace
#if defined(NORMALIZE_HACK)  
  float s = 1.f/sqrt(x*x+y*y+z*z+w*w);
  x *= s; y *= s; z *= s; w *= s;
#endif  

  quat_set(r,x,y,z,w);
}

#else

inline void quat_mul(quat_t* r, quat_t* a, quat_t* b)
{
  float x = mma(a->w,b->x,a->x,b->w) + mms(a->y,b->z,a->z,b->y);  
  float y = mms(a->w,b->y,a->x,b->z) + mma(a->y,b->w,a->z,b->x);
  float z = mma(a->w,b->z,a->x,b->y) + mms(a->z,b->w,a->y,b->x);
  float w = mms(a->w,b->w,a->x,b->x) - mma(a->y,b->y,a->z,b->z);

  quat_set(r,x,y,z,w);
}

#endif

double bv_norm(quat_t* q)
{
  double x = q->x;
  double y = q->y;
  double z = q->z;
  return x*x + y*y + z*z;
}

// row-major
typedef union {
  struct {
    float m00,m01,m02;
    float m10,m11,m12;
    float m20,m21,m22;
  };
  vec3_t row[3];
} mat33_t;

void mat33_printa(mat33_t* m)
{
  printf("{{%+a,%+a,%+a},\n",m->m00,m->m01,m->m02);
  printf(" {%+a,%+a,%+a},\n",m->m10,m->m11,m->m12);
  printf(" {%+a,%+a,%+a}}\n",m->m20,m->m21,m->m22);
}

void mat33_print(mat33_t* m)
{
  printf("{{%+f,%+f,%+f},\n",m->m00,m->m01,m->m02);
  printf(" {%+f,%+f,%+f},\n",m->m10,m->m11,m->m12);
  printf(" {%+f,%+f,%+f}}\n",m->m20,m->m21,m->m22);
}

float mat33_det(mat33_t* m)
{
  // being lazy: promote, compute, demote. Not that I
  // think it really matters here (should always be
  // ~identity matrix)
  return (float)(
    ((double)m->m00 * (double)m->m11 * (double)m->m22) +
    ((double)m->m01 * (double)m->m12 * (double)m->m20) +
    ((double)m->m02 * (double)m->m10 * (double)m->m21) -
    ((double)m->m02 * (double)m->m11 * (double)m->m20) -
    ((double)m->m00 * (double)m->m12 * (double)m->m21) -
    ((double)m->m01 * (double)m->m10 * (double)m->m22));
}

void mat33_mul(mat33_t* d, mat33_t* l, mat33_t* r)
{
#if defined(MAT_FMA)
  d->m00 = mma3(l->m00,r->m00,l->m01,r->m10,l->m02,r->m20);
  d->m01 = mma3(l->m00,r->m01,l->m01,r->m11,l->m02,r->m21);
  d->m02 = mma3(l->m00,r->m02,l->m01,r->m12,l->m02,r->m22);
  d->m10 = mma3(l->m10,r->m00,l->m11,r->m10,l->m12,r->m20);
  d->m11 = mma3(l->m10,r->m01,l->m11,r->m11,l->m12,r->m21);
  d->m12 = mma3(l->m10,r->m02,l->m11,r->m12,l->m12,r->m22);
  d->m20 = mma3(l->m20,r->m00,l->m21,r->m10,l->m22,r->m20);
  d->m21 = mma3(l->m20,r->m01,l->m21,r->m11,l->m22,r->m21);
  d->m22 = mma3(l->m20,r->m02,l->m21,r->m12,l->m22,r->m22);
#else
  d->m00 = l->m00 * r->m00 + l->m01 * r->m10 + l->m02 * r->m20;
  d->m01 = l->m00 * r->m01 + l->m01 * r->m11 + l->m02 * r->m21;
  d->m02 = l->m00 * r->m02 + l->m01 * r->m12 + l->m02 * r->m22;
  d->m10 = l->m10 * r->m00 + l->m11 * r->m10 + l->m12 * r->m20;
  d->m11 = l->m10 * r->m01 + l->m11 * r->m11 + l->m12 * r->m21;
  d->m12 = l->m10 * r->m02 + l->m11 * r->m12 + l->m12 * r->m22;
  d->m20 = l->m20 * r->m00 + l->m21 * r->m10 + l->m22 * r->m20;
  d->m21 = l->m20 * r->m01 + l->m21 * r->m11 + l->m22 * r->m21;
  d->m22 = l->m20 * r->m02 + l->m21 * r->m12 + l->m22 * r->m22;
#endif  
}

// L = LR
void mat33_lmul(mat33_t* l, mat33_t* r)
{
#if defined(MAT_FMA)
  float m00 = mma3(l->m00,r->m00,l->m01,r->m10,l->m02,r->m20);
  float m01 = mma3(l->m00,r->m01,l->m01,r->m11,l->m02,r->m21);
  float m02 = mma3(l->m00,r->m02,l->m01,r->m12,l->m02,r->m22);
  float m10 = mma3(l->m10,r->m00,l->m11,r->m10,l->m12,r->m20);
  float m11 = mma3(l->m10,r->m01,l->m11,r->m11,l->m12,r->m21);
  float m12 = mma3(l->m10,r->m02,l->m11,r->m12,l->m12,r->m22);
  float m20 = mma3(l->m20,r->m00,l->m21,r->m10,l->m22,r->m20);
  float m21 = mma3(l->m20,r->m01,l->m21,r->m11,l->m22,r->m21);
  float m22 = mma3(l->m20,r->m02,l->m21,r->m12,l->m22,r->m22);
#else  
  float m00 = l->m00 * r->m00 + l->m01 * r->m10 + l->m02 * r->m20;
  float m01 = l->m00 * r->m01 + l->m01 * r->m11 + l->m02 * r->m21;
  float m02 = l->m00 * r->m02 + l->m01 * r->m12 + l->m02 * r->m22;
  float m10 = l->m10 * r->m00 + l->m11 * r->m10 + l->m12 * r->m20;
  float m11 = l->m10 * r->m01 + l->m11 * r->m11 + l->m12 * r->m21;
  float m12 = l->m10 * r->m02 + l->m11 * r->m12 + l->m12 * r->m22;
  float m20 = l->m20 * r->m00 + l->m21 * r->m10 + l->m22 * r->m20;
  float m21 = l->m20 * r->m01 + l->m21 * r->m11 + l->m22 * r->m21;
  float m22 = l->m20 * r->m02 + l->m21 * r->m12 + l->m22 * r->m22;
#endif  

  l->m00 = m00; l->m01 = m01; l->m02 = m02;
  l->m10 = m10; l->m11 = m11; l->m12 = m12;
  l->m20 = m20; l->m21 = m21; l->m22 = m22;
}

// R = LR
void mat33_rmul(mat33_t* l, mat33_t* r)
{
#if defined(MAT_FMA)
  float m00 = mma3(l->m00,r->m00,l->m01,r->m10,l->m02,r->m20);
  float m01 = mma3(l->m00,r->m01,l->m01,r->m11,l->m02,r->m21);
  float m02 = mma3(l->m00,r->m02,l->m01,r->m12,l->m02,r->m22);
  float m10 = mma3(l->m10,r->m00,l->m11,r->m10,l->m12,r->m20);
  float m11 = mma3(l->m10,r->m01,l->m11,r->m11,l->m12,r->m21);
  float m12 = mma3(l->m10,r->m02,l->m11,r->m12,l->m12,r->m22);
  float m20 = mma3(l->m20,r->m00,l->m21,r->m10,l->m22,r->m20);
  float m21 = mma3(l->m20,r->m01,l->m21,r->m11,l->m22,r->m21);
  float m22 = mma3(l->m20,r->m02,l->m21,r->m12,l->m22,r->m22);
#else  
  float m00 = l->m00 * r->m00 + l->m01 * r->m10 + l->m02 * r->m20;
  float m01 = l->m00 * r->m01 + l->m01 * r->m11 + l->m02 * r->m21;
  float m02 = l->m00 * r->m02 + l->m01 * r->m12 + l->m02 * r->m22;
  float m10 = l->m10 * r->m00 + l->m11 * r->m10 + l->m12 * r->m20;
  float m11 = l->m10 * r->m01 + l->m11 * r->m11 + l->m12 * r->m21;
  float m12 = l->m10 * r->m02 + l->m11 * r->m12 + l->m12 * r->m22;
  float m20 = l->m20 * r->m00 + l->m21 * r->m10 + l->m22 * r->m20;
  float m21 = l->m20 * r->m01 + l->m21 * r->m11 + l->m22 * r->m21;
  float m22 = l->m20 * r->m02 + l->m21 * r->m12 + l->m22 * r->m22;
#endif
  
  r->m00 = m00; r->m01 = m01; r->m02 = m02;
  r->m10 = m10; r->m11 = m11; r->m12 = m12;
  r->m20 = m20; r->m21 = m21; r->m22 = m22;
}


void quat_to_mat33(mat33_t* m, quat_t* q)
{
  float x  = q->x, y  = q->y, z  = q->z, w  = q->w;
  float xx = x*x,  yy = y*y,  zz = z*z,  ww = w*w;
  float tx = 2*x,  ty = 2*y,  tz = 2*z;
  float xy = ty*x, xz = tz*x, yz = ty*z;
  float wx = tx*w, wy = ty*w, wz = tz*w;
  float t0 = ww-zz;
  float t1 = xx-yy;
  
  m->m00 = t0+t1;
  m->m11 = t0-t1;
  m->m22 = ww-xx-yy+zz;

  m->m10 = xy+wz; m->m01 = xy-wz;
  m->m20 = xz-wy; m->m02 = xz+wy;
  m->m21 = yz+wx; m->m12 = yz-wx;
}

void mat33_to_quat(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;
  float s;

  if (m22 >= 0) {
    float a = m00+m11;
    float b = m10-m01;
    float c = 1.f+m22;

    if (a >= 0) { 
      s = c + a;
      quat_set(q, m21-m12, m02-m20, b, s); // w
    }
    else { 
      s = c - a;
      quat_set(q, m02+m20, m21+m12, s, b); // z
    }
  }
  else {
    float a = m00-m11;
    float b = m10+m01;
    float c = 1.f-m22;
    
    if (a > 0) {
      s = c + a;
      quat_set(q, s, b, m02+m20, m21-m12); // x
    }
    else {
      s = c - a;
      quat_set(q, b, s, m21+m12, m02-m20); // y
    }
  }

  quat_scale(q, 0.5f*rsqrtf(s));
}

#if !defined(ANGLE_LIMIT)
#define quat_sample uniform_quat
#else
#define quat_sample quat_limited
#endif

#if defined(MAT_TEST)
// track min/max determinates
float det_min = 1.0;
float det_max = 1.0;
#endif

// short chain test sampling schemes.
//void sample_2(quat_t* a, quat_t* b) { uniform_quat(a); uniform_quat(b); }
//void sample_2(quat_t* a, quat_t* b) { quat_near_max_ortho(a,b); }
void sample_2(quat_t* a, quat_t* b) { quat_near_max_near(a,b); }


void check_test()
{
#if 1
  const uint64_t n = UINT64_C(10000000000)>>1;
  dquat_t q = {.x=0.0, .y=0.0, .z=0.0, .w=1.0};
  dquat_t r;

  // save the current state
  uint64_t s0 = rng_state[0];
  uint64_t s1 = rng_state[1];
  reset_generators(s0,s1);

  for(uint64_t i=0; i<n; i++) {
    uniform_dquat(&r);
    dquat_mul(&q, &r, &q);
  }

  // reset to same state to get same sequence
  reset_generators(s0,s1);

  // decompose same 'n' terms
  for(uint64_t i=0; i<n; i++) {
    uniform_dquat(&r);
    dquat_conj(&r);
    dquat_mul(&q, &q, &r);
  }
#else
  const uint64_t n = UINT64_C(10000000000)>>1;
  quat_t q = {.x= 0.f, .y=0.f,. z=0.f, .w=1.f};
  quat_t r;

  // save the current state
  uint64_t s0 = rng_state[0];
  uint64_t s1 = rng_state[1];
  reset_generators(s0,s1);

  for(uint64_t i=0; i<n; i++) {
    uniform_quat(&r);
    quat_mul(&q, &r, &q);
  }

  // reset to same state to get same sequence
  reset_generators(s0,s1);

  // decompose same 'n' terms
  for(uint64_t i=0; i<n; i++) {
    uniform_quat(&r);
    quat_conj(&r);
    quat_mul(&q, &q, &r);
  }
#endif  

  double bn = q.x*q.x+q.y*q.y+q.z*q.z;
  double ae = ascale*atan(sqrt(bn)/q.w);

  printf("ae  = %e\n", ae);
}



#if defined(LONG_CHAIN)

// long chain test
void test(quat_t* q, const uint32_t n)
{
  quat_t qn,r;

  // save the current state
  uint64_t s0 = rng_state[0];
  uint64_t s1 = rng_state[1];
  reset_generators(s0,s1);

  quat_set_id(&qn);

#if defined(MAT_TEST)
  mat33_t mn,mr;
  quat_to_mat33(&mr, &qn);
#endif  

  for(uint32_t i=0; i<n; i++) {
    quat_sample(&r);
    
#if !defined(MAT_TEST)
    quat_mul(&qn, &r, &qn);
#else    
    quat_to_mat33(&mr,&r);
    mat33_lmul(&mn, &mr);
#endif    
  }

  // reset to same state to get same sequence
  reset_generators(s0,s1);

  // decompose same 'n' terms
  for(uint32_t i=0; i<n; i++) {
    quat_sample(&r);
    quat_conj(&r);

#if !defined(MAT_TEST)
    quat_mul(&qn, &qn, &r);
#else    
    quat_to_mat33(&mr,&r);
    mat33_rmul(&mr,&mn);
#endif    
  }

#if !defined(MAT_TEST)
  quat_dup(q,&qn);
#else    
  // gather matrix measures

  float det = mat33_det(&mr);
  if (det > det_max) { det_max = det; }
  if (det < det_min) { det_min = det; }

  mat33_to_quat(q, &mr);
#endif  
}


#else

// short chain test: Q = QA, Q=QB, Q=QB^*, Q=QA^*
void test(quat_t* q, const uint32_t n)
{
  quat_t r,a,b;

  quat_set_id(&r);

#if defined(MAT_TEST)
  mat33_t mr,ma,mb;
  quat_to_mat33(&mr,&r);
#endif  
  
  for(uint32_t i=0; i<n; i++) {
    sample_2(&a,&b);
#if !defined(MAT_TEST)    
    quat_mul(&r, &r, &a);
    quat_mul(&r, &r, &b);
#else
    quat_to_mat33(&ma, &a);
    quat_to_mat33(&mb, &b);
    mat33_lmul(&mr, &ma);
    mat33_lmul(&mr, &mb);
#endif    
    
    quat_conj(&a);
    quat_conj(&b);
    
#if !defined(MAT_TEST)    
    quat_mul(&r, &r, &b);
    quat_mul(&r, &r, &a);
#else
    quat_to_mat33(&ma,&a);
    quat_to_mat33(&mb,&b);
    mat33_lmul(&mr, &mb);
    mat33_lmul(&mr, &ma);
#endif    
  }

#if !defined(MAT_TEST)    
  quat_dup(q,&r);
#else

  float det = mat33_det(&mr);
  if (det > det_max) { det_max = det; }
  if (det < det_min) { det_min = det; }
  
  mat33_to_quat(q,&mr);
#endif  
}

#endif


double error_m[MAX_POW+1];
double error_a[MAX_POW+1];
double error_det_lo[MAX_POW+1];
double error_det_hi[MAX_POW+1];


#ifdef FAST_TEST
#define BASE_I 0x0001ffff
#define ADJ_I  32
#else
#define BASE_I 0x03ffffff
#define ADJ_I  256
#endif

void rt_test()
{
  for(uint32_t p=0;p<=MAX_POW;p++) {
    quat_t q;
    double maxe = 0.0;
    double maxa = 0.0;
    uint32_t i = BASE_I>>p;
    uint32_t n = 1<<p;

    printf(".");
    fflush(stdout);
    
    do {
      i--;

#ifdef DAZ  
      fp_daz_enable();
#endif  
#ifdef ROUND_TZ
      fp_set_rz();
      test(&q,n);
      fp_set_rn();
#else
      test(&q,n);
#endif      
#ifdef DAZ  
      fp_daz_disable();
#endif

      double bn = bv_norm(&q);
      double w  = q.w;
      double w2 = w*w;
      double ae = bn/w2;
      double me = fabs(1.0-sqrt(bn + w*w));
      if (me > maxe) { maxe = me; i += ADJ_I; }
      if (ae > maxa) { maxa = ae; i += ADJ_I; }
    } while(i != 0);
    
    error_m[p] = maxe;
    error_a[p] = ascale*atan(sqrt(maxa));

#ifdef MAT_TEST
    error_det_lo[p] = det_min;
    error_det_hi[p] = det_max;
    det_min = det_max = 1.0;
#endif
  }
  printf("\n");

#if defined(MAT_TEST)  
  printf(" det-l: "); for(uint32_t p=0;p<=MAX_POW;p++) { printf("%e,", error_det_lo[p]); } printf("\n");
  printf(" det-h: "); for(uint32_t p=0;p<=MAX_POW;p++) { printf("%e,", error_det_hi[p]); } printf("\n");
#endif
  
  printf(" me: "); for(uint32_t p=0;p<=MAX_POW;p++) { printf("%e,", error_m[p]); } printf("\n");
  printf(" ae: "); for(uint32_t p=0;p<=MAX_POW;p++) { printf("%e,", error_a[p]); } printf("\n");
}

// for quick checking sampling strategies
void foo()
{
  quat_t t,a,b,ia;
  double max = 0;
  double min = 1.f;

  do {
    uint32_t i = 2096;
    
    do {
      quat_near_max_near(&a,&b);
      quat_dup(&ia,&a);
      quat_conj(&ia);
      //quat_sample(&b);
      quat_mul(&t, &a,  &b);
      quat_mul(&t, &a,  &b);
      quat_mul(&t, &ia, &t);
      double dot = quat_dot(&t,&b);
      if (dot > max) { printf("  max: %e ",dot-1.f); ln();quat_printa(&a); quat_printa(&b);ln();
	 quat_print(&a); quat_print(&b);
	ln(); max=dot; } 
      if (dot < min) { printf("  min: %e ",1.f-dot); ln(); quat_printa(&a); quat_printa(&b);ln();
	 quat_print(&a); quat_print(&b);
	ln(); min=dot; } 
    } while(--i != 0);


  } while (1);
}


int main(void)
{
  uint64_t s0;
  uint64_t s1;

  
#ifdef RANDOMIZE
  s0 = _rdtsc();
#else  
  s0 = 0x77535345;
#endif  
  s1 = 0x1234567;
  
  reset_generators(s0,s1);

  rt_test();
  
  return 0;
}
