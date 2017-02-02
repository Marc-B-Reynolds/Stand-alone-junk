// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io

// This performs sloppy empirical testing. Specificially:
// 1) finding max error and RMS is dot based rather than atan2
// 2) ignores the known density properties of xforms and is
//    pseudo+quasi random sampling.
// 3) does not hone in to get a tighter error bound


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
#define RANDOMIZE

// if def compute RMS error
#define RMS

// number of quant bits per component
#define BITS 9

// end config

#define XSTR(X) STR(X)
#define STR(X)  #X

#include "../SFH/quat.h"

#if defined USE_SOBOL
#define SOBOL_EXTRAS
#define SOBOL_IMPLEMENTATION
#include "../SFH/Sobol.h"

sobol_2d_t qrng;
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

#define EPS (F32_MIN_NORMAL)

//static inline float rng_f32(void)
//{
//  return (rng_u64() >> 40)*TO_FP32;
//}

// for quickly/first-pass hacking in arch specific junk
// !!!!!! the recip hurts performance w current gcc/clang..
//        actually does the 1/x and product...sigh.
static inline float sgn(float x) { return x >= 0.f ? 1.f : -1.f; }
static inline float mulsgn(float x, float v) { return x >= 0.f ? v : -v; }
static inline float rsqrt(float v) { return 1.f/sqrtf(v); }
static inline float recip(float v) { return 1.f/v; }

void reset_generators(uint64_t s0, uint64_t s1, uint32_t len)
{
#ifdef USE_SOBOL
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0>>32));
#endif
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}


//static inline uint32_t f32_to_bits(float v)      { uint32_t r; memcpy(&r,&v,4); return r; }
//static inline float    f32_from_bits(uint32_t v) { float r;    memcpy(&r,&v,4); return r; }

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

float uniform_hdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*0x1p-24f;
    y = (v & 0xFFFFFF)*0x1p-24f;
    d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

void uniform_s2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_disc(&v);  
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
  float  d0 = uniform_hdisc(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
}

#ifdef USE_SOBOL
void sobol_quat(quat_t* q)
{
  float  p0[2];
  float  p1[2];
  float  d1 = sobol_uniform_d1(&qrng, p1) + EPS;
  float  s1 = rsqrt(d1);
  float  d0 = sobol_uniform_hd1(&qrng, p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0[1], s*p1[0], s*p1[1], p0[0]);
}
#endif

void vec3_printa(vec3_t* v)
{
  printf("{% a, % a, % a} ",v->x,v->y,v->z);
}

void vec3_print(vec3_t* v)
{
  printf("{% f, % f, % f} ",v->x,v->y,v->z);
}

void quat_printa(quat_t* v)
{
  printf("{% a, % a, % a, % a} ",v->x,v->y,v->z,v->w);
}

void quat_print(quat_t* v)
{
  printf("{% f, % f, % f, % f} ",v->x,v->y,v->z,v->w);
}

void ln()
{
  printf("\n");
}

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

static inline void quatd_mul(quatd_t* r, quatd_t* a, quatd_t* b)
{
  double x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;  
  double y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
  double z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
  double w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
  quatd_set(r,x,y,z,w);
}

float a_error(quat_t* a, quat_t* b)
{
  quatd_t R,A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  quatd_conj(&A);
  quatd_mul(&R,&B,&A);

  double x = R.w;
  double y = sqrt(R.x*R.x+R.y*R.y+R.z*R.z);
  double t = atan2(y,x);

  return (float)(1.1459156036376953125e2*t);
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

void map_id_bc(vec3_t* D, vec3_t* S) {vec3_dup(D,S);}
void map_id_cb(vec3_t* D, vec3_t* S) {vec3_dup(D,S);}

//********************************************************

// hacky macro 
#define LOAD_Q(Q)  float x=Q->x, y=Q->y, z=Q->z, w=Q->w;

// half-angle (scaled to unit ball)
void fha(vec3_t* v, quat_t* q)
{
  float d = 1.f + q->w;
  float s = QUAT_RSQRT(d);
  quat_put_bv(v,q,s);
}

void iha(quat_t* q, vec3_t* v)
{
  float d = vec3_norm(v);
  float s = QUAT_SQRT(2.f-d); 
  quat_bv_set_scale(q,v,s);
  q->w = 1.f-d;
}

// Cayley transform
void fct(vec3_t* v, quat_t* q)
{
  float s = 1.f/(1.f+q->w);
  quat_put_bv(v,q,s);
}

void ict(quat_t* q, vec3_t* v)
{
  float s = 2.f/(1.f+vec3_norm(v));
  quat_bv_set_scale(q,v,s);
  q->w = s-1.f;
}

#define Km 0.4142135679721832275390625f // sqrt(2)-1
#define Kp 2.414213657379150390625f     // sqrt(2)+1 = 1/(sqrt(2)-1)

// harmonic mean (scaled to unit ball)
void fhm(vec3_t* v, quat_t* q)
{
#if 0  
  quat_t t;
  quat_dup(&t,q);
  quat_usqrt(&t);
  quat_fct(v,&t);
  vec3_scale(v, 1.f/Km);
#else
  float w = q->w;
  float s = Kp/(1.f+w+sqrtf(2+w+w));
  quat_put_bv(v,q,s);
#endif  
}

#define Khm 0.17157287895679473876953125f // 3-2sqrt(2)

void ihm(quat_t* q, vec3_t* v)
{
#if 0
  vec3_t t;
  vec3_set_scale(&t, v, Km);
  quat_ict(q, &t);
  quat_upow2(q);
#else
  float d = Khm*vec3_norm(v);
  float a = (1.f+d);         
  float b = (1.f-d)*(Km*4.f);         
  float c = 1.f/(a*a);     

  quat_bv_set_scale(q, v, b*c);
  q->w = (1.f+d*(d-6.f))*c;
#endif
}


// log(q) & exp(b) - scaled
#define Kem (PI/2.f)

void fem(vec3_t* v, quat_t* q)
{
  float w = q->w+EPS;
  float k = sqrtf(1.f-w*w)+EPS;
  float s = (2.f/PI)*atanf(k/w)/k;
  quat_put_bv(v,q,s);
}

void iem(quat_t* q, vec3_t* v)
{
  float a = Kem*sqrtf(vec3_norm(v));
  float s = Kem*sinf(a);
  float k = s/(a+EPS);
  quat_bv_set_scale(q,v,k);
  q->w = cosf(a);
}

// unit quaternion to ball and inverse
typedef void (*s3_to_b3)(vec3_t*, quat_t*);
typedef void (*b3_to_s3)(quat_t*, vec3_t*);

// maps between cube and ball (both directions)
typedef void (*cb_map)(vec3_t*, vec3_t*);

typedef struct {
  s3_to_b3 f;   //
  b3_to_s3 g;   //
  cb_map   c;   //
  cb_map   s;   //
  char*    name;
} maps_t;



// define maps
typedef void (*map_func_t)(vec3_t*, vec3_t*);

#define DEF(S,X) {&f ## S, &i ## S, &map_ ## X ## _bc, &map_ ## X ## _cb, XSTR(S##X)}

maps_t maps[] =
{
  DEF(hm,id),
  DEF(hm,vp),
  DEF(hm,rs),
  DEF(hm,aev),
  
  DEF(ha,id),
  DEF(ha,rs),
  DEF(ha,vp),
  DEF(ha,aev),
  
  DEF(ct,id),
  DEF(ct,vp),
  DEF(ct,rs),
  DEF(ct,aev),
  
  DEF(em,id),
  DEF(em,vp),
  DEF(em,rs),
  DEF(em,aev)
};

#define NUM_FUNCS (sizeof(maps)/sizeof(maps[0]))

#undef DEF

#define ULP2 0x1p-23f
#define QSCALE_(X) ((float)(1<<(X)))
#define QSCALE(X)   (QSCALE_(X)-(QSCALE_(X)*ULP2))
#define DSCALE(X)   (1.f/QSCALE_(X))

#define BITS_T (1<<BITS)
#define BITS_M (BITS_T-1)

// simple truncate
uint32_t quant(float f, float s) { uint32_t i = (uint32_t)(f*s); return i; }
float    dequant(uint32_t b, float s) { return (b+0.5f)*s; }

void vdecode(vec3_t* v, uint32_t b)
{
  v->x = 2.f*dequant(b & BITS_M, DSCALE(BITS))-1.f; b >>= BITS;
  v->y = 2.f*dequant(b & BITS_M, DSCALE(BITS))-1.f; b >>= BITS;
  v->z = 2.f*dequant(b & BITS_M, DSCALE(BITS))-1.f;
}

uint32_t vencode(vec3_t* v)
{
  uint32_t b;
  b  = quant(0.5f*v->x+0.5f, QSCALE(BITS)); 
  b |= quant(0.5f*v->y+0.5f, QSCALE(BITS)) <<   BITS; 
  b |= quant(0.5f*v->z+0.5f, QSCALE(BITS)) << 2*BITS;
  return b;
}

// smallest-of-three
uint32_t sotf(quat_t* q)
{
  float    aw = fabs(q->w);
  float    ax = fabs(q->x);
  float    ay = fabs(q->y);
  float    az = fabs(q->z);
  float    m  = aw;
  uint32_t id = 3;
  uint32_t r;
  vec3_t   t;

  if (ax > m) { id=0; m=ax; }
  if (ay > m) { id=1; m=ay; }
  if (az > m) { id=2; }

  r = id << 30;

  float s = sgn(q->f[id]);

  id = (id+1)&3; t.x = s*q->f[id];
  id = (id+1)&3; t.y = s*q->f[id];
  id = (id+1)&3; t.z = s*q->f[id];

  return r|vencode(&t);
}

void soti(quat_t* q, uint32_t b)
{
  uint32_t id = b >> 30;
  vec3_t v;
  
  vdecode(&v, b);

  q->f[id] = sqrtf(1.f-vec3_norm(&v));
  id = (id+1)&3; q->f[id] = v.x; 
  id = (id+1)&3; q->f[id] = v.y; 
  id = (id+1)&3; q->f[id] = v.z; 
}

void sobol_sot_test(uint64_t s0, uint64_t s1)
{
  quat_t q,r,mq;
  uint32_t bits;
  float t;
  float e = 2.f;
#if defined RMS
  double     rms = 0.0;
#endif

  reset_generators(s0,s1,SYM_TRIALS);
  quat_set(&mq,0,0,0,1);
  
  for(uint32_t i=0; i<SYM_TRIALS; i++) {
    sobol_quat(&q);
    bits = sotf(&q);
    soti(&r, bits);
    t = fabs(quat_dot(&q, &r));
    
#if defined RMS
    // sloppy measure
    if (t < 1.f) {
      double ae = 114.59156036376953125f*acos(t);
      rms += ae*ae;
    }
#endif      
    
    if (t < e) {
      e = t;
      quat_dup(&mq, &q);
#if 0      
      t = 114.59156036376953125f*acos(t);
      printf("          %f %s", t, "                                         "); 
      quat_print(&q); quat_print(&r); ln();
#endif      
    }
  }
  
  printf("%8s: ", "sot");
  quat_dup(&q,&mq);
  
  bits = sotf(&q);
  soti(&r, bits);

  if (sgn(r.w) != sgn(q.w))
    quat_neg(&r,&r);
  
  t = a_error(&q, &r);
  
  printf("% 2.4f ", t);
  
#if defined RMS
  printf("% 2.4f ", sqrt(rms/(1.0*SYM_TRIALS)));
#endif      
  printf("                                  ");
  quat_print(&q); quat_print(&r); ln();
}

void uniform_sot_test(uint64_t s0, uint64_t s1)
{
  quat_t q,r,mq;
  uint32_t bits;
  float t;
  float e = 2.f;
#if defined RMS
  double     rms = 0.0;
#endif

  reset_generators(s0,s1,SYM_TRIALS);
  quat_set(&mq,0,0,0,1);
  
  for(uint32_t i=0; i<SYM_TRIALS; i++) {
    uniform_quat(&q);
    bits = sotf(&q);
    soti(&r, bits);
    t = fabs(quat_dot(&q, &r));
    
#if defined RMS
    // sloppy measure
    if (t < 1.f) {
      double ae = 114.59156036376953125f*acos(t);
      rms += ae*ae;
    }
#endif      
    
    if (t < e) {
      e = t;
      quat_dup(&mq, &q);
#if 0      
      t = 114.59156036376953125f*acos(t);
      printf("          %f %s", t, "                                         "); 
      quat_print(&q); quat_print(&r); ln();
#endif      
    }
  }
  
  printf("%8s: ", "sot");
  quat_dup(&q,&mq);
  
  bits = sotf(&q);
  soti(&r, bits);

  if (sgn(r.w) != sgn(q.w))
    quat_neg(&r,&r);
  
  t = a_error(&q, &r);
  
  printf("% 2.4f ", t);
  
#if defined RMS
  printf("% 2.4f ", sqrt(rms/(1.0*SYM_TRIALS)));
#endif      
  printf("                                  ");
  quat_print(&q); quat_print(&r); ln();
}


// pseudo random sampling
void uniform_rt_test(uint64_t s0, uint64_t s1)
{
  printf("PRNG round trip test\n");
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    s3_to_b3   f  = maps[s].f;
    b3_to_s3   g  = maps[s].g;
    map_func_t bc = maps[s].c;
    map_func_t cb = maps[s].s;
    float      e  = 2.f;
    float      t;
    quat_t     q,r,mq;
    vec3_t     b,rb;
    vec3_t     c,rc;
    uint32_t   bits;
#if defined RMS
    double     rms = 0.0;
#endif      

    reset_generators(s0,s1,SYM_TRIALS);
    quat_set(&mq,0,0,0,1);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      // test quaternion
      uniform_quat(&q);

      // encode
      f(&b, &q);  
      bc(&c, &b);
      bits = vencode(&c);

      // decode
      vdecode(&rc, bits);
      cb(&rb, &rc);
      g(&r, &rb);
      
      t = quat_dot(&q, &r);

#if defined RMS
      // sloppy measure
      if (t < 1.f) {
	double ae = 114.59156036376953125f*acos(t);
	rms += ae*ae;
      }
#endif      

      if (t < e) {
	e = t;
	quat_dup(&mq, &q);
	//t = 114.59156036376953125f*acos(t);
	//printf("          %f ",t); vec3_print(&b); quat_print(&q); quat_print(&r); ln();
      }
    }

    printf("%8s: ", maps[s].name);
    quat_dup(&q,&mq);
    
    // encode
    f(&b, &q);  
    bc(&c, &b);
    bits = vencode(&c);
    
    // decode
    vdecode(&rc, bits);
    cb(&rb, &rc);
    g(&r, &rb);
    
    t = a_error(&q, &r);
    
    printf("% 2.4f ", t);

    #if defined RMS
    printf("% 2.4f ", sqrt(rms/(1.0*SYM_TRIALS)));
    #endif      


    vec3_print(&b); quat_print(&q); quat_print(&r); ln();
  }
}

// sobol sampling
void sobol_rt_test(uint64_t s0, uint64_t s1)
{
  printf("Sobol round trip test\n");
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    s3_to_b3   f  = maps[s].f;
    b3_to_s3   g  = maps[s].g;
    map_func_t bc = maps[s].c;
    map_func_t cb = maps[s].s;
    float      e  = 2.f;
    float      t;
    quat_t     q,r,mq;
    vec3_t     b,rb;
    vec3_t     c,rc;
    uint32_t   bits;
#if defined RMS
    double     rms = 0.0;
#endif      

    reset_generators(s0,s1,SYM_TRIALS);
    quat_set(&mq,0,0,0,1);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      // test quaternion
      sobol_quat(&q);

      // encode
      f(&b, &q);  
      bc(&c, &b);
      bits = vencode(&c);

      // decode
      vdecode(&rc, bits);
      cb(&rb, &rc);
      g(&r, &rb);
      
      t = quat_dot(&q, &r);

#if defined RMS
      // sloppy measure
      if (t < 1.f) {
	double ae = 114.59156036376953125f*acos(t);
	rms += ae*ae;
      }
#endif      

      if (t < e) {
	e = t;
	quat_dup(&mq, &q);
	//t = 114.59156036376953125f*acos(t);
	//printf("          %f ",t); vec3_print(&b); quat_print(&q); quat_print(&r); ln();
      }
    }

    printf("%8s: ", maps[s].name);
    quat_dup(&q,&mq);
    
    // encode
    f(&b, &q);  
    bc(&c, &b);
    bits = vencode(&c);
    
    // decode
    vdecode(&rc, bits);
    cb(&rb, &rc);
    g(&r, &rb);
    
    t = a_error(&q, &r);
    
    printf("% 2.4f ", t);

    #if defined RMS
    printf("% 2.4f ", sqrt(rms/(1.0*SYM_TRIALS)));
    #endif      


    vec3_print(&b); quat_print(&q); quat_print(&r); ln();
  }
}


// a simple round-trip test for debugging inital mapping function
//   half-angle, exp, etc.
void rt_xform_test(uint64_t s0, uint64_t s1)
{
  float max = -1.f;
  quat_t p,r,diff;
  vec3_t v;
  float  t;

  reset_generators(s0,s1,0xFFFFFFF);

  for(uint32_t i=0; i<0xFFFFFFF; i++) {

    uniform_quat(&p);
    p.w = fabs(p.w);
    fhm(&v,&p);
    ihm(&r,&v);

    quat_sub(&diff, &p, &r);

    t = quat_norm(&diff);
      
    if (t > max) {
      max = t;
      printf("%08x: %f % a ",i, sqrtf(vec3_norm(&v)), sqrtf(t));
      vec3_print(&v); quat_print(&p); quat_print(&r); quat_printa(&diff);
      printf("\n");
    }
  }
}


// minimal test round-trip quant/dequant bit patterns
void dq_rt()
{
  printf("dq_rt:\n");
  for(uint32_t i=0; i<(1<<(3*BITS)); i++) {
    vec3_t   v;
    uint32_t r;

    vdecode(&v,i);
    r = vencode(&v);

    if (i != r)
      printf("%08x %08x\n", i,r);
  }
  printf("  done\n");
}


void foo()
{
  vec3_t v;

  for(uint32_t z=0; z<(1<<(BITS-1)); z++) {
    for(uint32_t y=0; y<(1<<(BITS-1)); y++) {
      for(uint32_t x=0; x<(1<<(BITS-1)); x++) {
	uint32_t b = (z<<(2*BITS))|(y<<(BITS))|x;
	
	// positive/negative per comp
	b |= (1<<(  BITS-1));
	b |= (1<<(2*BITS-1));
	b |= (1<<(3*BITS-1));
	vdecode(&v,b);

	if (vec3_norm(&v) <= 1.f) {
	  printf("%08x ", b); vec3_print(&v); ln();
	}
      }
    }
  }
}



int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;

  //dq_rt();
  //foo(); return 0;

#ifdef RANDOMIZE
  s0 = _rdtsc();
#else  
  s0 = 0x77535345;
#endif  
  s1 = 0x1234567;
  reset_generators(s0,s1,SYM_TRIALS);
  
  printf("bits/dim: " XSTR(BITS) "\n");
  
  rt_xform_test(s0,s1);

#ifdef USE_SOBOL
  sobol_rt_test(s0,s1);
  sobol_sot_test(s0,s1);
#endif
  uniform_rt_test(s0,s1);
  uniform_sot_test(s0,s1);

  
  return 0;
}
