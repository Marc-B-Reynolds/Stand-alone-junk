// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/XXX

// NOTE: almost all of routines here are proof-of-concept junk
// This is lazy...you gotta edit main to change the transforms

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "xmmintrin.h"

//#define RANDOMIZE

// histogram size
#define HLEN 80

#define TLEN 0x7FFFFFF
// number of samples per axis of rotation
#define DLEN 512

//static inline float sgn(float x) { return copysignf(1.f,x); }

// external code: xoroshiro128+

uint64_t rng_state[2];

#define TO_FP32 (1.f/16777216.f)
#define ULP1 TO_FP32
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#define EPS (F32_MIN_NORMAL)

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

inline float rsqrtf(float v) { return 1.f/sqrtf(v); }

void reset_generators(uint64_t s0, uint64_t s1)
{
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}

typedef struct { float x,y;     } vec2_t;
typedef struct { float x,y,z;   } vec3_t;

typedef union  { struct{ float x,y,z,w; }; float f[4]; } quat_t;

typedef struct { double x,y;    } vec2d_t;


static inline void quat_set(quat_t* q, float x, float y, float z, float w)
{
  q->x=x; q->y=y; q->z=z; q->w=w;
}

typedef struct {
  // column major storage 'hidden' by explict members
  float m00,m10,m20;
  float m01,m11,m21;
  float m02,m12,m22;
} mat33_t;


// for error computations of using native ~1/sqrt without fixup
inline float rsqrtf_a(float x) 
{
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

inline float rsqrtf_nr(float x)
{
  float  x0 = rsqrtf_a(x);
  return x0*(1.5f - 0.5f*x*x0*x0);
}  

void ln(void) {printf("\n");}

void vec3_print(vec3_t* v)
{
  printf("(% f,% f,% f) ",v->x,v->y,v->z);
}

void vec3_printa(vec3_t* v)
{
  printf("(%+a,%+a,%+a) ",v->x,v->y,v->z);
}

void quat_print(quat_t* q)
{
  printf("(% f,% f,% f,% f) ",q->x,q->y,q->z,q->w);
}

void quat_printa(quat_t* q)
{
  printf("(%+a,%+a,%+a,%+a) ",q->x,q->y,q->z,q->w);
}

#define CBIAS (1.f/68719476736.f) // 0x1p-36f

// uniform in postive-x half disc
static float uniform_hdisc(vec2_t* p)
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

static float uniform_disc(vec2_t* p)
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

static double uniform_quat(quat_t* q)
{
  vec2_t p0,p1;
  float  d1 = uniform_disc(&p1) + EPS;
  float  s1 = rsqrtf(d1);
  float  d0 = uniform_hdisc(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);

  return acosf(p0.x);
}

static inline void quat_scale(quat_t* q, float s)
{
  q->x *= s; q->y *= s; q->z *= s; q->w *= s;
}

inline void quat_dup(quat_t* d, quat_t* s)
{
  d->x = s->x;
  d->y = s->y;
  d->z = s->z;
  d->w = s->w;
}


// alternate conversion w/o diagonal reduction
void quat_to_mat33_ndr(mat33_t* m, quat_t* q)
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

// common conversion (diagonals are reduce using q.q=1)
void quat_to_mat33_std(mat33_t* m, quat_t* q)
{
  float x  = q->x, y  = q->y, z  = q->z, w  = q->w;
  float tx = 2 *x, ty = 2 *y, tz = 2 *z;  // 2x,  2y,  2z
  float xx = tx*x, yy = ty*y, zz = tz*z;  // 2xx, 2yy, 2zz
  float xy = ty*x, xz = tz*x, yz = ty*z;  // 2xy, 2xy, 2yz
  float wx = tx*w, wy = ty*w, wz = tz*w;  // 2wx, 2wy, 2wz

  m->m00 = 1.f-(yy+zz); m->m11 = 1.f-(xx+zz); m->m22 = 1.f-(xx+yy);

  m->m10 = xy+wz; m->m01 = xy-wz;
  m->m20 = xz-wy; m->m02 = xz+wy;
  m->m21 = yz+wx; m->m12 = yz-wx;
}


// remove scaling ex using recip
static inline float recipf(float x) { return 1.f/x; } // -- stub for recip op + nr-step

void quat_to_mat33_nu(mat33_t* m, quat_t* q)
{
  float x  = q->x, y  = q->y, z  = q->z, w  = q->w;
  float xx = x*x,  yy = y*y,  zz = z*z,  ww = w*w;
  float s  = 2.f*recipf(xx+yy+zz+ww);
  float sx = s*x,  sy = s*y,  sz = s*z;
  float xy = sy*x, xz = sz*x, yz = sy*z;
  float wx = sx*w, wy = sy*w, wz = sz*w;

  m->m00 = 1.f-s*(yy+zz); m->m11 = 1.f-s*(xx+zz); m->m22 = 1.f-s*(xx+yy);

  m->m10 = xy+wz; m->m01 = xy-wz;
  m->m20 = xz-wy; m->m02 = xz+wy;
  m->m21 = yz+wx; m->m12 = yz-wx;
}


uint32_t c0=0,c1=0,c2=0,c3=0;

void mat33_to_quat_std(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;
  float t1 = m22+m11;
  float t0 = m00+t1;

  if (t0 > 0.f) {
    float w = 1.f+t0;
    float s = 0.5f*rsqrtf(w);
    quat_set(q, s*(m21-m12), s*(m02-m20), s*(m10-m01), s*w);
    return;
  }

  t0 = m00-t1;
  
  if (t0 > 0.f) {
    float x = 1.f+t0;
    float s = 0.5f*rsqrtf(x);
    quat_set(q, s*x, s*(m01+m10), s*(m02+m20), s*(m21-m12));
    return;
  }

  t0 = m11-m22;
  t1 = 1.f-m00;
  
  if (t0 > 0.f) {
    float y = t1+t0;
    float s = 0.5f*rsqrtf(y);
    quat_set(q, s*(m01+m10), s*y, s*(m12+m21), s*(m02-m20));
  }
  else {
    float z = t1-t0;
    float s = 0.5f*rsqrtf(z);
    quat_set(q, s*(m02+m20), s*(m12+m21), s*z, s*(m10-m01));
  }
}

void mat33_to_quat_small(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;
  float t  = 1 + m00 + m11 + m22;
  float s  = 0.5f*rsqrtf(t);
  quat_set(q, s*(m21-m12), s*(m02-m20), s*(m10-m01), s*t);
}

// Mike Day's version (shuffled around & switched to column vectors)
// http://www.insomniacgames.com/converting-a-rotation-matrix-to-a-quaternion/
void mat33_to_quat_day(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;
  float d;

  if (m22 >= 0) {
    float a = m00+m11;
    float b = m10-m01;
    float c = 1.f+m22;

    if (a >= 0) { 
      d = c + a;
      quat_set(q, m21-m12, m02-m20, b, d); // w
    }
    else { 
      d = c - a;
      quat_set(q, m02+m20, m21+m12, d, b); // z
    }
  }
  else {
    float a = m00-m11;
    float b = m10+m01;
    float c = 1.f-m22;
    
    if (a > 0) {
      d = c + a;
      quat_set(q, d, b, m02+m20, m21-m12); // x
    }
    else {
      d = c - a;
      quat_set(q, b, d, m21+m12, m02-m20); // y
    }
  }

  quat_scale(q, 0.5f*rsqrtf(d));
}

static inline uint32_t f2bits(float v)    { uint32_t r; memcpy(&r,&v,4); return r; }
static inline float    bits2f(uint32_t v) { float    r; memcpy(&r,&v,4); return r; }
static inline uint32_t sb(float f)        { return f2bits(f) & 0x80000000; }
static inline float    fxor(float f, uint32_t b)
{
  return bits2f(f2bits(f)^b);
}

// branch free version of Day...just to show correctness.
void mat33_to_quat_day_bf(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;

  // isolate a sufficently large component
  uint32_t s0 = sb(m22);
  float    a  = m00 + fxor(m11,s0);              // m00 + (m11^s0)
  uint32_t s1 = sb(a);
  uint32_t sp = s0^s1;
  float    e0 = 1.f + fxor(m22,s0) + fxor(a,s1); // 1 + |m22| + |t|
  float    s  = 0.5f*rsqrtf(e0);

  // the other three components
  float    e1 = m10-fxor(m01,s0);                // m10 - (m01^s0)
  float    e2 = m02-fxor(m20,sp);                // m02 - (m02^sp)
  float    e3 = m21-fxor(m12,s1);                // m21 - (m12^s1)

  e0 *= s; e1 *= s; e2 *= s; e3 *= s;            // scaling

  uint32_t id = (s0 >> 30)^(sp >> 31);           // write ordering

  q->f[3^id] = e0;
  q->f[2^id] = e1;
  q->f[1^id] = e2;
  q->f[0^id] = e3;
}

// high error but branch free.
void mat33_to_quat_bf0(quat_t* q, mat33_t* m)
{	
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;

  float a = 1.f+m00;
  float b = 1.f-m00;
  float c = m11+m22;
  float d = m11-m22;

  // abs to prevent errors, could use max against zero
  // or a bias amount as per below.
  float w = fabsf(a+c); // 4w^2
  float x = fabsf(a-c); // 4x^2
  float y = fabsf(b+d); // 4y^2
  float z = fabsf(b-d); // 4z^2

  // assumes rsqrtf(0) = inf
  w *= 0.5f*rsqrtf(w);
  x *= 0.5f*rsqrtf(x);
  y *= 0.5f*rsqrtf(y);
  z *= 0.5f*rsqrtf(z);

  x = copysignf(x, m21-m12);
  y = copysignf(y, m02-m20);
  z = copysignf(z, m10-m01);

  quat_set(q, x,y,z,w);
}

#ifndef BF1_CUT
#define BF1_CUT 0.02f
#endif

void mat33_to_quat_bf1(quat_t* q, mat33_t* m)
{
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;
  float t  = 1.f + m00 + m11 + m22;

  // listed rt error is using 'std' quat 2 matrix
  // cut    rt-error prob
  // 0.0001 0.014877 0.993634
  // 0.0002 0.010233 0.990994
  // 0.0004 0.007769 0.987265
  // 0.0008 0.004991 0.981993
  // 0.0016 0.003587 0.974531
  // 0.0032 0.002935 0.963969
  // 0.0064 0.001946 0.949045
  // 0.0128 0.001367 0.927966
  // 0.0256 0.000975 0.898184
  // 0.0512 0.000753 0.856216
  
  if (t > BF1_CUT) {
    c0++;
    float s  = 0.5f*rsqrtf(t);
    quat_set(q, s*(m21-m12), s*(m02-m20), s*(m10-m01), s*t);
    return;
  }
  c1++;

  // just using 'Day' for this toy version..lazy
  mat33_to_quat_day(q,m);
}

// naive use largest component...isn't worthwhile
void mat33_to_quat_silly(quat_t* q, mat33_t* m)
{
  double m00=m->m00, m01=m->m01, m02=m->m02;
  double m10=m->m10, m11=m->m11, m12=m->m12;
  double m20=m->m20, m21=m->m21, m22=m->m22;

  double w2  = 1 + m00 + m11 + m22;
  double x2  = 1 + m00 - m11 - m22;
  double y2  = 1 - m00 + m11 - m22;
  double z2  = 1 - m00 - m11 + m22;

  if (w2 >= x2 && w2 >= y2 && w2 >= z2) {
    double s  = 0.5/sqrt(w2);
    quat_set(q, s*(m21-m12), s*(m02-m20), s*(m10-m01), s*w2);
    return;
  }

  if (z2 >= x2 && z2 >= y2) {
    double s  = 0.5/sqrt(z2);
    quat_set(q, s*(m02+m20), s*(m21+m12), s*z2, s*(m10-m01));
    return;
  }

  if (y2 >= x2) {
    double s  = 0.5/sqrt(y2);
    quat_set(q, s*(m10+m01), s*y2, s*(m21+m12), s*(m02-m20));
    return;
  }

  double s  = 0.5/sqrt(x2);
  quat_set(q, s*x2, s*(m10+m01), s*(m02+m20), s*(m21-m12));
}



#define PI 3.1415927410125732421875f

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

static void quatd_mul(quatd_t* r, quatd_t* a, quatd_t* b)
{
  double x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  double y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  double z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  double w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quatd_set(r,x,y,z,w);
}

#define TO_R3_DEGREES 114.59156036376953125  // 2(180/pi), the 2 for H to R3 angle measure

static float a_error(quat_t* a, quat_t* b)
{
  quatd_t R,A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  quatd_conj(&A);
  quatd_mul(&R,&B,&A);

  double x = fabs(R.w);
  double y = sqrt(R.x*R.x+R.y*R.y+R.z*R.z);
  double t = atan(y/x);
  return (float)(TO_R3_DEGREES*t);
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

  double e = 0.0;
  double h[HLEN];
//quat_t hq[HLEN];
  quat_t q;
  uint32_t mc[2] = {0,0};

  reset_generators(s0,s1);

  memset(h, 0, sizeof(h[0])*HLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    double   a   = uniform_quat(&q);
    uint32_t hid = (uint32_t)(((2.f*HLEN)/PI)*fabs(a));
    mat33_t  m;
    quat_t   r;
    
    if (hid >= HLEN) hid=HLEN-1;

    quat_to_mat33_std(&m, &q);
    //quat_to_mat33_ndr(&m, &q);

    //quat_scale(&q, 0.8f+.4f*0x1p-64f*rng_u64());
    //quat_to_mat33_nu(&m, &q);
    
    //mat33_to_quat_std(&r, &m);
    //mat33_to_quat_day(&r, &m);
    //mat33_to_quat_day_bf(&r, &m);
    //mat33_to_quat_small(&r, &m);
    //mat33_to_quat_bf0(&r, &m);
    mat33_to_quat_bf1(&r, &m);
    //mat33_to_quat_silly(&r, &m);

    double ae = a_error(&q,&r);
    
    if (ae > h[hid]) { h[hid] = ae; /* quat_dup(&(hq[hid]),&q); */ }
    
    if (ae > e) {
      e = ae;
      printf("%08x %f ", i, e);
      quat_print(&q);
      quat_print(&r);
      //vec3_printa(&d);
      //printf("%e ", m.m10-m.m01);
      //printf("%a %d", m.m00+m.m11+m.m22,method);
      //printf("%f %d ", a, hid);
      ln();
    }
  }
  
  printf("\n{ ");

   for(uint32_t i=0; i<HLEN-1; i++) {
     printf("%e, ", h[i]);
  }

  printf("%e}\n", h[HLEN-1]);

  float scale = 1.f/(c0+c1+c2+c3+F32_MIN_NORMAL);
  printf("{%f, %f, %f, %f}\n",scale*c0,scale*c1,scale*c2,scale*c3);

  scale = 1.f/(mc[0]+mc[1]);
  printf("{%f, %f}\n",mc[0]*scale, mc[1]*scale);


#if 0  
  for(uint32_t i=0; i<HLEN; i++) {
    quat_t q0,q1;
    mat33_t m;
    quat_dup(&q0, &(hq[i]));
    quat_to_mat33_std(&m, &q0);
    mat33_to_quat_small(&q1, &m);
    quat_print(&q0);
    quat_print(&q1); ln();
  }
#endif
  
}
