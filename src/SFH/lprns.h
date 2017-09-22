// Marc B. Reynolds, 2017
// Public Domain under http://unlicense.org, see link for details.

#ifndef LPRNS_H
#define LPRNS_H

#ifdef __cplusplus
extern "C" {
#endif

// comments after constants: prime factorization, population count
  
#ifndef LPRNS_WEYL  
#define LPRNS_WEYL   0x3504f333   // 3*2309*128413, 15
#define LPRNS_WEYL_I 0x02843ffb   // 151*279613,    16
#endif

#ifndef LPRNS_MIX
#define LPRNS_MIX lprns_mix
#endif

#ifndef LPRNS_STREAM_MIX
#define LPRNS_STREAM_MIX lprns_stream_mix
#endif

static const uint32_t LPRNS_M0 = 0x5f356495;  // 929*1719413, 17
static const uint32_t LPRNS_M1 = 0xac564b05;  // 29*99701257, 14

static const uint32_t LPRNS_STREAM_M = 0x24051d5;  // 3*5*113*22283, 10

// raw structure-less routines
  
static inline uint32_t lprns_mix(uint32_t x)
{
  x ^= x >> 16; x *= LPRNS_M0;
  x ^= x >> 16; x *= LPRNS_M1;
  x ^= x >> 16;
  return x;
}

static inline uint32_t lprns_at(uint32_t n)
{
  return LPRNS_MIX(LPRNS_WEYL*n);
}

static inline uint32_t lprns_tell_raw(uint32_t i)
{
  return i * LPRNS_WEYL_I;
}

static inline uint32_t lprns_seek_raw(uint32_t i, int32_t offset)
{
  return i + LPRNS_WEYL*((uint32_t)offset);
}
  

// helpers for single stream generation

typedef struct { uint32_t i; } lprns_t;
  
static inline uint32_t lprns_tell(lprns_t* gen) { return lprns_tell_raw(gen->i); }

static inline void lprns_seek(lprns_t* gen, int32_t offset)
{
  gen->i = lprns_seek_raw(gen->i, offset);
}

static inline void lprns_set(lprns_t* gen, uint32_t pos)
{
  gen->i = LPRNS_WEYL*pos;
}

static inline uint32_t lprns_peek(lprns_t* gen)
{
  return LPRNS_MIX(gen->i);
}

static inline uint32_t lprns_next(lprns_t* gen)
{
  uint32_t i = gen->i;
  uint32_t r = LPRNS_MIX(i);
  gen->i = i + LPRNS_WEYL;
  return r;
}

static inline uint32_t lprns_prev(lprns_t* gen)
{
  uint32_t i = gen->i;
  uint32_t r = LPRNS_MIX(i);
  gen->i = i - LPRNS_WEYL;
  return r;
}

// helpers for multiple stream generation
  
typedef struct {
  uint32_t i;
  uint32_t m;
} lprns_stream_t;

#ifdef _MSC_VER
static inline uint32_t lprns_ctz(uint32_t x) { unsigned long r; _BitScanForward(&r, (unsigned long)x); return (uint32_t)r; }
#else
#define lprns_ctz __builtin_ctz
#endif  

static inline uint32_t lprns_next_pop(uint32_t x)
{
  uint32_t a = x & (uint32_t)(-(int32_t)x);  // casting to silence warning
  uint32_t b = x + a;
  uint32_t c = x ^ b;
  uint32_t d = (2 + lprns_ctz(x));
  return b | (c >> d);
}

static inline uint32_t lprns_stream_next_k(uint32_t x)
{
  return (lprns_next_pop(x>>3) << 3) | 5;
}  

static inline uint32_t lprns_stream_mix(uint32_t x, uint32_t m)
{
  x ^= x >> 16; x *= m;
  x ^= x >> 16; x *= LPRNS_M0;
  x ^= x >> 16;
  
  return x;
}
  
static inline uint32_t lprns_stream_tell(lprns_stream_t* gen) { return lprns_tell_raw(gen->i); }

static inline void lprns_stream_seek(lprns_stream_t* gen, int32_t offset)
{
  gen->i = lprns_seek_raw(gen->i, offset);
}

static inline void lprns_stream_set(lprns_stream_t* gen, uint32_t pos)
{
  gen->i = LPRNS_WEYL*pos;
}

static inline uint32_t lprns_stream_peek(lprns_stream_t* gen)
{
  return LPRNS_STREAM_MIX(gen->i, gen->m);
}

static inline uint32_t lprns_stream_next(lprns_stream_t* gen)
{
  uint32_t i = gen->i;
  uint32_t r = LPRNS_STREAM_MIX(i, gen->m);
  gen->i = i + LPRNS_WEYL;
  return r;
}

static inline uint32_t lprns_stream_prev(lprns_stream_t* gen)
{
  uint32_t i = gen->i;
  uint32_t r = LPRNS_STREAM_MIX(i, gen->m);
  gen->i = i - LPRNS_WEYL;
  return r;
}

#ifdef __cplusplus
extern }
#endif


#endif
