// Marc B. Reynolds, 2017
// Public Domain under http://unlicense.org, see link for details.

#ifndef LPRNS_H
#define LPRNS_H

#ifdef __cplusplus
extern "C" {
#endif

// comments after constants: prime factorization, population count
  
// for overriding Weyl constant and its multiplicative inverse
#ifndef LPRNS_WEYL  
#define LPRNS_WEYL   0x3504f333   // 3*2309*128413, 15
#define LPRNS_WEYL_I 0x02843ffb   // 151*279613,    16
//#define LPRNS_WEYL   0x6a09e667   // 83*727*294833, 16
//#define LPRNS_WEYL_I 0xb39d557    // 7*29*269*3449, 17
#endif

// for overriding default mixing function
#ifndef LPRNS_MIX
#define LPRNS_MIX lprns_mix
#endif

// for overriding default stream mixing function
#ifndef LPRNS_STREAM_MIX
#define LPRNS_STREAM_MIX lprns_stream_mix
#endif

// for overriding default values of standard mixing function:
// cascade of two xorshift multiplies + xorshift
#ifndef LPRNS_M0  

// example: ad hoc (questionable p-value @ ~2.4%)
//static const uint32_t LPRNS_M0 = 0x5f356495;  // 929*1719413, 17
//static const uint32_t LPRNS_M1 = 0xac564b05;  // 29*99701257, 14
//static const uint32_t LPRNS_S0 = 16;
//static const uint32_t LPRNS_S1 = 16;
//static const uint32_t LPRNS_S2 = 16;

// example: ad hoc (questionable p-value @ ~3.8%)
//static const uint32_t LPRNS_M0 = 0x5f356495;  // 929*1719413, 17
//static const uint32_t LPRNS_M1 = 0xac564b05;  // 29*99701257, 14
//static const uint32_t LPRNS_S0 = 15;
//static const uint32_t LPRNS_S1 = 14;
//static const uint32_t LPRNS_S2 = 16;

// example: using xxhash's finalizer (questionable p-value @ ~2.2%)
static const uint32_t LPRNS_M0 = 0x85ebca77;  // 2246822519, 19
static const uint32_t LPRNS_M1 = 0xc2b2ae3d;  // 3266489917, 17
static const uint32_t LPRNS_S0 = 15;
static const uint32_t LPRNS_S1 = 13;
static const uint32_t LPRNS_S2 = 16;

// example: using murmur3's finalizer (questionable p-value @ ~3.1%)
//static const uint32_t LPRNS_M0 = 0x85ebca6b;  // 15809*142123, 18
//static const uint32_t LPRNS_M1 = 0xc2b2ae35;  // 1223*1483*1801, 16
//static const uint32_t LPRNS_S0 = 16;
//static const uint32_t LPRNS_S1 = 13;
//static const uint32_t LPRNS_S2 = 16;
  
#endif

// an example initial stream constant
static const uint32_t LPRNS_STREAM_M = 0x24051d5;  // 3*5*113*22283, 10

// raw structure-less routines
  
static inline uint32_t lprns_mix(uint32_t x)
{
  x ^= x >> LPRNS_S0; x *= LPRNS_M0;
  x ^= x >> LPRNS_S1; x *= LPRNS_M1;
  x ^= x >> LPRNS_S2;
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

#ifndef LPRNS_ALT_STREAMS  

static inline uint32_t lprns_stream_next_k(uint32_t x)
{
  return (lprns_next_pop(x>>3) << 3) | 5;
}  

static inline uint32_t lprns_stream_mix(uint32_t x, uint32_t m)
{
  x ^= x >> LPRNS_S0; x *= m;
  x ^= x >> LPRNS_S1; x *= LPRNS_M0;
  x ^= x >> LPRNS_S2;
  
  return x;
}

#else

// just to be compatiable
static inline uint32_t lprns_stream_next_k(uint32_t x)
{
  return x+1;
}  
  
static inline uint32_t lprns_stream_mix(uint32_t x, uint32_t m)
{
  x ^= m;
  x ^= x >> LPRNS_S0; x *= LPRNS_M0;
  x ^= x >> LPRNS_S1; x *= LPRNS_M1;
  x ^= x >> LPRNS_S2;
  return x;
}
#endif

  
static inline uint32_t lprns_stream_at_raw(uint32_t n, uint32_t m)
{
  return LPRNS_STREAM_MIX(LPRNS_WEYL*n, m);
}

  static inline uint32_t lprns_stream_at(lprns_stream_t* gen, uint32_t n)
{
  return LPRNS_STREAM_MIX(LPRNS_WEYL*n, gen->m);
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
