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

// 64-bit tables
// table of (mostly) strong pseudo primes
//   from annotated-psps-below-2-to-64.txt
//   https://github.com/janfeitsma/psp2
static const uint64_t ssp[] = {   //      line:   factors
  UINT64_C(          4367580493), //     10488: S 7789,560737
  UINT64_C(          4579461601), //     10695: C 11,13,29,43,61,421
  UINT64_C(         10767313577), //     15362: S 34589,311293
  UINT64_C(         21597596693), //     20529: S 93997,229769
  UINT64_C(         96513897391), //     38408: S 58711,1643881
  UINT64_C(        607635714421), //     82700: S 551197,1102393
  UINT64_C(       1435151140751), //    118092: S 1176391,1219961
  UINT64_C(       2732310234139), //    154614: S 413243,6611873
  UINT64_C(      69328929744361), //    589688: C 7,131,157,313,521,2953
  UINT64_C(    1901144131488253), //   2358747: S 30831349 61662697
  UINT64_C(    7184310885566047), //   4127787: S 51112393,140559079
  UINT64_C(   33911839972153249), //   7960721: S 25786393,1315105993
  UINT64_C(   56370226140903553), //   9877199: S 27821081,2026169513
  UINT64_C(   93272521892736997), //  12235939: S 258580813,360709369
  UINT64_C(  230009522353054601), //  17985330: S 70457323,3264522587
  UINT64_C(  490764915315675001), //  24875236: S 495360937,990721873
  UINT64_C(  704253761715274237), //  29041869: S 329160649,2139544213
  UINT64_C( 1082847644532417349), //  34938694: S 198434569,5456950621
  UINT64_C(10104474739839124217), //  91621119: S 1059584549,9536260933
  UINT64_C(13574630470989219157), // 104135979: S 715716569,18966489053
  UINT64_C(18446744066047760377), // 118968378: S 1920767767,9603838831
};

// series of primes on [2^32,2^64-1]
static const uint64_t prime_table[] = {
  UINT64_C(4294974227), UINT64_C(6442480817),
  UINT64_C(9663694031), UINT64_C(14495567389),
  UINT64_C(21743333141), UINT64_C(32614964861),
  UINT64_C(48922412347), UINT64_C(73383566089),
  UINT64_C(110075336911), UINT64_C(165113042849),
  UINT64_C(247669518221), UINT64_C(371504279677),
  UINT64_C(557256383111), UINT64_C(835884549079),
  UINT64_C(1253826802541), UINT64_C(1880740124009),
  UINT64_C(2821110190979), UINT64_C(4231665328669),
  UINT64_C(6347497916533), UINT64_C(9521246922157),
  UINT64_C(14281870362541), UINT64_C(21422805455021),
  UINT64_C(32134208201351), UINT64_C(48201312288053),
  UINT64_C(72301968441497), UINT64_C(108452952592697),
  UINT64_C(162679428909511), UINT64_C(244019143321747),
  UINT64_C(366028714941581), UINT64_C(549043072426297),
  UINT64_C(823564608646421), UINT64_C(1235346912984637),
  UINT64_C(1853020369413607), UINT64_C(2779530554123033),
  UINT64_C(4169295831171469), UINT64_C(6253943746737131),
  UINT64_C(9380915620063543), UINT64_C(14071373430094217),
  UINT64_C(21107060145122761), UINT64_C(31660590217623769),
  UINT64_C(47490885326410567), UINT64_C(71236327989660149),
  UINT64_C(106854491984449031), UINT64_C(160281737976624569),
  UINT64_C(240422606964998737), UINT64_C(360633910447461227),
  UINT64_C(540950865671113663), UINT64_C(811426298506687627),
  UINT64_C(1217139447759994417), UINT64_C(1825709171639984381),
  UINT64_C(2738563757460019319), UINT64_C(4107845636190005863),
  UINT64_C(6161768454284966521), UINT64_C(9242652681427439033),
  UINT64_C(13863979022141142007), UINT64_C(2349224459502084301),
  UINT64_C(3523836689253128279), UINT64_C(5285755033879734937),
  UINT64_C(7928632550819536027), UINT64_C(11892948826229352001),
  UINT64_C(0xffffffffffffffc5)      // largest prime < 2^64
};


#ifndef LENGHTOF
#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))
#endif

void test_all(void)
{
  uint32_t x   = 3;
  uint32_t cnt = 1;    // we're skipping two
  uint32_t m   = 0xff;
  uint32_t l   = 2;

  printf("is_prime_u32\n");

  int e = 25;

  e = 20; // temp hack
  
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
  printf("  done\n");

  printf("is_prime_u64\n");

  for(size_t i=0; i<LENGTHOF(ssp); i++) {
    if (is_prime_u64(ssp[i]) == 0) continue;
    printf("X(%u)",(uint32_t)i);
  }

  for(size_t i=0; i<LENGTHOF(prime_table); i++) {
    if (is_prime_u64(prime_table[i]) == 1) continue;
    printf("X(%u)",(uint32_t)i);
  }
  printf("  done\n");
}



int main(void)
{
  test_all();

  return 0;
}
