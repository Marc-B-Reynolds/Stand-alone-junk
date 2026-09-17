// -*- coding: utf-8 -*-

#include <stdint.h>
#include <stdbool.h>

// LAZY!
#include "SFH/intops/prime.c"

#include <stdio.h>
#include <math.h>
#include <string.h>

// Table[With[{n = PrimePi[2^i-1]}, {n, "0x" <> IntegerString[Prime[n], 16]}], {i,8,32}]
const pair_u32_t test_vector[] = {
  {.a=54, .b=0xfb},
  {.a=97, .b=0x1fd},
  {.a=172, .b=0x3fd},
  {.a=309, .b=0x7f7},
  {.a=564, .b=0xffd},
  {.a=1028, .b=0x1fff},
  {.a=1900, .b=0x3ffd},
  {.a=3512, .b=0x7fed},
  {.a=6542, .b=0xfff1},
  {.a=12251, .b=0x1ffff},
  {.a=23000, .b=0x3fffb},
  {.a=43390, .b=0x7ffff},
  {.a=82025, .b=0xffffd},
  {.a=155611, .b=0x1ffff7},
  {.a=295947, .b=0x3ffffd},
  {.a=564163, .b=0x7ffff1},
  {.a=1077871, .b=0xfffffd},
  {.a=2063689, .b=0x1ffffd9},
  {.a=3957809, .b=0x3fffffb},
  {.a=7603553, .b=0x7ffffd9},
  {.a=14630843, .b=0xfffffc7},
  {.a=28192750, .b=0x1ffffffd},
  {.a=54400028, .b=0x3fffffdd},
  {.a=105097565, .b=0x7fffffff},
  {.a=203280221, .b=0xfffffffb}
};


void test_all(void)
{
  uint32_t x   = 3;
  uint32_t cnt = 1;    // we're skipping two
  uint32_t m   = 0xff;
  uint32_t l   = 2;

  printf("test all\n");

  int e = 25;

  //e = 10; // temp hack
  
  for(int i=0; i<e; i++) {
    printf("  [%08x, %08x] :",(x)&(~3u),x|m);
    fflush(stdout);

    do {
      bool v = is_prime_u32(x);
      l    = v ? x : l;               // track last prime
      cnt += v;                       // count seen primes
      x   += 2;
    } while((x & m) != 1);

    printf("%u %u", cnt, l);

    // check computed vs. test vector
    if ((cnt != test_vector[i].a) || (l != test_vector[i].b)) {
      printf(" FAIL: should be %u %u", test_vector[i].a, test_vector[i].b);
    }
    
    printf("\n");
    if (m == (uint32_t)(-1)) break;
    
    m |= (m<<1);
  }
}



int main(void)
{
  test_all();

  return 0;
}
