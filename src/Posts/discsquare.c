// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/math/2017/01/08/SquareDisc.html
// Scalar only, more toward clairty than performance

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <x86intrin.h>
#include <math.h>

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

// simple defs (union not actually used)
typedef union { struct{ float x,y; }; float f[2]; } vec2_t;

void vec2_printa(vec2_t* v)
{
  printf("(%a %a) ",v->x,v->y);
}

void vec2_print(vec2_t* v)
{
  printf("(%f %f) ",v->x,v->y);
}

void vec2_sub(vec2_t* d, vec2_t*a, vec2_t* b)
{
  d->x = a->x - b->x;
  d->y = a->y - b->y;
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

// 


//****** stretch

#define EPS F32_MIN_NORMAL

void map_rs_sd(vec2_t* d, vec2_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = x2 > y2 ? x : y;
  m = fabsf(m)*rsqrt(x2+y2+EPS);
  d->x = m*x;
  d->y = m*y;
}

void map_rs_ds(vec2_t* d, vec2_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = x2 > y2 ? x : y;
  float r  = recip(fabsf(m)+EPS);
  m = sqrtf(x2+y2)*r;
  d->x = m*x;
  d->y = m*y;
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
void map_con_ds(vec2_t* d, vec2_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = sqrtf(x2+y2);

  if (x2 > y2) {
    float r = recip(fabs(x));
    d->x = mulsgn(x, m);
    d->y = m*(4*RPI)*atanf(y*r);
  }
  else {
    float r = recip((fabsf(y)+EPS));
    d->x = m*(4*RPI)*atanf(x*r);
    d->y = mulsgn(y, m);
  }
}

// sin(pi*x/4): order-3, end-point constrained, min-abs-err on [-1,1]
float ssin_a2(float x) { return x*(25711.f/32768.f-x*x*5202995.f/67108864.f); }

void map_con_sd_a2(vec2_t* d, vec2_t* s)
{
  float x = s->x;
  float y = s->y;
  float t;

  if (fabsf(x) > fabsf(y)) {
    float r = recip(x);
    t = ssin_a2(y*r);
    d->x = x*sqrtf(1.f-t*t);
    d->y = x*t;
  }
  else {
    float r = recip(y+EPS);
    t = ssin_a2(x*r);
    d->x = y*t;
    d->y = y*sqrtf(1.f-t*t);
  }
}



//****** approximate equal area

// square->disc
void map_aea_sd(vec2_t* d, vec2_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;

  if (x2 > y2) {
    float m = sgn(x);
    d->y = 0.5f*SQRT2*y;
    d->x = m*sqrtf(x2-0.5f*y2);
  } else {
    float m = sgn(y);
    d->x = 0.5f*SQRT2*x;
    d->y = m*sqrtf(y2-0.5f*x2);
  }
}

// disc->square
void map_aea_ds(vec2_t* d, vec2_t* s)
{
  float x  = s->x;
  float y  = s->y;
  float x2 = x*x;
  float y2 = y*y;
  float m  = sqrtf(x2+y2);

  if (x2 > y2) {
    d->y = SQRT2*y;
    d->x = mulsgn(x, m);
  } else {
    d->x = SQRT2*x;
    d->y = mulsgn(y, m);
  }
}

// Silly little round-trip test and spew out max sum-of-abs-diff as seen.
// Only purpose is to catch bad translation of math to code.
void test()
{
  float max = ULP1;
  
  for(uint32_t i=0; i<0xFFFFFFF; i++) {
    vec2_t p,d,r,diff;
    p.x = 2.f*rng_f32()-1.f;
    p.y = 2.f*rng_f32()-1.f;
    map_con_sd_a2(&d,&p);
    map_con_ds(&r,&d);
    vec2_sub(&diff, &p, &r);

    float t = fabs(diff.x)+fabs(diff.y);
    if (t > max) {
      max = t;
      vec2_printa(&diff); vec2_print(&p); vec2_print(&r); ln();
    }
  }
}

int main(int argc, char** argv)
{
  uint64_t s0;
  uint64_t s1;
  
  rng_state[0] = s0 = _rdtsc();
  rng_state[1] = s1 = 0x1234567;
  rng_u64();
  test();
  
  return 0;
}
