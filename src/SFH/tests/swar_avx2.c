// you really don't want to look at this

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "bitops.h"
#include "intops.h"
#include "prng_small.h"

#include "swar_avx2.h"

#define for_range(I,S,E) for(uint32_t I=S; I<E; I++)

#define wrap_reg_8(A,R)  uint8_t  A[32]; memcpy(A,&R,32);
#define wrap_reg_16(A,R) uint16_t A[16]; memcpy(A,&R,32);
#define wrap_reg_32(A,R) uint32_t A[ 8]; memcpy(A,&R,32);
#define wrap_reg_64(A,R) uint64_t A[ 4]; memcpy(A,&R,32);
#define back_2_reg(R,A)  memcpy(&R,A,32);


// for reference functions that are simple repeats in each element

static inline __m256i avx2_wrap_ufunc_8(__m256i x, uint32_t (*f)(uint32_t))
{
  wrap_reg_8(a,x); for_range(i,0,32) {a[i]=(uint8_t)f(a[i]);} back_2_reg(x,a); return x;
}

static inline __m256i avx2_wrap_ufunc_16(__m256i x, uint32_t (*f)(uint32_t))
{
  wrap_reg_16(a,x); for_range(i,0,16) {a[i]=(uint16_t)f(a[i]);} back_2_reg(x,a); return x;
}

static inline __m256i avx2_wrap_ufunc_32(__m256i x, uint32_t (*f)(uint32_t))
{
  wrap_reg_32(a,x); for_range(i,0, 8) {a[i]=(uint32_t)f(a[i]);} back_2_reg(x,a); return x;
}

static inline __m256i avx2_wrap_ufunc_64(__m256i x, uint64_t (*f)(uint64_t))
{
  wrap_reg_64(a,x); for_range(i,0, 4) {a[i]=(uint64_t)f(a[i]);} back_2_reg(x,a); return x;
}


#define DEF_WRAP_8(N,T)  static inline __m256i N##_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x, T); } 
#define DEF_WRAP_16(N,T) static inline __m256i N##_16x16_ref(__m256i x) { return avx2_wrap_ufunc_16(x, T); }
#define DEF_WRAP_32(N,T) static inline __m256i N##_32x8_ref (__m256i x) { return avx2_wrap_ufunc_32(x, T); }
#define DEF_WRAP_64(N,T) static inline __m256i N##_64x4_ref (__m256i x) { return avx2_wrap_ufunc_64(x, T); }

#define DEF_SIMPLE_8(N)  static inline __m256i N##_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x, N##_32); } 
#define DEF_SIMPLE_16(N) static inline __m256i N##_16x16_ref(__m256i x) { return avx2_wrap_ufunc_16(x, N##_32); }
#define DEF_SIMPLE_32(N) static inline __m256i N##_32x8_ref (__m256i x) { return avx2_wrap_ufunc_32(x, N##_32); }
#define DEF_SIMPLE_64(N) static inline __m256i N##_64x4_ref (__m256i x) { return avx2_wrap_ufunc_64(x, N##_64); }

// all are present (8,16,32,64) and the names match up
#define DEF_SIMPLE_USET(N)                                                                \
static inline __m256i N##_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x, N##_32); }  \
static inline __m256i N##_16x16_ref(__m256i x) { return avx2_wrap_ufunc_16(x, N##_32); }  \
static inline __m256i N##_32x8_ref (__m256i x) { return avx2_wrap_ufunc_32(x, N##_32); }  \
static inline __m256i N##_64x4_ref (__m256i x) { return avx2_wrap_ufunc_64(x, N##_64); }

static inline uint32_t negate_32(uint32_t x)   { return (uint32_t)(-(int32_t)x); }
static inline uint64_t negate_64(uint64_t x)   { return (uint64_t)(-(int64_t)x); }
static inline uint32_t inc_32(uint32_t x)      { return x+1; }
static inline uint64_t inc_64(uint64_t x)      { return x+1; }
static inline uint32_t dec_32(uint32_t x)      { return x-1; }
static inline uint64_t dec_64(uint64_t x)      { return x-1; }
static inline uint32_t shr1_32(uint32_t x)     { return x>>1; }

DEF_SIMPLE_USET(bit_parity)
DEF_SIMPLE_USET(negate)
DEF_SIMPLE_USET(inc)
DEF_SIMPLE_USET(dec)
DEF_SIMPLE_USET(bit_lowest_clear)
DEF_SIMPLE_USET(bit_lowest_set)
DEF_SIMPLE_USET(bit_lowest_changed)

//DEF_SIMPLE_8(bit_lowest_changed) 
//DEF_SIMPLE_16(bit_lowest_changed)
//DEF_SIMPLE_32(bit_lowest_changed)
//DEF_SIMPLE_64(bit_lowest_changed)
     

// the "reference" version are a super janky hacko fest
static inline uint64_t pop_64_up(uint64_t x) { return (uint64_t)pop_64(x); }
static inline uint64_t ctz_64_up(uint64_t x) { return (uint64_t)ctz_64(x); }
static inline uint64_t clz_64_up(uint64_t x) { return (uint64_t)clz_64(x); }

static inline __m256i shr_1_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x,shr1_32); }

static inline __m256i umin_8x32_k (__m256i x, uint8_t  k) { return _mm256_min_epu8 (x, _mm256_set1_epi8 ((int8_t )k)); }
static inline __m256i umin_16x16_k(__m256i x, uint16_t k) { return _mm256_min_epu16(x, _mm256_set1_epi16((int16_t)k)); }
static inline __m256i umin_32x8_k (__m256i x, uint32_t k) { return _mm256_min_epu32(x, _mm256_set1_epi32((int32_t)k)); }

static inline __m256i usub_8x32_k (__m256i x, uint8_t  k) { return _mm256_sub_epi8 (x, _mm256_set1_epi8 ((int8_t )k)); }
static inline __m256i usub_16x16_k(__m256i x, uint16_t k) { return _mm256_sub_epi16(x, _mm256_set1_epi16((int16_t)k)); }
static inline __m256i usub_32x8_k (__m256i x, uint32_t k) { return _mm256_sub_epi32(x, _mm256_set1_epi32((int32_t)k)); }

static inline __m256i pop_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x,pop_32); }
static inline __m256i pop_16x16_ref(__m256i x) { return avx2_wrap_ufunc_16(x,pop_32); }
static inline __m256i pop_32x8_ref (__m256i x) { return avx2_wrap_ufunc_32(x,pop_32); }
static inline __m256i pop_64x4_ref (__m256i x) { return avx2_wrap_ufunc_64(x,pop_64_up); }

static inline uint32_t bit_reverse_8_ (uint32_t x) { return (bit_reverse_32(x) >> 24); }
static inline uint32_t bit_reverse_16_(uint32_t x) { return (bit_reverse_32(x) >> 16); }
static inline __m256i  bit_reverse_8x32_ref (__m256i x) { return avx2_wrap_ufunc_8 (x, bit_reverse_8_); } 
static inline __m256i  bit_reverse_16x16_ref(__m256i x) { return avx2_wrap_ufunc_16(x, bit_reverse_16_); }
static inline __m256i  bit_reverse_32x8_ref (__m256i x) { return avx2_wrap_ufunc_32(x, bit_reverse_32);  }
static inline __m256i  bit_reverse_64x4_ref (__m256i x) { return avx2_wrap_ufunc_64(x, bit_reverse_64);  }

static inline __m256i ctz_8x32_ref (__m256i x) { return umin_8x32_k (avx2_wrap_ufunc_8 (x,ctz_32),  8); }
static inline __m256i ctz_16x16_ref(__m256i x) { return umin_16x16_k(avx2_wrap_ufunc_16(x,ctz_32), 16); }
static inline __m256i ctz_32x8_ref (__m256i x) { return umin_32x8_k (avx2_wrap_ufunc_32(x,ctz_32), 32); }

static inline __m256i clz_8x32_ref (__m256i x) { return usub_8x32_k (avx2_wrap_ufunc_8 (x,clz_32), 32-8); }
static inline __m256i clz_16x16_ref(__m256i x) { return usub_16x16_k(avx2_wrap_ufunc_16(x,clz_32), 16); }
static inline __m256i clz_32x8_ref (__m256i x) { return              avx2_wrap_ufunc_32(x,clz_32)     ; }

static inline __m256i broadcast_lo_8x32_ref(__m256i x)
{
  wrap_reg_8(a,x); for_range(i,1,32) {a[i]=a[0];} back_2_reg(x,a); return x;
}

static inline __m256i broadcast_lo_16x16_ref(__m256i x)
{
  wrap_reg_16(a,x); for_range(i,1,16) {a[i]=a[0];} back_2_reg(x,a); return x;
}

static inline __m256i broadcast_lo_32x8_ref(__m256i x)
{
  wrap_reg_32(a,x); for_range(i,1,8) {a[i]=a[0];} back_2_reg(x,a); return x;
}

static inline __m256i broadcast_lo_64x4_ref(__m256i x)
{
  wrap_reg_64(a,x); for_range(i,1,4) {a[i]=a[0];} back_2_reg(x,a); return x;
}



__m256i avx2_random(prng_t* prng) { __m256i x; uint64_t a[4]; for_range(i,0, 4) {a[i]=prng_u64(prng);} back_2_reg(x,a); return x;}


bool avx2_equal(__m256i a, __m256i b) { return ((uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(a,b))) == 0xFFFFFFFF; }



// as-in the width/number of the operations being performed (for error dumps)
typedef enum {
  avx2_8x32,
  avx2_16x16,
  avx2_32x8,
  avx2_64x4,
  avx2_128x2,
} avx2_lanes_t;

typedef struct { uint64_t d[4]; } avx2_test_vector_t;

typedef struct {
  __m256i (*f0)(__m256i);
  __m256i (*f1)(__m256i);
  char*   name;
  avx2_lanes_t lanes;
} avx2_ufunc_compare_t;

// meh..quick hacks
#define XSTR(X) STR(X)
#define STR(X)  #X
#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))


static inline u256_t byte_zip_128x2_ref(u256_t x)
{
  u256_t m = _mm256_setr_epi8(0x00,0x08,0x01,0x09,0x02,0x0a,0x03,0x0b,0x04,0x0c,0x05,0x0d,0x06,0x0e,0x07,0x0f,
			      0x00,0x08,0x01,0x09,0x02,0x0a,0x03,0x0b,0x04,0x0c,0x05,0x0d,0x06,0x0e,0x07,0x0f);
  
  return _mm256_shuffle_epi8(x,m);
}

static inline u256_t byte_unzip_128x2_ref(u256_t x)
{
  u256_t m = _mm256_setr_epi8(0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x01,0x03,0x05,0x07,0x09,0x0b,0x0d,0x0f,
			      0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x01,0x03,0x05,0x07,0x09,0x0b,0x0d,0x0f);
  
  return byte_shuffle_128x2(x,m);
}


// macro to lower chances of copypasta mistakes
#define DEF_FUNCS(X,L) {.f0=X##_##L,.f1=X##_##L##_ref,.name=XSTR(X##_##L),.lanes=avx2_##L}

#define DEF_FUNCS_4(X) DEF_FUNCS(X,8x32), DEF_FUNCS(X,16x16), DEF_FUNCS(X,32x8), DEF_FUNCS(X,64x4)

avx2_ufunc_compare_t avx2_ufuncs[] =
{
  DEF_FUNCS(byte_zip,128x2),
  DEF_FUNCS(byte_unzip,128x2),
  DEF_FUNCS(shr_1,8x32),

  DEF_FUNCS_4(negate),
  DEF_FUNCS_4(dec),
  DEF_FUNCS_4(inc),
  DEF_FUNCS_4(pop),
  DEF_FUNCS_4(bit_parity),
  DEF_FUNCS_4(bit_reverse),
  DEF_FUNCS_4(broadcast_lo),
  DEF_FUNCS_4(bit_lowest_clear),
  DEF_FUNCS_4(bit_lowest_set),
  DEF_FUNCS_4(bit_lowest_changed),

  DEF_FUNCS(ctz,8x32),
  DEF_FUNCS(ctz,16x16),
  
  DEF_FUNCS(clz,8x32),
  DEF_FUNCS(clz,16x16),
};

#undef DEF_FUNCS

//----------------------------------------------------------------------

static char* print_vsep = "│";

void dump_hex_8     (uint8_t  v)  { printf("%02x",v); }
void dump_hex_sep_8 (uint8_t  v)  { printf("%s%02x",print_vsep,v); }
void dump_hex_16    (uint16_t v)  { printf("%04x",v); }
void dump_hex_sep_16(uint16_t v)  { printf("%s%04x",print_vsep,v); }
void dump_hex_32    (uint32_t v)  { printf("%08x",v); }
void dump_hex_sep_32(uint32_t v)  { printf("%s%08x",print_vsep,v); }
void dump_hex_64    (uint64_t v)  { printf("%016lx",v); }
void dump_hex_sep_64(uint64_t v)  { printf("%s%016lx",print_vsep,v); }

// array order: low element first (left-to-right)
void avx2_print_8x32 (__m256i x) { wrap_reg_8 (s,x); dump_hex_8 (s[0]); for_range(i,1,32) dump_hex_sep_8 (s[i]); }
void avx2_print_16x16(__m256i x) { wrap_reg_16(s,x); dump_hex_16(s[0]); for_range(i,1,16) dump_hex_sep_16(s[i]); }
void avx2_print_32x8 (__m256i x) { wrap_reg_32(s,x); dump_hex_32(s[0]); for_range(i,1, 8) dump_hex_sep_32(s[i]); }
void avx2_print_64x4 (__m256i x) { wrap_reg_64(s,x); dump_hex_64(s[0]); for_range(i,1, 4) dump_hex_sep_64(s[i]); }

// register order: low element last
// change tick marks except at 128 bit lane boundary
void avx2_reg_print_8x32 (__m256i x) { wrap_reg_8 (s,x); dump_hex_8 (s[31]); for_range(i,1,32) dump_hex_sep_8 (s[31-i]); }
void avx2_reg_print_16x16(__m256i x) { wrap_reg_16(s,x); dump_hex_16(s[15]); for_range(i,1,16) dump_hex_sep_16(s[15-i]); }
void avx2_reg_print_32x8 (__m256i x) { wrap_reg_32(s,x); dump_hex_32(s[ 7]); for_range(i,1, 8) dump_hex_sep_32(s[ 7-i]); }
void avx2_reg_print_64x4 (__m256i x) { wrap_reg_64(s,x); dump_hex_64(s[ 3]); for_range(i,1, 4) dump_hex_sep_64(s[ 3-i]); }


void avx2_print_diff_8x32 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_8(s,x);
  if (s[0]) dump_hex_8(s[0]); else printf("  ");
  for_range(i,1,32) { if(s[i]) dump_hex_sep_8(s[i]); else printf("%s  ", print_vsep); }
}

void avx2_print_diff_16x16 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_16(s,x);
  if (s[0]) dump_hex_16(s[0]); else printf("    ");
  for_range(i,1,16) { if(s[i]) dump_hex_sep_16(s[i]); else printf("%s    ", print_vsep); }
}

void avx2_print_diff_32x8 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_32(s,x);
  if (s[0]) dump_hex_32(s[0]); else printf("        ");
  for_range(i,1,8) { if(s[i]) dump_hex_sep_32(s[i]); else printf("%s        ", print_vsep); }
}

void avx2_print_diff_64x4 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_64(s,x);
  if (s[0]) dump_hex_64(s[0]); else printf("                ");
  for_range(i,1,4) { if(s[i]) dump_hex_sep_64(s[i]); else printf("%s                ", print_vsep); }
}


void avx2_dump_ufuncs_8x32_lb(void)
{
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──\n");
  printf("\n");
}

void avx2_dump_ufuncs_16x16_lb(void)
{
  printf("────┴────┴────┴────┴────┴────┴────┴────┴");
  printf("────┴────┴────┴────┴────┴────┴────┴────");
  printf("\n");
}

void avx2_dump_ufuncs_32x8_lb(void)
{
  printf("────────┴────────┴────────┴────────┴");
  printf("────────┴────────┴────────┴────────\n");
}

void avx2_dump_ufuncs_64x4_lb(void)
{
  printf("────────────────┴────────────────┴────────────────┴────────────────\n");
}


void avx2_dump_ufuncs_8x32(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_8x32(  x); printf("\n");
  printf("  ref:"); avx2_print_8x32(ref); printf("\n");
  printf("  out:"); avx2_print_8x32(out); printf("\n");
  printf("      "); avx2_print_diff_8x32(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_8x32_lb();
}


void avx2_dump_ufuncs_16x16(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_16x16(  x); printf("\n");
  printf("  ref:"); avx2_print_16x16(ref); printf("\n");
  printf("  out:"); avx2_print_16x16(out); printf("\n");
  printf("      "); avx2_print_diff_16x16(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_16x16_lb();

}

void avx2_dump_ufuncs_32x8(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_32x8(  x); printf("\n");
  printf("  ref:"); avx2_print_32x8(ref); printf("\n");
  printf("  out:"); avx2_print_32x8(out); printf("\n");
//printf("      "); avx2_print_diff_32x8(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_32x8_lb();
}

void avx2_dump_ufuncs_64x4(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_64x4(  x); printf("\n");
  printf("  ref:"); avx2_print_64x4(ref); printf("\n");
  printf("  out:"); avx2_print_64x4(out); printf("\n");
//printf("      "); avx2_print_diff_64x4 (ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_64x4_lb();
}

#define VEC_REP4(X) X,X,X,X

static const avx2_test_vector_t avx2_test_vectors[] =
{
  { VEC_REP4(UINT64_C(0)) },
  { VEC_REP4(UINT64_C(0xffffffffffffffff)) },
  { VEC_REP4(UINT64_C(0x00ff00ff00ff00ff)) },
  { VEC_REP4(UINT64_C(0xff00ff00ff00ff00)) },
  { VEC_REP4(UINT64_C(0xff000000ff000000)) },
  { VEC_REP4(UINT64_C(0x8080808080808080)) },
  { VEC_REP4(UINT64_C(0x0808080808080808)) },
  { VEC_REP4(UINT64_C(0x1111111111111111)) },
};


uint32_t avx2_compare_ufuncs(prng_t* prng, avx2_ufunc_compare_t* set, uint32_t trials)
{
  // maximum number of broken evals to dump before bailing
  static const uint32_t max_errors = 10;

  uint32_t errors = 0;

  printf("%s\n", set->name);
  
  for(uint32_t i=0; i<LENGTHOF(avx2_test_vectors); i++) {
    __m256i x   = loadu_256((void*)(avx2_test_vectors+i));
    __m256i out = set->f0(x);
    __m256i ref = set->f1(x);

    //printf("  test:  "); avx2_print_8x32(  x); printf("\n");    
    //printf("  test:  "); avx2_dump_ufuncs_8x32_lb();
    
    if (!avx2_equal(out,ref)) {
      switch(set->lanes) {
	case avx2_64x4:  avx2_dump_ufuncs_64x4 (x,ref,out); break;
	case avx2_32x8:  avx2_dump_ufuncs_32x8 (x,ref,out); break;
	case avx2_16x16: avx2_dump_ufuncs_16x16(x,ref,out); break;
        default:         avx2_dump_ufuncs_8x32 (x,ref,out); break;
      }

      if (++errors > max_errors) {
	printf("bailing\n");
	return 1;
      }
    }
  }

  for(uint32_t i=0; i<trials; i++) {
    __m256i x   = avx2_random(prng);
    __m256i out = set->f0(x);
    __m256i ref = set->f1(x);

    if (!avx2_equal(out,ref)) {
      switch(set->lanes) {
	case avx2_64x4:  avx2_dump_ufuncs_64x4 (x,ref,out); break;
	case avx2_32x8:  avx2_dump_ufuncs_32x8 (x,ref,out); break;
	case avx2_16x16: avx2_dump_ufuncs_16x16(x,ref,out); break;
        default:         avx2_dump_ufuncs_8x32 (x,ref,out); break;
      }

      if (++errors > max_errors) {
	printf("bailing\n");
	return 1;
      }
    }
  }


  // want to sum up the number of functions that fail
  return errors ? 1 : 0;
}


uint32_t avx2_spot(prng_t* prng)
{
  uint32_t errors = 0;

  for(uint32_t i=0; i<LENGTHOF(avx2_ufuncs); i++) {
    uint32_t r = avx2_compare_ufuncs(prng, avx2_ufuncs+i, 0xfffff);

    errors += r;
  }

  return errors;
}


int main(void)
{
  prng_t prng;

  prng.state[0] = 0x1234567;
  prng.state[1] = 0x89abcd1;
  prng.state[2] = __rdtsc();
  prng_u64(&prng);

  avx2_spot(&prng);

  return 0;
}
