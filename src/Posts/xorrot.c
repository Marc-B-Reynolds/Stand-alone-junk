// Public Domain under http://unlicense.org, see link for details.

// Toy code for:
// http://marc-b-reynolds.github.io/math/2017/10/13/XorRotate.html

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// max width to support (2^LOG2_MAX_BITS)
#define LOG2_MAX_BITS 10

// the number of times to sample the generated inverse function
// to insure correctness. one should be enough since it's impossible
// to create a function which isn't a bijection. Any larger number
// is useless but might be more convincing if you don't buy the
// argument.
#define INVERSE_SAMPLES 10


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

inline uint64_t rot64(uint64_t x, uint32_t i)
{
#if defined(__clang__)
  return __builtin_rotateleft64(x,i);
#else
  return (x << i) | (x >> ((uint32_t)(-(int32_t)i) & 63));
#endif
}


#if !defined(_MSC_VER)
inline __uint128_t rot128(__uint128_t x, uint32_t i)
{
  return (x << i) | (x >> ((uint32_t)(-(int32_t)i) & 127));
}
#endif


// hack
#if defined(_MSC_VER)
#include <intrin.h>
_inline uint32_t ctz(uint32_t x) { unsigned long r; _BitScanForward(&r, (unsigned long)x); return (uint32_t)r; }
#else
inline uint32_t ctz(uint32_t x) { return __builtin_ctz(x); }
#endif

inline uint32_t xor_rot2(uint32_t x, uint32_t a, uint32_t b)
{
  return x^rot(x,a)^rot(x,b);
}

inline uint64_t xor64_rot2(uint64_t x, uint32_t a, uint32_t b)
{
  return x^rot64(x,a)^rot64(x,b);
}

#if !defined(_MSC_VER)

inline __uint128_t xor128_rot2(__uint128_t x, uint32_t a, uint32_t b)
{
  return x^rot128(x,a)^rot128(x,b);
}
#endif


// hacky period validatation: just count
// result is: 32>>ctz(a-b)
uint32_t period_32(uint32_t a, uint32_t b)
{
  uint32_t v = 1;
  uint32_t c = 0;

  do {
    c++;
    v = xor_rot2(v,a,b);
  } while (v != 1);

  return c;
}

// like 32-bit version: just count
uint32_t period_64(uint32_t a, uint32_t b)
{
  uint64_t v = 1;
  uint32_t c = 0;

  do {
    c++;
    v = xor64_rot2(v,a,b);
  } while (v != 1);

  return c;
}

typedef struct { uint32_t a,b; } pair_t;


typedef struct {
  uint32_t n; 

  // terms of inverse funtion
  uint32_t c;
  pair_t   term[LOG2_MAX_BITS];

  // terms of forward function
  uint32_t a,b; 
} reduce_t;

// for testing
uint32_t inv_32(reduce_t* r, uint32_t x)
{
  if (r->c != 0) x = rot(x,r->c);

  switch(r->n) {
    case 5:  x = xor_rot2(x,r->term[4].a, r->term[4].b); // not possible (if reduced)
    case 4:  x = xor_rot2(x,r->term[3].a, r->term[3].b);
    case 3:  x = xor_rot2(x,r->term[2].a, r->term[2].b);
    case 2:  x = xor_rot2(x,r->term[1].a, r->term[1].b);
    default: x = xor_rot2(x,r->term[0].a, r->term[0].b);
  }
  
  return x;
}

// for testing
uint64_t inv_64(reduce_t* r, uint64_t x)
{
  if (r->c != 0) x = rot64(x,r->c);

  switch(r->n) {
    case 6:  x = xor64_rot2(x,r->term[5].a, r->term[5].b); // not possible (if reduced)
    case 5:  x = xor64_rot2(x,r->term[4].a, r->term[4].b);
    case 4:  x = xor64_rot2(x,r->term[3].a, r->term[3].b);
    case 3:  x = xor64_rot2(x,r->term[2].a, r->term[2].b);
    case 2:  x = xor64_rot2(x,r->term[1].a, r->term[1].b);
    default: x = xor64_rot2(x,r->term[0].a, r->term[0].b);
  }
  
  return x;
}

// for testing
#if !defined(_MSC_VER)
 __uint128_t inv_128(reduce_t* r, __uint128_t x)
{
  if (r->c != 0) x = rot128(x,r->c);

  switch(r->n) {
    case 7:  x = xor128_rot2(x,r->term[6].a, r->term[6].b); // not possible (if reduced)
    case 6:  x = xor128_rot2(x,r->term[5].a, r->term[5].b);
    case 5:  x = xor128_rot2(x,r->term[4].a, r->term[4].b);
    case 4:  x = xor128_rot2(x,r->term[3].a, r->term[3].b);
    case 3:  x = xor128_rot2(x,r->term[2].a, r->term[2].b);
    case 2:  x = xor128_rot2(x,r->term[1].a, r->term[1].b);
    default: x = xor128_rot2(x,r->term[0].a, r->term[0].b);
  }
  
  return x;
}
#endif




// print the expression represented by 'r'
void reduce_print(reduce_t* r)
{
  // terms of the inverse
  for(uint32_t i=0; i<r->n; i++)
    printf("(0,%2d,%2d)",r->term[i].a,r->term[i].b);

  if (r->c != 0) { printf("%d",r->c); }
  
  printf("\n");
}

uint32_t xor_rot2_inv(uint32_t x, uint32_t a, uint32_t b)
{
  // Perform the five steps (and keep 'a' and 'b' in range given
  // how 'rot' is defined above) as a sequence of transforms.
  // The order reversed of above (products of powers of M commute).
  x = x^rot(x,a)^rot(x,b); a = (a+a) & 0x1f; b = (b+b) & 0x1f; // t0 = M x
  x = x^rot(x,a)^rot(x,b); a = (a+a) & 0x1f; b = (b+b) & 0x1f; // t1 = M^2 t0
  x = x^rot(x,a)^rot(x,b); a = (a+a) & 0x1f; b = (b+b) & 0x1f; // t2 = M^4 t1
  x = x^rot(x,a)^rot(x,b); a = (a+a) & 0x1f; b = (b+b) & 0x1f; // t3 = M^8 t2
  x = x^rot(x,a)^rot(x,b);                                     // x' = M^16 t3

  return x;
}

uint32_t reduce_validate(uint32_t p, reduce_t* r)
{
  uint32_t i = INVERSE_SAMPLES;

  // inital input 'x' could simply be '1'..the loop
  // is useless (see comment on INVERSE_SAMPLES).
  
  if (p == 5) {
    uint32_t x = 0x132457;
    do {
      uint32_t r0 = xor_rot2(x, r->a,r->b);
      uint32_t r1 = inv_32(r,r0);
      if (r1 != x) return 1;
      x = r0 * 0x123457;
    } while(--i > 0);
  }
  else if (p == 6) {
    uint64_t x = 0x132457;
    do {
      uint64_t r0 = xor64_rot2(x, r->a,r->b);
      uint64_t r1 = inv_64(r,r0);
      if (r1 != x) return 1;
      x = r0 * 0x123457;
      
    } while(--i > 0);
  }
#if !defined(_MSC_VER)
  else if (p == 7) {
    uint64_t x = 0x132457;
    do {
      __uint128_t r0 = xor128_rot2(x, r->a,r->b);
      __uint128_t r1 = inv_128(r,r0);
      if (r1 != x) return 1;
      x = r0 * 0x123457;
    } while(--i > 0);
  }
#endif  
  else return 1; // not implmented
  
  return 0;
}

// yes..this is dumb and doesn't really sort
void bubbleD(pair_t* lo, pair_t* hi)
{
  uint32_t a,b;

  if (hi->b < hi->a) {
    a = hi->a;
    hi->a = hi->b; 
    hi->a = a;
  }
  
  while(hi > lo) {
    if (hi->a < hi[-1].a) {
      a = hi[-1].a; b = hi[-1].b;
      hi[-1].a = hi->a;
      hi[-1].b = hi->b;
      hi->a = a;
      hi->b = b;
    }
    
    hi--;
  }
}

// given input 'M':
//   M = I + C^a   + C^b
//     = I + C^a(I + C^d)  with d=b-a
// reduce to the inverse to a minimal number of terms. Exception
// only folds in any final rotate term if it can be expressed
// in as I+C^e+C^f
void reduce(reduce_t* r, uint32_t p, uint32_t a, uint32_t b)
{
  uint32_t bits = (1<<p);
  uint32_t mask = bits-1;
  pair_t*  term = r->term;
  uint32_t c    = 0;

  term->a = r->a = a;
  term->b = r->b = b;
  term++;

  // this could be "simplifed" by being "clever" (ctz on a,b&d)
  while(1) {
    // square for the next term
    a+=a; a &= mask;
    b+=b; b &= mask;

    // if the 'd' goes to zero we're done
    if (b-a == 0) break;
    
    // if eiter 'a' or 'b' go to zero, then all that's left
    // is to sum up the solo shift term.
    if (a == 0) { do { c += b; b+=b; b &= mask; } while(b != 0); break; } 
    if (b == 0) { do { c += a; a+=a; a &= mask; } while(a != 0); break; } 

    // add current term and sort
    term->a = a; 
    term->b = b; 
    bubbleD(r->term, term);
    term++;
  }

  c &= mask;

  // see if we can fold in constant term:  have C^c check if there's (d=bits-c)
  // (I+C^d+C^e) -> (I + C^c + C^(e+c))
  if (c != 0) {
    uint32_t d = bits-c;
    pair_t*  t = term-1;

    do {
      if (t->a == d || t->b == d) {
	if (t->a == d) {
	  t->a = (t->b + c) & mask;
	  t->b = c;
	}
	else {
    uint32_t nb = (t->a + c) & mask;
    t->a = c;
	  t->b = nb;
	}
        bubbleD(r->term, t);
	r->c = c = 0;
	break;
      }
      t--;
    } while(t >= r->term); 
  }

  r->n = (uint32_t)(term-r->term);
  r->c = c;
}


// Brute force count the number of fixed point (32-bit). 
// is equal to: 1<<(1<<ctz(b-a))
uint32_t bf_count_fixed_point(uint32_t a, uint32_t b)
{
  uint32_t c = 1;   // zero is a fixed point
  uint32_t i = 1;   // start with one

  do {
    uint32_t r = xor_rot2(i,a,b);
    if (i == r) c++;
  } while(++i != 0);

  return c;
}

void xorrot_print_info_header()
{
  printf("                       fixed\n");
  printf("    M        period    points\n");
  //      (0,000,000):    xxx 
}

void xorrot_print_info(uint32_t p, uint32_t a, uint32_t b)
{
  uint32_t bits = 1<<p;          // number of bits in integer
  uint32_t dz   = ctz(b-a);
  uint32_t n    = bits>>dz;      // period
  uint32_t fp   = 1<<(1<<dz);    // number of fixed points
  reduce_t r;

  reduce(&r,p,a,b);

  //      (0,000,000): period f-points
  printf("(0,%3d,%3d):   %4d %7d ",a,b,n,fp);
  reduce_print(&r);
}

// run some tests and spew out some stats
void test(uint32_t p)
{
  uint32_t bits = 1<<p;
  uint32_t c = 0;
  uint32_t h[LOG2_MAX_BITS] = {0};
  uint32_t ifail = 0;

  printf("%4d-bit test: ", bits);

  // walk though all functions
  for(uint32_t a=1; a<bits-1; a++) {
    for(uint32_t b=a+1; b<=bits-1; b++) {
      uint32_t n  = ctz(b-a);
      reduce_t r;
      uint32_t t;
      reduce(&r,p,a,b);
      t = reduce_validate(p,&r);
      ifail += t;

#if 0
      // debug thing
      if (t != 0) {
	printf("xxx (0,%3d,%3d) -> ",a,b);
	reduce_print(&r);
	if (ifail > 5) return;
      }
#endif      

      // period is: bits >> n
      h[n]++;
      c++;
    }
  }

  // this should be: (2-3*2^p+4^p)/2
  printf("total funcs: %6d, period counts: ", c);

  // these should be: 2^(n-1)*bits-1
  for(int i=p-1; i>=0; i--)
    printf("%d ", h[i]);
  
  if (ifail != 0) {
    if (ifail != c)
      printf(" ** %d inverse fails**", ifail);
    else
      printf(" ** inverse not tested **");
  }
  printf("\n");
}


int main(int argc, char** argv)
{
  if (argc == 1) {
    for(uint32_t p=5; p<=LOG2_MAX_BITS; p++)
      test(p);
  }
  else {
    uint32_t bits = atoi(argv[1]);
    uint32_t p    = ctz(bits);
    argv += 2;
    argc -= 2;

    if (p <= LOG2_MAX_BITS && (1<<p) == bits) {
      xorrot_print_info_header();
      
      while(argc >= 2) {
	argc -= 2;
	uint32_t a = atoi(*argv++);
	uint32_t b = atoi(*argv++);
	xorrot_print_info(p,a,b);
      }
    } else printf("nope..try again\n");  
  }
  
  return 0;
}

