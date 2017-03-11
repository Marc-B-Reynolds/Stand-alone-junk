// Marc B. Reynolds, 2016-2017
// Public Domain under http://unlicense.org, see link for details.
//
// This is utter junk example code for some posts.
// quick cut/paste/mod so errors are likely

#ifndef QUAT_H
#define QUAT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QUAT_ATAN2
#define QUAT_ATAN2(Y,X) atan2f(Y,X)
#endif

#ifndef QUAT_SQRT
#define QUAT_SQRT(X) sqrtf(X)
#endif

#ifndef QUAT_RSQRT
#define QUAT_RSQRT(X) (1.f/QUAT_SQRT(X))
#endif

  
// temp hack
#if 0
#include "xmmintrin.h"

// for error computations of using native ~1/sqrt without fixup
static inline float rsqrtf_a(float x) 
{
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

static inline float rsqrt_nr(float x)
{
  float x0 = rsqrtf_a(x);
  return x0*(1.5f - x*0.5f*x0*x0);
}  

static inline float rsqrt_nr2(float x)
{
  float x0;
  x0 = rsqrtf_a(x);
  x0 = x0*(1.5f - x*0.5f*x0*x0);
  x0 = x0*(1.5f - x*0.5f*x0*x0);
  return x0;
}  

// for error computations of using native ~1/x without fixup
static inline float rcp_a(float x) 
{
  return _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(x)));
}

static inline float rcp_nr(float x) 
{
  float x0 = rcp_a(x);
  return x0*(2.f - x*x0);
}

  static inline float rsqrt_nr2(float x)
{
  float x0;
  x0 = rcp_a(x);
  x0 = x0*(2.f - x*x0);
  x0 = x0*(2.f - x*x0);
  return x0;
}  

#endif

// another temp hack
#define AX a->x
#define AY a->y
#define AZ a->z
#define AW a->w
#define BX b->x
#define BY b->y
#define BZ b->z
#define BW b->w
  
typedef union {
  struct { float x, y; };
  float f[2];
} vec2_t;

static inline void vec2_set(vec2_t* d, float x, float y)
{
  d->x = x; d->y = y;
}

static inline float vec2_dot(vec2_t* a, vec2_t* b)
{
  return a->x*b->x + a->y*b->y;
}

static inline void vec2_zero(vec2_t* v) { vec2_set(v,0.f,0.f); }
static inline void vec2_dup(vec2_t* d, vec2_t* s) { vec2_set(d,s->x,s->y); }
static inline float vec2_norm(vec2_t* a) { return vec2_dot(a,a); }

//------------------  

typedef union {
  struct{ float x, y, z; };
  float f[3];
} vec3_t;

static inline float vec3_dot(vec3_t* a, vec3_t* b)
{
  return a->x*b->x + a->y*b->y + a->z*b->z;
}

static inline float vec3_norm(vec3_t* a) { return vec3_dot(a,a); }

static inline void vec3_set(vec3_t* v, float x, float y, float z)
{
  v->x=x; v->y=y; v->z=z;
}

static inline void vec3_dup(vec3_t* a, vec3_t* b) { vec3_set(a, b->x,b->y,b->z); }

static inline void vec3_zero(vec3_t* v) { vec3_set(v, 0.f,0.f,0.f); }

static inline void vec3_scale(vec3_t* v, float s)
{
  v->x *= s;
  v->y *= s;
  v->z *= s;
}

static inline void vec3_neg(vec3_t* v) { v->x = -v->x; v->y = -v->y; v->z = -v->z; }

static inline void vec3_cross(vec3_t* r, vec3_t* a, vec3_t* b)
{
  float x = a->y*b->z - a->z*b->y;
  float y = a->z*b->x - a->x*b->z;
  float z = a->x*b->y - a->y*b->x;
  vec3_set(r, x, y, z);
}

void vec3_sub(vec3_t* d, vec3_t* a, vec3_t* b)
{
  d->x = a->x - b->x;
  d->y = a->y - b->y;
  d->z = a->z - b->z;
}

static inline void vec3_hmul(vec3_t* r, vec3_t* a, vec3_t* b)
{
  vec3_set(r, AX*BX, AY*BY, AZ*BZ);
}


static inline void vec3_set_scale(vec3_t* a, vec3_t* b, float s)
{
  vec3_set(a, s*b->x, s*b->y, s*b->z);
}

// SEE: marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
// for math and other versions
static inline void vec3_ortho_basis(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  // this assumes v->z not approaching -z
  float x = -v->x;
  float y =  v->y;
  float z =  v->z;
  
  float a = y/(1.f + z);
  float b = y*a;
  float c = x*a;
  
  vec3_set(xp, z+b, c,      x);
  vec3_set(yp, c,   1.f-b, -y);
}
  

//------------------

typedef union {
  struct{ float x, y, z, w;  };
  struct{ vec3_t b; float s; };
  float f[4];
} quat_t;


#define QUAT_BO(OP)    \
  r->x = a->x OP b->x; \
  r->y = a->y OP b->y; \
  r->z = a->z OP b->z; \
  r->w = a->w OP b->w;

#define QUAT_UO(OP) \
  r->x = OP a->x;   \
  r->y = OP a->y;   \
  r->z = OP a->z;   \
  r->w = OP a->w;

static inline void quat_bv_zero(quat_t* r)
{
  vec3_zero(&(r->b));
}

static inline void quat_one(quat_t* r)
{
  quat_bv_zero(r); r->w = 1.f;
}

static inline void quat_add(quat_t* r, quat_t* a, quat_t* b) { QUAT_BO(+) }
static inline void quat_sub(quat_t* r, quat_t* a, quat_t* b) { QUAT_BO(-) }

static inline void quat_neg(quat_t* r, quat_t* a)            { QUAT_UO(-) }

static inline void quat_set(quat_t* r, float x, float y, float z, float w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

static inline void quat_dup(quat_t* r, quat_t* a)
{
  quat_set(r, a->x,a->y,a->z,a->w);
}

static inline void quat_conj(quat_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

static inline float quat_bdot(quat_t* a, quat_t* b)
{
  return vec3_dot(&(a->b), &(b->b));
}

static inline float quat_bnorm(quat_t* a)
{
  return vec3_norm(&(a->b));
}

static inline float quat_dot(quat_t* a, quat_t* b)
{
  return quat_bdot(a,b)+a->w*b->w;
}

// angle in H wrt positive real. quite different
// result than acos(q->w).
float quat_angle(quat_t* q)
{
  return QUAT_ATAN2(QUAT_SQRT(quat_bnorm(q)), q->w);
}

// AB
static inline void quat_mul(quat_t* r, quat_t* a, quat_t* b)
{
  float x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  float y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  float z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  float w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quat_set(r,x,y,z,w);
}

// BA^*
static inline void quat_bac(quat_t* r, quat_t* a, quat_t* b)
{
  float x = -AX*BW + AW*BX - AZ*BY + AY*BZ;
  float y = -AY*BW + AZ*BX + AW*BY - AX*BZ;
  float z = -AZ*BW - AY*BX + AX*BY + AW*BZ;
  float w =  AW*BW + AX*BX + AY*BY + AZ*BZ;
  quat_set(r,x,y,z,w);
}

// R = sa A + ab B
static inline void quat_wsum(quat_t* r, quat_t* a, quat_t* b, float sa, float sb)
{
  float x = sa*a->x + sb*b->x;
  float y = sa*a->y + sb*b->y;
  float z = sa*a->z + sb*b->z;
  float w = sa*a->w + sb*b->w;
  quat_set(r,x,y,z,w);
}
  
static inline float quat_bv_norm(quat_t* r) { return quat_bdot(r,r); }
static inline float quat_norm(quat_t* r)  { return quat_dot(r,r); }

static inline void quat_bv_scale(quat_t* r, float s)
{
  vec3_scale(&(r->b), s);
}

static inline void quat_scale(quat_t* r, float s)
{
  vec3_scale(&(r->b), s);
  r->w *= s;
}

static inline void quat_bv_set_scale(quat_t* r, vec3_t* v, float s)
{
  vec3_set_scale(&(r->b), v, s);
}

static inline void quat_put_bv(vec3_t* v, quat_t* q, float s)
{
  vec3_set_scale(v, &(q->b), s);
}  

static inline void quat_sq(quat_t* q)
{
  float w = q->w;
  float d = quat_bv_norm(q);
  quat_bv_scale(q, w+w);
  q->w = w*w-d;
}

static inline void quat_upow2(quat_t* q)
{
  float t = 2.f*q->w;
  quat_bv_scale(q, t);
  q->w = t-1.f;
}
  
// Q = cos(t)+sin(t)U, |Q|=1
// sqrt(Q) = cos(t/2) + sin(t/2)U
static inline void quat_usqrt(quat_t* q)
{
  float d = 1.f + q->w;
  float s = QUAT_RSQRT(d+d);
  quat_bv_scale(q, s);
  q->w = d*s;
}

// SEE: marc-b-reynolds.github.io/XX
// Q = sqrt(ba^*)
static inline void quat_from_normals(quat_t* q, vec3_t* a, vec3_t* b)
{
  // this assumes b not approaching -a
  vec3_t v;
  float d = 1.f+vec3_dot(a,b);
  float s = QUAT_RSQRT(d+d);
  vec3_cross(&v,a,b);
  quat_bv_set_scale(q,&v,s);
  q->w = s*d;
}

// QvQ^*
static inline void quat_rot(vec3_t* r, quat_t* q, vec3_t* v)
{
  float k0 = q->w*q->w - 0.5f;
  float k1 = vec3_dot(v, &q->b);
  float x  = v->x*k0 + q->x*k1;
  float y  = v->y*k0 + q->y*k1;
  float z  = v->z*k0 + q->z*k1;
  x += q->w*(q->y*v->z-q->z*v->y);
  y += q->w*(q->z*v->x-q->x*v->z);
  z += q->w*(q->x*v->y-q->y*v->x);
  x += x;
  y += y;
  z += z;

  vec3_set(r,x,y,z);
}

// sqrt(Q), |Q|=1, Q.1 >= 0, return bivector part
static inline void quat_fha(vec3_t* v, quat_t* q)
{
  float d = 1.f + q->w;
  float s = QUAT_RSQRT(d+d);
  quat_put_bv(v,q,s);
}

// inverse half-angle from bivector part
static inline void quat_iha(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float s = QUAT_SQRT(1.f-d);
  quat_bv_set_scale(q,v,s+s);
  q->w = 1.f-(d+d);
}

// forward Cayley: |Q|=1, Q.1 >= 0
static inline void quat_fct(vec3_t* v, quat_t* q)
{
  float s = 1.f/(1.f+q->w);
  quat_put_bv(v,q,s);
}

// inverse Cayley: Q.1 = 0
static inline void quat_ict(quat_t* q, vec3_t* v)
{
  float s = 2.f/(1.f+vec3_norm(v));
  quat_bv_set_scale(q,v,s);
  q->w = s-1.f;
}

static inline int quat_in_cos_delta(quat_t* a, quat_t* b, float cosd) 
{
  return quat_dot(a,b) >= cosd;
}

static inline void quat_local_x(vec3_t* v, quat_t* q)
{
  float ty = 2.f * q->y;
  float tz = 2.f * q->z;
  float yy = ty  * q->y;
  float zz = tz  * q->z;
  float xy = ty  * q->x;
  float xz = tz  * q->x;
  float wy = ty  * q->w;
  float wz = tz  * q->w;
  
  vec3_set(v, 1.f-(yy+zz), xy+wz, xz-wy);
}

static inline void quat_local_y(vec3_t* v, quat_t* q)
{
  float tx = 2.f * q->x;
  float tz = 2.f * q->z;
  float xx = tx  * q->x;
  float zz = tz  * q->z;
  float xy = tx  * q->y;
  float yz = tz  * q->y;
  float wx = tx  * q->w;
  float wz = tz  * q->w;
  
  vec3_set(v, xy-wz, 1.f-(xx+zz), wx+yz);
}

static inline void quat_local_z(vec3_t* v, quat_t* q)
{
  float tx = 2.f * q->x;
  float ty = 2.f * q->y;
  float xx = tx  * q->x;
  float yy = ty  * q->y;
  float xz = tx  * q->z;
  float yz = ty  * q->z;
  float wx = tx  * q->w;
  float wy = ty  * q->w;
  
  vec3_set(v, wy+xz, yz-wx, 1.f-(xx+yy));
}

  
#if defined(QUAT_IMPLEMENTATION)
#else
#endif

#undef AX
#undef AY
#undef AZ
#undef AW
#undef BX
#undef BY
#undef BZ
#undef BW
  
#ifdef __cplusplus
extern }
#endif


#endif
