// Marc B. Reynolds, 2013-2016
// Public Domain under http://unlicense.org, see link for details.
//
// Documentation: http://marc-b-reynolds.github.io/shf/2016/04/19/prns.html

#ifndef PRNS_H
#define PRNS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct { uint64_t i; } prns_t;

#ifndef PRNS_WEYL
#define PRNS_WEYL   0x61c8864680b583ebL
#define PRNS_WEYL_I 0x0e217c1e66c88cc3L
#endif

#ifndef PRNS_WEYL_D
#define PRNS_WEYL_D 0x4f1bbcdcbfa54001L
#endif

#ifndef PRNS_MIX_S0
#ifdef  PRNS_MIX_13
#define PRNS_MIX_S0 30
#define PRNS_MIX_S1 27
#define PRNS_MIX_S2 31
#define PRNS_MIX_M0 0xbf58476d1ce4e5b9L
#define PRNS_MIX_M1 0x94d049bb133111ebL
#else
#define PRNS_MIX_S0 31
#define PRNS_MIX_S1 27
#define PRNS_MIX_S2 33
#define PRNS_MIX_M0 0x7fb5d329728ea185L
#define PRNS_MIX_M1 0x81dadef4bc2dd44dL
#endif
#endif

#ifndef PRNS_MIX
#define PRNS_MIX(X) prns_mix(X)
#endif 

#ifndef PRNS_MIX_D
#define PRNS_MIX_D(X) prns_mix(X)
#endif    

  
static inline uint64_t prns_tell(prns_t* gen)
{
  return gen->i * PRNS_WEYL_I;
}

static inline void prns_set(prns_t* gen, uint64_t pos)
{
  gen->i = PRNS_WEYL*pos;
}

static inline void prns_seek(prns_t* gen, int64_t offset)
{
  gen->i += PRNS_WEYL*((uint64_t)offset);
}

static inline uint64_t prns_mix(uint64_t x)
{
  x ^= (x >> PRNS_MIX_S0);
  x *= PRNS_MIX_M0;
  x ^= (x >> PRNS_MIX_S1);
  x *= PRNS_MIX_M1;
  
#ifndef PRNS_NO_FINAL_XORSHIFT
  x ^= (x >> PRNS_MIX_S2);
#endif

  return x;
}

static inline uint64_t prns_at(uint64_t n)
{
  return prns_mix(PRNS_WEYL*n);
}

static inline uint64_t prns_peek(prns_t* gen)
{
  return PRNS_MIX(gen->i);
}

static inline uint64_t prns_next(prns_t* gen)
{
  uint64_t i = gen->i;
  uint64_t r = PRNS_MIX(i);
  gen->i = i + PRNS_WEYL;
  return r;
}

static inline uint64_t prns_prev(prns_t* gen)
{
  uint64_t i = gen->i;
  uint64_t r = PRNS_MIX(i);
  gen->i = i - PRNS_WEYL;
  return r;
}

static inline uint64_t prns_start_down(prns_t* gen)
{
  return gen->i + PRNS_WEYL_D;
}

static inline uint64_t prns_down(uint64_t* state)
{
  uint64_t i = *state;
  uint64_t r = PRNS_MIX_D(i);
  *state = i + PRNS_WEYL_D;
  return r;
}

#ifdef __cplusplus
extern }
#endif


#endif
