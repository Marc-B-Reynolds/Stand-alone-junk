// Marc B. Reynolds, 2016
// Public Domain under http://unlicense.org, see link for details.
//
// EXCEPT: ..see links/paper/whatever
//   slerp_id_* 
//   slerp_eberly_* 
//   slerp_zeux_* 


// Toy code for: // http://marc-b-reynolds.github.io/
//
// to compile under VC you'll have to change the float hex-constants...couldn't
// be bothered.

// compile time configuration options

#define SANITY_CHECKS

#define SYM_TRIALS 0xFFFFF

#define TRIALS 10 // very temp hack

// ------

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#if 1 // LAZY HERE
#include <x86intrin.h>
//#include <x86intrin.h>
//#include "xmmintrin.h"

static inline float rsqrtf_a(float x) 
{
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

static inline float recip_a(float x) 
{
  return _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(x)));
}

static inline float rsqrt_nr(float x)
{
  float x0 = rsqrtf_a(x);
  return x0*(1.5f - x*0.5f*x0*x0);
}

static inline float rsqrt_nr2(float x)
{
  float x0 = rsqrtf_a(x);
  x0 = x0*(1.5f - x*0.5f*x0*x0);
  return x0*(1.5f - x*0.5f*x0*x0);
}

static inline float recip_nr(float x)
{
  float x0 = recip_a(x);
  return x0*(2.f - x*x0); // nope!
}  

#endif


#define SOBOL_IMPLEMENTATION
#define SOBOL_EXTRAS
#include "../../SFH/quat.h"
#include "../../SFH/Sobol.h"


sobol_2d_t qrng;

// xoroshiro128+

uint64_t rng_state[2];

#if defined(_MSC_VER)
#define TO_FP32 (1.f/16777216.f)
#define SLERP_CUT 0.999998986721038818359375f
#else
#define TO_FP32   0x1p-24f
#define SLERP_CUT 0x1.ffffdep-1
#endif


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
  return (rng_u64() >> 40)*TO_FP32;
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

// positive in 'x' half disc
float uniform_hdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v = rng_u64();
    x = (v >> 40)*TO_FP32;
    y = (v & 0xFFFFFF)*TO_FP32;
    d = x*x;
    y = 2.f*y-1.f; d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

static inline float sgn(float x) { return x >= 0.f ? 1.f : -1.f; }

// uniform on quarter disc {x,y}>=0
float uniform_qdisc(vec2_t* p)
{
  float d,x,y;
  uint64_t v;

  do {
    v  = rng_u64();
    x  = (v >> 40)*TO_FP32;
    y  = (v & 0xFFFFFF)*TO_FP32;
    d  = x*x;
    d += y*y;
  } while(d >= 1.f);

  p->x = x;
  p->y = y;

  return d;
}

// uniform dist on s3
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

// uniform dist with all positive components
void uniform_quat_p(quat_t* q)
{
  vec2_t p0,p1;
  float  d0 = uniform_qdisc(&p0);
  float  d1 = uniform_qdisc(&p1);
  float  s  = sqrtf((1.f-d0)/d1);
  
  quat_set(q, p0.x, p0.y, s*p1.x, s*p1.y);
}


void uniform_s1(vec2_t* p)
{
  float d,s,tx;
  vec2_t v;

  d  = uniform_disc(&v);
  s  = rsqrt_nr(d);
  tx = v.x + v.x;
  p->x = s*(tx*v.x-d);
  p->y = s*(tx*v.y);
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



// uniform dist with all positive components (LDS)
void quat_qr_p(quat_t* q)
{
  float x0,y0;
  float x1,y1;
  float d0,d1,s;
  float a[2];
  
  do {
    sobol_2d_next_f32(&qrng, a);
    d0 = a[0]*a[0]+a[1]*a[1];
  } while (d0 >=1.f);
  
  x0=a[0]; y0=a[1];
  
  do {
    sobol_2d_next_f32(&qrng, a);
    d1 = a[0]*a[0]+a[1]*a[1];
  } while (d1 >=1.f);
  
  x1=a[0]; y1=a[1];
  
  s = sqrtf((1.f-d0)/d1);
  
  quat_set(q, x0,y0,s*x1,s*y1);
}

void ln(void) {printf("\n");}

void quat_print(quat_t* q)
{
  printf("%+f+(%+f,%+f,%+f) ",q->w,q->x,q->y,q->z);
}

void quat_printa(quat_t* q)
{
  printf("%+a+(%+a,%+a,%+a) ",q->w,q->x,q->y,q->z);
}

void vec3_print(vec3_t* v)
{
  printf("(%+f,%+f,%+f) ",v->x,v->y,v->z);
}

void vec3_printa(vec3_t* v)
{
  printf("(%+a,%+a,%+a) ",v->x,v->y,v->z);
}

// ***************** half-hearted trig approx
// all the cosines are esp screwy..sollya
// ref script will do better

// damn-you microsoft for not supporting hex-floats
// all of this is very very lazy

#define PI 3.1415927410125732421875f

// atan_N(x) on [-1,1].

static inline float atan_7(float x)
{
  // 1.66290684247846942061144487572104929234984377645523e-6
  float x2=x*x;
  return x  * ( 0.99997723102569580078125f     +
         x2 * (-0.332623004913330078125f       +
         x2 * ( 0.19354121387004852294921875f  +
         x2 * (-0.116428203880786895751953125f +
         x2 * ( 5.2648954093456268310546875e-2f +
         x2 * (-1.1719689704477787017822265625e-2f))))));
}

static inline float atan_9(float x)
{
  // 3.7748949774459410845819875721049292349843776455244e-8
  float x2=x*x;
  return x  * ( 0.999999344348907470703125f  +
         x2 * (-0.3332988321781158447265625f +
         x2 * ( 0.199467599391937255859375f  +
         x2 * (-0.139094293117523193359375f  +
         x2 * ( 9.6439607441425323486328125e-2f +
         x2 * (-5.59337139129638671875e-2f +
         x2 * ( 2.187640778720378875732421875e-2f +
         x2 * (-4.05799411237239837646484375e-3f))))))));
}


// atan_h_N(x) on [1-sqrt(2), sqrt(2)-1].  tan(pi/8) = sqrt(2)-1

static inline float atan_h_4(float x)
{
  // 3.5239539266660406716193784436021150992881940747476e-6 
  float x2=x*x;
  return x *( 0.999939382076263427734375f+
         x2*(-0.3303958475589752197265625f+
         x2*  0.16358660161495208740234375f));
}

static inline float atan_h_5(float x)
{
  //  1.08602353003844395753613722008077444516570936145512e-7
  float x2=x*x;
  return x *( 0.999997615814208984375f     +
         x2*(-0.3331419527530670166015625f +
         x2*( 0.1958123743534088134765625f +
         x2*(-0.107805170118808746337890625f))));
}

static inline float atan_h_6(float x)
{
  // 4.71965678944543655871423692341031007285409143205719e-9
  float x2=x*x;
  return x *( 0.99999988079071044921875f    +
         x2*(-0.3333206474781036376953125f  +
         x2*( 0.19959522783756256103515625f +
         x2*(-0.13737906515598297119140625f +
         x2*  7.6941959559917449951171875e-2f))));
}

// forward trig: func_n_m = func on [-pi/n,pi/n] with m products
// if both sin/cos called common expression removes one (x^2)

static inline float cos_4_4(float x)
{
  // min abs err, l_inf norm ~= 3.32382e-8
  float x2=x*x;
  return 1.f+x2*(-0.49999892711639404296875f+x2*(4.16561998426914215087890625e-2f+x2*(-1.35968066751956939697265625e-3f)));

}

static inline float sin_4_4(float x)
{
  // min abs err, l_inf norm ~= 5.61231774e-7
  float x2=x*x;
  return x*(0.9999949932098388671875f+x2*(-0.166601598262786865234375f+x2*8.12153331935405731201171875e-3f));
}
  
static inline float cos_4_5(float x)
{
  // min abs err, l_inf norm ~= 1.0882486e-10
  float x2=x*x;
  float r;

  r = (-0.5f + x2*( 4.16666455566883087158203125e-2f
             + x2*(-1.388731296174228191375732421875e-3f
             + x2*  2.4432971258647739887237548828125e-5f)));
  
  return 1.f + x2*r;
}

static inline float sin_4_5(float x)
{
  // min abs err, l_inf norm ~= 1.8125140e-9
  float x2=x*x;
  float r;

  r =   x2*(-0.166666507720947265625f
      + x2*( 8.331983350217342376708984375e-3f
      + x2*(-1.94961365195922553539276123046875e-4f)));

  return x*(1.f+r);
}

static inline float sin_8_3(float x)
{
  // 4.83367470159342539962645894393071056104864748063499e-6
  return x*(0.999938428401947021484375 + x*x*(-0.16506682336330413818359375));
}

static inline float sin_8_4(float x)
{
  // 5.1053331915383724428944768209280833441209571318087e-9
  float x2=x*x;
  return x*(0.9999949932098388671875f+x2*(-0.166601598262786865234375f+x2*8.12153331935405731201171875e-3f));
}

static inline float sin_8_5(float x)
{
  // 2.7744545599129987932798952875514125471110778806934e-11
  float x2=x*x;
  return x*(1.f+x2*(-0.1666666567325592041015625f+x2*(8.333119563758373260498046875e-3f+x2*(-1.970127341337502002716064453125e-4f))));
}

static inline float cos_8_3(float x)
{
  // 1.7881393432610432412245647362324148300126967041499e-7
  float x2=x*x;
  return 0.999999821186065673828125f+x2*(-0.499980986118316650390625f+x2*4.13437895476818084716796875e-2f);
}

static inline float cos_8_4(float x)
{
  // 3.0457734816652631406026284427672838971041919783957e-10
  float x2=x*x;
  float r = x2*(-0.4999999701976776123046875f+x2*(4.16657663881778717041015625e-2f+x2*(-1.3804933987557888031005859375e-3f)));
  return 1.f+r;
}

static inline float cos_8_5(float x)
{
  // 1.35565225095633106849006492673341952228886372523635e-12
  float x2= x*x;
  float r = x2*(-0.5f+x2*(4.16666679084300994873046875e-2f+x2*(-1.388904987834393978118896484375e-3f+x2*2.4812869014567695558071136474609375e-5f)));
  return 1.f+r;
}


// *****************

// totally ingoring the actual worst case mag of input
static inline void quat_normalize(quat_t* r)
{
  float s = sqrtf(quat_norm(r));
  quat_scale(r,1.f/s);
}

typedef struct { double x,y,z,w; } quatd_t;

static inline void quat_to_d(quatd_t* r, quat_t* q)
{
  r->x = q->x; r->y = q->y; r->z = q->z; r->w = q->w;
}

static inline void quatd_store(quat_t* r, quatd_t* q)
{
  r->x = (float)q->x;
  r->y = (float)q->y;
  r->z = (float)q->z;
  r->w = (float)q->w;
}

static inline void quatd_set(quatd_t* r, double x, double y, double z, double w)
{
  r->x=x; r->y=y; r->z=z; r->w=w;
}

static inline void quatd_dup(quatd_t* r, quatd_t* a)
{
  quatd_set(r, a->x,a->y,a->z,a->w);
}

static inline void quatd_neg(quatd_t* a)
{
  // no -zero (assuming no relation of fp rules)
  quatd_set(a, 0.0-a->x, 0.0-a->y, 0.0-a->z, 0.0-a->w);
}

static inline void quatd_scale(quatd_t* a, float s)
{
  a->x *= s; a->y *= s; a->z *= s; a->w *= s;
}

static inline void quatd_add(quatd_t* r, quatd_t* a, quatd_t* b)
{
  double x = a->x + b->x;
  double y = a->y + b->y;
  double z = a->z + b->z;
  double w = a->w + b->w;
  quatd_set(r, x,y,z,w);
}

static inline void quatd_sdiv(quatd_t* a, double s)
{
  a->x /= s;
  a->y /= s;
  a->z /= s;
  a->w /= s;
}


static inline double quatd_dot(quatd_t* a, quatd_t* b)
{
  // from: http://toccata.lri.fr/gallery/ScalarProduct.en.html
  // not bother to compute BMAX for N=3 and using N=10 for
  // above, the error on this is < 3*0x1.1p-50 ~2.831e-15
  return a->x*b->x + a->y*b->y + a->z*b->z+ a->w*b->w;
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



void quatd_print(quatd_t* q)
{
  printf("%f+(%f,%f,%f) ",q->w,q->x,q->y,q->z);
}

void quatd_printa(quatd_t* q)
{
  printf("%a+(%a,%a,%a) ",q->w,q->x,q->y,q->z);
}


// goofying around version. could be made faster and more accurate.
// D&C in 2D instead of 4D.
void slerp_nutsy_ref(quat_t* R, quat_t* A, quat_t* B, float T)
{
  quatd_t a,b,m;
  double  c,f;
  double  t=T;
  uint32_t n=0;

  // convert to double
  quat_to_d(&a, A);
  quat_to_d(&b, B);

  // Since Q and -Q represent the same, choose both to have
  // positive scalars. 
  if (A->w < 0.0) quatd_neg(&a);
  if (B->w < 0.0) quatd_neg(&b);

  // insure a & b are in the same half-sphere
  // near meanless error around dot(a,b) approaching zero
  c = quatd_dot(&a,&b);
  
  if (quatd_dot(&a,&b) < 0.0) {
    quatd_neg(&b);
    c = -c;
  }

  n--;
  
  do {
    // bisector (relative half-angle)
    quatd_add(&m, &a, &b);
    quatd_scale(&m, 0.5);
    double d = quatd_dot(&m, &a);
    f = sqrt(d);
    quatd_sdiv(&m, f);
    
    t += t;
    n++;

    // treating 't' as integer so the exact compares
    if (t < 1.0) {
      if (t != 0.0)
        quatd_dup(&b, &m);
      else {
        quatd_store(R, &a);
        return;
      }
    }
    else if (t > 1.0) {
      if (t != 2.0) {
        quatd_dup(&a, &m);
        t -= 1.0;
      } else {
        quatd_store(R, &b);
        return;
      }
    } else {
      quatd_store(R, &m);
      return;
    }

    // f exact here is silly
  } while(f != 1.0 && n < 65);

  quatd_store(R, &m);
}

// geometric form directly using doubles. For sanity-checking
// the sanity-checking version.
void slerp_ref_ref(quat_t* R, quat_t* A, quat_t* B, float t)
{
  double d;
  {
    // not wasting time on an accurate scalar product
    double ax = A->x;
    double ay = A->y;
    double az = A->z;
    double aw = A->w;
    d = B->x*ax + B->y*ay + B->z*az + B->w*aw;
  }

  double ad = fabs(d);
  double s0, s1;
  
  if (ad < SLERP_CUT) {
    double a  = acos(ad);
    double sa = sin(a);
    s0 = sin((1-t) * a)/sa;
    s1 = sin((  t) * a)/sa;
  }
  else {
    s0 = 1 - t;
    s1 = t;
  }

  s1 = d >= 0 ? s1 : -s1;

  R->x = (float)(s0*A->x + s1*B->x);
  R->y = (float)(s0*A->y + s1*B->y);
  R->z = (float)(s0*A->z + s1*B->z);
  R->w = (float)(s0*A->w + s1*B->w);
}

// alegebraic form using doubles
void slerp_ref_ref_(quat_t* R, quat_t* A, quat_t* B, float t)
{
  double d;
  {
    double ax = A->x;
    double ay = A->y;
    double az = A->z;
    double aw = A->w;
    d = B->x*ax + B->y*ay + B->z*az + B->w*aw;
  }

  double ad = fabs(d);
  double s0, s1;
  
  if (ad < SLERP_CUT) {
    double s  = sqrt(1-d*d);
    double a  = atan2(s,ad);
    double ta = t*a;
    double sa = sin(ta);
    s1 = sa/s;
    s0 = cos(ta)-ad*s1;
  }
  else {
    s0 = 1 - t;
    s1 = t;
  }

  s1 = d >= 0 ? s1 : -s1;

  R->x = (float)(s0*A->x + s1*B->x);
  R->y = (float)(s0*A->y + s1*B->y);
  R->z = (float)(s0*A->z + s1*B->z);
  R->w = (float)(s0*A->w + s1*B->w);
}


// straight forward acos->atan2 version using all std funcs
void slerp_ref_0(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  
  if (d < SLERP_CUT) {
    float s = sqrtf(1-d*d);
    float a = atan2f(s,d);
    float c = cosf(t*a);
    s1 = sin(t*a)/s;//sqrtf(1-c*c)/s;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  // weighted sum: R = s0*A + sgn*s1*B
  quat_wsum(R, A, B, s0, sgn*s1);
}

void slerp_ref_1(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);

  if (d < SLERP_CUT) {
    t += t;                      // account for half-angle atan
    float s2 = 1.f-d*d;
    float rs = rsqrt_nr(s2);     // ~x^-.5 to at least 12-bit, 1 NR step
    float y  = 1-d;
    float a  = atanf(y*rs);      // still range reduction
    float s  = sinf(t*a);        // still range reduction
    float c  = sqrtf(1.f-s*s);   // can't ~1/sqrt(x) if max angle allowed
    s1 = s*rs;                   // sgn*s1*B can be computed while 'ic', etc in progress
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}

void slerp_ref_2(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  
  if (d < SLERP_CUT) {
    float s2 = 1.f-d*d;
    float rs = rsqrt_nr(s2);   
    float y  = 1-d;
    float a  = atanf(y*rs);      // still range reduction
    float hs = sin_4_5(t*a);
    float hc = sqrtf(1.f-hs*hs);
    float th = 2.f*hs;
    float c  = 1.f-hs*th;  // hc*hc-hs*hs;
    float s  = th*hc;
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}

// approximate trig ops
void slerp_ref_3(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  
  if (d < SLERP_CUT) {
    // set-up for atan
    float s2 = 1.f-d*d;
    float rs = rsqrt_nr(s2);   
    float y  = 1-d;
    float a  = atan_9(y*rs);       // ~atan on [0,1]

    // forward trig approx (or root for one)
    float ta = t*a;
    float hc = cos_4_5(ta);       // ~cos on [0, pi/4]
    float hs = sin_4_5(ta);       // ~sin on [0, pi/4]

    // double angle
    float th = 2.f*hc;
    float c  = hc*th-1.f;  // hc*hc-hs*hs
    float s  = th*hs;
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}


#if defined(_MSC_VER)
#define DOT_CLAMP 1.953125e-3f
#define SQRT2_O_2 0.707106769084930419921875f
#else
#define DOT_CLAMP 0x1p-9f
#define SQRT2_O_2 0xb.504f3p-4f
#endif

void slerp_ref_4(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  d = (d > DOT_CLAMP) ? d : DOT_CLAMP;
    
  if (d < SLERP_CUT) {
    // set-up for atan
    float x  = (d > SQRT2_O_2) ? 0 : 1;
    float ps = x*(PI*.25);
    float s2 = 1.f-d*d;
    float i  = recip_nr(1.f-x+d);
    float rs = rsqrt_nr(s2);   
    float y  = s2*rs-x;
    float a  = atan_h_6(y*i)+ps;
    
    // forward trig approx (or root for one)
    float ta = t*a;
    float hc = cos_4_5(ta);
    float hs = sin_4_5(ta);
    
    // double angle
    float th = 2.f*hc;
    float c  = hc*th-1.f;
    float s  = th*hs;
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }
  
  quat_wsum(R, A, B, s0, sgn*s1);
}


void slerp_ref_5(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  
  if (d < SLERP_CUT) {
    float s2 = 1.f-d*d;
    float rs = rsqrt_nr(s2);   
    float y  = 1-d;
    float a  = atan_9(y*rs);
    float ta = t*a;
    float hc = cos_4_5(ta);
    float hs = sqrtf(1.f-hc*hc);
    float th = 2.f*hc;
    float c  = hc*th-1.f;
    float s  = th*hs;
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}



static inline float cos_2_4(float x) {
  float x2 = x*x;
  float c;
  c  = x2*( +3.679168224334716796875e-2f);
  c  = x2*(c-0.495580852031707763671875f);
  c  =    (c+0.9994032382965087890625f);

  return c;
}



// error = 6.77119023748673498630523681640625e-5
// zero  = [|0.678780257701873779296875, 1.22571027278900146484375, 1.531032562255859375|]
static inline float sin_2_4(float x) {
  float x2 = x*x;
  float s;
  s  = x2*( +7.5143859721720218658447265625e-3f);
  s  = x2*(s-0.1656731069087982177734375f);
  s  = x *(s+0.999696791172027587890625f);
  return s;
}

static inline float p47(float x)
{
  float x2 = x*x;
  float t;
  t = x2 * (-2793121  * 0x1p-23f + 
      x2 * ( 13098501 * 0x1p-26f +
      x2 * (-4020455  * 0x1p-25f +
      x2 * ( 14980357 * 0x1p-28f +
      x2 * (-13750363 * 0x1p-30f)))));
  return x+x*t;
}

static inline float p48(float x)
{
  float x2 = x*x;
  float t;
  t = x2 * ( 8388417  * 0x1p-23f + 
      x2 * (-697561   * 0x1p-26f +
      x2 * ( 12988331 * 0x1p-27f +
      x2 * (-7066423  * 0x1p-27f +
      x2 * (-13750363 * 0x1p-30f)))));
  return x+x*t;
}


void slerp_ref_6(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);
  
  if (d < SLERP_CUT) {
    t += t;
    float s2 = 1.f-d*d;
    float rs = rsqrt_nr(s2);
    float y  = 1-d;
    float a  = p47(y*rs);
    float ta = t*a;
    float s  = sin_2_4(ta);
    float c  = sqrtf(1.f-s*s);
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}

// ****************
// precomputation


typedef struct {
  quat_t x,y;    // the 2D orthogonal basis
  float  a;      // the angle (+1 float to data-set)
} slerp_pc_t;

// all the various options in the reference section apply
// here as well.  simply showning the short and very slow
// version to be concise.

void slerp_pc_init_0(slerp_pc_t* k, quat_t* A, quat_t* B)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;

  d = fabsf(d);
  quat_dup(&k->x, A);

  if (d < SLERP_CUT) {
    float s  = sqrtf(1-d*d);
    float a  = atan2f(s,d);
    float is = sgn/s;
    quat_wsum(&k->y, A, B, -d*is, is);
    k->a = a;
  }
  else {
    quat_dup(&k->y, B);
    k->a = PI*0.5f;
  }
}

void slerp_pc_0(quat_t* r, slerp_pc_t* k, float t)
{
  float ta = t*k->a;
  float c  = cosf(ta);
  float s  = sqrt(1-c*c);
  quat_wsum(r, &k->x, &k->y, c, s);
}

// dump wrapper for testing
void slerp_pc_wrap_0(quat_t* R, quat_t* A, quat_t* B, float t)
{
  slerp_pc_t slerp;
  slerp_pc_init_0(&slerp,A,B);
  slerp_pc_0(R, &slerp, t);
}

//****************
// fixed steps

typedef struct {
  quat_t x,y;    // the 2D orthogonal basis
  float  s0,s1;  // current scale
  float  d0,d1;  // rotation
} slerp_fs_t;

void slerp_fs_init_0(slerp_fs_t* k, quat_t* A, quat_t* B, float f)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float a;

  d = fabsf(d);
  quat_dup(&k->x, A);

  if (d < SLERP_CUT) {
    float s  = sqrtf(1-d*d);
    float is = sgn/s;
    a  = atan2f(s,d);
    quat_wsum(&k->y, A, B, -d*is, is);
  }
  else {
    quat_dup(&k->y, B);
    a = PI*0.5f;
  }

  k->s0 = 1.f;
  k->s1 = 0.f;
  k->d0 = cosf(f*a);
  k->d1 = sinf(f*a);
}

void slerp_fs_0(quat_t* r, slerp_fs_t* k)
{
  float s0 = k->s0;
  float s1 = k->s1;
  quat_wsum(r, &k->x, &k->y, s0, s1);
  k->s0 = k->d0*s0 - k->d1*s1;
  k->s1 = k->d1*s0 + k->d0*s1;
}


typedef struct {
  quat_t q;       // current (duplicate)
  quat_t dq;      // the rotation
} slerp_fr_t;


void slerp_fr_init_0(slerp_fr_t* k, quat_t* A, quat_t* B, float f)
{
  float t = f;

  // slerp_ref_1: cut-and-paste as example - same reductions apply
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabsf(d);

  if (d < SLERP_CUT) {
    t += t; 
    float s2 = 1.0-d*d;
    float i  = recip_nr(1.f+d);
    float rs = rsqrt_nr(s2);
    float y  = s2*rs;          
    float a  = atanf(y*i);
    float s  = sinf(t*a);
    float c  = sqrtf(1.f-s*s);
    s1 = s*rs;
    s0 = c-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(&k->dq, A, B, s0, sgn*s1);
  quat_dup(&k->q, A);
}

void slerp_fr_0(quat_t* r, slerp_fr_t* k)
{
  quat_mul(&k->q, &k->q, &k->dq);
  quat_dup(r, &k->q);
}

// ****************

void slerp_nlerp(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float d  = quat_dot(a,b);
  float s0 = 1.0f-t;
  float s1 = d >= 0 ? t : -t;
  
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}

// A.B >= 0 should be invariant
void slerp_nlerp_1(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float s0 = 1.0f-t;
  float s1 = t;
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}

// temp hack

// scale function
static inline float rrp_s_2(float d) { return 0.0450414f + d*(-0.0611203f + d*(0.0163106f)); }
static inline float rrp_s_3(float d) { return 0.0452436f + d*(-0.0650588f + d*(0.0272054f - 0.00741959*d)); }

// shape function
static inline float rrp_r_2(float t) { return -4.6476f  + t*(-2.54039f + 23.3009*t); }
static inline float rrp_r_3(float t) { return -4.75907f + t*( 1.06188f + t*(5.10996f + 23.4539f*t)); }

void slerp_rrp_2_2(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float d  = quat_dot(a,b);
  float ad = fabsf(d);
  float k  = rrp_s_2(ad);
  float tp = t-0.5f;
  float s  = tp*rrp_r_2(fabsf(tp));
  float tt = t+k*s;
  float s0 = 1 - tt;
  float s1 = d > 0 ? tt : -tt;
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}

void slerp_rrp_3_3(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float d  = quat_dot(a,b);
  float ad = fabsf(d);
  float k  = rrp_s_3(ad);
  float tp = t-0.5f;
  float s  = tp*rrp_r_3(fabsf(tp));
  float tt = t+k*s;
  float s0 = 1 - tt;
  float s1 = d > 0 ? tt : -tt;
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}


// ****************
// "Slerping Clock Cycles", J.M.P. van Waveren, 2005
// scalar examples tweaked to C and local parameter ordering 

void slerp_id_at_start_of_paper(quat_t* result, quat_t* from, quat_t* to, float t)
{
  float cosom, absCosom, sinom, omega, scale0, scale1;
  cosom = quat_dot(from, to);
  absCosom = fabsf( cosom );
  
  if ( ( 1.0f - absCosom ) > 1e-6f ) {
    omega = acosf( absCosom );
    sinom = 1.0f / sinf( omega );
    scale0 = sinf( ( 1.0f - t ) * omega ) * sinom; 
    scale1 = sinf( t * omega ) * sinom;
  } else {
    scale0 = 1.0f - t;
    scale1 = t;
  }
  scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1;

  quat_wsum(result, from, to, scale0, scale1);
}

//  SlerpTransformed in paper
void slerp_id_ref_0(quat_t* result, quat_t* from, quat_t* to, float t)
{
  float cosom, absCosom, sinom, sinSqr, omega, scale0, scale1;
  cosom = quat_dot(from, to);
  absCosom = fabsf( cosom );

  if ( ( 1.0f - absCosom ) > 1e-6f ) {
    sinSqr = 1.0f - absCosom * absCosom;
    sinom = 1.0f / sqrtf( sinSqr );
    omega = atan2f( sinSqr * sinom, absCosom );
    scale0 = sinf( ( 1.0f - t ) * omega ) * sinom;
    scale1 = sinf( t * omega ) * sinom;
  } else {
    scale0 = 1.0f - t;
    scale1 = t;
  }
  scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1;
  quat_wsum(result, from, to, scale0, scale1);
}

static inline float ATanPositive( float y, float x ) {
  float a, d, s, t;
  if ( y > x ) {
    a = -x / y;
    d = M_PI / 2;
  } else {
    a = y / x;
    d = 0.0f;
  }
  s = a * a;
  t = 0.0028662257f;
  t *= s;
  t += -0.0161657367f;
  t *= s;
  t += 0.0429096138f;
  t *= s;
  t += -0.0752896400f;
  t *= s;
  t += 0.1065626393f;
  t *= s;
  t += -0.1420889944f;
  t *= s;
  t += 0.1999355085f;
  t *= s;
  t += -0.3333314528f;
  t *= s;
  t += 1.0f;
  t *= a;
  t += d;
  return t;
}

static inline float SinZeroHalfPI( float a )
{
  float s, t;
  s = a * a;
  t = -2.39e-08f;
  t *= s;
  t += 2.7526e-06f;
  t *= s;
  t += -1.98409e-04f;
  t *= s;
  t += 8.3333315e-03f;
  t *= s;
  t += -1.666666664e-01f;
  t *= s;
  t += 1.0f;
  t *= a;
  return t;
}

void slerp_id_ref_1(quat_t* result, quat_t* from, quat_t* to, float t)
{
  float cosom, absCosom, sinom, sinSqr, omega, scale0, scale1;
  cosom = quat_dot(from, to);
  absCosom = fabsf( cosom );

  if ( ( 1.0f - absCosom ) > 1e-6f ) {
    sinSqr = 1.0f - absCosom * absCosom;
    sinom  = rsqrt_nr(sinSqr); //ReciprocalSqrt( sinSqr );
    omega  = ATanPositive( sinSqr * sinom, absCosom );
    scale0 = SinZeroHalfPI( ( 1.0f - t ) * omega ) * sinom;
    scale1 = SinZeroHalfPI( t * omega ) * sinom;
  } else {
    scale0 = 1.0f - t;
    scale1 = t;
  }
  scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1;
  quat_wsum(result, from, to, scale0, scale1);
}


    
// ****************
// "A Fast and Accurate Algorithm for Computing SLERP", David Eberly, 2011

// table copy-n-pasted from paper
#define OP_MU_1  1.62943436108234530  // 5.745259x10-3 0.3214 0.8477
#define OP_MU_2  1.73965850021313961  // 1.092666x10-3 0.3080 0.8344
#define OP_MU_3  1.79701067629566813  // 2.809387x10-4 0.3026 0.8288
#define OP_MU_4  1.83291820510335812  // 8.409177x10-5 0.2998 0.8258
#define OP_MU_5  1.85772477879039977  // 2.763477x10-5 0.2980 0.8238
#define OP_MU_6  1.87596835698904785  // 9.678992x10-6 0.2967 0.8224
#define OP_MU_7  1.88998444919711206  // 3.551215x10-6 0.2958 0.8214
#define OP_MU_8  1.90110745351730037  // 1.349968x10-6 0.2952 0.8206
#define OP_MU_9  1.91015881189952352  // 5.277561x10-7 0.2946 0.8200
#define OP_MU_10 1.91767344933047190  // 2.110597x10-7 0.2942 0.8195
#define OP_MU_11 1.92401541194159076  // 8.600881x10-8 0.2938 0.8191
#define OP_MU_12 1.92944142668012797  // 3.560875x10-8 0.2935 0.8188
#define OP_MU_13 1.93413793373091059  // 1.494321x10-8 0.2932 0.8184
#define OP_MU_14 1.93824371262559758  // 6.344653x10-9 0.2930 0.8182
#define OP_MU_15 1.94186426368404708  // 2.721482x10-9 0.2928 0.8180
#define OP_MU_16 1.94508125972497303  // 1.177902x10-9 0.2926 0.8178

const float mu[] = {OP_MU_1,OP_MU_2,OP_MU_3,OP_MU_4,OP_MU_5,OP_MU_6,OP_MU_7,OP_MU_8,OP_MU_9,OP_MU_10,OP_MU_11,OP_MU_12,OP_MU_13,OP_MU_14,OP_MU_15,OP_MU_16};

const float eberly_u[] = {  1/3.f,  1/10.f,  1/21.f,  1/36.f,  1/55.f,  1/78.f, 1/105.f, 1/136.f,
                          1/171.f, 1/210.f, 1/253.f, 1/300.f, 1/351.f, 1/406.f, 1/465.f, 1/528.f};
const float eberly_v[] = {  1/3.f,   2/5.f,   3/7.f,   4/9.f,  5/11.f,  6/13.f,  7/15.f,  8/17.f,
                           9/19.f, 10/21.f, 11/23.f, 12/25.f, 13/27.f, 14/29.f, 15/31.f, 16/33.f};


// generic version: constructed from the paper
static inline void slerp_eberly_g(quat_t* R, quat_t* A, quat_t* B, float t, uint32_t n)
{
  float d  = quat_dot(A,B);
  float s  = 1.f-t;
  float t2 = t*t;
  float s2 = s*s;

  t = (d >= 0) ? t : -t;
  d = fabsf(d)-1.f;

  // one more rounding step for the mu product..removable
  float s0 = 1.f + mu[n]*(eberly_u[n]*s2 - eberly_v[n])*d;
  float s1 = 1.f + mu[n]*(eberly_u[n]*t2 - eberly_v[n])*d;
  float k0,k1,u,v;
  
  for (int i = n-1; i >= 0; --i) {
    u  = eberly_u[i];
    v  = eberly_v[i];
    k0 = (u*s2 - v)*d;
    k1 = (u*t2 - v)*d;
    s0 = 1.f + s0*k0;
    s1 = 1.f + s1*k1;
  }
  
  s0 = s*s0;
  s1 = t*s1;

  quat_wsum(R, A, B, s0, s1);
}

// let the compiler do what it will
void slerp_eberly_1g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 1); }
void slerp_eberly_2g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 2); }
void slerp_eberly_3g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 3); }
void slerp_eberly_4g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 4); }
void slerp_eberly_5g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 5); }
void slerp_eberly_6g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 6); }
void slerp_eberly_7g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 7); }
void slerp_eberly_8g(quat_t* R, quat_t* A, quat_t* B, float t) { slerp_eberly_g(R, A, B, t, 8); }


// slerp_eberly_8, u8 and u7 directly cut-paste-modified from paper

const float u8[] =
{ 1.f/(1*3), 1.f/(2*5), 1.f/(3*7), 1.f/(4*9), 1.f/(5*11), 1.f/(6*13), 1.f/(7*15), OP_MU_8/(8*17) } ;

const float v8[] = 
{ 1.f/3, 2.f/5, 3.f/7, 4.f/9, 5.f/11, 6.f/13, 7.f/15, OP_MU_8*8/17 } ;


void slerp_eberly_8(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float x   = quat_dot(A,B);
  float sgn = (x >= 0 ? 1 : (x = -x, -1));
  float xm1 = x-1;
  float d   = 1-t;
  float t2  = t*t;
  float d2  = d*d;
  float bT[8], bD[8];

  for (int i = 7; i >= 0; --i) {
    bT[i] = (u8[i]*t2 - v8[i])*xm1;
    bD[i] = (u8[i]*d2 - v8[i])*xm1;
  }
  
  float s1 = sgn*t*(1 + bT[0] * (1+bT[1]*(1 + bT[2]*(1 + bT[3]*(1 + bT[4]*(1 + bT[5]*(1 + bT[6]*(1 + bT[7]))))))));
  float s0 =     d*(1 + bD[0] * (1+bD[1]*(1 + bD[2]*(1 + bD[3]*(1 + bD[4]*(1 + bD[5]*(1 + bD[6]*(1 + bD[7]))))))));

  quat_wsum(R, A, B, s0, s1);
}


// ****************
// http://zeuxcg.org/2015/07/23/approximating-slerp/

// these are cut-paste-modify of
//   http://gist.github.com/zeux/1935b5f6d1c8c311e68bbd4a13955dfa


// A0,B0,K0 variant
void slerp_zeux_1(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float ca = quat_dot(a,b);
  float ad = fabsf(ca);
  
  float k  = 0.931872f + ad * (-1.25654f + ad * 0.331442f);
  float ot = t + t * (t - 0.5f) * (t - 1) * k;
  float s0 = 1 - ot;
  float s1 = ca > 0 ? ot : -ot;
  
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}


// A1,B1,K1 variant: sample as n2 except constants. see post.

// A2,B2,K2 variant
void slerp_zeux_2(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float ca = quat_dot(a, b);
  float d  = fabsf(ca);
  float A  = 1.0904f   + d * (-3.2452f  + d * (3.55645f - d * 1.43519f));
  float B  = 0.848013f + d * (-1.06021f + d * 0.215638f);
  float k  = A * (t - 0.5f) * (t - 0.5f) + B;
  float ot = t + t * (t - 0.5f) * (t - 1) * k;
  float s0 = 1 - ot;
  float s1 = ca > 0 ? ot : -ot;
  
  quat_wsum(r, a, b, s0, s1);
  quat_normalize(r);
}


// *****************

#if 0
void t_search()
{
  float d = 1.f;
  float t = .41f;

  do {
    float r  = 1.f/sqrtf(1-d*d);
    float b  = atanf(d*r);
    float c  = cosf(t*b);
    float s1 = sqrtf(1-c*c)*r;
    float s0 = c-s1;

    float s0x = 1.0f - t;
    float s1x = t;

  } while(0);
}
#endif

// *****************

typedef void (*slerp_func_t)(quat_t*,quat_t*,quat_t*,float);

typedef struct {
  slerp_func_t f;
  char*        name;
} slerp_name_t;

typedef struct {
  quat_t A;
  quat_t B;
  float  minDot;
} slerp_test_t;


#define SF(X) {&X,#X}

slerp_name_t funcs[] =
{
  //SF(slerp_nutsy_ref),
  //SF(slerp_ref_ref_),

  SF(slerp_ref_0),
  SF(slerp_ref_1),
  SF(slerp_ref_2),
  SF(slerp_ref_3),
  SF(slerp_ref_4),
  SF(slerp_ref_5),
  SF(slerp_ref_6),

  SF(slerp_id_ref_0),
  SF(slerp_id_ref_1),

  SF(slerp_rrp_2_2),
  SF(slerp_rrp_3_3),
#if 0  
  SF(slerp_eberly_1g),
  SF(slerp_eberly_2g),
  SF(slerp_eberly_3g),
  SF(slerp_eberly_4g),
  SF(slerp_eberly_5g),
  SF(slerp_eberly_6g),
  SF(slerp_eberly_7g),
  SF(slerp_eberly_8g),
#endif  
  SF(slerp_eberly_8),

  SF(slerp_zeux_1),
  SF(slerp_zeux_2),

  //SF(slerp_nlerp),

  // 
  //SF(slerp_pc_wrap_0),
};

#define NUM_WRAP  0
#define SLERP_REF slerp_ref_ref

#define NUM_FUNCS (sizeof(funcs)/sizeof(funcs[0]))


#define NUM_FUNCS_NW (NUM_FUNCS-NUM_WRAP)

void reset_generators(uint64_t s0, uint64_t s1)
{
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0+(s0>>32)));
  rng_state[0] = s0;
  rng_state[1] = s1;
  rng_u64();
}

void quat_canonical(quat_t* q)
{
  if (q->w > 0) return;
  
  quat_neg(q,q);
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


#define D(X) printf("(" #X ") ")

int quat_identical(quat_t* a, quat_t* b)
{
  // structured for relaxed IEEE rules
  float t0,t1;

  t0  = fabsf(a->x - b->x);
  t1  = fabsf(a->x + b->x);
  t0 += fabsf(a->y - b->y);
  t1 += fabsf(a->y + b->y);
  t0 += fabsf(a->z - b->z);
  t1 += fabsf(a->z + b->z);
  t0 += fabsf(a->w - b->w);
  t1 += fabsf(a->w + b->w);
  t0  = (t0 < t1) ? t0 : t1;

  return t0 != 0;
}


void ortho_test(uint64_t s0, uint64_t s1)
{
  quat_t A,B;
  quat_t NA,NB;
  float  t;

  printf("* ortho test\n");

  reset_generators(s0,s1);
  t = (rng_f32()-0.5f)*0.2; // huh..me thinking what here?
  
  quat_set(&A,0,0,0,1);
  quat_set(&B,0,0,1,0);
  t = 1.0f;
  
  quat_neg(&NA, &A);
  quat_neg(&NB, &B);
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    slerp_func_t slerp = funcs[s].f;

    printf("\n\n---- %30s \n", funcs[s].name);

    quat_t R0,R1,R2,R3;

    slerp(&R0, &A,  &B,  t);
    slerp(&R1, &A,  &NB, t);
    slerp(&R2, &NA, &B,  t);
    slerp(&R3, &NA, &NB, t);
    
    quat_printa(&R0); 
    quat_printa(&R1); ln();
    quat_printa(&R2); 
    quat_printa(&R3); ln();
    printf("%d ",  quat_identical(&R0,&R1));
    printf("%d ",  quat_identical(&R0,&R2));
    printf("%d\n", quat_identical(&R0,&R3));
  }
}


// all permutations of signs should return numerically
// identical results up to sign. No methods here have
// any directionally dependent code such as choosing
// an orthonogal direction as the relative quaternion
// approaches one (identity).
void sym_test(uint64_t s0, uint64_t s1)
{
  quat_t A,B,NA,NB;
  float  t;

  printf("* sym test\n");

  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    slerp_func_t slerp = funcs[s].f;
    uint32_t r = 0;

    reset_generators(s0,s1);
    t = rng_f32();
    
    quat_qr_p(&A);
    quat_qr_p(&B);
    
    quat_neg(&NA, &A);
    quat_neg(&NB, &B);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      
      quat_t R0,R1,R2,R3;
      
      slerp(&R0, &A,  &B,  t);
      slerp(&R1, &A,  &NB, t);
      slerp(&R2, &NA, &B,  t);
      slerp(&R3, &NA, &NB, t);
      
      r += quat_identical(&R0,&R1);
      r += quat_identical(&R0,&R2);
      r += quat_identical(&R0,&R3);

      t = 1.f-t;
      slerp(&R0, &B,  &A,  t);
      slerp(&R1, &B,  &NA, t);
      slerp(&R2, &NB, &A,  t);
      slerp(&R3, &NB, &NA, t);
      
      r += quat_identical(&R0,&R1);
      r += quat_identical(&R0,&R2);
      r += quat_identical(&R0,&R3);

      if (r != 0) {
        printf("   failed: %s\n", funcs[s].name);
        break;
      }
    }
  }
}

void inc_test(uint64_t s0, uint64_t s1)
{
}

void error_line(quat_t* E, quat_t* A, quat_t* B, float t, uint32_t s)
{
  quat_t R,D;
  
  printf("%15s: ", funcs[s].name);
  funcs[s].f(&R, A, B, t);
  float  a = a_error(&R, E);

  quat_print(&R);
  quat_print(A);
  quat_print(B);
  printf(" : %f\n%17s", a,": ");
  
  quat_sub(&D,E,&R);
  quat_print(E);
  quat_printa(&D);
  printf("[ %f ]\n",t);
}


void spot_dump(uint64_t s0, uint64_t s1)
{
  quat_t A,B,E;
  float  t;

  reset_generators(s0,s1);
  t = rng_f32();
  
  quat_qr_p(&A);
  quat_qr_p(&B);
  printf("%f (%f) ",t, fabsf(quat_dot(&A,&B)));
  quat_print(&A);
  quat_print(&B); ln();

  slerp_ref_ref(&E,&A,&B, t);
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    slerp_func_t slerp = funcs[s].f;
    quat_t R,D;

    printf("%15s: ", funcs[s].name);
    quat_set(&R,0,0,0,0);  // make macro
    slerp(&R, &A,  &B,  t);
    float  a = a_error(&R,&E);
    quat_sub(&D,&E,&R);
    quat_print(&R);
    quat_printa(&D);
    printf("%f\n",a);
    //ln();
  }
}


#if 0
static inline float dot_flatten(float d)
{
  d = (d <= 1.f) ? d : 2.f-d;
  d = d/sqrtf((1-d)*(1+d));
}
#endif

void brute_force_error(uint64_t s0, uint64_t s1)
{
  quat_t A,B,NA,NB;
  quat_t R0,R1;
  float  t,mt;

  printf("* brute_force\n");
  mt = 0.f;
  
  for(uint32_t s=0; s<NUM_FUNCS; s++) {
    slerp_func_t slerp = funcs[s].f;
  //uint32_t r = 0;
    float  e = 0.f;

    reset_generators(s0,s1);
    
    for(uint32_t i=0; i<SYM_TRIALS; i++) {
      float  d;

      t = rng_f32();
      quat_qr_p(&A);
      quat_qr_p(&B);
      slerp_ref_ref(&R0, &A, &B, t);
      slerp(&R1, &A, &B, t);

      d = a_error(&R0, &R1);

      if (d > e) {
	mt = t;
	quat_dup(&NA,&A);
	quat_dup(&NB,&B);
	e = d;
      }
    }

    slerp_ref_ref(&R0, &NA, &NB, mt);
    error_line(&R0, &NA, &NB, mt, s);
  }
}


int main()
{
  uint64_t s0;
  uint64_t s1;
  
  {
    s0 = _rdtsc();
    s1 = 0x1234567;
  }

  printf("SANITY TESTS\n");
  //sym_test(s0,s1); 
  //ortho_test(s0,s1); 
  //spot_dump(s0,s1);
  brute_force_error(s0,s1);

  return 0;
}

