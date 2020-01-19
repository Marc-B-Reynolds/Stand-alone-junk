
// Public Domain under http://unlicense.org, see link for details.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <signal.h>

#include <x86intrin.h>

/************************************
 * general purpose utils
 ************************************/

#if !defined(_MSC_VER)

inline uint32_t rot_32(uint32_t x, uint32_t i)
{
#if defined(__clang__)
  return __builtin_rotateleft32(x,i);
#else
  return (x << i) | (x >> (-i & 31));
#endif
}

inline uint64_t rot_64(uint64_t x, uint64_t i)
{
#if defined(__clang__)
  return __builtin_rotateleft64(x,i);
#else
  return (x << i)|(x >> (-i & 64));
#endif
}

inline uint32_t popcnt_32(uint32_t x) { return (uint32_t)__builtin_popcount(x); }
inline uint32_t popcnt_64(uint64_t x) { return (uint32_t)__builtin_popcountll(x); }

inline uint32_t ctz_32(uint32_t x)    { return (uint32_t)__builtin_ctz(x); }
inline uint32_t ctz_64(uint64_t x)    { return (uint32_t)__builtin_ctzll(x); }
inline uint32_t clz_32(uint32_t x)    { return (uint32_t)__builtin_clz(x); }
inline uint32_t clz_64(uint64_t x)    { return (uint32_t)__builtin_clzll(x); }

#else

inline uint32_t rot_32(uint32_t x, int i) { return _rotl(x,i);  }
inline uint64_t rot_64(uint64_t x, int i) { return _rotll(x,i); }

#endif


inline uint32_t neg_32(uint32_t x) { return (uint32_t)(-(int32_t)x); }
inline uint64_t neg_64(uint64_t x) { return (uint64_t)(-(int64_t)x); }
inline uint32_t abs_32(uint32_t x) { return (int32_t)x >= 0 ? x : neg_32(x); }
inline uint64_t abs_64(uint64_t x) { return (int64_t)x >= 0 ? x : neg_64(x); }
inline uint32_t sgn_32(uint32_t x) { return (uint32_t)(((int32_t)x)>>31); }
inline uint64_t sgn_64(uint64_t x) { return (uint64_t)(((int64_t)x)>>63); }


//#define SWAP(X,Y) { __typeof__(X) t = X; X=Y; Y=t; }

#define SWAP_32(X,Y) { uint32_t t = X; X=Y; Y=t; }
#define SWAP_64(X,Y) { uint64_t t = X; X=Y; Y=t; }

uint32_t gcd_32(uint32_t u, uint32_t v)
{
  if (u != 0 && v != 0) {
    uint32_t s;
    s   = ctz_32(u|v);
    u >>= ctz_32(u);
    
    do {
      v >>= ctz_32(v);
      if (u > v) { SWAP_32(u,v); }
      v = v - u;
    } while (v != 0);
    
    return u<<s;
  }
  
  return u+v;
}


uint64_t gcd_64(uint64_t u, uint64_t v)
{
  if (u != 0 && v != 0) {
    uint64_t s;
    s   = ctz_64(u|v);
    u >>= ctz_64(u);
    
    do {
      v >>= ctz_64(v);
      if (u > v) { SWAP_64(u,v); }
      v = v - u;
    } while (v != 0);
    
    return u<<s;
  }
  
  return u+v;
}

uint32_t mod_inverse_32(uint32_t a)
{
  uint32_t x;
  x = (a*a)+a-1;
  x *= 2-a*x;
  x *= 2-a*x;
  x *= 2-a*x;
  return x;
}

uint64_t mod_inverse_64(uint64_t a)
{
  uint64_t x;
  x = (a*a)+a-1;
  x *= 2-a*x;
  x *= 2-a*x;
  x *= 2-a*x;
  x *= 2-a*x;
  return x;
}

// next value with same popcnt
uint32_t next_pop_32(uint32_t x)
{
  uint32_t a = x & neg_32(x);
  uint32_t b = x + a;
  uint32_t c = x ^ b;
  uint32_t d = (2 + ctz_32(x));
  return b | (c >> d);
}


uint64_t next_pop_64(uint64_t x)
{
  uint64_t a = x & neg_64(x);
  uint64_t b = x + a;
  uint64_t c = x ^ b;
  uint64_t d = (2 + ctz_64(x));
  return b | (c >> d);
}

// next odd value with same popcnt as the odd input
uint32_t next_pop_odd_32(uint32_t x)
{
  return (next_pop_32(x>>1)<<1)|1;
}

uint64_t next_pop_odd_64(uint64_t x)
{
  return (next_pop_64(x>>1)<<1)|1;
}

/************************************
 * PRNG/LDS
 ************************************/


// xoroshiro128+
inline uint64_t rng_u64(uint64_t rng_state[static 2])
{
  uint64_t s0 = rng_state[0];
  uint64_t s1 = rng_state[1];
  uint64_t r  = s0 + s1;
  
  s1 ^= s0;
  rng_state[0] = rot_64(s0,55) ^ s1 ^ (s1<<14);
  rng_state[1] = rot_64(s1,36);
  
  return r;
}

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
  uint32_t c = ctz_32(sobol32.i);
  sobol32.d0 ^= 0x80000000 >> c;
  sobol32.i  -= 1;    
}

inline void sobol64_update()
{
  uint64_t c = ctz_64(sobol64.i);
  sobol64.d0 ^= UINT64_C(0x8000000000000000) >> c;
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

/************************************
 * constant filtering utils
 ************************************/

// Table[With[{p = Prime[n]}, {p, BaseForm[PowerMod[p, -1, 2^32], 16], BaseForm[Floor[(2^32 - 1)/p], 16]}], {n, 2, max}]

// prime, mod_inverse(prime), floor((2^32-1)/prime)
typedef struct { uint32_t value, inv, max, max_l; } prime_t;

prime_t prime[] = {                            // drop rate cumlative
  {   3, 0xaaaaaaab, 0x55555555, 0x55555555 }, // 0.589441  0.589441
  {   5, 0xcccccccd, 0x33333333, 0x33333333 }, // 0.189157  0.778598
  {   7, 0xb6db6db7, 0x24924924, 0x92492492 }, // 0.092536  0.871133
  {  11, 0xba2e8ba3, 0x1745d174, 0x5d1745d1 }, // 0.036746  0.907879
  {  13, 0xc4ec4ec5, 0x13b13b13, 0xb13b13b1 }, // 0.026086  0.933965
  {  17, 0xf0f0f0f1, 0x0f0f0f0f, 0x0f0f0f0f }, // 0.015160  0.949125
  {  19, 0x286bca1b, 0x0d79435e, 0x50d79435 }, // 0.012088  0.961213
  {  23, 0xe9bd37a7, 0x0b21642c, 0x8590b216 }, // 0.008238  0.969451
  {  29, 0x4f72c235, 0x08d3dcb0, 0x8d3dcb08 }, // 0.005167  0.974618
  {  31, 0xbdef7bdf, 0x08421084, 0x21084210 }, // 0.004493  0.979111
  {  37, 0x914c1bad, 0x06eb3e45, 0x306eb3e4 }, // 0.003188  0.982299
  {  41, 0xc18f9c19, 0x063e7063, 0xe7063e70 }, // 0.002590  0.984889
  {  43, 0x2fa0be83, 0x05f417d0, 0x5f417d05 }, // 0.002337  0.987226
  {  47, 0x677d46cf, 0x0572620a, 0xe4c415c9 }, // 0.001956  0.989182
  {  53, 0x8c13521d, 0x04d4873e, 0xcade304d }, // 0.001541  0.990723
  {  59, 0xa08ad8f3, 0x0456c797, 0xdd49c341 }, // 0.001244  0.991967
  {  61, 0xc10c9715, 0x04325c53, 0xef368eb0 }, // 0.001162  0.993129
  {  67, 0x07a44c6b, 0x03d22635, 0x7e16ece5 }, // 0.000961  0.994091
  {  71, 0xe327a977, 0x039b0ad1, 0x2073615a }, // 0.000860  0.994950
  {  73, 0xc7e3f1f9, 0x0381c0e0, 0x70381c0e }, // 0.000814  0.995764
  {  79, 0x613716af, 0x033d91d2, 0xa2067b23 }, // 0.000695  0.996459
  {  83, 0x2b2e43db, 0x03159721, 0xed7e7534 }, // 0.000627  0.997086
  {  89, 0xfa3f47e9, 0x02e05c0b, 0x81702e05 }, // 0.000545  0.997632
  {  97, 0x5f02a3a1, 0x02a3a0fd, 0x5c5f02a3 }, // 0.000458  0.998090
  { 101, 0x7c32b16d, 0x0288df0c, 0xac5b3f5d }, // 0.000422  0.998511
  { 103, 0xd3431b57, 0x027c4597, 0x9c95204f }, // 0.000407  0.998918
  { 107, 0x8d28ac43, 0x02647c69, 0x456217ec }, // 0.000378  0.999296
  { 109, 0xda6c0965, 0x02593f69, 0xb02593f6 }, // 0.000365  0.999661
  { 113, 0x0fdbc091, 0x0243f6f0, 0x243f6f02 }  // 0.000339  1.000000
};

#define lengthof(x) ((uint32_t)(sizeof(x)/sizeof(x[0])))

// Returns 0 if (within limits of the table) 'x' is determined to be 
// square-free (prime factorization contains the same prime more than once)
// other returns found common factor. It assumes input is odd so does not
// check for multiple factors of 2.

//#define SQUARE_FREE_32 5
#define SQUARE_FREE_32 lengthof(prime)

uint32_t square_free_filter_32(uint32_t x)
{
  for (uint32_t i = 0; i<SQUARE_FREE_32; i++) {
    uint32_t m = prime[i].inv*x;
    // does 'x' have the prime as a factor?
    if (m <= prime[i].max) {
      // yes it does so check for another
      if (prime[i].inv*m <= prime[i].max) return prime[i].value;
    }
  }

  return 0;
}

uint32_t square_free_filter_64(uint64_t x)
{
  for (uint32_t i = 0; i<lengthof(prime); i++) {
    uint64_t inv = (uint64_t)prime[i].inv;
    uint64_t p   = (uint64_t)prime[i].value;
    uint64_t max = ((uint64_t)prime[i].max)<<32 | (uint64_t)prime[i].max_l;
    
    inv *= (2-p*inv);

    uint64_t v = inv*x;

    if (v <= max) { if (inv*v <= max) return (uint32_t)p; }
  }

  return 0;
}


// number of runs of 1s in odd 'x'
uint32_t num_bitstrings_32(uint32_t x)
{
  uint32_t c = 0;

  do {
    c++;
    // could do this with one ctz..to hell with that
    x >>= ctz_32(~x);  // nuke string of 1s
    x >>= ctz_32( x);  // nuke string of 0s
  } while (x != 0);

  return c;
}

uint32_t num_bitstrings_64(uint64_t x)
{
  uint32_t c = 0;

  do {
    x >>= ctz_64(~x);
    x >>= ctz_64( x);
    c++;
  } while (x != 0);

  return c;
}


// hacked version: if either 1s or 0s length too long..kill
uint32_t num_bitstrings_x_32(uint32_t x)
{
  uint32_t c = 0;

  if (clz_32(x^sgn_32(x)) < 9) {
    do {
      uint32_t len1 = ctz_32(~x); x >>= len1;
      uint32_t len0 = ctz_32( x); x >>= len0;
      c++;
      
      if (x == 0) return c;
      
      if ((len1|len0) > 7) return 0;
      
    } while (1);
  }
  return 0;
}


#define BITSTRING_CNT 8

// like num_bitstrings_32 but tallies length of runs (up to BITSTRING_CNT)
// longer are shoved into the last slot. (likewise assumes input is odd)
uint32_t cnt_bitstring_32(uint32_t x, uint8_t count[static BITSTRING_CNT])
{
  uint32_t num = 0;

  for (uint32_t i=0; i<BITSTRING_CNT; i++)
    count[i] = 0;

  while (x != 0) {
    uint32_t len = ctz_32(~x);

    x >>= len;
    len--;

    if (len >= BITSTRING_CNT) len = BITSTRING_CNT-1;

    count[len]++;

    num++;
    x >>= ctz_32(x);
  }

  return num;
}

uint32_t cnt_bitstring_64(uint64_t x, uint8_t count[static BITSTRING_CNT])
{
  uint32_t num = 0;

  for (uint32_t i=0; i<BITSTRING_CNT; i++)
    count[i] = 0;

  while (x != 0) {
    uint32_t len = ctz_64(~x);

    x >>= len;
    len--;

    if (len >= BITSTRING_CNT) len = BITSTRING_CNT-1;
    count[len]++;

    num++;
    x >>= ctz_64(x);
  }

  return num;
}


uint32_t cnt_bitstrings_32(uint32_t x, uint8_t count[static 2*BITSTRING_CNT])
{
  uint32_t num = 0;

  for (uint32_t i=0; i<BITSTRING_CNT; i++)
    count[i] = 0;

  while (x != 0) {
    uint32_t len = ctz_32(~x);

    x >>= len;
    len--;

    // considered too long
    if (len >= BITSTRING_CNT) return 0;
    count[len]++;

    len = ctz_32(x);
    x >>= len;

    // considered too long
    if (len >= BITSTRING_CNT) return 0;
    count[BITSTRING_CNT+len-1]++;
    num++;
  }

  return num;
}





/*********************************************
 * Constant selection methods
 *********************************************/

typedef struct {
  uint64_t rng_state[2];

  union {
  };

  void* code;  
  void* data;
  
} jah_env_t;

#define POPCOUNT_WINDOW_32 7

// mod integer multiplicate constant 'u' generation
// (this is very much a WIP)
uint32_t filter_mul_k_32(uint32_t u)
{
  u |= 1;

  // add range check here

  // reject popcounts not within window of 7 of 1/2. (~2%)
  // if that occurs multiply by odd constant to remap this
  // makes ~99.9957% of input accepted to next test.
  uint32_t pop = popcnt_32(u);

  if (abs_32(pop-16) >= POPCOUNT_WINDOW_32) {
    u   *= 0x2c2c57ed;
    pop  = popcnt_32(u);
    if (abs_32(pop-16) >= POPCOUNT_WINDOW_32) return 0;
  }
  
  uint32_t bstr = num_bitstrings_x_32(u);

  if (bstr < 6 || bstr > 13) {
    u   *= 0x2c2c57ed;
    pop  = popcnt_32(u);
    if (abs_32(pop-16) >= POPCOUNT_WINDOW_32) return 0;
    bstr = num_bitstrings_x_32(u);
    if (bstr < 6 || bstr > 13) return 0;
  }

  if (!square_free_filter_32(u))
    return u;

  return 0;
}

void test_filter()
{
  uint32_t c[2] = {0};
  uint32_t min = UINT32_C(-1);
  uint32_t max = UINT32_C( 0);

  for(uint64_t i=1; i<0x100000000; i+=2) {
    uint32_t v = filter_mul_k_32((uint32_t)i);
    if (v) {
      c[1]++;
      if (v<min)min=v;
      if (v>max)max=v;
    } else c[0]++;
  }
  printf("filter: {%u,%u} {%f,%f} min/max:%08x %08x\n",
	 c[0],c[1],
	 100.f*0x1.0p-31f*c[0],100.f*0x1.0p-31f*c[1],
	 min,max);
}

uint32_t get_mul_k_32(jah_env_t* env)
{
  do {
    // generate a 64 bit integer, try using lower
    // 32-bits first and then upper if it didn't
    // work out. rinse and repeat.
    uint64_t v = rng_u64(env->rng_state);
    uint32_t u = filter_mul_k_32((uint32_t)v);
    if (u) return u;
    u = filter_mul_k_32((uint32_t)(v>>32));
    if (u) return u;
  } while(1);
}


uint64_t get_mul_k_64(jah_env_t* env)
{
  do {
    uint64_t u = rng_u64(env->rng_state)|1;
    uint32_t t = abs_32(popcnt_64(u)-32);
    
    if (t >= 9) {
      u *= UINT64_C(0x1);
      t  = abs_32(popcnt_64(u)-32);
    }
    
    if (t < 9) {
      // bitstrings...
      
      if (!square_free_filter_64(u)) {
	return u;
      }
    }
  } while(1);
}




/*********************************************
 * The world's worst limited feature set JIT
 *********************************************/

#if !defined(_MSC_VER)

#include <sys/mman.h>

// create a 'len' length read/write/executable array
static void*
cb_alloc(uint32_t len)
{
  void* p = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (p != MAP_FAILED) {
    if (!mprotect(p, len, PROT_READ|PROT_WRITE|PROT_EXEC))
      return p;
  }

  fprintf(stderr, "error: couldn't create memory for JIT\n");
  exit(EXIT_FAILURE);
  return p;
}

#else

static void*
cb_alloc(uint32_T len)
{
  fprintf(stderr, "error: visual-c only partially stubbed out.\n");
  exit(EXIT_FAILURE);
}

#endif

//** write 'n' bytes to stream helpers

inline uint8_t* cb_w1(uint8_t* mem, uint8_t data)
{
  mem[0] = data; return mem+1;
}

inline uint8_t* cb_w2(uint8_t* mem, uint16_t data)
{
  ((uint16_t*)mem)[0] = data; return mem+2;
}

inline uint8_t* cb_w3(uint8_t* mem, uint32_t data)
{
  ((uint32_t*)mem)[0] = data; return mem+3;
}

inline uint8_t* cb_w4(uint8_t* mem, uint32_t data)
{
  ((uint32_t*)mem)[0] = data; return mem+4;
}

inline uint8_t* cb_w8(uint8_t* mem, uint64_t data)
{
  ((uint64_t*)mem)[0] = data; return mem+8;
}

void cb_dump(uint8_t* d)
{
  uint8_t b;
  
  printf("\ndump: ");
  do {
    b = *d++;
    printf("%02x ", b);
  } while (b != 0xc3);
  printf("\n");
}



typedef enum
{
  RAX = 0, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
//R8, R9, R10, R11, R12, R13, R14, R15,
  EAX = 0, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
//AX  = 0, CX,  DX,  BX,  SP,  BP,  SI,  DI,
//AL  = 0, CL,  DL,  BL,  AH,  CH,  DH,  BH
} reg_t;

#define RM(RD,RS,M) (((M<<3)|RD)<<3|RS)

// op RD RS ...
#define RMI(OP,RD,RS,M) ((((M<<3)|RD)<<3|RS)<<8|OP)

//#define RM(OP,RD,RS,M) ((((M<<3)|RD)<<3|RS)<<8|OP)

#define REX_W 0x48

//** slightly more generic helpers

// REX.W prefix
inline uint8_t* cb_rex_w(uint8_t* d)
{
  return cb_w1(d, REX_W);
}

inline uint8_t* cb_ret(uint8_t* d)
{
  return cb_w1(d, 0xc3);
}

// mov r0, r1 (flipped ordering since using 0x89)
inline uint8_t* cb_mov_32(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_w2(d, (uint16_t)RMI(0x89,r1,r0,3));
}

// mov r0, r1
inline uint8_t* cb_mov_64(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_mov_32(cb_rex_w(d), r0,r1);
}

// imul r0, r1
inline uint8_t* cb_mul_32(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_w1(cb_w2(d,0xaf0f), (uint8_t)RM(r0,r1,3));
}

// imul r0, r1
inline uint8_t* cb_mul_64(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_mul_32(cb_rex_w(d),r0,r1);
}

// rorx r0, r1, 32-k
inline uint8_t* cb_rot_32(uint8_t* d, reg_t r0, reg_t r1, uint32_t k)
{
  d = cb_w4(d,0xf07be3c4u);
  d = cb_w1(d,(uint8_t)RM(r0,r1,3));
  d = cb_w1(d,(32-k)&0x1f);
  return d;
}

// rorx r0, r1, 64-k
inline uint8_t* cb_rot_64(uint8_t* d, reg_t r0, reg_t r1, uint32_t k)
{
  d = cb_w4(d,0xf0fbe3c4);
  d = cb_w1(d,(uint8_t)RM(r0,r1,3));
  d = cb_w1(d,(64-k)&0x3f);
  return d;
}



//** hash cases

// movabs rdi, k
inline uint8_t* cb_load_64(uint8_t* d, uint64_t k)
{
  return cb_w8(cb_w2(d,0xbf48),k);
}

// imul eax, eax, k
inline uint8_t* cb_mul_k_32(uint8_t* d, uint32_t k)
{
  d = cb_w2(d, RMI(0x69,EAX,EAX,3));
  d = cb_w4(d, k);
  return d;
}

// multiply rax by k 
inline uint8_t* cb_mul_k_64(uint8_t* d, uint64_t k)
{
  d = cb_load_64(d,k);
  d = cb_mul_64(d,RAX,RDI);
  return d;
}

#if 0
uint8_t* cb_xorrot_32(uint8_t* d, uint32_t k0, uint32_t k1)
{
  // rorx ecx,eax, 32-k0
  // rorx edi,eax, 32-k1
  return d;
}
#endif


// nope
#define ABI __attribute__((sysv_abi))

typedef uint32_t ABI (func_32_t)(uint32_t);
typedef uint64_t ABI (func_64_t)(uint64_t);


/*********************************************
 * 
 *********************************************/

uint32_t sac_le_count_32(func_32_t* f)
{
  uint32_t t;

  // walk the 'n' samples
  for(uint32_t i=0; i<0xffff; i++) {
    uint32_t h = f(i);

    // flip each bit of input 'x'
    for (uint32_t j=0; j<32; j++) {
      uint32_t d = h ^ f(i ^ (1<<j));
      t += __builtin_popcount(d);
    }
  }
  return t;
}


/*********************************************
 * 
 *********************************************/

inline uint32_t xorrot_32(uint32_t x, uint32_t a, uint32_t b)
{
  return x^rot_32(x,a)^rot_32(x,b);
}


/*********************************************
 * Some garbage that doesn't belong here.
 *********************************************/

// a list of 32-bit integers (used by multply) in PRNGs and hashes
uint32_t u32_mul_test_v[] =
{
  0xac564b05, 0x01c8e815, 0x01ed0675,  // L'ecuyer tables: LCG
  0x2c2c57ed, 0x5f356495, 0x2c9277b5,  // L'ecuyer tables: MLCG
  0x85ebca77, 0xc2b2ae3d,              // xxhash32
  0x85ebca6b, 0xc2b2ae35,              // murmurhash3-32
 
  // https://github.com/skeeto/hash-prospector (2 xor-mul variants)
  // values that are even are commented out
  0x7feb352d, 0x846ca68b,
  0xa136aaad, 0x9f6d62d7,
  0x24f4d2cd, 0x1ba3b969,
  0xe2d0d4cb, 0x3c6ad939,
  0x5abe3ae5, 0x65639657,
  0x122eacad, 0x4d11c547,
  0xb7b9e4ad, 0xe5328a63,
  0xa5c34d53, 0x0c6524cf,
  0x0c166973, 0x99ad7299,
  0xa3d94b57, 0xf2c5b5d1,
  0x4985c6a9, 0x07624b2f,
  0x00826d4b, 0xce6b73a7,
  0x5f695533, 0x12e558d3,
  0xa72f8c9d, 0xaa189b8b,
  0xd76531b5, 0xeb08cda5,
/*0x952f8b96,*/ 0xf0b5b4d9,
  0xc8a26cb3, /*0x11e51a2e,*/
/*0x893de54a,*/ 0x3a26ba99,
  0x7df48b9b, 0xbcd79a97,
  0xeea6964b, 0xe709335b,
  0x86d2a755, 0x9ab7395b,
  0x2c88c9a7, 0xa1f2b677,
  // https://github.com/skeeto/hash-prospector (3 xor-mul variants)
  0xed5ad4bb, 0xac4c1b51, 0x31848bab,
  0xaeccedab, 0xac613e37, 0x19c89935,
  0x236f7153, 0x33cd8663, 0x3e06b66b,
  0x4260bb47, 0x27e8e1ed, 0x9d48a33b,
  0x3f6cde45, 0x51d608ef, 0x6e93639d,
  0x5dfa224b, 0x4bee7e4b, 0x930ee371,
  0x3964f363, 0x9ac3751d, 0x4e8772cb,
  0x66046c65, 0xd3f0865b, 0xf9999193,
  0xb1a89b33, 0x09136aaf, 0x5f2a44a7,
  0x24767aad, 0xdaa18229, 0xe9e53beb,
  0x42f91d8d, 0x61355a85, 0xdcf2a949,
  0x4df8395b, 0x466b428b, 0xb4b2868b,
  0x2bbed51b, 0xcd09896b, 0x38d4c587,
  0x0ab694cd, 0x4c139e47, 0x11a42c3b,
  0x7f1e072b, 0x8750a507, 0xecbb5b5f,
  0xf1be7bad, 0x73a54099, 0x3b85b963,
  0x66e756d5, 0xb5f5a9cd, 0x84e56b11,
  0x233354bb, 0xce1247bd, 0x855089bb,
  0xeb6805ab, 0xd2c7b7a7, 0x7645a32b,
  0x8288ab57, 0x0d1bfe57, 0x131631e5,
  0x45109e55, 0x3b94759d, 0xadf31ea5,
  0x26cd1933, 0xe3da1d59, 0x5a17445d,
  0x7001e6eb, 0xbb8e7313, 0x3aa8c523,
  0x49ed0a13, 0x83588f29, 0x658f258d,
  0x6cdb9705, 0x4d58d2ed, 0xc8642b37,
  0xa986846b, 0xbdd5372d, 0xad44de6b,
  0xc9575725, 0x9448f4c5, 0x3b7a5443,
  0xfc54c453, 0x08213789, 0x669f96eb,
  0xd47ef17b, 0x642fa58f, 0xa8b65b9b,
  0x00bfaa73, 0x8799c69b, 0x731985b1,
  0x953a55e9, 0x8523822b, 0x56e7aa63,
  0xa3d7345b, 0x7f41c9c7, 0x308bd62d,
  0x195565c7, 0x16064d6f, 0x0f9ec575,
  0x13566dbb, 0x59369a03, 0x990f9d1b,
  0x8430cc4b, 0xa7831cbd, 0xc6ccbd33,
  0x699f272b, 0x09c01023, 0x39bd48c3,
  0x336536c3, 0x4f0e38b1, 0x15d229f7,
  0x221f686d, 0xd8948a07, 0xed8a8345,
  0xd7ca8cbb, 0xeb4e259f, 0x34ab1143,
  0x7cb04f65, 0x9b96da73, 0x83625687,
  0x5156196b, 0x940d8869, 0x0086f473
};

uint64_t u64_mul_test_v[] =
{
 0x27bb2ee687b0b0fd, 0x2c6fe96ee78b6955, 0x369dea0f31a53f85, // L'ecuyer tables: LCG
 0x106689d45497fdb5, 0x6a5d39eae116586d, 0x2545f4914f6cdd1d, // L'ecuyer tables: <LCG
 0xc2b2ae3d27d4eb4f, 0x165667b19e3779f9,                     // xxhash-64
 0xff51afd7ed558ccd, 0xc4ceb9fe1a85ec53                      // MurmurHash3-64
};

void bar_cnt_spew(uint8_t c[static BITSTRING_CNT])
{
  printf(" (");
  for(uint32_t i=0; i<BITSTRING_CNT; i++)
    printf("%2u ", c[i]);
  printf(")");
}

void bar_a_spew(uint32_t n, uint32_t* c)
{
  printf(" {");
  for(uint32_t i=0; i<n; i++)
    printf("%3u,", c[i]);
  printf("}");
}

void bar_32()
{
  uint8_t  bsc[BITSTRING_CNT];
  uint32_t ps[17] = {0};
  uint32_t bt[BITSTRING_CNT] = {0};

  
  for(uint32_t i=0; i<lengthof(u32_mul_test_v); i++) {
    uint32_t v   = u32_mul_test_v[i];
    uint32_t pop = popcnt_32(v);
    uint32_t bs  = cnt_bitstring_32(v,bsc);
    //uint32_t cbs = num_bitstringsx_32(v);

    ps[pop <= 16 ? 16-pop : pop-16]++;
    for(uint32_t j=0; j<BITSTRING_CNT; j++) bt[j] += bsc[j];

    //if (bs != cbs) { printf("*"); };
    
    printf("%08x : %3u %3u %3u :", v, square_free_filter_32(v), pop,bs);
    bar_cnt_spew(bsc);
    printf("\n");
	   
  }
  bar_a_spew(17,ps); printf("/%u.", lengthof(u32_mul_test_v)); 
  printf("\n");
  bar_a_spew(BITSTRING_CNT,bt);
  printf("\n");
}

void bar_64()
{
  uint8_t bsc[BITSTRING_CNT];
  uint32_t ps[33] = {0};
  uint32_t bt[BITSTRING_CNT] = {0};

  for(uint32_t i=0; i<lengthof(u64_mul_test_v); i++) {
    uint64_t v   = u64_mul_test_v[i];
    uint32_t pop = popcnt_64(v);
    uint32_t bs  = cnt_bitstring_64(v,bsc);

    ps[pop <= 32 ? 32-pop : pop-32]++;
    for(uint32_t j=0; j<BITSTRING_CNT; j++) bt[j] += bsc[j];

    printf("%016lx : %3u %3u %3u", v, square_free_filter_64(v), pop,bs);
    bar_cnt_spew(bsc);
    printf("\n");
  }
  bar_a_spew(33,ps); printf("/%u.", lengthof(u64_mul_test_v)); 
  printf("\n");
  bar_a_spew(BITSTRING_CNT,bt);
  printf("\n");
}


/*********************************************
 * 
 *********************************************/


uint8_t* foo_32(uint8_t* d)
{
  d = cb_mov_32(d,EAX,EDI);
  d = cb_mul_k_32(d,0x123);
  d = cb_ret(d);
  return d;
}

uint8_t* foo_64(uint8_t* d)
{
  d = cb_mov_64(d,RAX,RDI);
  d = cb_mul_k_64(d,0x123);
  d = cb_ret(d);
  return d;
}


void test_32(func_32_t* f)
{
  for(uint32_t x=0; x<0xffff; x++) {
    uint32_t h0 = f(x);
    if (h0 != 0x123*x) {printf(".");}
  }
}

void test_64(func_64_t* f)
{
  for(uint64_t x=0; x<0xffff; x++) {
    uint64_t h0 = f(x);
    if (h0 != 0x123*x) {printf("x");}
  }
}


#if 0
void jah_init_64(jah_env_t* jah)
{
}

void jah_init_32(jah_env_t* jah)
{
}
#endif

void jah_init(jah_env_t* jah)
{
  jah->rng_state[0] = _rdtsc();
  jah->rng_state[1] = (uint64_t)((void*)jah);
}


void signal_catcher(int signo, siginfo_t* info, void* context)
{
}

void signal_init(struct sigaction* s)
{
  memset(s,0,sizeof(struct sigaction));
}


uint32_t done = 0;



static void sigint_handler(int sig) { done = 1; }

// max byte length of buffer
#define CB_BUFFER_LEN 0x10000

int
main(void)
{
  //signal(SIGINT, sigint_handler);
  
  void* cb = cb_alloc(CB_BUFFER_LEN);

  //uint32_t v = 0x555f;
  //printf(": %08x %08x\n", num_bitstrings_x_32(v), filter_mul_k_32(v));
  
  //bar_32(); //bar_64(); 
  test_filter();
  return 0;
  //printf("%02x ", RM(EAX,EAX,3));

  //uint8_t d[1024];
  //cb_ret(cb_rot_32(d, EAX,ECX,32-0x11));
  //cb_dump(d);

  //square_free_filter_64(0);  
  //printf("%04x ", RMI(0x89,EAX,ECX,3));
  //printf("%04x ", RMI(0x89,ECX,EAX,3));
  //printf("%04x ", RMI(0x89,EAX,EDI,3));
  //printf("%04x ", RMI(0x89,EDI,EAX,3));
  //return 0;
  
  foo_32(cb);
  test_32(cb);

#if 1
  foo_64(cb);
  test_64(cb);
#endif  
  
  return 0;
}
