// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io
// Scalar only, more toward clairty than performance
// no deps unless USE_SOBOL is defined, then needs Sobol.h from SFH directory


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include <math.h>

// config section

// run sobol sequence tests
#define USE_SOBOL

// number of trials to run per test
#define SYM_TRIALS 0xFFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// end config


#if defined USE_SOBOL
#define SOBOL_IMPLEMENTATION
#include "../SFH/Sobol.h"

// strat 3D geneator
sobol_3d_t qrng;
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
#define PI      3.1415927410125732421875f
#define RPI     0.3183098733425140380859375f
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#else
#define TO_FP32        0x1p-24f
#define SQRT2          0x1.6a09e6p0f
#define PI             0x3.243f6cp0f
#define RPI            0x5.17cc18p-4f
#define F32_MIN_NORMAL 0x1p-126f
#endif

static inline float rng_f32(void)
{
  return (rng_u64() >> 40)*TO_FP32;
}

typedef union { struct{ float x,y;   }; float f[2]; } vec2_t;
typedef union { struct{ float x,y,z; }; float f[3]; } vec3_t;

void vec2_dup(vec2_t* d, vec2_t* s)
{
  d->x = s->x;
  d->y = s->y;
}

static inline void vec2_set(vec2_t* d, float x, float y)
{
  d->x = x; d->y = y;
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

void vec3_printa(vec3_t* v)
{
  printf("{% a, % a, % a} ",v->x,v->y,v->z);
}

void vec3_print(vec3_t* v)
{
  printf("{% f, % f, % f} ",v->x,v->y,v->z);
}

void vec3_sub(vec3_t* d, vec3_t* a, vec3_t* b)
{
  d->x = a->x - b->x;
  d->y = a->y - b->y;
  d->z = a->z - b->z;
}

void vec3_dup(vec3_t* d, vec3_t* s)
{
  d->x = s->x;
  d->y = s->y;
  d->z = s->z;
}

static inline void vec3_set(vec3_t* d, float x, float y, float z)
{
  d->x = x;
  d->y = y;
  d->z = z;
}

static inline float vec3_dot(vec3_t* a, vec3_t* b)
{
  return a->x*b->x + a->y*b->y + a->z*b->z;
}


void ln()
{
  printf("\n");
}

// for quickly/first-pass hacking in arch specific junk
// !!!!!! the recip hurts performance w current gcc/clang..
//        actually does the 1/x and product...sigh.
static inline float sgn(float x) { return copysignf(1.f,x); }
static inline float mulsgn(float x, float v) { return x >= 0.f ? v : -v; }
static inline float rsqrt(float v) { return 1.f/sqrtf(v); }
static inline float recip(float v) { return 1.f/v; }


static inline uint32_t f32_to_bits(float v)      { uint32_t r; memcpy(&r,&v,4); return r; }
static inline float    f32_from_bits(uint32_t v) { float r;    memcpy(&r,&v,4); return r; }

#define ONE_THIRD (11184811.f/33554432.f)

// this proc is a hack of hacker's delight
float a_cbrt(float x)
{
  uint32_t ix = f32_to_bits(x);
  float    x0 = x;
  ix = ix/4 + ix/16;
  ix = ix   + ix/16;
  ix = ix   + ix/256;
  ix = 0x2a5137a0 + ix;
  x  = f32_from_bits(ix);
  x  = ONE_THIRD*(2.f*x + x0/(x*x));
  x  = ONE_THIRD*(2.f*x + x0/(x*x));
  return x;
}


//****** disc/square stuff

//****** stretch

#define EPS F32_MIN_NORMAL

void map_rs_sd(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = x2 > y2 ? x : y;
  m = fabsf(m)*rsqrt(x2+y2+EPS);
  vec2_set(D, m*x, m*y);
}

void map_rs_ds(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = x2 > y2 ? x : y;
  float r  = recip(fabsf(m)+EPS);
  m = sqrtf(x2+y2)*r;
  vec2_set(D, m*x, m*y);
}


//****** concentric
void map_con_sd(vec2_t* d, vec2_t* s)
{
  float x = s->x;
  float y = s->y;
  float t;

  if (fabsf(x) > fabsf(y)) {
    float r = recip(x);
    t = sinf(0.25f*PI*(y*r));
    d->x = x*sqrtf(1.f-t*t); // longer dep-chain this way (showing correctness)
    d->y = x*t;
  }
  else {
    float r = recip(y+EPS);
    t = sinf(0.25f*PI*(x*r));
    d->x = y*t;
    d->y = y*sqrtf(1.f-t*t);   // ditto
  }
}

// disc->square
void map_con_ds(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = sqrtf(x2+y2);

  if (x2 > y2) {
    float r = recip(fabs(x));
    D->x = mulsgn(x, m);
    D->y = m*(4*RPI)*atanf(y*r);
  }
  else {
    float r = recip((fabsf(y)+EPS));
    D->x = m*(4*RPI)*atanf(x*r);
    D->y = mulsgn(y, m);
  }
}

// square->disc
void map_aea_sd(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;

  if (x2 > y2) {
    float m = sgn(x);
    D->y = 0.5f*SQRT2*y;
    D->x = m*sqrtf(x2-0.5f*y2);
  } else {
    float m = sgn(y);
    D->x = 0.5f*SQRT2*x;
    D->y = m*sqrtf(y2-0.5f*x2);
  }
}

// disc->square
void map_aea_ds(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = sqrtf(x2+y2);

  if (x2 > y2) {
    D->y = SQRT2*y;
    D->x = mulsgn(x, m);
  } else {
    D->x = SQRT2*x;
    D->y = mulsgn(y, m);
  }
}


// square->disc
void map_fong_sd(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  float x2 = x*x;
  float y2 = y*y;
  float d  = x2+y2;
  float r  = rsqrt(d+EPS*EPS);
  float s  = r*sqrtf(d-x2*y2);
  
  vec2_set(D, s*x, s*y);
}

// disc->square
void map_fong_ds(vec2_t* D, vec2_t* S)
{
  // this is unstable
  double x  = S->x;
  double y  = S->y;
  double x2 = x*x;
  double y2 = y*y;

  if (x2 < 256.f*EPS || y2 < 256.f*EPS) { vec2_dup(D,S); return; }
  
  double d  = x2+y2;
  double xy = x*y;
  double r  = d*(d-4.f*xy*xy);
  double s  = sgn(xy)*sqrt(0.5*(d-sqrt(r)));
  D->x = s/(y+EPS);
  D->y = s/(x+EPS);  
}

// square->disc
void map_nowell_sd(vec2_t* D, vec2_t* S)
{
  float x  = S->x;
  float y  = S->y;
  D->x = x*sqrtf(1.f-0.5*y*y);
  D->y = y*sqrtf(1.f-0.5*x*x);
}

// disc->square
void map_nowell_ds(vec2_t* D, vec2_t* S)
{
  float x  = 0.5f*S->x;
  float y  = 0.5f*S->y;
  float x2 = x*x;
  float y2 = y*y;
  float t  = x2-y2;
  float a  = sqrtf(0.5f + t + SQRT2*x);
  float b  = sqrtf(0.5f + t - SQRT2*x);
  float c  = sqrtf(0.5f - t + SQRT2*y);
  float d  = sqrtf(0.5f - t - SQRT2*y);
  D->x = a-b;
  D->y = c-d;
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

  vec3_set(D, s*x, s*y, s*z);
}

void map_fong_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float d = x2+y2+z2;
  float n = rsqrt(d+EPS);
  float m = sqrtf(d-x2*y2-y2*z2-x2*z2+x2*y2*z2);
  m *= n;
  
  vec3_set(D,m*x,m*y,m*z);
}

// unstable..can't be bother to rewrite
void map_fong_bc(vec3_t* D, vec3_t* s)
{
  LOAD_XYZ(s);
  float v=y,w=z;

  if (fabs(x) < EPS) {
    vec2_t b;
    map_fong_ds(&b, &(vec2_t) {.x=v,.y=w});
    D->x = 0;
    D->y = b.x;
    D->z = b.y;
    return;
  }
  
  if (fabs(v) < EPS) {
    vec2_t b;
    map_fong_ds(&b, &(vec2_t) {.x=x,.y=w});
    D->x = b.x;
    D->y = 0;
    D->z = b.y;
    return;
  }
  
  if (fabs(w) < EPS) {
    vec2_t b;
    map_fong_ds(&b, &(vec2_t) {.x=x,.y=v});
    D->x = b.x;
    D->y = b.y;
    D->z = 0;
    return;
  }

  double v2 = v*v;
  double u2 = x*x;
  uint32_t swap = 0;

  if (u2<v2) { double t=u2;u2=v2;v2=t;swap=1;}

  double w2 = w*w;
  double u4 = u2*u2;
  double a  = v2*w2/u4;
  double t  = (v2 + w2)/u2;
  double b  = -(t + a);
  double c  = 1 + t;
  double d  = -(u2 + v2 + w2);
  double Do = b*b*c*c - 4*a*c*c*c - 4*b*b*b*d - 27*a*a*d*d + 18*a*b*c*d;
  double x2;

  if (Do < 0) {
    // one real, one complex conjugate pair
    double Co = ((2*b*b*b - 9*a*b*c + 27*a*a*d + cbrt(sqrt(-27*a*a*Do))/2));
    x2 = -1/(3*a) * ( b + Co + (b*b-3*a*c)/Co);
  }
  else {
    // three distict real roots
    double ro = (2*b*b*b - 9*a*b*c + 27*a*a*d)/2;
    double qo = a*sqrt(27*Do)/2;
    double th = atan2(qo,ro);
    x2 = -(b + 2 * pow(ro*ro+qo*qo, 1.f/6.f) * cos(th/3))/(3*a);
  }

  if (swap == 0) {
    D->x = sgn(x)*sqrt(x2);
    D->y = v/x*D->x;
    D->z = w/x*D->x;
  } else {
    D->y = sgn(v)*sqrt(x2);
    D->x = x/v*D->y;
    D->z = w/v*D->y;
  }
}

// didn't bother to tested w/o inverse
void map_nowell_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float u = x*sqrt(1.f-0.5f*y2-z2*(.5f+(1.f/1.3)*y2));
  float v = y*sqrt(1.f-0.5f*z2-x2*(.5f+(1.f/1.3)*z2));
  float w = z*sqrt(1.f-0.5f*x2-y2*(.5f+(1.f/1.3)*x2));

  vec3_set(D,u,v,w);
}


//****** ball/cube/cylinder stuff (via cylinder methods)
// a number of sqrt(div) could be broken into sqrt*rsqrt

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
    float r = recip(fabs(x));
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

  float a = x*sqrtf(1.f-0.5*y2);
  float b = y*sqrtf(1.f-0.5*x2);
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


#if 0
// cut-n-paste, fill in the blanks, reduce

// not reduced
void map_XXX_cb(vec3_t* tD, vec3_t* tS)
{
  float x  = tS->x;
  float y  = tS->y;
  float z  = tS->z;
  float x2 = x*x;
  float y2 = y*y;
  float z2 = z*z;
  float a,b;

  // square->disc
  vec2_t S = {.x=x,.y=y};
  vec2_t D;
  {
  }
  a = D.x;
  b = D.y;

  // cylinder->ball
  float a2 = a*a;
  float b2 = b*b;
  float d  = a2+b2;
  float s;

  if (z2 > d) {
    s = sqrtf((2.f/3.f)-d/(9*z2));
    z = z - d/(3.f*z);
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(tD,s*a,s*b,z);
}

// not reduced
void map_XXX_bc(vec3_t* tD, vec3_t* tS)
{
  float a  = tS->x;
  float b  = tS->y;
  float z  = tS->z;
  float x2 = a*a;
  float y2 = b*b;
  float z2 = z*z;
  float d  = x2+y2+z2;
  float t  = sqrtf(d);
  float s;

  // ball->cylinder
  if (x2+y2 < 5.f/4.f*z2) {
    s = sqrtf(3.f*t/(t+fabsf(z)));
    tD->z = sgn(z)*t;
  }
  else {
    s = t*rsqrt(x2+y2);
    tD->z = 1.5f*z;
  }

  // disc->square
  vec2_t S = {.x=s*a,.y=s*b};
  vec2_t D;
  {
  }

  tD->x = D.x;
  tD->y = D.y;
}



#endif

#if 0
// for testing disc method derivations: explict cube->cylinder->ball & inverse
typedef void (*map_2d_t)(vec2_t*, vec2_t*);

void map_wrap_cb(vec3_t* D, vec3_t* S, map_2d_t M)
{
  vec2_t AB;
  M(&AB, &(vec2_t){.x=S->x, .y=S->y});
  float a  = AB.x;
  float b  = AB.y;
  float z  = S->z;
  float a2 = a*a;
  float b2 = b*b;
  float d  = a2+b2;
  float z2 = z*z;
  float s;

  if (z2 > d) {
    s = sqrtf((2.f/3.f)-d/(9*z2));
    z = z - d/(3.f*z);
  }
  else {
    s = sqrtf(1-(4*z2)/(9*d));
    z *= (2.f/3.f);
  }

  vec3_set(D, s*a, s*b, z);
}

void map_wrap_bc(vec3_t* D, vec3_t* S, map_2d_t M)
{
  vec2_t AB;
  float x  = S->x;
  float y  = S->y;
  float z  = S->z;
  float x2 = x*x;
  float y2 = y*y;
  float z2 = z*z;
  float d  = x2+y2+z2;
  float t  = sqrtf(d);
  float s;

  if (x2+y2 < 5.f/4.f*z2) {
    s = sqrtf(3.f*t/(t+fabsf(z)));
    D->z = sgn(z)*t;
  }
  else {
    s = t*rsqrt(x2+y2);
    D->z = 1.5f*z;
  }

  M(&AB, &(vec2_t){.x=s*x, .y=s*y});
  D->x = AB.x;
  D->y = AB.y;
}

// temp hack ball/cube via square/disc
void map_hrs_cb(vec3_t* d, vec3_t* s)   { map_wrap_cb(d,s, map_rs_sd);     }
void map_hrs_bc(vec3_t* d, vec3_t* s)   { map_wrap_bc(d,s, map_rs_ds);     }
void map_hcon_cb(vec3_t* d, vec3_t* s)  { map_wrap_cb(d,s, map_con_sd);    }
void map_hcon_bc(vec3_t* d, vec3_t* s)  { map_wrap_bc(d,s, map_con_ds);    }
void map_haea_cb(vec3_t* d, vec3_t* s)  { map_wrap_cb(d,s, map_aea_sd);    }
void map_haea_bc(vec3_t* d, vec3_t* s)  { map_wrap_bc(d,s, map_aea_ds);    }
void map_hell_cb(vec3_t* d, vec3_t* s)  { map_wrap_cb(d,s, map_nowell_sd); }
void map_hell_bc(vec3_t* d, vec3_t* s)  { map_wrap_bc(d,s, map_nowell_ds); }
void map_hfong_cb(vec3_t* d, vec3_t* s) { map_wrap_cb(d,s, map_fong_sd);   }
void map_hfong_bc(vec3_t* d, vec3_t* s) { map_wrap_bc(d,s, map_fong_ds);   }
#endif

// define maps
typedef void (*map_func_t)(vec3_t*, vec3_t*);

typedef struct {
  map_func_t cb;
  map_func_t bc;
  char*      name;
} maps_t;

#define DEF(X) {& map_ ## X ## _cb, & map_ ## X ## _bc, #X}

maps_t maps[] =
{
  // cylinder based
  DEF(rsc),
  DEF(vp),
  DEF(aev),
  DEF(ell),

  // validation versions via cylinder
#if 0  
  DEF(hrs),
  DEF(hcon),
  DEF(haea),
  DEF(hell),
  DEF(hfong)
#endif

  // direct methods
  DEF(rs),
//DEF(nowell),        // no inverse
  DEF(fong)           // unstable
};

#define NUM_FUNCS (sizeof(maps)/sizeof(maps[0]))

#undef DEF


void reset_generators(uint64_t s0, uint64_t s1, uint32_t len)
{
#ifdef USE_SOBOL
  sobol_3d_init(&qrng, (uint32_t)s0, (uint32_t)(s0>>32), (uint32_t)s1);
#endif
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}

void uniform_s2_rt_test(uint64_t s0, uint64_t s1)
{
  printf("* uniform surface round-trip: name: rms cube orig-ball recon-ball cube-diff\n");
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    map_func_t cb = maps[s].cb;
    map_func_t bc = maps[s].bc;
    float      e  = 0.f;
    vec3_t     mp;
    vec3_t     p,d,r,diff;

    reset_generators(s0,s1,SYM_TRIALS);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      
      // generate a uniform point in [-1,1]^3
      uniform_s2(&p);
      bc(&r,&d); // ball->cube
      cb(&d,&p); // cube->ball
      vec3_sub(&diff, &p, &r);

      float t = vec3_dot(&diff,&diff);

      if (t > e) {
	e = t; vec3_dup(&mp, &p);
      }
    }

    // report
    cb(&d, &mp);
    bc(&r, &d);
    vec3_sub(&diff, &mp, &r);
    
    printf("%8s: %f ", maps[s].name, sqrtf(e));
    vec3_print(&d); vec3_print(&mp); vec3_print(&r); vec3_printa(&diff); ln();
  }
}

// simple round-trip test of pseudo-random points on cube
void uniform_rt_test(uint64_t s0, uint64_t s1)
{
  printf("* uniform round-trip:\n name:    "
	 "RMS       ball                              orig-cube"
	 "                        recon-cube"
	 "                        cube-diff\n");
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    map_func_t cb = maps[s].cb;
    map_func_t bc = maps[s].bc;
    float      e  = 0.f;
    vec3_t     mp;
    vec3_t     p,d,r,diff;

    reset_generators(s0,s1,SYM_TRIALS);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      
      // generate a uniform point in [-1,1]^3
      p.x = 2.f*rng_f32()-1.f;
      p.y = 2.f*rng_f32()-1.f;
      p.z = 2.f*rng_f32()-1.f;

      cb(&d,&p); // cube->ball
      bc(&r,&d); // ball->cube
      vec3_sub(&diff, &p, &r);

      float t = vec3_dot(&diff,&diff);

      if (t > e) {
	e = t; vec3_dup(&mp, &p);
      }
    }

    // report
    cb(&d, &mp);
    bc(&r, &d);
    vec3_sub(&diff, &mp, &r);
    
    printf("%8s: %f ", maps[s].name, sqrt(e));
    vec3_print(&d); vec3_print(&mp); vec3_print(&r); vec3_printa(&diff); ln();
  }
}

#ifdef USE_SOBOL
// simple round-trip test of sobol sequence points on cube
void sobol_rt_test(uint64_t s0, uint64_t s1)
{
  printf("* sobol round-trip:\n name:    "
	 "RMS       ball                              orig-cube"
	 "                        recon-cube"
	 "                        cube-diff\n");
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    map_func_t cb = maps[s].cb;
    map_func_t bc = maps[s].bc;
    float      e  = 0.f;
    vec3_t     mp;
    vec3_t     p,d,r,diff;

    reset_generators(s0,s1,SYM_TRIALS);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      
      // generate point in [-1,1]^3
      sobol_3d_next_f32(&qrng, p.f);
      cb(&d,&p); // cube->ball
      bc(&r,&d); // ball->cube
      vec3_sub(&diff, &p, &r);

      float t = vec3_dot(&diff,&diff);

      if (t > e) {
	e = t; vec3_dup(&mp, &p);
      }
    }

    // report
    cb(&d, &mp);
    bc(&r, &d);
    vec3_sub(&diff, &mp, &r);
    
    printf("%8s: %f ", maps[s].name,  sqrt(e));
    vec3_print(&d); vec3_print(&mp); vec3_print(&r); vec3_printa(&diff); ln();
  }
}
#endif

// Silly little round-trip test and spew out max sum-of-abs-diff as seen.
// Only purpose is to catch bad translation of math to code.

#define C2Bt map_vp_cb
#define B2Ct map_vp_bc
//#define C2Br map_haea_cb
#define XSTR(X) STR(X)
#define STR(X)  #X

void test(uint64_t s0, uint64_t s1)
{
  float max = -1.f;

  printf("* single method (" XSTR(C2Bt) ") round-trip:\n sample#  "
	 "RMS       ball                              orig-cube"
	 "                        recon-cube"
	 "                        cube-diff\n");

  reset_generators(s0,s1,0xFFFFFFF);
  
  for(uint32_t i=0; i<0xFFFFFFF; i++) {
    vec3_t p,d,r,diff;
    
    // generate a uniform point in [-1,1]^3
    p.x = rng_f32();
    p.y = rng_f32();
    p.z = rng_f32();
    
    p.x = 2.f*p.x-1.f;
    p.y = 2.f*p.y-1.f;
    p.z = 2.f*p.z-1.f;
    C2Bt(&d,&p);
    B2Ct(&r,&d);

    vec3_sub(&diff, &p, &r);
    
    float t = vec3_dot(&diff, &diff);
      
    if (t > max) {
      max = t;
      printf("%08x: %f ",i, sqrt(t));
      vec3_print(&d); vec3_print(&p); vec3_print(&r); vec3_printa(&diff); ln();
#if 0
      // dump-out a reference xform for same input
      C2Br(&d,&p);
      B2Cr(&r,&d);
      vec3_sub(&diff, &p, &r);
      printf("%8s:","");
      vec3_print(&d); vec3_print(&p); vec3_print(&r); vec3_printa(&diff); printf("\n");
#endif      
    }
  }
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

  test(s0,s1);
  //uniform_s2_rt_test(s0,s1);
  uniform_rt_test(s0,s1);

#ifdef USE_SOBOL
  sobol_rt_test(s0,s1);
#endif
  
  return 0;
}
