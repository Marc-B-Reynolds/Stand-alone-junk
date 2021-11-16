// Marc B. Reynolds, 2016-2021
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
inline float rsqrtf_a(float x) 
{
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

inline float rsqrtf_nr(float x)
{
  float  x0 = rsqrtf_a(x);
  return x0*(1.5f - 0.5f*x*x0*x0);
}  

inline float rsqrtf_nr2(float x)
{
  float x0;
  x0 = rsqrtf_a(x);
  x0 = x0*(1.5f - 0.5f*x*x0*x0);
  x0 = x0*(1.5f - 0.5f*x*x0*x0);
  return x0;
}

// for error computations of using native ~1/x without fixup
inline float rcp_a(float x) 
{
  return _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(x)));
}

inline float rcp_nr(float x) 
{
  float x0 = rcp_a(x);
  return x0*(2.f - x*x0);
}

inline float rsqrt_nr2(float x)
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

#include "vec2.h"
  

// SEE: marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
// for math and other versions
inline void vec3_ortho_basis(vec3_t* v, vec3_t* xp, vec3_t* yp)
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

inline void quat_bv_zero(quat_t* r)
{
  vec3_zero(&(r->b));
}

inline void quat_one(quat_t* r)
{
  quat_bv_zero(r); r->w = 1.f;
}

inline void quat_add(quat_t* r, quat_t* a, quat_t* b) { QUAT_BO(+) }
inline void quat_sub(quat_t* r, quat_t* a, quat_t* b) { QUAT_BO(-) }

inline void quat_neg(quat_t* r, quat_t* a)            { QUAT_UO(-) }

inline void quat_set(quat_t* r, float x, float y, float z, float w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

inline void quat_dup(quat_t* r, quat_t* a)
{
  quat_set(r, a->x,a->y,a->z,a->w);
}

// in-place conjugation: a^*
inline void quat_conj(quat_t* a)
{
  a->x = -a->x;
  a->y = -a->y;
  a->z = -a->z;
}

inline void quat_nconj(quat_t* a)
{
  a->w = -a->w;
}
  
inline float quat_bdot(quat_t* a, quat_t* b)
{
  return vec3_dot(&(a->b), &(b->b));
}

inline float quat_bnorm(quat_t* a)
{
  return vec3_norm(&(a->b));
}

inline float quat_dot(quat_t* a, quat_t* b)
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
inline void quat_mul(quat_t* r, quat_t* a, quat_t* b)
{
  float x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  float y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  float z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  float w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quat_set(r,x,y,z,w);
}

// BA^*
inline void quat_bac(quat_t* r, quat_t* a, quat_t* b)
{
  float x = -AX*BW + AW*BX - AZ*BY + AY*BZ;
  float y = -AY*BW + AZ*BX + AW*BY - AX*BZ;
  float z = -AZ*BW - AY*BX + AX*BY + AW*BZ;
  float w =  AW*BW + AX*BX + AY*BY + AZ*BZ;
  quat_set(r,x,y,z,w);
}

// R = sa A + ab B
inline void quat_wsum(quat_t* r, quat_t* a, quat_t* b, float sa, float sb)
{
  float x = sa*a->x + sb*b->x;
  float y = sa*a->y + sb*b->y;
  float z = sa*a->z + sb*b->z;
  float w = sa*a->w + sb*b->w;
  quat_set(r,x,y,z,w);
}
  
inline float quat_bv_norm(quat_t* r) { return quat_bdot(r,r); }
inline float quat_norm(quat_t* r)  { return quat_dot(r,r); }

inline void quat_bv_scale(quat_t* r, float s)
{
  vec3_scale(&(r->b), s);
}

inline void quat_scale(quat_t* r, float s)
{
  vec3_scale(&(r->b), s);
  r->w *= s;
}

inline void quat_bv_set_scale(quat_t* r, vec3_t* v, float s)
{
  vec3_set_scale(&(r->b), v, s);
}

inline void quat_put_bv(vec3_t* v, quat_t* q, float s)
{
  vec3_set_scale(v, &(q->b), s);
}  

inline void quat_sq(quat_t* q)
{
  float w = q->w;
  float d = quat_bv_norm(q);
  quat_bv_scale(q, w+w);
  q->w = w*w-d;
}

inline void quat_upow2(quat_t* q)
{
  float w = q->w;
  float t = 2.f*w;
  quat_bv_scale(q, t);
  q->w = w*t-1.f;
}
  
// Q = cos(t)+sin(t)U, |Q|=1
// sqrt(Q) = cos(t/2) + sin(t/2)U
inline void quat_usqrt(quat_t* q)
{
  float d = 1.f + q->w;
  float s = QUAT_RSQRT(d+d);
  quat_bv_scale(q, s);
  q->w = d*s;
}

// SEE: marc-b-reynolds.github.io/XX
// Q = sqrt(ba^*)
inline void quat_from_normals(quat_t* q, vec3_t* a, vec3_t* b)
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
inline void quat_rot(vec3_t* r, quat_t* q, vec3_t* v)
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
inline void quat_fha(vec3_t* v, quat_t* q)
{
  float d = 1.f + q->w;
  float s = QUAT_RSQRT(d+d);
  quat_put_bv(v,q,s);
}

// inverse half-angle from bivector part
inline void quat_iha(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float s = QUAT_SQRT(1.f-d);
  quat_bv_set_scale(q,v,s+s);
  q->w = 1.f-(d+d);
}

// forward Cayley: |Q|=1, Q.1 >= 0
inline void quat_fct(vec3_t* v, quat_t* q)
{
  float s = 1.f/(1.f+q->w);
  quat_put_bv(v,q,s);
}

// inverse Cayley: Q.1 = 0
inline void quat_ict(quat_t* q, vec3_t* v)
{
  float s = 2.f/(1.f+vec3_norm(v));
  quat_bv_set_scale(q,v,s);
  q->w = s-1.f;
}

inline int quat_in_cos_delta(quat_t* a, quat_t* b, float cosd) 
{
  return quat_dot(a,b) >= cosd;
}

inline void quat_local_x(vec3_t* v, quat_t* q)
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

inline void quat_local_y(vec3_t* v, quat_t* q)
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

inline void quat_local_z(vec3_t* v, quat_t* q)
{
  float tx = 2.f*q->x, ty = 2.f*q->y;
  float xx = tx *q->x, yy = ty *q->y, xz = tx*q->z;
  float yz = ty *q->z, wx = tx *q->w, wy = ty*q->w;
  vec3_set(v, wy+xz, yz-wx, 1.f-(xx+yy));
}

inline void quat_to_local(vec3_t* X, vec3_t* Y, vec3_t* Z, quat_t* q)
{
  float tx = 2.f*q->x, ty = 2.f*q->y, tz = 2.f*q->z;
  float xx = tx *q->x, yy = ty *q->y, zz = tz *q->z;
  float xy = ty *q->x, xz = tz *q->x, yz = ty *q->z;
  float wx = tx *q->w, wy = ty *q->w, wz = tz *q->w;

  vec3_set(X, 1.f-(yy+zz), xy+wz, xz-wy);
  vec3_set(Y, xy-wz, 1.f-(xx+zz), wx+yz);
  vec3_set(Z, wy+xz, yz-wx, 1.f-(xx+yy));
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
