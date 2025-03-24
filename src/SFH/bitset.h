
#pragma once

// super minimal (size & function) dense bitset. more bitvector than set
// but whatever.

// * needs defines for 64-bit popcount, leading & trailing zeros count
//   (pop_64/clz_64/ctz_64) zero count are never fed an input of zero.

static inline uint64_t bitset_ctz(uint64_t x) { return ctz_64(x); }
static inline uint64_t bitset_pop(uint64_t x) { return pop_64(x); }

typedef struct
{
  uint64_t  n;
  uint64_t  datasize;
  uint64_t* data;
} bitset_t;

// compile time configured bounds checking

static const uint64_t bitset_none = ~UINT64_C(0);

#if !defined(BITSET_NO_BOUND_CHECK)
#include <assert.h> // temp hack

static inline uint32_t bitset_check_bound(const bitset_t* const set, uint64_t n)
{
  assert(n < set->n);  // temp hack

  return 1;            // temp hack
}
#else
#define bitset_check_bound(S,N) (1)
#endif


// zero all elements of the set
static inline void bitset_empty(bitset_t* bitset)
{
  memset(bitset->data, 0, sizeof(uint64_t)*bitset->datasize);
}

// number of chunks in `data` for `n` elements
static inline uint64_t bitset_datasize(uint64_t n)
{
  uint64_t q = n >> 6;      // n / 64
  uint64_t r = n & 0x3f;    // n % 64

  return q + ((r == 0) ? 0 : 1);
}

// total number of bytes needed for `n` elements
static inline uint64_t bitset_memsize(uint64_t n)
{
  uint64_t esize = bitset_datasize(n);
  uint64_t bsize = esize << 4;
  uint64_t tsize = sizeof(bitset_t) + bsize;

  return tsize;
}

static inline uint32_t bitset_is_fixed(bitset_t* set)
{
  return set->data == (uint64_t*)(set+1);
}

static inline bitset_t* bitset_init_fixed(uint64_t n, void* mem)
{
  bitset_t* set = (bitset_t*)mem;

  if (set != NULL) {
    set->n        = n;
    set->datasize = bitset_datasize(n);
    set->data     = (uint64_t*)(set+1);
    bitset_empty(set);
  }
  
  return set;
}


#if !defined(BITSET_NO_MALLOC)

static inline bitset_t* bitset_create_fixed(uint64_t n)
{
  void* mem = (bitset_t*)malloc(bitset_memsize(n));
  
  return bitset_init_fixed(n, mem);
}

static inline void bitset_free(bitset_t* set)
{
  if (!bitset_is_fixed(set))
    free(set->data);
  free(set);
}

#endif


static inline uint64_t bitset_data_bit(uint64_t i) { return UINT64_C(1) << (i & 0x3f); }

static inline void bitset_set(bitset_t* set, uint64_t i)
{
  if (bitset_check_bound(set, i))
    set->data[i>>6] |= bitset_data_bit(i);
}

static inline void bitset_clear(bitset_t* set, uint64_t i)
{
  if (bitset_check_bound(set, i))
    set->data[i>>6] &= ~bitset_data_bit(i);
}

static inline void bitset_toggle(bitset_t* set, uint64_t i)
{
  if (bitset_check_bound(set, i))
    set->data[i>>6] ^= bitset_data_bit(i);
}

// returns non-zero value if element `i` is set
static inline uint64_t bitset_get_i(const bitset_t* set,  uint64_t i)
{
  if (bitset_check_bound(set, i))
    return set->data[i >> 6] & bitset_data_bit(i);

  return 0;
}

// returns value of element `i`
static inline uint64_t bitset_get(const bitset_t* set,  uint64_t i)
{
  return bitset_get_i(set, i) != 0;
}


// set element `i` and return non-zero value if it was already set
static inline uint64_t bitset_set_test(bitset_t* set, uint64_t i)
{
  if (bitset_check_bound(set, i)) {
    uint64_t e = i >> 6;
    uint64_t b = bitset_data_bit(i);
    uint64_t p = set->data[e] & b;
    
    set->data[e] |= b;
    
    return p;
  }
  return 0;
}

// clear element `i` and return non-zero value if it was set
static inline uint64_t bitset_clear_test(bitset_t* set, uint64_t i)
{
  if (bitset_check_bound(set, i)) {
    uint64_t e = i >> 6;
    uint64_t b = bitset_data_bit(i);
    uint64_t p = set->data[e] & b;
    
    set->data[e] &= ~b;
    
    return p;
  }
  return 0;
}


//------- could use SIMD for everything below here. meh. lazy.

static inline uint64_t bitset_min_u64(uint64_t a, uint64_t b)
{
  return (a <= b) ? a : b;
}

// returns the index 
uint64_t bitset_lo_diff(const bitset_t * s0, const bitset_t * s1)
{
  uint64_t e = bitset_min_u64(s0->datasize, s1->datasize);

  for(uint64_t i=0; i<e; i++) {
    uint64_t t = s0->data[i] ^ s1->data[i];
    
    if (t == 0) continue;

    // not handling the potential edge case when i=e-1
    return (i << 6) + bitset_ctz(t);
  }
  
  return bitset_none;
}

// returns `true` if the two set have no common elements
static inline bool bitset_disjoint(const bitset_t * s0, const bitset_t * s1)
{
  uint64_t e = bitset_min_u64(s0->datasize, s1->datasize);
  
  for(uint64_t i=0; i<e; i++) {
    if ((s0->data[i] & s1->data[i]) != 0) return false;
  }
  
  return true;
}

// returns `true` if the two set have at one or more common elements
static inline bool bitset_intersects(const bitset_t * s0, const bitset_t * s1)
{
  return !bitset_disjoint(s0,s1);
}


// returns the index of the lowest clear bit
static inline uint64_t bitset_lowest_clear(const bitset_t* set)
{
  for (uint64_t i=0; i < set->datasize; i++) {
    uint64_t v = set->data[i];

    if (v == bitset_none) continue;

    uint64_t r = (i << 6) + bitset_ctz(~v);

    if (r < set->n) return r;
  }
  
  return bitset_none;
}

// returns the index of the lowest set bit.
static inline uint64_t bitset_lowest_set(const bitset_t* set)
{
  for (uint64_t i=0; i < set->datasize; i++) {
    uint64_t v = set->data[i];
    if (v == 0) continue;
    return (i << 6) + bitset_ctz(v);
  }
  
  return bitset_none;
}

// returns the number of set bits in the set (the cardinality)
static inline uint64_t bitset_count(const bitset_t* set)
{
  uint64_t s0 = 0;
  uint64_t s1 = 0;
  uint64_t s2 = 0;
  uint64_t s3 = 0;
  uint64_t i  = 0;

  uint64_t* data = set->data;
  uint64_t  size = set->datasize;
  uint64_t  c    = size >> 2;

  while(c--) {
    s0 += bitset_pop(data[i  ]);
    s1 += bitset_pop(data[i+1]);
    s2 += bitset_pop(data[i+2]);
    s3 += bitset_pop(data[i+3]);
    i  += 4;
  }

  s2 += s3;
  
  if (size & 2) {
    s0 += bitset_pop(data[i  ]);
    s1 += bitset_pop(data[i+1]);
    i  += 2;
  }

  s1 += s2;
  
  if (size & 1) 
    s0 += bitset_pop(data[i]);

  return s0+s1;
}
