#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bmat.h"
#include "bitops.h"
#include "carryless.h"

// an example for blog post: 

// the function we're going to inspect (named to match post's matrix 'A')
static inline uint64_t A(uint64_t s)
{
  s ^= s << 27;
  s ^= s >> 15;
  s ^= s <<  5;

  return s;
}


//*******************************************************
// code in this block has minimal dependencies outside
// of this file.
// 
// clz_64 :    64-bit leading zero count
// cl_pow2_64: scatters lower 32 bits to even positions
//             (which is a carryless x^2)
//
// These are all very "inefficient" in terms of implementation
// but the techinques are fine.

typedef __uint128_t u128_t;
typedef struct {uint64_t lo,hi;} split_t;

static inline split_t split_u128(u128_t x)
{
  uint64_t lo = (uint64_t)x;
  uint64_t hi = (uint64_t)(x>>64);
  return (split_t){.hi=hi,.lo=lo};
}

static inline void print_u128(u128_t x)
{
  split_t s = split_u128(x);
  printf("%016lx:%016lx", s.hi,s.lo);
}

// paste together 
static inline u128_t u128(uint64_t hi, uint64_t lo)
{
  u128_t r = (u128_t)hi;
  return (r << 64) | (u128_t)lo;
}

// count leading zeros
static inline uint32_t clz_128(u128_t x)
{
  split_t s = split_u128(x);
  if (s.hi) return clz_64(s.hi);
  return 64 + clz_64(s.lo);
}

// computes: x^2
// the 64-bits of 'x' are scattered to the even
// bit positions of the 128-bit result.
static inline u128_t cl_pow2_full_64(uint64_t x)
{
  // intel can do this in one carryless product (FWIW)
  return u128(cl_pow2_64(x>>32), cl_pow2_64(x));
}

// computes: x mod n
// special cased for the assumption that n is exactly 65 bits
static inline u128_t cl_mod_128(u128_t x, u128_t n)
{
  uint32_t zn = clz_128(n);

  while (x >> 64) {
    uint32_t zx = clz_128(x);
    u128_t   sn = n << (zn-zx); 
    
    x ^= sn;
  }
  
  return x;
}

// return: x^2 mod n
static inline u128_t f2_jump_next_64(u128_t x, u128_t n)
{
  x = cl_pow2_full_64((uint64_t)x);
  x = cl_mod_128(x,n);
  return x;
}

void f2_base_build_64(uint64_t table[static 64])
{
  uint64_t r = 1;

  for(uint32_t i=0; i<64; i++) {
    table[i] = r;
    r = A(r);
  }
}

// compute: 2^2^n mod p for n on [0,63]
//   p has implied high bit (bit 64 is set)
void f2_jump_build_64(uint64_t table[static 64], uint64_t p)
{
  // the first six: 2^{2^n} mod p = 2^{2^n}
  uint64_t r;

  for(uint32_t n=0; n<6; n++) {
    // 2^{2^n}
    r = 1;
    r = r << (r << n);
    table[n] = r;
  }

  // 2^{2^6} mod p =
  // 2^{64}  mod p = p mod 2^64 (losing implied bit)
  table[6] = p;

  // real work begins:
  //   x_{n+i} = (x_n)^2 mod p
  u128_t k = u128(1,p);            // keep simple make implied bit explict
  u128_t x = k;
  
  for(uint32_t n=7; n<64; n++) {
    x = cl_pow2_full_64((uint64_t)x);
    x = cl_mod_128(x,k);
    
    table[n] = (uint64_t)x;
  }
}

//n = (n != UINT64_C(-1)) n : 1;

uint64_t f2_jump_i_64(uint64_t g, uint64_t s)
{
  uint64_t n = 0;

  // evaluate the polynomial 'g(A)x' using Horner's method
  do {
    if (g & 1) n ^= s;
    g = g >> 1;
    s = A(s);
  } while(g);

  return n;
}

uint64_t f2_jump_by_64(uint64_t table[static 64], uint32_t n, uint64_t s)
{
  uint64_t g = 0;

  return f2_jump_i_64(g,s);
}


uint64_t f2_jump_2p_64(uint64_t table[static 64], uint32_t p, uint64_t s)
{
  // not limiting 'p'
  uint64_t g = table[p];  // polynomial coefficients

  return f2_jump_i_64(g,s);
}


// dump table as c code
void table_dump_64(const char* name, uint64_t table[static 64])
{
  const char* fmt = "UINT64_C(0x%016lx), ";

  printf("const uint64_t %s[64] = {\n  ", name);
  
  for(uint32_t i=0; i<64; i++) {
    printf(fmt, table[i]);
    if ((i & 3)==3) printf("\n  ");
  }
  printf("\b\b};\n");
}


//*******************************************************
// matrix based (GF2 subdirectory) stuff below here

void do_the_stuff(bmat_param_64(m))
{
  uint64_t k[64];
  uint32_t cnt = bmat_fixed_points_64(m,k);
  uint64_t p   = bmat_charpoly_64(m);
  printf("// p = 1%016lx (characteristic polynomial)\n", p);

  // expecting full period so there should be no
  // fixed points (other than the trival x=0)
  if (cnt != 0) {
    printf("  unexpected found %2u fixed points ",cnt);
    
    if (cnt > 0) {
      if (cnt < 4) {
	printf("{%lx", k[0]);
	for(uint32_t i=1; i<cnt; i++) {
	  printf(",%lx", k[1]);
	}
	printf("}");
      }
      else {
	printf("{elided}");
      }
    }
  }

  // build the jump table
  uint64_t jump[64];
  uint64_t u_i[64] = {0};

  printf("\n");
  f2_jump_build_64(jump, p);
  printf("// 2^{2^n} mod 1%016lx\n", p);
  table_dump_64("jump_table", jump);
  printf("\n");

  // build the u_i table
  printf("\n");
  f2_base_build_64(u_i);
  printf("// A^i * (1,0,...0)^T\n");
  table_dump_64("jump_ui", u_i);
  printf("\n");

  for(uint32_t j=1; j<10; j++) {
    printf("{");
    uint64_t b = UINT64_C(1) << j;
    uint32_t cnt = 0;
    for(uint32_t i=0; i<64; i++) {
      uint32_t t = (u_i[i] & b) != 0;
      cnt += t;
      printf("%u,", t);
    }
    printf("\b}; // %2u : %u\n", j,cnt);
  }

  
  // foo
#if 0  
  for(uint32_t i=0; i<64; i++) {
    uint64_t r = f2_jump_2p_64(jump,i,1);
    
    printf("%016lx // %2u\n", r,i);
  }
#endif  
}

int main(void)
{
  bmat_def_64(m);

  bmat_from_ufunc_64(m, A);

  if (bmat_is_full_rank_64(m)) {
    do_the_stuff(m);
    return 0;
  }

  fprintf(stderr, "given a non-bijection\n");

  return 0;
}
  
