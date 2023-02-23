
// Toy code for:
// http://marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
//
// The error measurements are somewhat lacking.
//
// The noted number of issues is for x64 from gcc 6.3 -O3

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

#define HISTOGRAM
#define HISTOGRAM_DETAILED

// enable to test with both pseudo-random and Sobol sequences
#define USE_SOBOL

//#define TRIALS 0x7FFFFFFF
#define TRIALS 0x3FFFFFFF
//#define TRIALS 0xFFFFFF
#define HLEN 100

//#define HALF_SPHERE

#include "../SFH/quat.h"

#ifdef  USE_SOBOL
#define SOBOL_IMPLEMENTATION
#define SOBOL_EXTRAS
#include "../SFH/Sobol.h"

sobol_2d_t qrng;

#endif

#if defined(_MSC_VER)
#define TWO_OVER_PI_D 0.6366197723675813824328884038550313562154769897461
#define R2D_D 57.29577951308232286464772187173366546630859375
#define ULP1  5.9604644775390625e-8f
#else
#define TWO_OVER_PI_D 0x1.45f306dc9c883p-1
#define R2D_D 0x1.ca5dc1a63c1f8p5
#define ULP1  0x1p-24f
#endif

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
  return (rng_u64() >> 40)*ULP1;
}

static inline float sgn(float x) { return copysignf(1.f,x); }

// uniform on half disk (x>=0)
float uniform_hdisk(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*ULP1;
    y = (v & 0xFFFFFF)*ULP1;
    d = x*x;
    y = 2.f*x-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

// uniform on disk
float uniform_disk(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*ULP1;
    y = (v & 0xFFFFFF)*ULP1;
    x = 2.f*x-1.f; d  = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

// uniform on S2: half sphere (z >= 0)
void uniform_hs2(vec3_t* p)
{
  float d,s;
  vec2_t v;

  d = uniform_hdisk(&v);  
  s = 2.f*sqrtf(1.f-d);
  p->x = 1.f-2.f*d;
  p->y = s*v.y;
  p->z = s*v.x;
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
  printf("(% f,% f,% f) ",v->x,v->y,v->z);
}

void vec3_printa(vec3_t* v)
{
  printf("(%+a,%+a,%+a) ",v->x,v->y,v->z);
}

typedef union {
  struct {
    float m00,m01,m02;
    float m10,m11,m12;
    float m20,m21,m22;
  };
  vec3_t row[3];
  float  f[3*3];
} m33_t;

void m33_print(m33_t* m)
{
  vec3_print(m->row  ); ln();
  vec3_print(m->row+1); ln();
  vec3_print(m->row+2); ln();
}

// no degenerate case handling: issues 28
void ortho_frisvad(vec3_t* n, vec3_t* b1, vec3_t* b2)
{
  float a = 1.f / (1.f + n->z);
  float b = -n->x*n->y*a;
  vec3_set(b1, 1.f - n->x*n->x*a, b, -n->x);
  vec3_set(b2, b, 1.f - n->y*n->y*a, -n->y);
}


// reworked (rot 180 degree in orthogonal complement)
void ortho_frisvad_m1(vec3_t* n, vec3_t* b1, vec3_t* b2)
{
  float x = n->x, y = n->y, z = n->z;
  float a = 1.f/(1.f+z);
  float b = a*x*y;
  vec3_set(b1, a*x*x-1.f, b, x);
  vec3_set(b2, b, a*y*y-1.f, y);
}

// updated version issues: 21 (rot 180 degree in orthogonal complement)
void ortho_basis_1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x = v->x, y = v->y, z = v->z;
  float a = y/(z+1.f);
  float b = y*a;
  float c = x*a;
  
  vec3_set(xp, -z-b, c,  x);
  vec3_set(yp, c, b-1.f, y);
}

// original post version issues: 23
void ortho_basis_1o(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float z =  v->z;
  float x = -v->x;
  float y =  v->y;
  float a = y/(z+1.f); //   y/(z+1)
  float b = y*a;       // y^2/(z+1)
  float c = x*a;       // -xy/(z+1)
  
  vec3_set(xp, z+b, c,      x);  // {z+y/(z+1),   -xy/(z+1), -x}
  vec3_set(yp, c,   1.f-b, -y);  // {-xy/(z+1), 1-y^2/(z+1), -y}
}

// issues: 28
void ortho_basis_2(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = -sgn(z);  // -sgn(z) = -sz
  float az = fabsf(z);
  float a  = y/(az+1.f); //   y/(|z|+1)
  float b  = y*a;            // y^2/(|z|+1)
  float c  = -x*a;           // -xy/(|z|+1)
  
  vec3_set(xp, az+b, c,    sz*x); // {|z|+y/(|z|+1),   -xy/(|z|+1), -x}
  vec3_set(yp, c,   1.f-b, sz*y); // {  -xy/(|z|+1), 1-y^2/(|z|+1), -y}
}

// issues: 32
void ortho_basis_2a(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float mx = -v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = y/(fabsf(z)+1.f);
  float b  = y*a;
  float c  = mx*a;
  
  vec3_set(xp, z+sz*b, sz*c, mx); 
  vec3_set(yp, c,   1.f-b, -sz*y); 
}

// "Building an Orthonormal Basis, Revisited"
// Tom Duff, James Burgess, Per Christensen, Christophe Hery,
// Andrew Kensler,Max Liani, and Ryusuke Villemin
// Journal of Computer Graphics Techniques, Vol. 6, No. 1, 2017

// issues: 36
void ortho_basis_pixar(vec3_t* n, vec3_t* b1, vec3_t* b2)
{
  float sign = copysignf(1.0f, n->z);
  const float a = -1.0f / (sign + n->z);
  const float b = n->x * n->y * a;
  vec3_set(b1, 1.0f + sign * n->x * n->x * a, sign * b, -sign * n->x);
  vec3_set(b2, b, sign + n->y * n->y * a, -n->y);
}

// negate both results: maintains v=cross(xp,yp)
// issues: 36->32
#if 0
void ortho_basis_pixar_r1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = 1.0f/(sz+z);
  float b  = x*y*a;
  vec3_set(xp, sz*x*x*a - 1.f, sz*b, sz*x);
  vec3_set(yp, b, y*y*a-sz, y);
}
#else
// this should compile to same with -O3 and yet doesn't
// issues: 30
void ortho_basis_pixar_r1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float sx = sz*x;
  float a  = 1.0f/(sz+z);
  float b  = x*y*a;
  vec3_set(xp, sx*x*a - 1.f, sz*b, sx);
  vec3_set(yp, b, y*y*a-sz, y);
}
#endif

// sub-express (ya)
// issues: 29
void ortho_basis_pixar_r2(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = 1.0f/(sz+z);
  float ya = y*a;
  float b  = x*ya;
  vec3_set(xp, sz*x*x*a - 1.f, sz*b, sz*x);
  vec3_set(yp, b, y*ya-sz, y);
}

// from 'r1': multiply xp though by sz: v=sgn(z)cross(xp,yp)
// issues: 28
void ortho_basis_pixar_l1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = 1.0f/(sz+z);
  float b  = x*y*a;
  vec3_set(xp, x*x*a - sz, b, x);
  vec3_set(yp, b, y*y*a - sz, y);
}

// issues: 26
void ortho_basis_pixar_l2(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = 1.0f/(sz+z);
  float ya = y*a;
  float b  = x*ya;
  vec3_set(xp, x*x*a - sz, b, x);
  vec3_set(yp, b, y*ya - sz, y);
}

// issues: 24
void ortho_basis_l1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = y/(z+sz);
  float b  = y*a;
  float c  = x*a;
  
  vec3_set(xp, -z-b, c, x);
  vec3_set(yp, c, b-sz, y);
}

// issues: 27
void ortho_basis_r1(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x  = v->x;
  float y  = v->y;
  float z  = v->z; 
  float sz = sgn(z);
  float a  = y/(z+sz);
  float b  = y*a;
  float c  = x*a;
  vec3_set(xp, -z-b, c, x);
  vec3_set(yp, sz*c, sz*b-1, sz*y);
}


// for measuring effect of simply promoting to double
void ortho_basis_1_pd(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  double x = v->x, y = v->y, z = v->z;
  double a = y/(z+1.0);
  double b = y*a;
  double c = x*a;
  
  vec3_set(xp, -z-b, c,  x);
  vec3_set(yp, c, b-1.f, y);
}

// for future post note
void ortho_max_reviewer_7(vec3_t* n, vec3_t* b1, vec3_t* b2)
{
  double dthreshold = -0.9999999999776;
  float rthreshold = -0.7f;
  float x = n->x, y=n->y, z=-n->z;
  
  if (z >= rthreshold) {
    const float a = 1.f/(1.f + z);
    const float b = -x*y*a ;
    vec3_set(b1, 1.0f - x*x*a , b , -x);
    vec3_set(b2, b, 1.0f - y*y*a, -y);
  }
  else {
    double dx = (double)x;
    double dy = (double)y;
    double dz = (double)z;
    const double d = 1./sqrt(dx*dx + dy*dy + dz*dz);
    dx *= d;
    dy *= d;
    dz *= d;
    if(z >= dthreshold) {
      const double a = 1.0 /(1.0 + dz);
      const double b = -dx*dy*a ;
      vec3_set(b1,  1.0f - (float)(dx*dx*a) , (float)b , (float)(-dx));
      vec3_set(b2, (float)b , 1.0f - (float)(dy*dy*a) ,  (float)(-dy));
    }
    else {
      vec3_set(b1,  0.f, -1.f, 0.f);
      vec3_set(b2, -1.f,  0.f, 0.f);
    }
  }
}


void ortho_hocevar_(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x =v->x, y=v->y, z=v->z;
  float s;
  
  if (fabs(x) > fabs(z)) {
    s = 1.f/sqrtf(x*x+y*y);
    vec3_set(xp, -s*y, s*x, 0.f);
    vec3_cross(yp, v, xp);
  } else {
    s = 1.f/sqrtf(y*y+z*z);
    vec3_set(xp, 0.f, -s*z, s*y);
    vec3_cross(yp, v, xp);
  }
}


void ortho_hocevar(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x =v->x, y =v->y, z =v->z;
  float x2=x*x,  y2=y*y,  z2=z*z;
  float s;
  
  if (x2 > z2) {
    s = 1.f/sqrtf(x2+y2);
    vec3_set(xp, -s*y, s*x, 0.f);
    vec3_cross(yp, v, xp);
  } else {
    s = 1.f/sqrtf(y2+z2);
    vec3_set(xp, 0.f, -s*z, s*y);
    vec3_cross(yp, v, xp);
  }
}

void ortho_forsyth(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  float x =v->x, y =v->y, z =v->z;
  float s;
  
  if (fabsf(y) < 0.99f) {
    s = 1.f/sqrtf(x*x+z*z);
    vec3_set(xp, -s*z, 0.f, s*x);
    vec3_cross(yp, v, xp);
  } else {
    s = 1.f/sqrtf(y*y+z*z);
    vec3_set(xp, 0.f, s*z, -s*y);
    vec3_cross(yp, v, xp);
  }
}


void ortho_hm(vec3_t* u, vec3_t* xp, vec3_t* yp)
{
  float x =u->x, y =u->y, z =u->z;
  float ax = fabsf(x), ay = fabsf(y), az = fabsf(z);
  
  if (ax <= ay && ax <= az)
    vec3_set(xp, 0, -z, y);
  else if (ay <= ax && ay <= az)
    vec3_set(xp, -z, 0, x);
  else
    vec3_set(xp, -y, x, 0);

  float s = 1.f/sqrtf(xp->x*xp->x + xp->y*xp->y + xp->z*xp->z);
  vec3_scale(xp,s);

  vec3_cross(yp, u, xp);
}





typedef void(*ortho_gen_t)(vec3_t* v, vec3_t* xp, vec3_t* yp);

typedef struct {
  ortho_gen_t f;
  char*       name;
} gens_t;

// only full shell generators
gens_t gens[] = {
#if defined(HALF_SPHERE)
  {&ortho_basis_1,        "method1"},
  {&ortho_frisvad,        "frisvad"},
  {&ortho_frisvad_m1,     "frisvad (m1)"},
  {&ortho_basis_pixar,    "pixar"}
#else
  {&ortho_basis_1,        "method1"},
  {&ortho_basis_1_pd,     "method1 (pd)"},
#if 0  
  {&ortho_hocevar,        "hovevar"},
  {&ortho_forsyth,        "forsyth"},
  {&ortho_hm,             "hm"},
  {&ortho_frisvad,        "frisvad"},
  {&ortho_frisvad_m1,     "frisvad (m1)"},
  {&ortho_basis_1,        "method1"},
  {&ortho_basis_2,        "method2"},
  {&ortho_basis_2a,       "method2a"},
  {&ortho_basis_pixar,    "pixar"},
  {&ortho_basis_pixar_r1, "pixar_r1"},
  {&ortho_basis_pixar_r2, "pixar_r2"},
  {&ortho_basis_pixar_l1, "pixar_l1"},
  {&ortho_basis_pixar_l2, "pixar_l2"},
  {&ortho_basis_l1,       "method_l1"},
  {&ortho_basis_r1,       "method_r1"}
#endif  
#endif  
};

#define NUM_GENS (sizeof(gens)/sizeof(gens[0]))

#if defined(HISTOGRAM)
// yes, very hacky..too lazy to shove in struct
double histo[HLEN];
#if defined(HISTOGRAM_DETAILED)
double histo_d_xy[HLEN];
double histo_d_xz[HLEN];
double histo_d_yz[HLEN];
double histo_m_x[HLEN];
double histo_m_y[HLEN];
#endif
#endif


typedef struct {
  double   xz,yz,xy;
  double   minm0,minm1;
  double   maxm0,maxm1;
  double   rms;
  double   peak;
  uint64_t cnt;
  vec3_t   v0,v1,v2,v3,v4,v5,v6;
} ortho_error_t;

void ortho_error_init(ortho_error_t* e)
{
  memset(e, 0, sizeof(ortho_error_t));
  e->minm0 = e->minm1 = e->maxm0 = e->maxm1 = 1.f;

#if defined(HISTOGRAM)
  // yes, very hacky
  memset(histo,     0, sizeof(double)*HLEN);

#if defined(HISTOGRAM_DETAILED)
  memset(histo_d_xz, 0, sizeof(double)*HLEN);
  memset(histo_d_yz, 0, sizeof(double)*HLEN);
  memset(histo_d_xy, 0, sizeof(double)*HLEN);
  memset(histo_m_x,  0, sizeof(double)*HLEN);
  memset(histo_m_y,  0, sizeof(double)*HLEN);
#endif
#endif
}

static inline double vec3_dot_d(vec3_t* a, vec3_t* b)
{
  double x = a->x;
  double y = a->y;
  double z = a->z;
  return x*b->x + y*b->y + z*b->z;

}

static inline double vec3_norm_d(vec3_t* a)
{
  return vec3_dot_d(a,a);
}

void ortho_check(ortho_error_t* e, vec3_t* v, vec3_t* x, vec3_t* y)
{
  double xz,yz,xy;
  double mx,my;
  uint32_t display=0;

  // NOT SOUND MEASURES..speed of testing..should be good enough
  xz = fabs(vec3_dot_d(v,x));
  yz = fabs(vec3_dot_d(v,y));
  xy = fabs(vec3_dot_d(x,y));
  mx = vec3_norm_d(x);
  my = vec3_norm_d(y);
  
  if (e->xz < xz)         {e->xz = xz;    vec3_dup(&e->v0, v); display=1; }
  if (e->yz < yz)         {e->yz = yz;    vec3_dup(&e->v1, v); display=1; }
  if (e->xy < xy)         {e->xy = xy;    vec3_dup(&e->v2, v); display=1; }
  if (e->minm0 > mx)      {e->minm0 = mx; vec3_dup(&e->v3, v); display=1; }
  else if (e->maxm0 < mx) {e->maxm0 = mx; vec3_dup(&e->v4, v); display=1; }
  if (e->minm1 > my)      {e->minm1 = my; vec3_dup(&e->v5, v); display=1; }
  else if (e->maxm1 < my) {e->maxm1 = my; vec3_dup(&e->v6, v); display=1; }

  // add RMS measure
  double t,error;
  mx = sqrt(mx)-1.0;
  my = sqrt(my)-1.0;
  t = vec3_norm_d(v);
  t = sqrt(t) -1.0; error  = t*t;
  error += mx*mx;
  error += my*my;
  error += xz*xz;
  error += yz*yz;
  error += xy*xy;

  if (error > e->peak) e->peak = error;

#if defined(HISTOGRAM)
  // bins of histogram are wrt height of cap
  float    hidf = 0.5f*(1.f-v->z);
  if (hidf < 0) hidf = 0;
  uint32_t hid = (uint32_t)(HLEN*hidf);
  if (hid >= HLEN) hid=HLEN-1;
  
  if (error > histo[hid]) histo[hid] = error;

#if defined(HISTOGRAM_DETAILED)
  mx = fabs(mx); my = fabs(my);
  if (xz > histo_d_xz[hid]) histo_d_xz[hid] = xz;
  if (yz > histo_d_yz[hid]) histo_d_yz[hid] = yz;
  if (xy > histo_d_xy[hid]) histo_d_xy[hid] = xy; 
  if (mx > histo_m_x[hid])  histo_m_x[hid]  = mx;
  if (my > histo_m_y[hid])  histo_m_y[hid]  = my;
#endif
#endif
  
  e->rms += error;
  e->cnt++;
  
  if (display) {
    printf("."); fflush(stdout);
  }
}

void ortho_basis_spew(ortho_gen_t f, vec3_t* v)
{
  vec3_t x,y;
  f(v, &x, &y);
  vec3_print(v);  vec3_printa(v);  ln();
  vec3_print(&x); vec3_printa(&x); ln();
  vec3_print(&y); vec3_printa(&y); ln();
}


double ortho_spew(ortho_gen_t f, ortho_error_t* e)
{
  double minx = sqrt(e->minm0);
  double maxx = sqrt(e->maxm0);
  double miny = sqrt(e->minm1);
  double maxy = sqrt(e->maxm1);
  double avx  = 1.0-TWO_OVER_PI_D*acos(e->xz);
  double avy  = 1.0-TWO_OVER_PI_D*acos(e->yz);
  double axy  = 1.0-TWO_OVER_PI_D*acos(e->xy);
  double rms  = sqrt(e->rms/(6.0*e->cnt));
  double peak = sqrt(e->peak/6.0);

#if 0  
  printf("\nmax v.x:   %e %a | rel angle error: %e %a\n", e->xz, e->xz, avx,avx); ortho_basis_spew(f,&e->v0);
  printf("\nmax v.y:   %e %a | rel angle error: %e %a\n", e->yz, e->yz, avy,avy); ortho_basis_spew(f,&e->v1);
  printf("\nmax x.y:   %e %a | rel angle error: %e %a\n", e->xy, e->xy, axy,axy); ortho_basis_spew(f,&e->v2);
  printf("\n1-min|x|:  %e %a\n", 1.0-minx, 1.0-minx); ortho_basis_spew(f,&e->v3);
  printf("\nmax|x|-1:  %e %a\n", maxx-1.0, maxx-1.0); ortho_basis_spew(f,&e->v4);
  printf("\n1-min|y|:  %e %a\n", 1.0-miny, 1.0-miny); ortho_basis_spew(f,&e->v5);
  printf("\nmax|y|-1:  %e %a\n", maxy-1.0, maxy-1.0); ortho_basis_spew(f,&e->v6);
#endif  
  printf("max error: %e %a\n",   peak, peak);
  printf("\nRMS:       %e %a\n", rms, rms);

  return rms;
}

void gen_reset(uint64_t s0, uint64_t s1)
{
  rng_state[0] = s0;
  rng_state[1] = s1;
  
#ifdef  USE_SOBOL
  sobol_2d_init(&qrng, (uint32_t)rng_u64(), (uint32_t)rng_u64());
#endif
}


// histogram data is dot product
#define SPEW_ANGLE(D) printf("%e, ", D)
//#define SPEW_ANGLE(D) printf("%e, ", 1.0-TWO_OVER_PI_D*acos(D))

void ortho_test(uint64_t s0, uint64_t s1)
{
  double rms[NUM_GENS];
  
  for(uint32_t gi=0; gi<NUM_GENS; gi++) {
    ortho_gen_t f = gens[gi].f;
    ortho_error_t e0;
    
    vec3_t n;
    vec3_t x;
    vec3_t y;

    gen_reset(s0,s1);
    
    ortho_error_init(&e0);
    printf("method: %s\n", gens[gi].name);
    
#ifdef  USE_SOBOL
    printf("sobol\n");
    
    for(uint64_t i=0; i<TRIALS; i++) {
      sobol_uniform_hs2(&qrng, n.f);
      f(&n, &x, &y);
      ortho_check(&e0, &n, &x, &y);

#ifndef HALF_SPHERE      
      n.z = -n.z;
      f(&n, &x, &y);
      ortho_check(&e0, &n, &x, &y);
#endif      
    }
    printf("\npseudo-random\n");
#endif
    fflush(stdout);
    
    for(uint64_t i=0; i<TRIALS; i++) {
#ifndef HALF_SPHERE      
      uniform_s2(&n);
#else
      uniform_hs2(&n);
#endif      
      f(&n, &x, &y);
      ortho_check(&e0, &n, &x, &y);
    }
    
    rms[gi] = ortho_spew(f, &e0);
    ln();

#if defined(HISTOGRAM)
    printf("RMS { "); for(uint32_t i=0; i<HLEN; i++) { printf("%e, ", sqrt(histo[i]/6.f)); }  printf("}\n\n");

#if defined(HISTOGRAM_DETAILED)
    printf("x.z { ");     for(uint32_t i=0; i<HLEN; i++) { SPEW_ANGLE(histo_d_xz[i]); } printf("}\n\n");
    printf("y.z { ");     for(uint32_t i=0; i<HLEN; i++) { SPEW_ANGLE(histo_d_yz[i]); } printf("}\n\n");
    printf("x.y { ");     for(uint32_t i=0; i<HLEN; i++) { SPEW_ANGLE(histo_d_xy[i]); } printf("}\n\n");
    
    printf("|1-|x|| { "); for(uint32_t i=0; i<HLEN; i++) { printf("%e, ", histo_m_x[i]);  } printf("}\n\n");
    printf("|1-|y|| { "); for(uint32_t i=0; i<HLEN; i++) { printf("%e, ", histo_m_y[i]);  } printf("}\n\n");
#endif

#endif    
  }

  printf("method   RMS\n");
  for(uint32_t gi=0; gi<NUM_GENS; gi++) {
    printf("%10s %e\n", gens[gi].name, rms[gi]);
  }

}


static inline void bar(vec3_t* v, vec3_t* xp, vec3_t* yp)
{
  {
    float x  = v->x;
    float y  = v->y;
    float z  = v->z;
    float sz = -sgn(z);
    float az = fabsf(z);
    float a  = y/(az+1.f); //   y/(|z|+1)
    float b  = y*a;        // y^2/(|z|+1)
    float c  = -x*a;       // -xy/(|z|+1)
    vec3_set(xp, az+b, c, sz*x);
  }

  {
    vec3_t* n = v;
    float sign = copysignf(1.0f, n->z);
    const float a = 1.0f / (sign + n->z);
    const float b = n->x * n->y * a;
    vec3_set(yp, sign * n->x * n->x * a - 1.f, sign * b, sign * n->x);
  }
}


// quick sanity check hack to dump out the first COUNT produced by each method
#define COUNT 100
void foo(uint64_t s0, uint64_t s1)
{
  m33_t b;
  vec3_t v,d;

  gen_reset(s0,s1);
  uniform_s2(b.row);
  
  for(uint32_t c=0; c<COUNT; c++) {
    uniform_s2(b.row);
    printf("-----\n");

    for(uint32_t gi=0; gi<NUM_GENS; gi++) {
      printf("%s\n", gens[gi].name);
      ortho_gen_t f = gens[gi].f;
      
      f(b.row, b.row+1, b.row+2);
      vec3_cross(&v, b.row+1, b.row+2);
      vec3_sub(&d, b.row, &v);
      m33_print(&b); vec3_print(&v); vec3_print(&d); ln();
    }
  }
}

int main()
{
  uint64_t t = __rdtsc();
  uint64_t s0,s1;

  s0 = t;
  s1 = t ^ 0x123456789;
  
//foo(s0,s1); return 0;
  ortho_test(s0,s1);
  
  return 0;
}

