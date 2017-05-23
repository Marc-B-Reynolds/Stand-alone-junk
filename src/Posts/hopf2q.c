// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io

// ex. requirements:
// * quaternion header
// * Sobol seq header (optional, SEE: USE_SOBOL)

// Testing for branch point selection
// 0) Use the optional sobol sampling is a good idea
// 1) SYM_TRIALS should be highish (say 0x07FFFFFF)
// 2) Probablity of in common case measures:
//    * define UNIFORM_ONLY for assumption of uniform input
// 3) Max error measures:
//    * UNIFORM_ONLY not defined
//    * ONLY_METHOD_0 for common case
//    * ONLY_NOT_METHOD_0 for other case(s)
//    * neither ONLY_* defined for overall

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// config section

// run sobol sequence tests (requires the header)
#define USE_SOBOL

// number of trials to run per test
//#define SYM_TRIALS (0x07FFFFFF<<2)
#define SYM_TRIALS 0x07FFFFFF
//#define SYM_TRIALS 0x00FFFFFF

// if undef repeats same sequence each run
//#define RANDOMIZE

// number of entries in histogram
#define HLEN 100

// histogram to build
// 0: by rotation angle
// 1: by magnitude of z component of x'
// 2: by magnitude of z component of x' on [.99,1]
// ?: expand HIST_FUNC macro
#define HISTO_TYPE 99

// if defined use doubles for Hopf to quaternion conversion
//#define DOUBLE2Q

// should expand to value on [0,1]
// HOPF: degenerate zone is when x^2+y^2 is approaching 1
// (for computation: w^2+z^2 is approaching zero)
#define HIST_FUNC() (s->x*s->x + s->y*s->y)

//#define UNIFORM_ONLY
//#define ONLY_METHOD_0
//#define ONLY_NOT_METHOD_0

// choose between atan or acos based error measuring
//#define ACOS_MEASURE

#define MAX_METHODS 6

// end config

#define XSTR(X) STR(X)
#define STR(X)  #X

#include "../SFH/quat.h"

#if defined(RANDOMIZE)
#include <x86intrin.h>
#endif

#if defined USE_SOBOL
#define SOBOL_EXTRAS
#define SOBOL_IMPLEMENTATION
#include "../SFH/Sobol.h"

sobol_2d_t qrng;
sobol_1d_t srng;
#endif

// external code: xoroshiro128+

uint64_t rng_state[2];

#define ULP1 TO_FP32

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

#if defined(_MSC_VER)
#define TO_FP32 (1.f/16777216.f)
#define PI      3.1415927410125732421875f
#define RPI     0.3183098733425140380859375f
#define SQRT2   1.41421353816986083984375f
#define ISQRT3  0.57735025882720947265625f
#define F32_MIN_NORMAL 1.17549435082228750796873653722224567781866555677209e-38f
#else
#define TO_FP32        0x1p-24f
#define SQRT2          0x1.6a09e6p0f  // sqrt(2)
#define ISQRT3         0x9.3cd3ap-4f  // 1/sqrt(3)
#define PI             0x3.243f6cp0f
#define RPI            0x5.17cc18p-4f
#define F32_MIN_NORMAL 0x1p-126f
#endif

#define EPS F32_MIN_NORMAL

inline float rng_f32(void)
{
  return (rng_u64() >> 40)*TO_FP32;
}

inline float rng_f32_s(void)
{
  return 2.f*rng_f32()-1.f;
}

inline float  sgn(float x)   { return copysignf(1.f,x); }
inline double sgnd(double x) { return copysign(1.0, x); }

double quat_mag(quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;
  return sqrt(x*x+y*y+z*z+w*w);
}


// explict list of quaternions to test

quat_t test_vector_data[] =
{
  {{-.5f, -.5f, -.5f, .5f}},
  {{ 0.f,  .5f*SQRT2,  0.f, .5f*SQRT2}},
  {{ .5f,  .5f,  .5f, .5f}},
  {{0.533773f, -0.463778f,  0.533769f,  0.463774f}},
  {{-.5f,  .5f,  .5f, .5f}}
 
};

uint32_t test_vector_pos;

#define LOAD_Q(Q)  float  x=Q->x, y=Q->y, z=Q->z, w=Q->w;
#define LOAD_QD(Q) double x=Q->x, y=Q->y, z=Q->z, w=Q->w;


void normalize_hack(quat_t* q)
{
  LOAD_QD(q);
  double s = 1.0/sqrt(x*x+y*y+z*z+w*w);
  quat_set(q, (float)(s*x),(float)(s*y),(float)(s*z),(float)(s*w));
}

void test_vector(quat_t* q)
{
  quat_dup(q, test_vector_data + test_vector_pos);
  test_vector_pos++;
}

// reset state data of PRNG & Sobol
void reset_generators(uint64_t s0, uint64_t s1, uint32_t len)
{
#ifdef USE_SOBOL
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0>>32));
  sobol_1d_init(&srng, (uint32_t)s0);
#endif
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
  test_vector_pos = 0;
}

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


float uniform_disc(vec2_t* p)
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

float uniform_hdisc(vec2_t* p)
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

float uniform_qdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x  = (v >> 40)*TO_FP32;
    y  = (v & 0xFFFFFF)*TO_FP32;
    d  = x*x;
    d += y*y;
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

// all positive (for sym)
void uniform_ps2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_qdisc(&v);  
  s = 2.f*sqrtf(1.f-d);
  p->x = s*v.x;
  p->y = s*v.y;
  p->z = 1.f-2.f*d;
}

void uniform_quat(quat_t* q)
{
  vec2_t p0,p1;
  float  d1 = uniform_disc(&p1) + EPS;
  float  s1 = 1.f/sqrtf(d1);
  float  d0 = uniform_hdisc(&p0);
  float  s0 = sqrtf(1.f-d0);
  float  s  = s0*s1;

  quat_set(q, p0.y, s*p1.x, s*p1.y, p0.x);
}

void uniform_quat_from_z(quat_t* q)
{
  vec2_t p;
  float  d = uniform_disc(&p);
  float  s = sqrtf(1.f-d);
  quat_set(q, p.x, p.y, 0.f, s);
}


#define CBIAS (1.f/68719476736.f) // 0x1p-36f

void uniform_s1_px(vec2_t* p)
{
  float d = uniform_hdisc(p);
  float s = 1.f/sqrtf(d+(CBIAS*CBIAS)); 
  p->x += CBIAS;                    
  p->x *= s;
  p->y *= s;
}


void uniform_quat_about_x(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  quat_set(q, p.y, 0.f, 0.f, fabsf(p.x)); // silly...should direct half-circle instead
}

void uniform_quat_about_xy(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  float b = p.y * 0.5f*SQRT2;
  quat_set(q, b, b, 0.f, fabsf(p.x)); // silly...should direct half-circle instead
}

void uniform_quat_about_diag(quat_t* q)
{
  vec2_t p;
  uniform_s1_px(&p);
  float b = p.y * ISQRT3;
  quat_set(q, b,b,b, fabsf(p.x)); // silly...should direct half-circle instead
}

#ifdef USE_SOBOL

void sobol_quat_about_x(quat_t* q)
{
  float b = sinf(PI*(sobol_1d_next_f32(&srng)-0.5f));
  quat_set(q, b, 0.f, 0.f, sqrtf(1.f-b*b));
}

void sobol_quat_about_xy(quat_t* q)
{
  float t = sinf(PI*(sobol_1d_next_f32(&srng)-0.5f));
  float b = t * 0.5f*SQRT2;
  quat_set(q, b, b, 0.f, sqrtf(1.f-t*t));
}

void sobol_quat_about_diag(quat_t* q)
{
  float a = PI*(sobol_1d_next_f32(&srng)-0.5f);
  float t = sinf(a);
  float b = t * ISQRT3;
  float w = sqrtf(1.f-t*t);
  quat_set(q, b,b,b, w);
}

void sobol_quat(quat_t* q)
{
  float p0[2];
  float p1[2];
  float d1 = sobol_uniform_d1(&qrng, p1) + EPS;
  float s1 = 1.f/sqrtf(d1);
  float d0 = sobol_uniform_hd1(&qrng, p0);
  float s0 = sqrtf(1.f-d0);
  float s  = s0*s1;

  quat_set(q, p0[1], s*p1[0], s*p1[1], p0[0]);
}


#endif


// H in binary64 for error measures
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

// returned angle is in degrees and as R^3 and not H.
// use doubles to compute relative quaternion BA*,
// and atan2 to determine angle between A & B. This
// measure isn't effected by errors in magnitude..
// better or worse than acos measure..it depends.
// (well, really you can choose which measure)
float a_error(quat_t* a, quat_t* b)
{
#if !defined(ACOS_MEASURE)  
  quatd_t R,A,B;
  quat_to_d(&A,a);
  quat_to_d(&B,b);
  quatd_conj(&A);
  quatd_mul(&R,&B,&A);

  double x = fabs(R.w);
  double y = sqrt(R.x*R.x+R.y*R.y+R.z*R.z);
  double t = atan2(y,x);
  float  r = (float)(1.1459156036376953125e2*t);

  if (r <= 180.f) return r;

  return 180.f-r;
#else
  double ax=a->x, ay=a->y, az=a->z, aw=a->w;
  double bx=b->x, by=b->y, bz=a->z, bw=b->w;
  double d = fabs(ax*bx + ay*by + az*bz + aw*bw);
  double t = d <= 1.0 ? acos(d) : 0;
  return 1.1459156036376953125e2*t;
#endif  
}

double m_error(quat_t* a)
{
  double ax=a->x, ay=a->y, az=a->z, aw=a->w;
  return sqrt(ax*ax+ay*ay+az*az+aw*aw);
}

uint32_t quat_to_hopf_d(vec3_t* v, quat_t* q)
{
  double x=q->x, y=q->y, z=q->z, w=q->w;
  uint32_t r = 0;

  double gamma = 2.0*atan2(z, w);
  double alpha = 2.0*atan2(sqrt(x*x+y*y), sqrt(w*w+z*z));
  double beta  = atan2(y*z-w*x, w*y+x*z);
  beta = 0.5*gamma-atan2(x,y);

  v->x = (float)alpha;
  v->y = (float)beta;
  v->z = (float)gamma;

  return r;
}

uint32_t quat_to_hopf(vec3_t* v, quat_t* q)
{
  float x=q->x, y=q->y, z=q->z, w=q->w;
  uint32_t r = 0;

  float gamma = 2.f*atan2f(z, w);
  float alpha = 2.f*atan2f(sqrtf(x*x+y*y), sqrtf(w*w+z*z));
  float beta  = atan2f(y*z-w*x, w*y+x*z);

  v->x = alpha;
  v->y = beta;
  v->z = gamma;

  return r;
}


// pointer to quaternion generator
typedef void (*qgen_t)(quat_t*);

// pointer to quaternion to ZYX method
typedef uint32_t (*to_zyx_t)(vec3_t*, quat_t*);

typedef struct {
  qgen_t   f;
  uint32_t t;
  char*    name;
} gens_t;

#define DEF(S)    {&S, SYM_TRIALS, XSTR(S)}
#define DEFX(S,T) {&S, T, XSTR(S) }

gens_t gens[] =
{
#if !defined(UNIFORM_ONLY)  
  DEFX(test_vector, sizeof(test_vector_data)/sizeof(quat_t)),

#if defined(USE_SOBOL)  
  DEF(sobol_quat_about_diag),
  //DEF(sobol_quat_about_x),
  //DEF(sobol_quat_about_xy),
#endif  

  DEF(uniform_quat_about_diag),
  DEF(uniform_quat_from_z),
  //DEF(uniform_quat_about_x),
  //DEF(uniform_quat_about_xy),
#else  
#if defined(USE_SOBOL)  
  DEF(sobol_quat),
#endif
#endif  
  DEF(uniform_quat)
};

#undef  DEF
#undef  DEFX
#define NUM_GENS (sizeof(gens)/sizeof(gens[0]))


typedef struct {
  to_zyx_t f;
  to_zyx_t fd;
  char*    name;
} maps_t;

#define DEF(S)    {&S, (void*)0, XSTR(S)}
#define DEFX(S,D) {&S, &D,       XSTR(S D)}

// defines a table of quat->XYZ conversions, DEF for single method DEFX to
// additionally report the error of the second method.
maps_t maps[] =
{
 //DEF(quat_to_hopf_d),
   DEF(quat_to_hopf)
};

#undef DEF
#undef DEFX
#define NUM_FUNCS (sizeof(maps)/sizeof(maps[0]))

#if !defined(DOUBLE2Q)
void hopf_to_quat(quat_t* q, vec3_t* v)
{
  float ha    = 0.5f*v->x;
  float b     = v->y;
  float hg    = 0.5f*v->z;
  float sinHa = sinf(ha);
  float cosHa = cosf(ha);
  q->x = sinHa*sinf(hg-b);
  q->y = sinHa*cosf(hg-b);
  q->z = cosHa*sinf(hg);
  q->w = cosHa*cosf(hg);

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}
#else
void hopf_to_quat(quat_t* q, vec3_t* v)
{
  double ha    = 0.5*v->x;
  double b     = v->y;
  double hg    = 0.5*v->z;
  double sinHa = sin(ha);
  double cosHa = cos(ha);
  q->x = sinHa*sin(hg-b);
  q->y = sinHa*cos(hg-b);
  q->z = cosHa*sin(hg);
  q->w = cosHa*cos(hg);

  // below here temp. hacks for error measures
  if (q->w >= 0.f) return;
  quat_neg(q,q);
}
#endif


typedef struct {
  quat_t s;      // untransformed src
  double max;    // max error in bin
} e_histo_t;


typedef struct {
  e_histo_t a[HLEN];
} a_error_t;

a_error_t a_errors[NUM_FUNCS];

void error_init()
{
  memset(a_errors, 0, sizeof(a_errors));
}

double error_add(quat_t* s, quat_t* r, a_error_t* e)
{
  double   ae  = a_error(s,r);
  double   t   = fabs(ae);
  uint32_t hid;

  // hiso

#if !defined(HISTO_TYPE) || (HISTO_TYPE == 0)  
  hid = (uint32_t)(((2.f*HLEN)/PI)*acosf(s->w));
#elif (HISTO_TYPE == 1)
#if !defined(ZXZ)  
  hid = (uint32_t)((HLEN)*(2.f*fabsf(s->x*s->z-s->w*s->y)));
#else
  hid = (uint32_t)((HLEN)*(2.0*fabsf(1.0-2.0*(s->x*s->x+s->z*s->z))));
#endif  
#elif (HISTO_TYPE == 2)
  {
    float t = 2.f*fabsf(s->x*s->z-s->w*s->y);
    t = t > .99f ? (t-.99f)/.01f : 0.f;
    hid = (uint32_t)((HLEN)*t);
  }
#else
  hid = (uint32_t)(HLEN*HIST_FUNC());
#endif  

  if (hid >= HLEN) hid=HLEN-1;

  e_histo_t* h = &e->a[hid]; 
  
  if (ae > h->max) {
    h->max = ae;
    quat_dup(&h->s, s);
  }

  return t;
}

void dump_xform(quat_t* q)
{
  vec3_t x,y,z;
  quat_to_local(&x,&y,&z,q);
  printf("  x':"); vec3_print(&x); vec3_printa(&x); ln(); 
  printf("  y':"); vec3_print(&y); vec3_printa(&y); ln(); 
  printf("  z':"); vec3_print(&z); vec3_printa(&z); ln(); 
}

// dump out simple error reports per tested method
void error_dump()
{
  uint32_t s;

  for(s=0; s<NUM_FUNCS; s++) {
    a_error_t* e  = &a_errors[s];
    double     m  = 0;
    uint32_t   mi = 0;

    printf("%8s:\n", maps[s].name);

    printf("max: "); for(uint32_t i=0; i<HLEN; i++) { printf("%f, ", e->a[i].max); if (e->a[i].max > m) { m = e->a[i].max; mi = i; } } ln();

    // dump out global max error info
    to_zyx_t f  = maps[s].f;
    quat_t*  q  = &e->a[mi].s;
    quat_t   r;
    vec3_t   v;

    f(&v, q);
    hopf_to_quat(&r, &v);

    printf("global max: %f, mag-error: %e ",m, fabs(1.0-m_error(q)));
    
    printf("\n  orig   alpha/beta/gamma  recon\n");
    quat_print(q); vec3_print(&v); quat_print(&r);
    printf("\n  ");
    quat_printa(q); vec3_printa(&v); quat_printa(&r); ln();

    printf("orig: %e\n", 1.0-quat_mag(q));  dump_xform(q);
    printf("recon:\n"); dump_xform(&r);
    ln();
  }
}


uint64_t method_count[MAX_METHODS];

void rt_test(uint64_t s0, uint64_t s1)
{
  // walk through all the conversion functions
  for(uint32_t ci=0; ci<NUM_FUNCS; ci++) {
    to_zyx_t   f  = maps[ci].f;
    a_error_t* et = &a_errors[ci];

    printf("%s\n", maps[ci].name);
    memset(method_count, 0, sizeof(method_count));

    for(uint32_t gi=0; gi<NUM_GENS; gi++) {
      qgen_t   gen = gens[gi].f;
      uint32_t len = gens[gi].t;
      quat_t   q,r;
      vec3_t   hopf;
      uint32_t s;
      double   e,me = 0;
      
      reset_generators(s0,s1,len);
      printf("  %s\n", gens[gi].name);
      
      // generate 'len' test values
      for(uint32_t i=0; i<len; i++) {
	gen(&q);                  // generate test quaternion (Q)
	s = f(&hopf, &q);         // use method to convert to Hopf coordinate
	e = 0;
	method_count[s]++;        // track prob

#if defined(ONLY_METHOD_0)	
        if (s == 0)
#elif defined(ONLY_NOT_METHOD_0)	
        if (s != 0)
#endif	
	{
	  hopf_to_quat(&r, &hopf);  // round-trip back to quaternion (Q')
	  e = error_add(&q,&r,et);  // measure and track the error
	}
	
	// spew out peak error per method as seen
	if (e > me) {
  	  me = e;
	  if (maps[ci].fd != (void*)0) {
	    // same computation by second method if specified
	    vec3_t vd;
	    quat_t rd;
	    uint32_t s2 = maps[ci].fd(&vd, &q);
	    hopf_to_quat(&rd, &vd);
	    double ed = a_error(&q, &rd);

	    printf("%08x (%d/%d): ", i,s,s2);
	    
	    if (me > ed) {
	      vec3_t vdiff;
	      vec3_sub(&vdiff, &vd, &hopf);
	      printf("!");

	      if (me > 0.1) {
		vec3_print(&vd); vec3_print(&vdiff);
		printf("\n                ");
	      }
	    }
	    else {
	      if (me < ed)
		printf("^");
	      else
		printf(" ");
	    }
	    
	    printf(" %f %f ", me,ed);
	  }
	  else {
	    printf("%08x (%d): %f ", i,s, me);
	  }
	  
	  quat_print(&q);
	  vec3_print(&hopf);
	  quat_print(&r);

	  vec3_t xp;
	  quat_local_x(&xp,&q);
	  vec3_print(&xp);
	  printf("\n");
	}
      }
    }

    for(uint32_t i=0; i<MAX_METHODS; i++) {
      printf("%d ", (uint32_t)method_count[i]);
    }
    
    ln();
  }
}

// a simple round-trip test for debugging dev of mapping function
#define TLEN 0x7FFFFFF

void rt_xform_test(uint64_t s0, uint64_t s1)
{
  double me = 0.0;
  quat_t p,r,diff;
  vec3_t v;
  float xmin= 1.f,ymin= 1.f,zmin= 1.f;
  float xmax=-1.f,ymax=-1.f,zmax=-1.f;
  
  reset_generators(s0,s1,TLEN);

  for(uint32_t i=0; i<TLEN; i++) {
    //uniform_quat_about_diag(&p);
    uniform_quat(&p);
    p.w = fabsf(p.w);

    // test generalize or explict
    uint32_t s = quat_to_hopf(&v,&p);
    hopf_to_quat(&r,&v);

    if (v.x > xmax) { xmax = v.x; } else if (v.x < xmin) { xmin = v.x; }
    if (v.y > ymax) { ymax = v.y; } else if (v.y < ymin) { ymin = v.y; }
    if (v.z > zmax) { zmax = v.z; } else if (v.z < zmin) { zmin = v.z; }
    quat_sub(&diff, &p, &r);

    double e = a_error(&p,&r);
    
    if (e > me || e > 1.f) {
      vec3_t vd;
      quat_t rd;
      s = quat_to_hopf(&vd,&p);
      hopf_to_quat(&rd,&vd);
      double ed = a_error(&p, &rd);
      
      me = e;
      printf("%08x: %d %f %f",i,s, me,ed);
      printf(" -- % a -- ", 0.5f-(p.x*p.z-p.y*p.w));
      quat_print(&p);
      vec3_print(&v);
      quat_print(&r);
      quat_printa(&diff);
      printf("\n");
    }
  }

  printf("x:(%f,%f) y:(%f,%f) z:(%f,%f)\n", xmin,xmax,ymin,ymax,zmin,zmax);
}

int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;

  error_init();

#ifdef RANDOMIZE
  s0 = _rdtsc();
#else  
  s0 = 0x77535345;
#endif  
  s1 = 0x1234567;

  reset_generators(s0,s1,SYM_TRIALS);
    
  rt_test(s0,s1);
  error_dump();
  ln();

  return 0;
}
