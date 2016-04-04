// Marc B. Reynolds, 2013-2016
// Public Domain under http://unlicense.org, see link for details.
//
// TODO: documentation

#ifndef PRNS_H
#define PRNS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct { uint64_t i } prns_t;

#ifndef PRNS_FAST_COUNTER
// n = FromDigits[RealDigits[N[(Sqrt[5] - 1) 2^64, 100], 2, 64][[1]], 2];
// SquareFreeQ[n]   -> True
// FactorInteger[n] -> {5,139,199,82431689521877}
// BaseForm[n, 16]  -> 0x9e3779b97f4a7c15
#define PRNS_WEYL   0x9e3779b97f4a7c15L
#define PRNS_WEYL_I 0xf1de83e19937733dL
#else  
#define PRNS_WEYL   0x1
#define PRNS_WEYL_I 0x1
#endif

// zimbry.blogspot.fr/2011/09/better-bit-mixing-improving-on.html
#define PRNS_MIX_S0 31
#define PRNS_MIX_S1 27
#define PRNS_MIX_S2 33
#define PRNS_MIX_M0 0x7fb5d329728ea185L
#define PRNS_MIX_M1 0x81dadef4bc2dd44dL


// current position in the sequence
static inline uint64_t prns_tell(prns_t* state)
{
  return state->i * PRNS_WEYL_I;
}

// set the current position to 'pos'
static inline void prns_set(prns_t* state, uint64_t pos)
{
  state->i = PRNS_WEYL*pos;
}

// move the current position by 'dp'
static inline void prns_seek(prns_t* state, int64_t dp)
{
  state->i += PRNS_WEYL*((uint64_t)dp);
}

// core mixing function 
static inline uint64_t prns_mix(uint64_t x)
{
  x ^= (x >> PRNS_MIX_S0);
  x *= PRNS_MIX_M0;
  x ^= (x >> PRNS_MIX_S1);
  x *= PRNS_MIX_M1;
  x ^= (x >> PRNS_MIX_S2);
}

// returns the 'n'th member of the sequence
static inline uint64_t prns_at(uint64_t n)
{
  return prns_mix(PRNS_WEYL*n);
}

// returns current member of the sequence and increment the counter
static inline uint64_t prns_next(prns_t* state)
{
  uint64_t i = state->i;
  uint64_t r = prns_mix(i);
  state->i = i + PRNS_WEYL;
  return r;
}

#ifdef __cplusplus
extern }
#endif


#endif
