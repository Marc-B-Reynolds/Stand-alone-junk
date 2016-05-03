
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

//#define PRNS_FAST_COUNTER
#include "../prns.h"

#define L_POW 5
#define LEN (1<<L_POW)

uint64_t history[LEN];

#define HASH_ITEMS 0xFFFFF

#if 1 // LAZY HERE
#include <x86intrin.h>
#endif

// cut-n-paste of xoroshiro128+
// http://xoroshiro.di.unimi.it/xoroshiro128plus.c
uint64_t s[2];

static inline uint64_t rotl(const uint64_t x, int k)
{
  return (x << k) | (x >> (64 - k));
}

uint64_t next(void) {
  const uint64_t s0 = s[0];
  uint64_t s1 = s[1];
  const uint64_t result = s0 + s1;
  
  s1 ^= s0;
  s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
  s[1] = rotl(s1, 36); // c
  
  return result;
}

// end of cut-n-paste

void spot_check(uint64_t e, uint64_t a, char* msg)
{
  if (e==a) return;

  printf("error: 0x%0" PRIx64  " 0x%0" PRIx64 " %s\n", e, a, msg);
}

// minimal spot checks
int main(int argc, char** argv)
{
  prns_t rng;
  uint32_t i = 0;
  uint64_t o = __rdtsc(); // <- LAZY HERE
  uint64_t t;

  s[0]=o;
  s[1]=1;

  //*** PRNG testing
  printf("PRNG --testing members: [0x%0" PRIx64  ", 0x%0" PRIx64 "] \n", o, o+LEN);

  prns_set(&rng, o);

  // history = LEN members starting at 'o'
  for(i=0; i<LEN; i++) {
    history[i] = prns_next(&rng);
    spot_check(prns_tell(&rng), i+o+1, "tell fubar");
  }

  // current position should be LEN
  t = prns_tell(&rng);

  spot_check(LEN, t-o, "length fubar");

  // check explict member query against history
  for(i=0; i<LEN; i++)
    spot_check(history[i], prns_at(i+o), "at fubar");

  // seek back to 'o' and check
  prns_seek(&rng, -LEN);
  spot_check(history[0], prns_next(&rng), "seek fubar 1");

  // run backwards through the history with prev
  prns_set(&rng, t-1);

  for(i=LEN-1; i<LEN; i--) {
    spot_check(history[i], prns_prev(&rng), "prev fubar");
  }

  //*** Insanely poor hash testing

  memset(history, 0, 8*LEN);
  
  for(i=0; i<HASH_ITEMS; i++) {
    // choose some statistically random coordinates
    uint64_t x = next() >> 32;
    uint64_t y = next() >> 32; 
    uint64_t h = prns_mix(x<<32 ^ y);

    history[h >> (64-L_POW)]++;
  }

  // Pearson's
  double chi = 0.0;
  double ave = (1.0*HASH_ITEMS)/LEN;  // Ei
  double s   = 1.0/ave;
  double d;
  
  for(i=0; i<LEN; i++) {
    d   =  history[i]-ave;
    chi += d*d;
  }
  chi *= s;

  printf("chi squared =  %f\n\n", chi);

  printf("---- python cut-n-paste for p-value --------\nfrom scipy.stats import chisquare\nchisquare([");
  for(i=0; i<LEN-1; i++)
    printf("%d, ", (uint32_t)history[i]);

  printf("%d], f_exp=%d)\n", (uint32_t)history[i], HASH_ITEMS/LEN);
}


