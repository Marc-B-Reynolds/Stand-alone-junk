// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include <math.h>

#define SOBOL_IMPLEMENTATION
#define SOBOL_EXTRAS

#include "../../SFH/quat.h"
#include "../../SFH/Sobol.h"

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

#define EPS F32_MIN_NORMAL

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

// uniform on disc
float uniform_disc(vec2_t* p)
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


void uniform_quat(quat_t* q)
{
  vec2_t p0,p1;
  float  d1 = uniform_disc(&p1) + EPS;
  float  s1 = rsqrt(d1);
  float  d0 = uniform_disc(&p0);  // or positive in 'x' since -Q & Q are equivalent
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
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
static inline float sgn(float x) { return x >= 0.f ? 1.f : -1.f; }
static inline float mulsgn(float x, float v) { return x >= 0.f ? v : -v; }
static inline float rsqrt(float v) { return 1.f/sqrtf(v); }
static inline float recip(float v) { return 1.f/v; }


static inline uint32_t f32_to_bits(float v)      { uint32_t r; memcpy(&r,&v,4); return r; }
static inline float    f32_from_bits(uint32_t v) { float r;    memcpy(&r,&v,4); return r; }


//****** ball/cube/cylinder stuff 

// hacky macros to fill coordinate
#define LOAD_XYZ(S)  float x=S->x, y=S->y, z=S->z;
#define LOAD_XYZ2(S) float x=S->x, y=S->y, z=S->z, x2=x*x, y2=y*y, z2=z*z;

// stretch
void map_rs_cb(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float m = x2 > y2 ? x : y;
  float s = fabsf(m)*rsqrt(x2+y2+z2+EPS);

  vec3_set(D, s*x, s*y, s*z);
}

void map_rs_bc(vec3_t* D, vec3_t* S)
{
  LOAD_XYZ2(S);

  float m = x2 > y2 ? x : y;
  float s = sqrtf(x2+y2+z2)/(fabsf(m)+EPS);

  vec3_set(D, s*x, s*y, s*z);
}

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

//********************************************************

typedef struct {
} quat_quant_t;


#if 0
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
#endif

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

  //test(s0,s1);
  //uniform_s2_rt_test(s0,s1);
  //uniform_rt_test(s0,s1);

#ifdef USE_SOBOL
  //sobol_rt_test(s0,s1);
#endif
  
  return 0;
}
