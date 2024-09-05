
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define CARRYLESS_IMPLEMENTATION

#include "carryless.h"
#include "prng_small.h"

#if 0

static const uint32_t test_runs = 0x10000000;

uint32_t cl_unit_rng_32(rng_t* rng) { return rng_next_u32(rng)|1u; }
uint32_t cr_unit_rng_32(rng_t* rng) { return rng_next_u32(rng)|(1u<<31); }
uint32_t cc_unit_rng_32(rng_t* rng)
{
  uint32_t r = cl_unit_rng_32(rng); return r ^= (r>>1);
}

uint64_t cl_unit_rng_64(rng_t* rng) { return rng_next_u64(rng)|1u; }
uint64_t cr_unit_rng_64(rng_t* rng) { return rng_next_u64(rng)|(1u<<63); }
uint64_t cc_unit_rng_64(rng_t* rng)
{
  uint64_t r = cl_unit_rng_64(rng); return r ^= (r>>1);
}

typedef struct {
  char*    name;
  uint32_t identity;
  uint32_t (*draw)(rng_t*);
  uint32_t (*inv)(uint32_t);
  uint32_t (*mul)(uint32_t,uint32_t);
} def_32_t;

typedef struct {
  char*    name;
  uint64_t identity;
  uint64_t (*draw)(rng_t*);
  uint64_t (*inv)(uint64_t);
  uint64_t (*mul)(uint64_t,uint64_t);
} def_64_t;


uint32_t test_32(def_32_t* def, rng_t* rng)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<test_runs; i++) {
    uint32_t a = def->draw(rng);
    uint32_t b = def->mul(a);
    uint32_t r = def->mul(a,b);
    
    if (r != def->identity) {
      printf("%08x : %08x %08x\n",a,b,r);
    }
  }
    
    return errors;
}

uint64_t test_64(def_64_t* def, rng_t* rng)
{
  uint64_t errors = 0;

  for(uint64_t i=0; i<test_runs; i++) {
    uint64_t a = def->draw(rng);
    uint64_t b = def->mul(a);
    uint64_t r = def->mul(a,b);
    
    if (r != def->identity) {
      printf("%016lx : %016lx %016lx\n",a,b,r);
    }
  }
    
  return errors;
}
  
#endif


int main(void)
{
  return 0;
}
