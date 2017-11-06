// This is a battery test for:
//
// The testing will run until stopped, unless NUMBER_OF_RUNS is defined
//
// Building requires either of these libraries:
//   http://github.com/Marc-B-Reynolds/TestU01x
//   http://simul.iro.umontreal.ca/testu01/tu01.html
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <intrin.h>
//#include "x86intrin.h


#include "gdef.h"
#include "swrite.h"
#include "bbattery.h"
#include "scomp.h"
#include "swalk.h"
#include "svaria.h"

// if define test stream version, changing to a new stream
// each battery run.
#define TEST_STREAM

// if defined uses top bits of the two uniform integers
// instead of all of the first and remaining for the second.
//#define SANE_DOUBLE_CONVERT

// inject this note at the top of the output...for custom
// mixing or whatever other note desired.
#define NOTATION "default"

// Run with specified inital state. Undefined uses __rdtsc()
#define INITAL_STATE 0

#define NUMBER_OF_RUNS 10

// Setup whatever configuration to be tested


// if defined then the value is the number of time to run the
// battery...otherwise will continue until stopped.
//#define NUMBER_OF_RUNS 100

#define NAME "weyl"


#if defined(_MSC_VER)
#define inline _inline
#define I2F (1.0/((1.0*(1<<22))*(1.0*(1<<30))))
#else
#include <x86intrin.h>
#define I2F 0x1p-52f
#endif

#include "util.h"
#include "unif01.h"
#include "swrite.h"

//#include "../SFH/lprns.h"
#include <float.h>


uint32_t test[] = {
  0x703e132b, // 6/4/6     01110000001111100001001100101011 -0x8fc1ecd5
  0x8fc1ecd5, // 6/5/5     10001111110000011110110011010101 (3+Sqrt[17])/2
  0x58585903, // 5/7/4
  0x1f83d9ab, // 5/7/6     00011111100000111101100110101011  Sqrt[17]-4
  0x3504f335, // 6/6/6     00110101000001001111001100110101
  0x6a09e663, // 6/7/7     01101010000010011110011001100011
  0x2f83d9a5, // 6/6/7
  0x8df6f819, // 8/7/6
  0x6a09e665, // 6/8/8     01101010000010011110011001100101
  0x7f4a7c15, // 7/6/7     01111111010010100111110000010101  2^32 (1 + Sqrt[5])/2
  0x9df5f0b3, // 7/4/5
  0x78adf777, // 7/8/5     
  0x78adf775, // 6/6/7
  0xf1bbcdcb, // 9/7/8     11110001101110111100110111001011
  0x6a09e667, // 9/8/8     01101010000010011110011001100111  Sqrt[2]
  0x3504f333, // 8/10/9    00110101000001001111001100110011
  0x3504f333, // 8/10/9
  0x9e3779b9, // 9/10/10   (Sqrt[5]+1)/2
  0x144cbc89, // 13/9/9
  0xbb67ae87, // 13/8/8     1+Sqrt[3]
  0x2026fda7, // 10/7/8
  0x2003dbd7, // 17/17/19
  0x2fa0be83, // 18/17/20
  0x2002adff, // 22/22/23
};

uint32_t current_constant = 0x703e132b;
uint32_t state;
uint32_t state_backup;

typedef struct {
  uint32_t i;
  uint32_t m;
} lprns_stream_t;

// count trailing zeros (no zero input)
#ifdef _MSC_VER
static inline uint32_t ctz(uint32_t x) { unsigned long r; _BitScanForward(&r, (unsigned long)x); return (uint32_t)r; }
#else
#define ctz __builtin_ctz
#endif  

// next number with same population count
static inline uint32_t next_pop(uint32_t x)
{
  uint32_t a = x & (uint32_t)(-(int32_t)x);  // casting to silence warning
  uint32_t b = x + a;
  uint32_t c = x ^ b;
  uint32_t d = (2 + ctz(x));
  return b | (c >> d);
}

// Table[With[{p = Prime[n]}, {p, BaseForm[PowerMod[p, -1, 2^32], 16], BaseForm[Floor[(2^32 - 1)/p], 16]}], {n, 2, 20}]

// prime, mod_inverse(prime), floor((2^32-1)/prime)
typedef struct { uint32_t value, inv, max; } prime_t;

prime_t prime[] = {                // drop rate cumlative
  {   3, 0xaaaaaaab, 0x55555555 }, // 0.589441  0.589441
  {   5, 0xcccccccd, 0x33333333 }, // 0.189157  0.778598
  {   7, 0xb6db6db7, 0x24924924 }, // 0.092536  0.871133
  {  11, 0xba2e8ba3, 0x1745d174 }, // 0.036746  0.907879
  {  13, 0xc4ec4ec5, 0x13b13b13 }, // 0.026086  0.933965
  {  17, 0xf0f0f0f1, 0x0f0f0f0f }, // 0.015160  0.949125
  {  19, 0x286bca1b, 0x0d79435e }, // 0.012088  0.961213
  {  23, 0xe9bd37a7, 0x0b21642c }, // 0.008238  0.969451
  {  29, 0x4f72c235, 0x08d3dcb0 }, // 0.005167  0.974618
  {  31, 0xbdef7bdf, 0x08421084 }, // 0.004493  0.979111
  {  37, 0x914c1bad, 0x06eb3e45 }, // 0.003188  0.982299
  {  41, 0xc18f9c19, 0x063e7063 }, // 0.002590  0.984889
  {  43, 0x2fa0be83, 0x05f417d0 }, // 0.002337  0.987226
  {  47, 0x677d46cf, 0x0572620a }, // 0.001956  0.989182
  {  53, 0x8c13521d, 0x04d4873e }, // 0.001541  0.990723
  {  59, 0xa08ad8f3, 0x0456c797 }, // 0.001244  0.991967
  {  61, 0xc10c9715, 0x04325c53 }, // 0.001162  0.993129
  {  67, 0x07a44c6b, 0x03d22635 }, // 0.000961  0.994091
  {  71, 0xe327a977, 0x039b0ad1 }, // 0.000860  0.994950
  {  73, 0xc7e3f1f9, 0x0381c0e0 }, // 0.000814  0.995764
  {  79, 0x613716af, 0x033d91d2 }, // 0.000695  0.996459
  {  83, 0x2b2e43db, 0x03159721 }, // 0.000627  0.997086
  {  89, 0xfa3f47e9, 0x02e05c0b }, // 0.000545  0.997632
  {  97, 0x5f02a3a1, 0x02a3a0fd }, // 0.000458  0.998090
  { 101, 0x7c32b16d, 0x0288df0c }, // 0.000422  0.998511
  { 103, 0xd3431b57, 0x027c4597 }, // 0.000407  0.998918
  { 107, 0x8d28ac43, 0x02647c69 }, // 0.000378  0.999296
  { 109, 0xda6c0965, 0x02593f69 }, // 0.000365  0.999661
  { 113, 0x0fdbc091, 0x0243f6f0 }  // 0.000339  1.000000
};

// returns zero if prime factorization of 'x' has two or more
// of any primes in the table.
uint32_t prime_filter(uint32_t x)
{
  for (uint32_t i = 0; i<sizeof(prime) / sizeof(prime_t); i++) {
    uint32_t m = prime[i].inv*x;
    // does 'x' have the prime as a factor?
    if (m <= prime[i].max) {
      // yes it does so check for another
      if (prime[i].inv*m <= prime[i].max) return 0;
    }
  }

  return 1;
}

// number of runs of 1s in odd 'x'
uint32_t num_bitstrings(uint32_t x)
{
  uint32_t c = 1;

  x -= 1;

  while (x != 0) {
    c++;
    x >>= ctz(x);
    x += 1;
    x = x & (x - 1);
  }

  return c;
}

unif01_Gen* gen;

#define SWALK_DIM 2048
#define SPACING_DROP 0
#define SPACING_TAKE 30

void foo(void)
{
  scomp_Res*  res = scomp_CreateRes();
  swalk_Res*  walk = swalk_CreateRes();
  sres_Basic* basic = sres_CreateBasic();

  for (uint32_t i = 0; i<sizeof(test) / 4; i++) {
    current_constant = test[i];
    state = 0;
    //printf("  %08x %+f: ", current_constant, (int32_t)current_constant * (1.f/0x7fffffff));
    printf("  %08x: ", current_constant);
    scomp_LinearComp(gen, res, 1, 2048, 0, 32);
    printf("LinearComp = %f, ", res->JumpSize->pVal2[gofw_Mean]);
    scomp_LinearComp(gen, res, 1, 10362, 0, 32);
    printf("%f ", res->JumpSize->pVal2[gofw_Mean]);

    swalk_RandomWalk1(gen, walk, 1, 32, 0, 32, SWALK_DIM, SWALK_DIM);
    printf("Walk = %f ", walk->H[0]->pVal2[gofw_Mean]);

    svaria_AppearanceSpacings(gen, basic, 1, 2560, 2560, 0, 30, 6);
    printf("Appearance = %f ", basic->pVal2[gofw_Mean]);

    scomp_LempelZiv(gen, basic, 1, 8, 8, 24);
    printf("LempelZiv = %f, ", basic->pVal2[gofw_Mean]);
    scomp_LempelZiv(gen, basic, 1, 15, 8, 24);
    printf("%f ", basic->pVal2[gofw_Mean]);
   

    printf("\n");
  }
}


// min/max bitstrings to accept (runs of 1s)
// Not very happy about these values...needed ATM to
// cut candidate set.
#define MAX_BITSTRING 9
#define MIN_BITSTRING 7
#define BITSTRING_CNT 7

// number of runs of 1s in odd 'x'
uint32_t bitstrings(uint32_t x, uint32_t* count)
{
  uint32_t num = 0;

  for (uint32_t i = 0; i<BITSTRING_CNT; i++)
    count[i] = 0;

  while (x != 0) {
    uint32_t len = ctz(~x);

    x >>= len;
    len--;

    if (len >= BITSTRING_CNT) len = BITSTRING_CNT;
    count[len]++;

    num++;
    x >>= ctz(x);
  }

  return num;
}

uint32_t count_filter(uint32_t* cnt)
{
  if (cnt[0] > 9 || cnt[0] < 5) return 0;
  if (cnt[1] > 2) return 0;
  if (cnt[2] > 2) return 0;
  if (cnt[3] > 1) return 0;
  if (cnt[4] > 1) return 0;
  if (cnt[5] > 1) return 0;
  if (cnt[6] > 0) return 0;
  return 1;
}



// Start searching for constants from 'x'. 
// Stops when out of values with the same population count.
uint32_t search_from(uint32_t x)
{
  scomp_Res*  scomp = scomp_CreateRes();
  swalk_Res*  walk  = swalk_CreateRes();
  sres_Basic* basic = sres_CreateBasic();

  uint32_t c = 0;
  uint32_t visited = 0;
  uint32_t l;
  uint32_t cnt[BITSTRING_CNT];


  current_constant = x;

  do {
    // skip too small values
    if ((x >> (32 - 3)) != 0) {

      // skip values with too few or many bitstrings
      l = bitstrings(x, cnt);
      if (l >= MIN_BITSTRING && l <= MAX_BITSTRING) {

        // skip values that aren't square-free (only testing
        // a small number of explicit primes)
        if (count_filter(cnt) && prime_filter(x)) {
          double pval;
          visited++;
          current_constant = x;
          state = 0;

          //if ((visited & 0xFF)==0) {printf(".");}

           // quick LZ test: drop bottom 8 & use 24 bits from
           // each and build 2^8 tree
          scomp_LempelZiv(gen, basic, 1, 8, 8, 24);
          pval = basic->pVal2[gofw_Mean];

          if (pval > 0.4 && pval < 0.6) {
            scomp_LempelZiv(gen, basic, 1, 15, 0, 32);
            pval = basic->pVal2[gofw_Mean];

            if (pval > 0.127 && pval < 0.81) {
              
              
              // temp hack to bail
              //c++;  if (c > 0xffff) x = 0xf800000;

              scomp_LinearComp(gen, scomp, 1, 10362, 0, 32);
              pval = scomp->JumpSize->pVal2[gofw_Mean];

              if (pval > 0.134 && pval < 0.963) {
                printf("%08x %f\n", current_constant, pval);
              }
            }
          }
        }
      }
    }

    // next odd value with same population count
    x = (next_pop(x >> 1) << 1) | 1;
  } while ((int32_t)x > 0);

  printf("visited=%d (%08x)\n", visited, current_constant);

  return c;
}


// search with specified population count
uint32_t search_pop(uint32_t pop)
{
  uint32_t x = (1 << (pop-1)) - 1;

  x |= 0x2000000;

  return search_from(x);
}



//#define XOR_MIX

static uint32_t next_u32(void* p, void* s)
{
  uint32_t r = state * current_constant;

  state++;
#if defined(XOR_MIX)
  return r ^ (r >> 16);
#else
  return r;
#endif
}

// for double tests
static double next_f64(void* p, void* s)
{
  uint32_t l = next_u32(0,0) >> 6;
  uint32_t h = next_u32(0,0) >> 6;
  uint64_t i = l;
  i |= (h << (32-6));

  return i*DBL_EPSILON;
}

// these are for the adapative smallcrush and don't really have
// any real work to do at the moment.

static void* get_state(void* p, void* s)
{
  return &state_backup;
}

static void set_state(void* p, void* s)
{
  state = state_backup;
}


// dump start of test state: useful if one wanted to
// check questionable p-values (or to repeat failures
// sanity checks)
static void print_state(void* s)
{
  printf("  S = 0x%08x\n", state);
}

// 
unif01_Gen* createGenerator()
{
  unif01_Gen* gen = util_Malloc(sizeof(unif01_Gen));

  state = 0;
  gen->state = 0;
  gen->param = 0;
  gen->name = NAME;
  gen->GetU01  = (void*)&next_f64;
  gen->GetBits = (void*)&next_u32;
  gen->Write = &print_state;

#ifdef ADAPTIVE
  gen->getCurrentState = (void*)&get_state;
  gen->setCurrentState = (void*)&set_state;
#endif

  return gen;
}

void deleteGenerator(unif01_Gen* gen)
{
  if (gen != NULL) {
    //util_Free(gen->param);
    //util_Free(gen->state);
    util_Free(gen);
  }
}


int main(void)
{
  uint32_t    s = (uint32_t)__rdtsc();
  uint32_t    c = -1;
  uint32_t    t;

  gen = createGenerator();

  swrite_Basic = FALSE; // only print summary

  uint64_t z = s;

  foo(); return 0;
#if 0
  uint32_t num = search_pop(19);
  printf("num=%d\n", num);
  fflush(stdout);
  return 0;
#endif
  
  // dump out the configuration
  printf("  WEYL: 0x%08x\n", current_constant);

#ifdef   INITAL_STATE
  state = INITAL_STATE;
#else
  stat = s;
#endif

  // the state is initialized to a sobol sequence in
  // an attempt to get as good a coverage of the state
  // space as possible.
  
  do {
    // setting the raw state of the generator
    printf("run %d -- state = 0x%08x\n", -(int32_t)c, state);

    //sknuth_Gap(gen, res2, 1, 1024, 22, 0.0, .00390625);
    bbattery_Rabbit(gen, 32768.0);

#if defined(NUMBER_OF_RUNS)
        if (0 - c >= NUMBER_OF_RUNS) break;
#endif

    // could improve the domain of the search
    t = ctz(c) * 2;
    s ^= 0x80000000 >> t;
    c -= 1;

    //state = s;
  } while (1);

  deleteGenerator(gen);

  return 0;
}
