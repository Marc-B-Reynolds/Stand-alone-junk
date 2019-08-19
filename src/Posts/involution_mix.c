// Public Domain under http://unlicense.org, see link for details.

// http://marc-b-reynolds.github.io/

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// if defined run testu01 (needs to be installed if enabled) rabbit battery 
//   clang -O3 -Wall -Wextra -o foo involution_mix.c -lm -ltestu01 -lmylib
//#define TESTU01

// number of draws for avalanche test
//#define SAC_LEN 0x00800000  // post uses this
#define SAC_LEN 0x003fffff
//#define SAC_LEN 0x0003ffff
//#define SAC_LEN 0x000fffff

//#define DUMP_FIGURE

// if defined brute force check function is an involution
//#define VALIDATE_INVOLUTION

#if defined(TESTU01)
#define RABBIT

// number of draws for rabbit test
#define RABBIT_DRAWS 2048.0

// run smallcrush battery if defined
#define SMALLCRUSH

// run crush battery if defined (FYI: call it 25-30 minute per hash function)
//#define CRUSH
#endif

// function pointers
typedef uint32_t (*hash32_t)(uint32_t);
typedef uint64_t (*hash64_t)(uint64_t);

typedef enum
{
  F_INVOL = 1<<0	      
} flag_t;

typedef struct {
  hash32_t f;
  uint32_t flags;
  char*    name;
} hash32_table_t;

//---------------------------------------

void u64_array_print(uint64_t* d, uint32_t n)
{
  printf("{");

  for (uint32_t i=0; i<n-1; i++)
    printf("%ld,", d[i]);

  printf("%ld}\n", d[n-1]);
}

void u64_array_print_f(uint64_t* d, uint32_t n, double s)
{
  printf("{");

  for (uint32_t i=0; i<n-1; i++)
    printf("%f,", s*(double)d[i]);

  printf("%f}\n", s*(double)d[n-1]);
}

void u64_array_print_e(uint64_t* d, uint32_t n, double s)
{
  printf("{");

  for (uint32_t i=0; i<n-1; i++)
    printf("%e,", s*(double)d[i]);

  printf("%e}\n", s*(double)d[n-1]);
}

static uint64_t rng_state[2] = {0xac564b0527d4eb2d, 0};

inline uint64_t rotl(const uint64_t v, int i)
{
#if defined(__clang__)    
  return __builtin_rotateleft64(v,(uint32_t)i);
#else
  return (v << i)|(v >> (64-i));
#endif  
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

inline uint32_t rng_u32(void)
{
  return (uint32_t)rng_u64();
}
// end of PRNG


// Sobol stuff
typedef struct { uint32_t i, d0; } sobol32_t;
typedef struct { uint64_t i, d0; } sobol64_t;

sobol32_t sobol32 = { .d0=0, .i=~0u };
sobol64_t sobol64 = { .d0=0, .i=~0u };

inline void sobol32_init(uint32_t hash)
{
  sobol32.d0 = hash;
  sobol32.i  = ~0u;
}

inline void sobol64_init(uint64_t hash)
{
  sobol64.d0 = hash;
  sobol64.i  = ~0u;
}

inline void sobol32_update()
{
  uint32_t c = (uint32_t)__builtin_ctz(sobol32.i);
  sobol32.d0 ^= 0x80000000 >> c;
  sobol32.i  -= 1;    
}

inline void sobol64_update()
{
  uint64_t c = (uint64_t)__builtin_ctzl(sobol64.i);
  sobol64.d0 ^= 0x8000000000000000LL >> c;
  sobol64.i  -= 1;    
}

inline uint32_t sobol32_next()
{
  sobol32_update();
  return sobol32.d0;
}

inline uint64_t sobol64_next()
{
  sobol64_update();
  return sobol64.d0;
}



// rotate left
inline uint32_t rot(uint32_t x, uint32_t i)
{
#if defined(__clang__)    
  return __builtin_rotateleft32(x,i);
#elif defined(_MSC_VER)
  return _rotl(x,i);
#else  
  return (x << i) | (x >> (-i & 31));
#endif  
}

inline uint32_t xor_rot2(uint32_t x, uint32_t a, uint32_t b)
{
  return x^rot(x,a)^rot(x,b);
}

inline uint32_t xor_rot3(uint32_t x, uint32_t a, uint32_t b, uint32_t c)
{
  return rot(x,a)^rot(x,b)^rot(x,c);
}

#define UNUSED __attribute__((unused))

// a uniform random number for baseline
uint32_t ref(UNUSED uint32_t h)
{
  // the XORing of hi into lo bits is overkill..but why not
  uint64_t u = rng_u64();
  u ^= u >> 32;
  return (uint32_t)u;
}

// wang hash
uint32_t wang(uint32_t h)
{
  h = (h ^ 61) ^ (h >> 16);
  h *= 9;
  h = h ^ (h >> 4);
  h *= 0x27d4eb2d;
  h = h ^ (h >> 15);
  return h;
}

// xxHash
uint32_t xxh32(uint32_t h)
{
  h ^= h >> 15; h *= 0x85ebca77;
  h ^= h >> 13; h *= 0xc2b2ae3d;
  h ^= h >> 16;
  return h;
}

// MurmurHash3
uint32_t mh3(uint32_t h)
{
  h ^= h >> 16; h *= 0x85ebca6b;
  h ^= h >> 13; h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

// https://github.com/skeeto/hash-prospector
uint32_t hp0(uint32_t x)
{
  x ^= x >> 16; x *= 0x7feb352d;
  x ^= x >> 15; x *= 0x846ca68b;
  x ^= x >> 16;
  return x;
}

// https://github.com/skeeto/hash-prospector
uint32_t hp1(uint32_t x)
{
  x ^= x >> 17; x *= 0xed5ad4bb;
  x ^= x >> 11; x *= 0xac4c1b51;
  x ^= x >> 15; x *= 0x31848bab;
  x ^= x >> 14;
  return x;
}

// pcg_output_rxs_m_xs_32_32
uint32_t pcg(uint32_t x)
{
  uint32_t t = ((x >> ((x >> 28) + 4)) ^ x) * 277803737;
  return (t >> 22) ^ t;
}

// bad idea #1
uint32_t g0(uint32_t x)
{
  if (x & 1) {
    x *= 0xac564b05;
    x += 0x85ebca77;
  }
  else {
    x -= 0x85ebca77;
    x *= 0xdc33c9cd;
  }
  return x;
}

#define M0 0x5f356495
#define M1 0x32c446bd

//#define M0 0xac564b05
//#define M1 0xdc33c9cd

//#define M0 0x2c9277b5
//#define M1 0xcc2bfe9d
//#define M0 0x34ed9de5
//#define M1 0xec2c57ed
//#define M0 0x01c8e815
//#define M1 0x608fa73d

uint32_t f0(uint32_t x)
{
  x *= M0;
  x ^= (x>>25);
  x *= M1;
  return x;
}

// involution rot2 is (x,K,K+16)

uint32_t f1(uint32_t x)
{
  x *= M0;
  x = xor_rot2(x,6,22);
  x *= M1;
  return x;
}

uint32_t f2(uint32_t x)
{
  x ^= x >> 16;
  x *= M0;
  x = xor_rot2(x,6,22);
  x *= M1;
  x ^= x >> 16;
  return x;
}

uint32_t f3(uint32_t x)
{
  x = xor_rot2(x,11,16); x *= M0;
  x = xor_rot2(x, 6,22); x *= M1;
  x = xor_rot3(x,10,21,26);
  return x;
}

// goodness of fit statstics
typedef struct {
  double perBitX2;
  double iBitX2;
  double oBitX2;
  double maxBias;
  double iMaxBias;
  double oMaxBias;
} gof_t;

// oi = 1st index: bit position of flipped input
//      2nd index: bit position in output
typedef struct {
  uint64_t oi[32][32];  // per bit position counts
  uint64_t oiI[32];     // output bit
  uint64_t oiO[32];     // input bit
  uint64_t pop[33];     // population count
  gof_t    le;          // gof for low entropy input
  gof_t    he;          // gof for high entropy input
  uint32_t n;           // number of inputs processed
} stats32_t;

double bias_max(uint64_t* d, double s, uint32_t n)
{
  double max = 0.0;

  for(uint32_t i=0; i<n; i++) {
    double t = fabs(fma((double)d[i],s, -1.0));
    max = (max >= t) ? max : t;
  }
  
  return max;
}

// hack dump for blog plots
void bias_array(uint64_t* d, double s, uint32_t w, uint32_t h)
{
  printf("[");
  do {
    h--;
    uint32_t id = w*h;
    
    for(uint32_t x=0; x<w; x++) {
      double v = fma((double)d[id+x], s, -1.0);
      uint32_t m = (uint32_t)round(255.0*v);
      printf("%d,",m);
    }
  } while(h != 0);
  printf("]\n");
}




// Core of computing a chi-squared statistic where the expected
// distribution is uniform:  Sum[(oi[n]-e)^2]
//   oi: observed counts
//   e:  expected counts (scaled probablity)
//   n:  length of array (oi)
// The final division (and any extra required scaling) is up
// to the caller.
static double gof_chi_squared_eq(uint64_t* oi, uint32_t n, double e)
{
#if 0
  double r = 0.0;

  for (uint32_t i=0; i<n; i++) {
    double d = ((double)oi[i])-e;
    r = fma(d,d,r);
  }

  return r;
#else
  double r0 = 0.0;
  double r1 = 0.0;
  double r2 = 0.0;
  double r3 = 0.0;

  uint32_t j = n >> 2;
  uint32_t i = 0;

  while(j != 0) {
    double d0 = ((double)oi[i  ])-e;
    double d1 = ((double)oi[i+1])-e;
    double d2 = ((double)oi[i+2])-e;
    double d3 = ((double)oi[i+3])-e;
    r0 = fma(d0,d0,r0);
    r1 = fma(d1,d1,r1);
    r2 = fma(d2,d2,r2);
    r3 = fma(d3,d3,r3);
    i += 4; j--;
  }

  double re = 0.0;

  n &= 3;
  
  while (n != 0) {
    double d = ((double)oi[i])-e;
    re = fma(d,d,re);
    i++; n--;
  }
  return r0+r1+r2+r3+re;
#endif  
}

// walk the per-bit data and compute a chi-squared statistic
static double bias_chi_squared(stats32_t* s)
{
  uint32_t n = s->n;
  
  // each bit as p=.5 of being flipped so expected counts per bin is n/2
  double ei   = (double)n*0.5;
  double base = sqrt(gof_chi_squared_eq(&s->oi[0][0], 32*32, ei));

  // the 'n' input samples have each bit flipped (the extra 32 factor)
  return (100.0/(32.0*ei))*base;
}

// clear all incremental count storage and
// reset any PRNG and/or LDS to common starting point
void bias_clear_counts(stats32_t* s)
{
  s->n = 0;
  
  memset(&s->oi,  0, sizeof(s->oi));
  memset(&s->oiI, 0, sizeof(s->oiI));
  memset(&s->oiO, 0, sizeof(s->oiO));
  memset(&s->pop, 0, sizeof(s->pop));

  rng_state[0] = 0xac564b0527d4eb2d;
  rng_state[1] = 0x85ebca77;

  sobol32_init(0);
}

void sac_gather_oio(stats32_t* s)
{
  for (uint32_t j=0; j<32; j++) {
    uint64_t sum = 0;
    for (uint32_t i=0; i<32; i++) {
      sum += s->oi[i][j];
    }
    s->oiO[j] = sum;
  }
}

// gather counts with low-entropy input
void sac_le_count_32(stats32_t* s, hash32_t f, uint32_t n)
{
  s->n += n;
  
  // walk the 'n' samples
  for(uint32_t i=0; i<n; i++) {
    uint32_t x = sobol32_next();
    uint32_t h = f(x);

    // flip each bit of input 'x'
    for (uint32_t i=0; i<32; i++) {
      uint32_t t   = h ^ f(x ^ (1<<i));
      uint32_t pop = 0;

      // gather bit-by-bit counts
      for (uint32_t j=0; j<32; j++) {
	uint32_t b = (t >> j) & 1;
	pop         += b;
	s->oi[i][j] += b;
      }

      s->oiI[i] += pop;
      s->pop[pop]++;
    }
  }
}

// gather counts with 'n' high-entropy values (add extra count gathering of le variant)
void sac_he_count_32(stats32_t* s, hash32_t f, uint32_t n)
{
  s->n += n;
  
  for(uint32_t i=0; i<n; i++) {
    uint32_t x = rng_u32();
    uint32_t h = f(x);

    for (uint32_t i=0; i<32; i++) {
      uint32_t t   = h ^ f(x ^ (1<<i));
      uint32_t pop = 0;

      for (uint32_t j=0; j<32; j++) {
	uint32_t b = (t >> j) & 1;
	pop         += b;
	s->oi[i][j] += b;
      }

      s->oiI[i] += pop;
      s->pop[pop]++;
    }
  }
}

// gather counts with very low-entropy input {0,1,2...}
void sac_vle_count_32(stats32_t* s, hash32_t f, uint32_t n)
{
  s->n += n;
  
  for(uint32_t i=0; i<n; i++) {
    uint32_t x = i;
    uint32_t h = f(x);

    for (uint32_t i=0; i<32; i++) {
      uint32_t t   = h ^ f(x ^ (1<<i));
      uint32_t pop = 0;

      for (uint32_t j=0; j<32; j++) {
	uint32_t b = (t >> j) & 1;
	pop         += b;
	s->oi[i][j] += b;
      }

      s->oiI[i] += pop;
      s->pop[pop]++;
    }
  }
}


void gof_print(gof_t* gof)
{
  printf("max: %g, chi: %f\n", gof->maxBias,gof->perBitX2);
}

// helper: chi-squared for oiI & oiO
double bias_chi_squared_rc(stats32_t* s, uint64_t* o)
{
  double e = (32.0/2.0)*(double)s->n;
  double r = sqrt(gof_chi_squared_eq(o, 32, e));
  return (100.0/e)*r;
}

void sac_gof_32(stats32_t* s, gof_t* gof)
{
  // 1/expected
  double ie = 2.0/s->n;
  
  sac_gather_oio(s);
  gof->perBitX2 = bias_chi_squared(s);
  gof->iBitX2   = bias_chi_squared_rc(s,&s->oiI[0]);
  gof->oBitX2   = bias_chi_squared_rc(s,&s->oiO[0]);
  gof->maxBias  = 100.0*bias_max(&s->oi[0][0], ie, 32*32);
  gof->iMaxBias = 100.0*bias_max(&s->oiI[0],   ie*(1.0/32.0), 32);
  gof->oMaxBias = 100.0*bias_max(&s->oiO[0],   ie*(1.0/32.0), 32);

#if defined(DUMP_FIGURE)  
  bias_array(&s->oi[0][0], ie, 32,32);
#endif  
}

// helpers: gather 'n' samples and gen all statistics

void sac_vle_32(stats32_t* s, hash32_t f, uint32_t n)
{
  sac_vle_count_32(s,f,n);
  sac_gof_32(s, &s->le);
}

void sac_le_32(stats32_t* s, hash32_t f, uint32_t n)
{
  sac_le_count_32(s,f,n);
  sac_gof_32(s, &s->le);
}

// helper: gather 'n' samples and gen all statistics
void sac_he_32(stats32_t* s, hash32_t f, uint32_t n)
{
  sac_he_count_32(s,f,n);
  sac_gof_32(s, &s->he);
}


#if defined(TESTU01)

#include "gdef.h"
#include "swrite.h"
#include "bbattery.h"
#include "scomp.h"
#include "swalk.h"
#include "svaria.h"
#include "util.h"
#include "unif01.h"
#include "swrite.h"

#include <float.h>

static uint32_t state; 

static uint32_t next_u32(UNUSED void* p, hash32_t func)
{
  return func(state++);
}

static double next_f64(void* p, void* s)
{
  uint32_t a = next_u32(p,s);
  uint32_t b = next_u32(p,s);
  uint64_t i = (((uint64_t)a) << 21) + (((uint64_t)b) >> 11);
  
  return (double)i*0x1.0p-53;
}

static void print_state(UNUSED void* s)
{
  printf("  S = 0x%04x\n", state);
}

static unif01_Gen tu01 =
{
 .name = "temp",
 .GetU01  = (void*)&next_f64,
 .GetBits = (void*)&next_u32,
 .Write   = &print_state
};

#endif

// brute-force check that 'func' is an involution
void rt_sanity(hash32_t func)
{
  printf("   round-trip: ");

  uint32_t i=0;
  do {
    uint32_t r = func(func(i));

    if (r != i) {
      printf("failed\n");
      return;
    }
    
  } while(++i != 0);

  printf("pass\n");
}


static void test_32(hash32_table_t* t)
{
  hash32_t f = t->f;

#if 1  
  stats32_t stats = {.oi={{0}}};

  printf("TESTING: %s\n", t->name);
  sac_vle_32(&stats,f, SAC_LEN);
  printf("cn: "); gof_print(&stats.le);

  bias_clear_counts(&stats);
  sac_le_32(&stats,f, SAC_LEN);
  printf("le: "); gof_print(&stats.le);

  bias_clear_counts(&stats);
  sac_he_32(&stats,f, SAC_LEN);
  printf("he: "); gof_print(&stats.he);
#endif  

#if defined(DUMP_RAW_EXTRA)
  printf("oiI:"); u64_array_print(&stats.oiI[0], 32);
  printf("oiO:"); u64_array_print(&stats.oiO[0], 32);
  printf("pop:"); u64_array_print(&stats.pop[0], 32);
#endif  
  

#if defined(TESTU01)
  state      = 0;
  tu01.state = f;
  tu01.name  = t->name;

#if defined(RABBIT)  
  bbattery_Rabbit(&tu01, RABBIT_DRAWS*32.0);
#endif

#if defined(SMALLCRUSH)  
  bbattery_SmallCrush(&tu01);
#endif

#if defined(CRUSH)  
  bbattery_Crush(&tu01);
#endif

#endif
  
#if defined(VALIDATE_INVOLUTION)
  if (t->flags & F_INVOL)
    rt_sanity(f);
#endif

  printf("-------------\n");
}


#define M0 0x5f356495
#define M1 0x32c446bd

uint32_t foo(uint32_t x)
{
  x ^= x >> 17;
  x *= M0;
  x ^= x >> 25;
  x *= M1;
  x ^= x >> 17;
  return x;
}

static hash32_table_t hash32_table[] =
{
#if 1
 {.f = &ref,    .name = "baseline",    .flags=0 },
 {.f = &g0,     .name = "g0",          .flags=F_INVOL },
 {.f = &f0,     .name = "f0",          .flags=F_INVOL },
 {.f = &f1,     .name = "f1",          .flags=F_INVOL },
 {.f = &f2,     .name = "f2",          .flags=F_INVOL },
 {.f = &f3,     .name = "f3",          .flags=F_INVOL },
//{.f = &wang,   .name = "wang",        .flags=0 },
 {.f = &xxh32,  .name = "xxhash32",    .flags=0 },
 {.f = &mh3,    .name = "murmurhash3", .flags=0 },
 {.f = &hp0,    .name = "lowbias",     .flags=0 },
 {.f = &hp1,    .name = "triple32",    .flags=0 },
//{.f = &pcg,    .name = "pcg",         .flags=0 }
#else
 {.f = &foo,    .name = "foo",         .flags=0 }
#endif 
};

static const uint32_t num_hash32 = sizeof(hash32_table)/sizeof(hash32_table[0]);

int main()
{
#if defined(TESTU01)
  swrite_Basic = FALSE; // only print summary
#endif  

  for(uint32_t i=0; i<num_hash32; i++)
    test_32(hash32_table+i);

  return 0;
}

