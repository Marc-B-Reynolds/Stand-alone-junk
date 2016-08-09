
// Toy code for: // http://marc-b-reynolds.github.io/quaternion/2016/08/09/TwoNormToRot.html
//
// Generate uniform points on the unit sphere, form the rotation, reconstruct the
// target normal and roughly measure angular error.  Ignores the degenerate
// case in all implementations.
//
// to compile under VC you'll have to change the float hex-constants...couldn't
// be bothered.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#if 1 // LAZY HERE
#include <x86intrin.h>
#endif

// compile time configuration options

// enable to test with both pseudo-random and Sobol sequences
#define USE_SOBOL

#define TRIALS 0xFFFFFFF

#include "../SFH/quat.h"

// xoroshiro128+

uint64_t rng_state[2];

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

static inline float rng_f32(void)
{
  return (rng_u64() >> 40)*0x1p-24f;
}

// uniform on disk
float uniform_disk(vec2_t* p)
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

// uniform on S2
void uniform_s2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_disk(&v);  
  s = 2.f*sqrtf(1.f-d);
  p->x = s*v.x;
  p->y = s*v.y;
  p->z = 1.f-2.f*d;
}

void ln(void) {printf("\n");}

void vec3_print(vec3_t* v)
{
  printf("(%+f,%+f,%+f) ",v->x,v->y,v->z);
}

void vec3_printa(vec3_t* v)
{
  printf("(%+a,%+a,%+a) ",v->x,v->y,v->z);
}

void matrix_ver(float* m, vec3_t* a, vec3_t* b)
{
  vec3_t v;
  vec3_t s;
  vec3_cross(&v,a,b);
  vec3_hmul(&s,&v,&v);
   
  float d  = vec3_dot(a,b);
  float r  = 1.f/(1.f+d);
  float rz = v.z*r;
  float xy = v.x*v.y*r;  
  float xz = v.x*rz;  
  float yz = v.y*rz;
  
  m[1]= xy-v.z; m[3]= xy+v.z;
  m[2]= xz+v.y; m[6]= xz-v.y;
  m[5]= yz-v.x; m[7]= yz+v.x;
  
  m[0]= d+r*s.x;
  m[4]= d+r*s.y;
  m[8]= d+r*s.z;
}

void m33_xform(vec3_t* r, float* m, vec3_t* v)
{
  r->x = m[0]*v->x + m[1]*v->y + m[2]*v->z;
  r->y = m[3]*v->x + m[4]*v->y + m[5]*v->z;
  r->z = m[6]*v->x + m[7]*v->y + m[8]*v->z;
}

float spew(vec3_t* r, vec3_t* a, vec3_t* b, char c)
{
  float d = vec3_dot(r,b);
  float e = vec3_dot(a,b);
  printf("%c: %f ", c, 57.2958f*acosf(d));
  vec3_print(r);
  vec3_print(b); 
  vec3_print(a); 
  printf("%+f %10f\n", e, 57.2958f*acosf(e));
  return d;
}


int main()
{
  uint64_t t = __rdtsc();
  
  rng_state[0] = t;
  rng_state[1] = t ^ _rdtsc();
  float d0 = 1.f;
  float d1 = 1.f;

  for(uint32_t i=0; i<TRIALS; i++) {
    vec3_t a,b,r,s;
    quat_t q;
    float  m[9];
    
    // 2 uniform normals
    uniform_s2(&a);
    uniform_s2(&b);
   
    quat_from_normals(&q,&a,&b);  // quat ver
    quat_rot(&r,&q,&a);
    
    matrix_ver(m,&a,&b);          // matrix ver
    m33_xform(&s,m,&a);
    
    if (vec3_dot(&r,&b) < d0) { d0 = spew(&r, &a,&b, 'Q'); }
    if (vec3_dot(&s,&b) < d1) { d1 = spew(&s, &a,&b, 'M'); }
  }
  
  return 0;
}

