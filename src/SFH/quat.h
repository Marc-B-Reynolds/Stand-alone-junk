// Marc B. Reynolds, 2016-2023
// Public Domain under http://unlicense.org, see link for details.
//
// This is utter junk example code for some posts.
// quick cut/paste/mod so errors are likely

#ifndef   QUAT_H
#define   QUAT_H

#ifndef   VEC3_H
#include "vec3.h"  
#endif

#ifndef QUAT_ATAN2
#define QUAT_ATAN2(Y,X) atan2f(Y,X)
#endif

static inline float quat_util_sgn_f32(float x) { return copysignf(1.f,x); }  

// another temp hack
#define AX a->x
#define AY a->y
#define AZ a->z
#define AW a->w
#define BX b->x
#define BY b->y
#define BZ b->z
#define BW b->w

//------------------

typedef union {
  struct{ float x, y, z, w;  };
  struct{ vec3_t b; float s; };
  float f[4];
} quat_t;


static inline void quat_set(quat_t* r, float x, float y, float z, float w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

#define QUAT_DEF_SET(V,X,Y,Z,W) quat_t V = {.x=(X),.y=(Y),.z=(Z),.w=(W)}

  
#define QUAT_BO(OP) \
  r->x = AX OP BX;  \
  r->y = AY OP BY;  \
  r->z = AZ OP BZ;  \
  r->w = AW OP BW;

#define QUAT_UO(OP) \
  r->x = OP AX;     \
  r->y = OP AY;     \
  r->z = OP AZ;     \
  r->w = OP AW;

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


static inline void quat_set_id(quat_t* r)
{
  quat_set(r,0.f,0.f,0.f,1.f);
}  

static inline void quat_dup(quat_t* r, quat_t* a)
{
  quat_set(r, AX,AY,AZ,AW);
}

// in-place conjugation: a^*
static inline void quat_conj(quat_t* a)
{
  AX = -AX;
  AY = -AY;
  AZ = -AZ;
}

// in-place -a^*
static inline void quat_nconj(quat_t* a)
{
  AW = -AW;
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
  return quat_bdot(a,b)+AW*BW;
}

// angle in H wrt positive real. quite different
// result than acos(q->w).
static inline float quat_angle(quat_t* q)
{
  return QUAT_ATAN2(f32_sqrt(quat_bnorm(q)), q->w);
}

  // TODO: mma/mms mul/bac variants
  
// AB
static inline void quat_mul(quat_t* r, quat_t* a, quat_t* b)
{
  float x = AW*BX + AX*BW + AY*BZ - AZ*BY;  
  float y = AW*BY - AX*BZ + AY*BW + AZ*BX;
  float z = AW*BZ + AX*BY - AY*BX + AZ*BW;
  float w = AW*BW - AX*BX - AY*BY - AZ*BZ;
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
  float x = sa*AX + sb*BX;
  float y = sa*AY + sb*BY;
  float z = sa*AZ + sb*BZ;
  float w = sa*AW + sb*BW;
  quat_set(r,x,y,z,w);
}

static inline void quat_wsum_hq(quat_t* r, quat_t* a, quat_t* b, float sa, float sb)
{
  float x = f32_mma(sa,AX, sb,BX);
  float y = f32_mma(sa,AY, sb,BY);
  float z = f32_mma(sa,AZ, sb,BZ);
  float w = f32_mma(sa,AW, sb,BW);
  quat_set(r,x,y,z,w);
}

  
static inline float quat_bv_norm(quat_t* r) { return quat_bdot(r,r); }
static inline float quat_norm(quat_t* r)    { return quat_dot(r,r); }

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
  float w = q->w;
  float t = 2.f*w;
  quat_bv_scale(q, t);
  q->w = w*t-1.f;
}
  
// Q = cos(t)+sin(t)U, |Q|=1
// sqrt(Q) = cos(t/2) + sin(t/2)U
static inline void quat_usqrt(quat_t* q)
{
  float d = 1.f + q->w;
  float s = f32_rsqrt(d+d);
  quat_bv_scale(q, s);
  q->w = d*s;
}

// slerp(a,b,1/2)
static inline void quat_bisect(quat_t* r, quat_t* a, quat_t* b)
{
  float d  = quat_dot(a,b);
  float sa = f32_rsqrt(2.f+2.f*fabsf(d));
  float sb = quat_util_sgn_f32(d)*sa;
  quat_wsum(r,a,b,sa,sb);
}  

// SEE: marc-b-reynolds.github.io/XX
// Q = sqrt(ba^*)
static inline void quat_from_normals(quat_t* q, vec3_t* a, vec3_t* b)
{
  // this assumes b not approaching -a
  vec3_t v;
  float d = 1.f+vec3_dot(a,b);
  float s = f32_rsqrt(d+d);
  vec3_cross(&v,a,b);
  quat_bv_set_scale(q,&v,s);
  q->w = s*d;
}

// TODO: fma version
  
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
  float s = f32_rsqrt(d+d);
  quat_put_bv(v,q,s);
}

// inverse half-angle from bivector part
static inline void quat_iha(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float s = f32_sqrt(1.f-d);
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

// TODO: fma and non reduced expression form (that should be the default)
// sigh: change 1-(a^2+b^2) terms to opposite set?
//   except that requires w^2 term

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
  float tx = 2.f*q->x, ty = 2.f*q->y;
  float xx = tx *q->x, yy = ty *q->y, xz = tx*q->z;
  float yz = ty *q->z, wx = tx *q->w, wy = ty*q->w;
  vec3_set(v, wy+xz, yz-wx, 1.f-(xx+yy));
}

static inline void quat_to_local(vec3_t* X, vec3_t* Y, vec3_t* Z, quat_t* q)
{
  float tx = 2.f*q->x, ty = 2.f*q->y, tz = 2.f*q->z;
  float xx = tx *q->x, yy = ty *q->y, zz = tz *q->z;
  float xy = ty *q->x, xz = tz *q->x, yz = ty *q->z;
  float wx = tx *q->w, wy = ty *q->w, wz = tz *q->w;

  vec3_set(X, 1.f-(yy+zz), xy+wz, xz-wy);
  vec3_set(Y, xy-wz, 1.f-(xx+zz), wx+yz);
  vec3_set(Z, wy+xz, yz-wx, 1.f-(xx+yy));
}

// quat_map_{a}2{b} rotates bivector part a to b:  let t=sqrt(ba^*) q=tat^*
//   this is for variable rename. implement WRT to one and expand other variants.
//   (I point to an example since that's only clear to me)
static inline void quat_map_x2y(quat_t* q, quat_t* a) { quat_set(q,-AY, AX, AZ, AW); }
static inline void quat_map_x2z(quat_t* q, quat_t* a) { quat_set(q,-AZ, AY, AX, AW); }
static inline void quat_map_y2x(quat_t* q, quat_t* a) { quat_set(q, AY,-AX, AZ, AW); }
static inline void quat_map_y2z(quat_t* q, quat_t* a) { quat_set(q, AX,-AZ, AY, AW); }
static inline void quat_map_z2x(quat_t* q, quat_t* a) { quat_set(q, AZ, AY,-AX, AW); }
static inline void quat_map_z2y(quat_t* q, quat_t* a) { quat_set(q, AX, AZ,-AY, AW); }


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
  
#endif
