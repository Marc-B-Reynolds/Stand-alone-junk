// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/2017/08/08/QuatRotMatrix.html

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
//#define HLEN 128

// number of samples
//#define TLEN 0x1FFFFFFF
#define TLEN 0x07FFFFFF

//inline float sgn(float x) { return copysignf(1.f,x); }

// external code: xoroshiro128+

uint64_t rng_state[2];

#define TO_FP32 (1.f/16777216.f)
#define ULP1 TO_FP32
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#define EPS (F32_MIN_NORMAL)


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


inline void quat_set(quat_t* q, float x, float y, float z, float w)
{
  q->x=x; q->y=y; q->z=z; q->w=w;
}


inline void quat_set_from_f64(quat_t* q, double x, double y, double z, double w)
{
  q->x=(float)x;
  q->y=(float)y;
  q->z=(float)z;
  q->w=(float)w;
}

typedef struct {
  // column major storage 'hidden' by explict members
  float m00,m10,m20;
  float m01,m11,m21;
  float m02,m12,m22;
} mat33_t;


inline float rsqrtf(float v) { return 1.f/sqrtf(v); }

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
  printf("(%+14a,%+14a,%+14a,%+14a) ",q->x,q->y,q->z,q->w);
}

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


inline void quat_scale(quat_t* q, float s)
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

// alternate conversion w/o diagonal reduction (diagonal)
void quat_to_mat33_ndr_3(mat33_t* m, quat_t* q)
{
  float x = q->x, y = q->y, z = q->z, w = q->w;

  float dwx = (w-x)*(w+x); // m11,m22
  float dwy = (w-y)*(w+y); // m00,m22
//float dwz = (w-z)*(w+z); // m00,m11

//float dxy = (x-y)*(x+y); // m00,m11
  float dxz = (x-z)*(x+z); // m00,m22
  float dyz = (y-z)*(y+z); // m11,m22

  m->m00 = dwy+dxz;
  m->m11 = dwx+dyz;
  m->m22 = dwx-dyz;
  //m->m00 = dwz+dxy;
  //m->m11 = dwz-dxy;
  //m->m22 = dwy-dxz;
  
  //m->m00 = 0.5f*(dwy+dxz+dwz+dxy);
  //m->m11 = 0.5f*(dwx+dyz+dwz-dxy);
  //m->m22 = 0.5f*(dwx-dyz+dwy-dxz);

  float tx = 2*x,  ty = 2*y,  tz = 2*z;
  float xy = ty*x, xz = tz*x, yz = ty*z;
  float wx = tx*w, wy = ty*w, wz = tz*w;

  m->m10 = xy+wz; m->m01 = xy-wz;
  m->m20 = xz-wy; m->m02 = xz+wy;
  m->m21 = yz+wx; m->m12 = yz-wx;
}



typedef struct { float h,l; } f32_pair_t;

// (a*b) exactly represented by unevaluated pair (h+l)
// * |l| <= ulp(h)/2
// * provided a+b does not overflow. 
inline void f32_2mul(f32_pair_t* p, float a, float b)
{
  float x = a*b;
  float y = fmaf(a,b,-x);
  p->h = x;
  p->l = y;
}

inline float f32_mma(float a, float b, float c, float d)
{
  float t = c*d;
  float e = fmaf(c,d,-t);
  float f = fmaf(a,b, t);
  return f+e;
}

inline float f32_mms(float a, float b, float c, float d)
{
  return f32_mma(a,b,c,-d); // temp hack
}

void quat_to_mat33_ndr_fma_(mat33_t* m, quat_t* q)
{
  float x = q->x, y = q->y, z = q->z, w = q->w;

  float wx = (w-x)*(w+x);      // m11,m22
  float wy = (w-y)*(w+y);      // m00,m22
//float wz = (w-z)*(w+z);      // m00,m11
//float xy = (x-y)*(x+y);      // m00,m11
  float xz = (x-z)*(x+z);      // m00,m22
  float yz = (y-z)*(y+z);      // m11,m22

  m->m00 = wy+xz;
  m->m11 = wx+yz;
  m->m22 = wx-yz;
  
  float tx = 2*x,  ty = 2*y,  tz = 2*z;

  m->m10 = f32_mma(tx,y,w,tz); // xy+wz;
  m->m01 = f32_mms(tx,y,w,tz); // xy-wz;
  m->m02 = f32_mma(tx,z,w,ty); // xz+wy;
  m->m20 = f32_mms(tx,z,w,ty); // xz-wy;
  m->m21 = f32_mma(ty,z,w,tx); // yz+wx;
  m->m12 = f32_mms(ty,z,w,tx); // yz-wx;
}

// temp hack (hybrid)
void quat_to_mat33_ndr_fma(mat33_t* m, quat_t* q)
{
  float x  = q->x, y = q->y, z  = q->z, w = q->w;
  float tx = 2*x, ty = 2*y,  tz = 2*z;

  if (w >= 0.5f) {
    float wx = (w-x)*(w+x);      // m11,m22
    float wy = (w-y)*(w+y);      // m00,m22
  //float wz = (w-z)*(w+z);      // m00,m11
  //float xy = (x-y)*(x+y);      // m00,m11
    float xz = (x-z)*(x+z);      // m00,m22
    float yz = (y-z)*(y+z);      // m11,m22
    
    m->m00 = wy+xz;
    m->m11 = wx+yz;
    m->m22 = wx-yz;
  }
  else {
    m->m00 = 1.f-f32_mma(ty,y,tz,z);
    m->m11 = 1.f-f32_mma(tx,x,tz,z);
    m->m22 = 1.f-f32_mma(tx,x,ty,y);
  }

  m->m10 = f32_mma(tx,y,w,tz); // xy+wz;
  m->m01 = f32_mms(tx,y,w,tz); // xy-wz;
  m->m02 = f32_mma(tx,z,w,ty); // xz+wy;
  m->m20 = f32_mms(tx,z,w,ty); // xz-wy;
  m->m21 = f32_mma(ty,z,w,tx); // yz+wx;
  m->m12 = f32_mms(ty,z,w,tx); // yz-wx;
}


void quat_to_mat33_foo(mat33_t* m, quat_t* q)
{
  float x = q->x,  y = q->y,  z = q->z, w = q->w;
  float tx = 2*x,  ty = 2*y,  tz = 2*z;
  float/*xy = ty*x,*/ xz = tz*x, yz = ty*z;
  float wx = tx*w, wy = ty*w /*, wz = tz*w*/;

  m->m02 = xz+wy;
  m->m21 = yz+wx;
  m->m12 = yz-wx;

  m->m10 = f32_mma(tx,y,w,tz);
  m->m01 = f32_mms(tx,y,w,tz);
  m->m20 = f32_mms(tx,z,w,ty);
  m->m02 = f32_mma(tx,z,w,ty);
  m->m21 = f32_mma(ty,z,w,tx);
  m->m12 = f32_mms(ty,z,w,tx);
  
  if (q->w >= 0.5) {
    float t0 = (w-x)*(w+x);
    float t1 = (z-y)*(z+y);
    m->m00 = (x-z)*(x+z)+(w-y)*(w+y);
    m->m11 = t0-t1;
    m->m22 = t0+t1;
  }
  else {
    float xx = tx*x, yy = ty*y, zz = tz*z;  // 2xx, 2yy, 2zz
    m->m00 = 1.f-(yy+zz); m->m11 = 1.f-(xx+zz); m->m22 = 1.f-(xx+yy);
  }
}

// remove scaling ex using recip
inline float recipf(float x) { return 1.f/x; }

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



inline uint32_t f2bits(float v)    { uint32_t r; memcpy(&r,&v,4); return r; }
inline float    bits2f(uint32_t v) { float    r; memcpy(&r,&v,4); return r; }
inline uint32_t sb(float f)        { return f2bits(f) & 0x80000000; }
inline float    fxor(float f, uint32_t b)
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

// signs go wanky as 'w' goes toward zero.
void mat33_to_quat_ok(quat_t* q, mat33_t* m)
{	
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;

  float t0 = m00+m11+m22, d0 = t0+1.f;
  float t1 = m00-m11-m22, d1 = t1+1.f;
  float t2 = m11-m00-m22, d2 = t2+1.f;
  float t3 = m22-m00-m11, d3 = t3+1.f;
  
  float s1 = m21-m12, s2 = m02-m20, s3 = m10-m01;
  float a1 = m21+m12, a2 = m02+m20, a3 = m10+m01;
  
  float w = 0.25f*sqrtf(d0*d0 + s1*s1 + s2*s2 + s3*s3);
  float x = 0.25f*sqrtf(d1*d1 + s1*s1 + a2*a2 + a3*a3);
  float y = 0.25f*sqrtf(d2*d2 + s2*s2 + a1*a1 + a3*a3);
  float z = 0.25f*sqrtf(d3*d3 + s3*s3 + a1*a1 + a2*a2);

  x = copysignf(x, s1);
  y = copysignf(y, s2);
  z = copysignf(z, s3);
  
  quat_set(q, x,y,z,w);
}

// minimal testing looks good to ~0x1.16p-4f (0.06787109375)
// @ 0.1015625 reaches here 87.1%
// @ 0.0678711 reaches here 91.3%

// playin' it a bit safe. need to put a better empirical test
// in this code.
#define OKC_CUT 0.0678711f

void mat33_to_quat_okc(quat_t* q, mat33_t* m)
{	
  float m00=m->m00, m01=m->m01, m02=m->m02;
  float m10=m->m10, m11=m->m11, m12=m->m12;
  float m20=m->m20, m21=m->m21, m22=m->m22;

  float t0 = m00+m11+m22, d0 = t0+1.f;
  float t1 = m00-m11-m22, d1 = t1+1.f;
  float t2 = m11-m00-m22, d2 = t2+1.f;
  float t3 = m22-m00-m11, d3 = t3+1.f;
  
  float s1 = m21-m12, s2 = m02-m20, s3 = m10-m01;
  float a1 = m21+m12, a2 = m02+m20, a3 = m10+m01;
  
  float w = 0.25f*sqrtf(d0*d0 + s1*s1 + s2*s2 + s3*s3);
  float x = 0.25f*sqrtf(d1*d1 + s1*s1 + a2*a2 + a3*a3);
  float y = 0.25f*sqrtf(d2*d2 + s2*s2 + a1*a1 + a3*a3);
  float z = 0.25f*sqrtf(d3*d3 + s3*s3 + a1*a1 + a2*a2);

#if 1
  if (w > OKC_CUT) {
    x = copysignf(x, s1);
    y = copysignf(y, s2);
    z = copysignf(z, s3);
  }
  else {
    if (x > 0.25f) {
      y = copysignf(y,a3);
      z = copysignf(z,a2);
      w = copysignf(w,s1);
    }
    else if (y > 0.25f) {
      x = copysignf(x, a3);
      z = copysignf(z, a1);
      w = copysignf(w, s2);
    }
    else {
      x = copysignf(x, a2);
      y = copysignf(y, a1);
      w = copysignf(w, s3);
    }
  }
#else
  // this is only for testing
  if (w >= x && w >= y && w >= z) {
    x = copysignf(x, s1);
    y = copysignf(y, s2);
    z = copysignf(z, s3);
  }
  else if (x >= w && x >= y && x >= z) {
    y = copysignf(y,a3);
    z = copysignf(z,a2);
    w = copysignf(w,s1);
  }
  else if (y >= w && y >= x && y >= z) {
    x = copysignf(x, a3);
    z = copysignf(z, a1);
    w = copysignf(w, s2);
  }
  else {
    x = copysignf(x, a2);
    y = copysignf(y, a1);
    w = copysignf(w, s3);
  }
#endif  
  
  quat_set(q, (float)x,(float)y,(float)z,(float)w);
}

// sign corrected version (promote to double computation)
// the number of products makes using 'extended' precision
// computations more expensive. Would need to go that way
// for in/out in doubles to tighten the error bound
void mat33_to_quat_okc_d(quat_t* q, mat33_t* m)
{	
  double m00=m->m00, m01=m->m01, m02=m->m02;
  double m10=m->m10, m11=m->m11, m12=m->m12;
  double m20=m->m20, m21=m->m21, m22=m->m22;

  double t0 = m00+m11+m22, d0 = t0+1.f;
  double t1 = m00-m11-m22, d1 = t1+1.f;
  double t2 = m11-m00-m22, d2 = t2+1.f;
  double t3 = m22-m00-m11, d3 = t3+1.f;
  
  double s1 = m21-m12, s2 = m02-m20, s3 = m10-m01;
  double a1 = m21+m12, a2 = m02+m20, a3 = m10+m01;
  
  double w = 0.25*sqrt(d0*d0 + s1*s1 + s2*s2 + s3*s3);
  double x = 0.25*sqrt(d1*d1 + s1*s1 + a2*a2 + a3*a3);
  double y = 0.25*sqrt(d2*d2 + s2*s2 + a1*a1 + a3*a3);
  double z = 0.25*sqrt(d3*d3 + s3*s3 + a1*a1 + a2*a2);

#if 1
  // see notes on compute in singles version
  
  if (w > (double)OKC_CUT) {
    x = copysign(x,s1);
    y = copysign(y,s2);
    z = copysign(z,s3);
  }
  else {
    if (x > 0.25) {
      y = copysign(y,a3);
      z = copysign(z,a2);
      w = copysign(w,s1);
    }
    else if (y > 0.25) {
      x = copysign(x,a3);
      z = copysign(z,a1);
      w = copysign(w,s2);
    }
    else {
      x = copysign(x,a2);
      y = copysign(y,a1);
      w = copysign(w,s3);
    }
  }
#else
  // this is only for testing
  if (w >= x && w >= y && w >= z) {
    x = copysign(x, s1);
    y = copysign(y, s2);
    z = copysign(z, s3);
  }
  else if (x >= w && x >= y && x >= z) {
    y = copysign(y,a3);
    z = copysign(z,a2);
    w = copysign(w,s1);
  }
  else if (y >= w && y >= x && y >= z) {
    x = copysign(x, a3);
    z = copysign(z, a1);
    w = copysign(w, s2);
  }
  else {
    x = copysign(x, a2);
    y = copysign(y, a1);
    w = copysign(w, s3);
  }
#endif  
  
  quat_set_from_f64(q, x,y,z,w);
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
    float s  = 0.5f*rsqrtf(t);
    quat_set(q, s*(m21-m12), s*(m02-m20), s*(m10-m01), s*t);
    return;
  }

  // just using 'Day' for this toy version..lazy
  mat33_to_quat_day(q,m);
}

#define PI 3.1415927410125732421875f

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

typedef void(*quat_to_mat33_t)(mat33_t*, quat_t*);

typedef struct {
  quat_to_mat33_t f;
  char*           name;
} q2m_t;

q2m_t q2m[] =
{
 {.f=&quat_to_mat33_std,     .name="standard"},
 {.f=&quat_to_mat33_ndr,     .name="no diagonal reduction"},
 {.f=&quat_to_mat33_ndr_3,   .name="no diagonal reduction (factor 1)"},
 {.f=&quat_to_mat33_ndr_fma, .name="no diagonal reduction (fma)"},
 {.f=&quat_to_mat33_foo,     .name="hack"},
};

typedef void(*mat33_to_quat_t)(quat_t*, mat33_t*);

typedef struct {
  mat33_to_quat_t f;
  char*           name;
} m2q_t;

m2q_t m2q[] =
{
 {.f=&mat33_to_quat_std,    .name="standard"},
 {.f=&mat33_to_quat_small,  .name="small"},
 {.f=&mat33_to_quat_day,    .name="day"},
 {.f=&mat33_to_quat_day_bf, .name="day (branch free)"},
 {.f=&mat33_to_quat_bf0,    .name="high eror branch free"},
 {.f=&mat33_to_quat_bf1,    .name="small with cut"},
 {.f=&mat33_to_quat_ok,     .name="overkill (bf)"},
 {.f=&mat33_to_quat_okc,    .name="overkill"},
 {.f=&mat33_to_quat_okc_d,  .name="overkill (double promote)"},
};


static void test(uint32_t fxi, uint32_t ixi)
{
  double e = 0.0;
  double h[HLEN];
  quat_t q;
  quat_to_mat33_t fx = q2m[fxi].f;
  mat33_to_quat_t ix = m2q[ixi].f;

  memset(h, 0, sizeof(h[0])*HLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    double   a   = uniform_quat(&q);
    uint32_t hid = (uint32_t)(((2.f*HLEN)/PI)*fabs(a));
    mat33_t  m;
    quat_t   r;

    // clamp to histogram length
    if (hid >= HLEN) hid=HLEN-1;

    fx(&m, &q);
    ix(&r, &m);

    double ae = a_error(&q,&r);

    // track peak error per histogram slot
    if (ae > h[hid]) { h[hid] = ae; /* quat_dup(&(hq[hid]),&q); */ }
    
    if (ae > e) {
      e = ae;
      printf("%08x %f ", i, e);
#if 0
      quat_print(&q);
      quat_print(&r);
#else
      quat_printa(&q);
      quat_printa(&r);
#endif      

      //vec3_printa(&d);
      //printf("%e ", m.m10-m.m01);
      //printf("%a %d", m.m00+m.m11+m.m22,method);
      //printf("%f %d ", a, hid);
      ln();
    }
  }

  printf("\n %s:%s (peak=%e) \n{ ", q2m[fxi].name, m2q[ixi].name,e);

   for(uint32_t i=0; i<HLEN-1; i++) {
     printf("%e, ", h[i]);
  }

  printf("%e}\n", h[HLEN-1]);
}

void test_all_pairs(uint64_t s0, uint64_t s1)
{
  for(uint32_t i=0; i<sizeof(m2q)/sizeof(m2q_t); i++) {
    for(uint32_t j=0; j<sizeof(q2m)/sizeof(q2m_t); j++) {
      reset_generators(s0,s1);
      test(j,i);
    }
  }
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

#if 0
  test_all_pairs(s0,s1);
#else  
  reset_generators(s0,s1);
  //test(1,2);
  //test(0,6);
  //test(1,6);
  //test(0,7);
  test(0,8);
  test(1,8);
#endif  
}
