// Marc B. Reynolds, 2016-2024
// Public Domain under http://unlicense.org, see link for details.
//
// This is utter junk example code for some posts.
// quick cut/paste/mod so errors are likely

#ifndef   VEC2_H
#define   VEC2_H

#ifndef   F32_UTIL_H
#include "f32_util.h"
#endif
  
typedef union {
  struct { float x, y; };
  float f[2];
} vec2_t;

#define AX a->x
#define AY a->y
#define BX b->x
#define BY b->y
  
static inline void vec2_set(vec2_t* d, float x, float y)
{
  d->x = x; d->y = y;
}

  
static inline void  vec2_zero(vec2_t* v) { vec2_set(v,0.f,0.f); }
static inline void  vec2_dup(vec2_t* d, vec2_t* s) { vec2_set(d,s->x,s->y); }

static inline void vec2_add(vec2_t* d, vec2_t* a, vec2_t* b)
{
  d->x = AX+BX;
  d->y = AY+BY;
}

static inline void vec2_sub(vec2_t* d, vec2_t* a, vec2_t* b)
{
  d->x = AX-BX;
  d->y = AY-BY;
}
  
static inline float vec2_dot(vec2_t* a, vec2_t* b)
{
  return AX*BX + AY*BY;
}

static inline float vec2_norm(vec2_t* a)
{
  return vec2_dot(a,a);
}

static inline void vec2_neg(vec2_t* a)
{
  vec2_set(a,-AX,-AY);
}

// complex: conjugate (a^*)
static inline void vec2_conj(vec2_t* a)
{
  a->y = -a->y;
}

static inline void vec2_ortho(vec2_t* d, vec2_t* a)
{
  vec2_set(d,-AY,AX);
}

// complex: d=ab
static inline void vec2_mul(vec2_t* d, vec2_t* a, vec2_t* b)
{
  vec2_set(d,AX*BX-AY*BY, AX*BY+AY*BX);
}

// complex: d=ab^*
static inline void vec2_mulc(vec2_t* d, vec2_t* a, vec2_t* b)
{
  vec2_set(d,AX*BX+AY*BY, AY*BX-AX*BY);
}

// complex: d=a^2
static inline void vec2_sq(vec2_t* d, vec2_t* a)
{
  float x=a->x, y=a->y, xy=x*y;
  vec2_set(d,(x+y)*(x-y),xy+xy);
}

// complex: d=sqrt(a) where |a|=1
static inline void vec2_usqrt(vec2_t* d, vec2_t* a)
{
  float x  = a->x, y = a->y;
  float m  = x+1.f; m = f32_sqrtf(m+m);
  float rx = 0.5f * m;
  float ry = y/(m + 0x1p-126f);
  vec2_set(d, rx,ry);
}  
  
#undef AX
#undef AY
#undef BX
#undef BY
  
#endif
