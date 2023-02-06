// Marc B. Reynolds, 2016-2023
// Public Domain under http://unlicense.org, see link for details.
//
// This is utter junk example code for some posts.
// quick cut/paste/mod so errors are likely

#ifndef VEC3_H
#define VEC3_H

#ifdef __cplusplus
extern "C" {
#ifdef __emacs_hack
}  
#endif  
#endif

#include "f32_util.h"
  
typedef union {
  struct{ float x, y, z; };
  float f[3];
} vec3_t;

#define AX a->x
#define AY a->y
#define AZ a->z
#define BX b->x
#define BY b->y
#define BZ b->z

static inline void vec3_set(vec3_t* v, float x, float y, float z)
{
  v->x=x; v->y=y; v->z=z;
}

static inline void vec3_dup(vec3_t* a, vec3_t* b) { vec3_set(a, b->x,b->y,b->z); }

static inline void vec3_zero(vec3_t* v) { vec3_set(v, 0.f,0.f,0.f); }


static inline void vec3_add(vec3_t* d, vec3_t* a, vec3_t* b)
{
  d->x = AX + BX; d->y = AY + BY; d->z = AZ + BZ;
}

static inline void vec3_sub(vec3_t* d, vec3_t* a, vec3_t* b)
{
  d->x = AX - BX; d->y = AY - BY; d->z = AZ - BZ;
}

static inline float vec3_dot(vec3_t* a, vec3_t* b)
{
  return AX*BX + AY*BY + AZ*BZ;
}

static inline float vec3_norm(vec3_t* a) { return vec3_dot(a,a); }

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


static inline void vec3_hmul(vec3_t* r, vec3_t* a, vec3_t* b)
{
  vec3_set(r, AX*BX, AY*BY, AZ*BZ);
}

static inline void vec3_set_scale(vec3_t* a, vec3_t* b, float s)
{
  vec3_set(a, s*BX, s*BY, s*BZ);
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

  
#undef AX
#undef AY
#undef AZ
#undef BX
#undef BY
#undef BZ
  
#ifdef __cplusplus
extern }
#endif


#endif
