
// Public Domain under http://unlicense.org, see link for details.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/************************************
 * utils
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

inline uint32_t ctz_32(uint32_t x)    { return (uint32_t)__builtin_ctz(x); }
inline uint32_t ctz_64(uint64_t x)    { return (uint32_t)__builtin_ctzll(x); }

#else

inline uint32_t rot_32(uint32_t x, int i) { return _rotl(x,i);  }
inline uint64_t rot_64(uint64_t x, int i) { return _rotll(x,i); }

#endif

inline uint32_t neg_32(uint32_t x) { return (uint32_t)(-(int32_t)x); }
inline uint64_t neg_64(uint64_t x) { return (uint64_t)(-(int64_t)x); }


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

/************************************
 * constant filtering
 ************************************/

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

// Returns 0 if (within limits of the table) 'x' is determined to be not
// square-free (prime factorization contains the same prime more than once)
// It assumes input is odd so does not check for multiple factors of 2.
uint32_t square_free_filter(uint32_t x)
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

#if 0
// number of runs of 1s in odd 'x'
uint32_t num_bitstrings_32(uint32_t x)
{
  uint32_t c = 1;

  x -= 1;

  while (x != 0) {
    c++;
    x >>= ctz_32(x);
    x += 1;
    x = x & (x - 1);
  }

  return c;
}
#endif

/*********************************************
 * The world's worst limited feature set JIT
 *********************************************/

#if !defined(_MSC_VER)

#include <sys/mman.h>

static void*
cb_alloc(uint32_t len)
{
  void* p = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (p != MAP_FAILED) {
    if (!mprotect(p, len, PROT_READ|PROT_WRITE|PROT_EXEC))
      return p;
  }
  
  exit(EXIT_FAILURE);
  return p;
}

#else

static void*
cb_alloc(uint32_T len)
{
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
  return cb_w2(d,RMI(0x89,r1,r0,3));
}

// mov r0, r1
inline uint8_t* cb_mov_64(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_mov_32(cb_rex_w(d), r0,r1);
}

// imul r0, r1
inline uint8_t* cb_mul_32(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_w1(cb_w2(d,0xaf0f), RM(r0,r1,3));
}

// imul r0, r1
inline uint8_t* cb_mul_64(uint8_t* d, reg_t r0, reg_t r1)
{
  return cb_mul_32(cb_rex_w(d),r0,r1);
}

// rorx r0, r1, 32-k
inline uint8_t* cb_rot_32(uint8_t* d, reg_t r0, reg_t r1, uint32_t k)
{
  d = cb_w4(d,0xf07be3c4);
  d = cb_w1(d,RM(r0,r1,3));
  d = cb_w1(d,(32-k)&0x1f);
  return d;
}

// rorx r0, r1, 64-k
inline uint8_t* cb_rot_64(uint8_t* d, reg_t r0, reg_t r1, uint32_t k)
{
  d = cb_w4(d,0xf0fbe3c4);
  d = cb_w1(d,RM(r0,r1,3));
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

/************************************
 * 
 ************************************/

inline uint32_t xorrot_32(uint32_t x, uint32_t a, uint32_t b)
{
  return x^rot_32(x,a)^rot_32(x,b);
}

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

// max byte length of buffer
#define CB_BUFFER_LEN 0x10000


int
main(void)
{
  void* cb = cb_alloc(CB_BUFFER_LEN);

  printf("%02x ", RM(EAX,EAX,3));

  uint8_t d[1024];
  cb_ret(cb_rot_32(d, EAX,ECX,32-0x11));
  cb_dump(d);

  
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
