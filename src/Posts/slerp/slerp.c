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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#if 1 // LAZY HERE
#include <x86intrin.h>
#include "xmmintrin.h"

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

static inline float recip_nr(float x)
{
  float x0 = recip_a(x);
  return x0*(2.f - x*x0); // nope!
}  

#endif

// compile time configuration options

// enable to test with both pseudo-random and Sobol sequences
#define USE_SOBOL
#define SOBOL_IMPLEMENTATION
#define SOBOL_EXTRAS

#include "../../SFH/quat.h"
#include "../../SFH/Sobol.h"

#define TRIALS 10 // very temp hack

sobol_2d_t qrng;

// xoroshiro128+

uint64_t rng_state[2];

#if defined(_MSC_VER)
#define TO_FP32 (1.f/16777216.f)
#define SLERP_CUT 0x1.ffffdep-1
#else
#define TO_FP32 0x1p-24f
#define SLERP_CUT 0x1.ffffdep-1
//#define SLERP_CUT .9f
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

// uniform on disk
float uniform_disk(vec2_t* p)
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

// uniform on quarter disk {x,y}>=0
float uniform_qdisk(vec2_t* p)
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

void uniform_s1(vec2_t* p)
{
  float d,s,tx;
  vec2_t v;

  d  = uniform_disk(&v);
  s  = rsqrt_nr(d);
  tx = v.x + v.x;
  p->x = s*(tx*v.x-d);
  p->y = s*(tx*v.y);
}

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

// uniform dist with all positive components (pseudo-random)
void quat_pr_p(quat_t* q)
{
  vec2_t p0,p1;
  float  d0 = uniform_qdisk(&p0);
  float  d1 = uniform_qdisk(&p1);
  float  s  = sqrtf((1.f-d0)/d1);
  
  quat_set(q, p0.x, p0.y, s*p1.x, s*p1.y);
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

static inline void quat_normalize(quat_t* r)
{
  float s = sqrtf(quat_norm(r));
  quat_scale(r,s);
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

void quatd_print(quatd_t* q)
{
  printf("%f+(%f,%f,%f) ",q->w,q->x,q->y,q->z);
}

void quatd_printa(quatd_t* q)
{
  printf("%a+(%a,%a,%a) ",q->w,q->x,q->y,q->z);
}


void slerp_nutsy_ref(quat_t* R, quat_t* A, quat_t* B, float T)
{
  quatd_t a,b,m;
  double  c,f;
  double  t=T;

  // convert to double
  quat_to_d(&a, A);
  quat_to_d(&b, B);

  // insure a & b are in the same half-sphere
  // near meanless error around dot(a,b) approaching zero
  c = quatd_dot(&a,&b);
  
  if (quatd_dot(&a,&b) < 0.0) {
    quatd_neg(&b);
    c = -c;
  }
  
  do {
    // bisector (relative half-angle)
    quatd_add(&m, &a, &b);
    quatd_scale(&m, 0.5);    // no error
    double d = quatd_dot(&m, &a);
    f = sqrt(d);
    quatd_sdiv(&m, f);
    
    t += t;

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

    //quatd_print(&m);
    //printf("%2d %f %f %f %a %a\n", n, c, s, d,f,t);
    //n++;
  } while(f != 1.0);

  quatd_store(R, &m);
}

// geometric form directly using doubles. For sanity-checking
// the sanity-checking version.
void slerp_ref_ref(quat_t* R, quat_t* A, quat_t* B, float t)
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


// straight forward acos->atan2 version using all std funcs
void slerp_ref_0(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabs(d);
  
  if (d < SLERP_CUT) {
    float s = sqrtf(1-d*d);
    float a = atan2f(s,d);
    float c = cosf(t*a);
    s1 = sqrtf(1-c*c)/s;
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

  d = fabs(d);

  if (d < SLERP_CUT) {
    t += t;                      // account for half-angle atan
    float s2 = 1.f-d*d;
    float i  = recip_nr(1.f+d);  // ~x^-1  to at least 12-bit, 1 NR step
    float rs = rsqrt_nr(s2);     // ~x^-.5 to at least 12-bit, 1 NR step
    float y  = s2*rs;          
    float a  = atanf(y*i);       // still range reduction
    float s  = sinf(t*a);        // still range reduction
    float sm = 1.f-s*s;
    float ic = rsqrt_nr(sm);
    s1 = s*rs;                   // sgn*s1*B can be computed while 'ic', etc in progress
    s0 = sm*ic-d*s1;
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

  d = fabs(d);
  
  if (d < SLERP_CUT) {
    float s2 = 1.f-d*d;
    float i  = recip_nr(1.f+d);
    float rs = rsqrt_nr(s2);   
    float y  = s2*rs;          
    float a  = atanf(y*i);     
    float c  = cosf(t*a);      // opposite choice just to show
    float s  = sqrtf(1-c*c);   // and std root
    float tc = c+c;
    float cx = tc*c-1.f;
    float sx = tc*c*s;

    s1 = sx*rs;
    s0 = cx-d*s1;
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

  d = fabs(d);
  
  if (d < SLERP_CUT) {
    // 
    float s2 = 1.f-d*d;
    float i  = recip_nr(1.f+d);
    float rs = rsqrt_nr(s2);   
    float y  = s2*rs;          
    float a  = atanf(y*i);        // lazy

    // forward trig approx (or root for one)
    float ta = t*a;
    float c  = cos_4_5(ta);       // ~cos on [0, pi/4]
    float s  = sin_4_5(ta);       // ~sin on [0, pi/4]

    // double angle
    float tc = c+c;
    float cx = tc*c-1.f;
    float sx = tc*c*s;

    s1 = sx*rs;
    s0 = cx-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}

// branch-free sketch:
void slerp_ref_4(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;
  float s0,s1;

  d = fabs(d);
  
  // lerp vs. slerp part not show
  if (d < SLERP_CUT) {

    // atan range reduction
    float t  = (d < 0.707106769084930419921875f) ? 1 : 0;
    float s2 = 1.f-d*d;
    float i  = recip_nr(d+1.f-t);  
    float rs = rsqrt_nr(s2);       
    float y  = s2*rs+t;            

    float a  = atanf(y*i)+t*(PI*.25f);

    // forward trig approx, double angle 
    float ta = t*a;
    float c  = cos_4_5(ta);
    float s  = sin_4_5(ta);
    float tc = c+c;
    float cx = tc*c-1.f;
    float sx = tc*c*s;

    s1 = sx*rs;
    s0 = cx-d*s1;
  } else {
    s0 = 1.0f - t;
    s1 = t;
  }

  quat_wsum(R, A, B, s0, sgn*s1);
}

// ****************
// precomputation

typedef struct {
  quat_t q0;
  quat_t q1;
  float  a;
} slerp_k_t;

void slerp_k_init(slerp_k_t* k, quat_t* A, quat_t* B)
{
  float d   = quat_dot(A,B);
  float sgn = d >= 0 ? 1 : -1;

  // lazy straight forward way
  quat_wsum(&k->q0, A, B, sgn, 1.f);
  quat_normalize(&k->q0);

  if (d < SLERP_CUT) {
    float x = quat_dot(&k->q0, B);
    float s = sqrtf(1-d*d);
    float b = atan2f(s,d);
    // nope
  }
  else {
    quat_dup(&k->q1, B);
    //k->a =  nope
  }
}

void slerp_k_0(quat_t* r, slerp_k_t* k, float t)
{
  float ta = t*k->a;
  float c  = cos_4_5(ta);
  float s  = sqrt(1-c*c);
  quat_wsum(r, &k->q0, &k->q1, c, s);
}

void slerp_k_1(quat_t* r, slerp_k_t* k, float t)
{
  float ta = t*k->a;
  float c  = cos_8_4(ta);
  float s  = sin_8_4(ta);
  float tc = c+c;
  float dc = tc*c-1.f;
  float ds = tc*c*s;

  quat_wsum(r, &k->q0, &k->q1, dc, ds);
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
  absCosom = fabs( cosom );

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
  absCosom = fabs( cosom );

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

// revalidate and generalize
const float u8[] = // 1 /[i (2 i + 1 )] for i >= 1
{ 1.f/(1*3), 1.f/(2*5), 1.f/(3*7), 1.f/(4*9), 1.f/(5*11), 1.f/(6*13), 1.f/(7*15), OP_MU_8/(8*17) } ;
const float v8[] = // i /(2 i + 1) for i >= 1
{ 1.f/3, 2.f/5, 3.f/7, 4.f/9, 5.f/11, 6.f/13, 7.f/15, OP_MU_8*8/17 } ;

void slerp_eberly_1(quat_t* R, quat_t* A, quat_t* B, float t)
{
  float x   = quat_dot(A,B);
  float sgn = x >= 0 ? 1 : -1;
  float xm1 = fabs(x)-1;
  float d   = 1-t;
  float t2  = t*t;
  float d2  = d*d;

  float s1 = sgn*t*(1.f + OP_MU_1/3.f*(t2-1.f)*xm1);
  float s0 =     d*(1.f + OP_MU_1/3.f*(d2-1.f)*xm1);

  quat_wsum(R, A, B, s0, s1);
}

void slerp_eberly_8_(quat_t* R, quat_t* A, quat_t* B, float t)
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

void slerp_zeux_2(quat_t* r, quat_t* a, quat_t* b, float t)
{
  float ca = quat_dot(a, b);
  float d  = fabsf(ca);
  float A  = 1.0904f + d * (-3.2452f + d * (3.55645f - d * 1.43519f));
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
//SF(slerp_ref_ref),
  SF(slerp_ref_0),
  SF(slerp_ref_1),
  SF(slerp_ref_2),
  SF(slerp_ref_3),
//SF(slerp_ref_4),
  SF(slerp_id_ref_0),
  SF(slerp_id_ref_1),
  SF(slerp_eberly_1),
  SF(slerp_eberly_8),
  SF(slerp_zeux_1),
  SF(slerp_zeux_2),
  SF(slerp_nlerp),
};

#define SLERP_REF slerp_ref_ref

#define NUM_FUNC (sizeof(funcs)/sizeof(funcs[0]))

void reset_generators(uint64_t s0, uint64_t s1)
{
  sobol_2d_init(&qrng, (uint32_t)s0, (uint32_t)(s0+(s0>>32)));
  rng_state[0] = s0;
  rng_state[1] = s1;
}

void quat_canonical(quat_t* q)
{
  if (q->w > 0) return;
  
  quat_neg(q,q);
}

#define D(X) printf("(" #X ") ")

int quat_identical(quat_t* a, quat_t* b)
{
  // structured for relaxed IEEE rules
  float t0,t1;

  t0  = (a->x - b->x);
  t1  = (a->x + b->x);
  t0 += (a->y - b->y);
  t1 += (a->y + b->y);
  t0 += (a->z - b->z);
  t1 += (a->z + b->z);
  t0 += (a->w - b->w);
  t1 += (a->w + b->w);
  t0  = fabs(t0);
  t1  = fabs(t1);
  t0  = (t0 < t1) ? t0 : t1;

  printf("(%a) ", t0);

  return 0;
}


void sym_test(uint64_t s0, uint64_t s1)
{
  quat_t A,B,E;
  quat_t NA,NB;
  float  t;

  reset_generators(s0,s1);
  t = rng_f32();  // change to neighboorhood of .2
  
  quat_qr_p(&A);
  quat_qr_p(&B);

  quat_set(&A,0,0,0,1);
  quat_set(&B,0,0,1,0);
  t = 1.f;

  
  quat_neg(&NA, &A);
  quat_neg(&NB, &B);

  SLERP_REF(&E,&A,&B,t);
  //quat_print(&A); ln();
  //quat_print(&B); ln();
  quat_print(&E); ln();
  
  for(uint32_t s=0; s<sizeof(funcs)/sizeof(funcs[0]); s++) {
    slerp_func_t slerp = funcs[s].f;

    quat_t R0,R1,R2,R3;

    // all permutations of signs should return numerically
    // identical results up to sign. No methods here have
    // any directionally dependent code such as choosing
    // an orthonogal direction as the relative quaternion
    // approaches one (identity).
    slerp(&R0, &A,  &B,  t);
    slerp(&R1, &A,  &NB, t);
    slerp(&R2, &NA, &B,  t);
    slerp(&R3, &NA, &NB, t);
    
    printf("\n\n---- %30s \n", funcs[s].name);
    quat_print(&R0); 
    quat_print(&R1); ln();
    quat_print(&R2); 
    quat_print(&R3); ln();
    printf("%d ",  quat_identical(&R0,&R1));
    printf("%d ",  quat_identical(&R0,&R2));
    printf("%d\n", quat_identical(&R0,&R3));

#if 0    
    t = 1.f-t;
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
#endif    
  }
}


int main()
{
  uint64_t s0;
  uint64_t s1;
  
  {
    s0 = __rdtsc();
    s1 = 0x1234567;
  }

  sym_test(s0,s1); return 0;


  for(uint32_t s=0; s<sizeof(funcs)/sizeof(funcs[0]); s++) {


    for(uint32_t i=0; i<TRIALS; i++) {
      float t = .1f;
      quat_t A,B,R0,R1,R2,R3,R4;
      
      quat_qr_p(&A);
      quat_qr_p(&B);
      
      slerp_nutsy_ref(&R0,&A,&B,t);    
      slerp_ref_ref(&R1,&A,&B,t);
      slerp_ref_0(&R2,&A,&B,t);
      slerp_ref_1(&R3,&A,&B,t);
      slerp_ref_2(&R4,&A,&B,t);
      
      //slerp_foo(&R3,&A,&B,t);
      //slerp_eberly_1(&R4,&A,&B,t);
      //id_slerp_ref_1(&R4,&A,&B,t);
      //slerp_zeux_2(&R4,&A,&B,t);
      //slerp_nlerp(&R4,&A,&B,t);
      
      
      quat_print(&R0); ln();
      quat_print(&R1); ln();
      quat_print(&R2); ln();
      quat_print(&R3); ln();
      quat_print(&R4); ln();ln();
      
      quat_printa(&R0); ln();
      quat_printa(&R1); ln();
      quat_printa(&R2); ln();
      quat_printa(&R3); ln();
      quat_printa(&R4); ln();
    }
  }
  
  return 0;
}

