// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

const uint64_t bmat_main_diagonal_mask_8  = UINT64_C(0x8040201008040201);
const uint64_t bmat_main_diagonal_mask_16 = UINT64_C(0x0008000400020001);
const uint64_t bmat_main_diagonal_mask_32 = UINT64_C(0x0000000200000001);

#if defined(SWAR_AVX2_H)
#define REP_SHIFT(X,S) {X,X<<S,X<<(2*S),X<<(3*S)}

// main diagonal (mask/markers) in 256 bit
const u256_data_t bmat_md_256_16 = {.u64 = REP_SHIFT(bmat_main_diagonal_mask_16, 4) };
const u256_data_t bmat_md_256_32 = {.u64 = REP_SHIFT(bmat_main_diagonal_mask_32, 2) };
const u256_data_t bmat_md_256_64 = {.u64 = REP_SHIFT(UINT64_C(1), 1) };

#undef REP_SHIFT
#endif

// equal comparison workers: written to assume true so always visits all
static inline uint64_t bmat_equal_n(uint64_t* a, uint64_t* b, uint32_t n)
{
  uint64_t t=a[0]^b[0]; for(uint32_t i=1; i<n; i++) { t |= a[i]^b[i]; } return (t == 0);
}

static inline uint64_t bmat_is_zero_n(uint64_t* m, uint32_t n)
{
  uint64_t t=m[0]; for(uint32_t i=1; i<n; i++) { t |= m[i]; } return (t == 0);
}

// D = A*B (where * is Hadamard product)
void bmat_and_n(bmat_rparam_n(d), uint64_t * a, uint64_t * b, uint32_t n)
{
  for(uint32_t i=0; i<n; i++) d[i] = a[i]&b[i];
}

void bmat_sum_n(bmat_rparam_n(d), uint64_t* a, uint32_t n)
{
  for(uint32_t i=0; i<n; i++) d[i] ^= a[i];
}

void bmat_add_n(bmat_rparam_n(d), uint64_t* a, uint64_t* b, uint32_t n)
{
  for(uint32_t i=0; i<n; i++) d[i] = a[i]^b[i];
}

// sum (k*a[i])
uint64_t bmat_kmul_sum(uint64_t* a, uint64_t k, uint32_t n)
{
  uint64_t r = 0;
  
  for(uint32_t i=0; i<n; i++) r ^= k & a[i];

  return r;
}

// change to swap2
void bmat_row_swap_8_i (bmat_param_8 (m), uint32_t r0, uint32_t r1) { bmat_adup_8 (a,m); uint8_t  t=a[r0]; a[r0]=a[r1]; a[r1]=t; array_to_bmat_8 (m,a); }
void bmat_row_swap_16_i(bmat_param_16(m), uint32_t r0, uint32_t r1) { bmat_adup_16(a,m); uint16_t t=a[r0]; a[r0]=a[r1]; a[r1]=t; array_to_bmat_16(m,a); }
void bmat_row_swap_32_i(bmat_param_32(m), uint32_t r0, uint32_t r1) { bmat_adup_32(a,m); uint32_t t=a[r0]; a[r0]=a[r1]; a[r1]=t; array_to_bmat_32(m,a); }
void bmat_row_swap_64_i(bmat_param_64(m), uint32_t r0, uint32_t r1) {                    uint64_t t=m[r0]; m[r0]=m[r1]; m[r1]=t; }

void bmat_row_swap_8 (bmat_param_8 (m), uint32_t r0, uint32_t r1) { bmat_row_swap_8_i (m, r0&0x07, r1&0x07); }
void bmat_row_swap_16(bmat_param_16(m), uint32_t r0, uint32_t r1) { bmat_row_swap_16_i(m, r0&0x0f, r1&0x0f); }
void bmat_row_swap_32(bmat_param_32(m), uint32_t r0, uint32_t r1) { bmat_row_swap_32_i(m, r0&0x1f, r1&0x1f); }
void bmat_row_swap_64(bmat_param_64(m), uint32_t r0, uint32_t r1) { bmat_row_swap_64_i(m, r0&0x3f, r1&0x3f); }

uint64_t bmat_get_row_8 (bmat_param_8 (m), uint32_t n) { return (m[0]             >> ( 8*(n&7))) & 0xff; }
uint64_t bmat_get_row_16(bmat_param_16(m), uint32_t n) { return (m[(n & 0x0f)>>2] >> (16*(n&3))) & 0xffff; }
uint64_t bmat_get_row_32(bmat_param_32(m), uint32_t n) { return (m[(n & 0x1f)>>1] >> (32*(n&1))) & 0xffffffff; }
uint64_t bmat_get_row_64(bmat_param_64(m), uint32_t n) { return  m[(n & 0x3f)   ]; }


void bmat_set_row_8(bmat_param_8(m), uint32_t r, uint64_t v)
{
  uint64_t mask = UINT64_C(0xff);

  v      = (v & mask) << (r*8);
  mask <<= (r*8);
  
  m[0] = (m[0] & ~mask) ^ v;
}

void bmat_set_row_16(bmat_param_16(m), uint32_t r, uint64_t v)
{
  uint64_t mask = UINT64_C(0xffff);

  v      = (v & mask) << (16*(r&3));
  mask <<= (16*(r&3));

  m[r>>2] = (m[r>>2] & ~mask) ^ v;
}

void bmat_set_row_32(bmat_param_32(m), uint32_t r, uint64_t v)
{
  uint64_t mask = UINT64_C(0xffffffff);

  v      = (v & mask) << (32*(r&1));
  mask <<= (32*(r&1));
  
  m[r>>1] = (m[r>>1] & ~mask) ^ v;
}

void bmat_set_row_64(bmat_param_64(m), uint32_t r, uint64_t v)
{
  r &= 0x3f;
  m[r] = v;
}

uint64_t bmat_get_8 (bmat_param_8 (m), uint32_t r, uint32_t c) { return (bmat_get_row_8 (m,r) >> (c & 0x07)) & 1; }
uint64_t bmat_get_16(bmat_param_16(m), uint32_t r, uint32_t c) { return (bmat_get_row_16(m,r) >> (c & 0x0f)) & 1; }
uint64_t bmat_get_32(bmat_param_32(m), uint32_t r, uint32_t c) { return (bmat_get_row_32(m,r) >> (c & 0x1f)) & 1; }
uint64_t bmat_get_64(bmat_param_64(m), uint32_t r, uint32_t c) { return (bmat_get_row_64(m,r) >> (c & 0x3f)) & 1; }

bool bmat_equal_8 (bmat_rparam_8 (a), bmat_param_8 (b)) { return a[0] == b[0];         }
bool bmat_equal_16(bmat_rparam_16(a), bmat_param_16(b)) { return bmat_equal_n(a,b, 4); }
bool bmat_equal_32(bmat_rparam_32(a), bmat_param_32(b)) { return bmat_equal_n(a,b,16); }
bool bmat_equal_64(bmat_rparam_64(a), bmat_param_64(b)) { return bmat_equal_n(a,b,64); }

bool bmat_is_zero_8 (bmat_param_8(m))  { return m[0] == 0; }
bool bmat_is_zero_16(bmat_param_16(m)) { return bmat_is_zero_n(m, 4); }
bool bmat_is_zero_32(bmat_param_32(m)) { return bmat_is_zero_n(m,16); }
bool bmat_is_zero_64(bmat_param_64(m)) { return bmat_is_zero_n(m,64); }

// D = A+B  (restrict on impl & not in forward decl on purpose)
void bmat_add_8 (bmat_rparam_8 (d), bmat_param_8 (a), bmat_param_8 (b)) { bmat_add_n(d,a,b, 1); }
void bmat_add_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b)) { bmat_add_n(d,a,b, 4); }
void bmat_add_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b)) { bmat_add_n(d,a,b,16); }
void bmat_add_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b)) { bmat_add_n(d,a,b,64); }

// D += A 
void bmat_sum_8 (bmat_rparam_8 (d), bmat_param_8 (a)) { bmat_sum_n(d,a, 1); }
void bmat_sum_16(bmat_rparam_16(d), bmat_param_16(a)) { bmat_sum_n(d,a, 4); }
void bmat_sum_32(bmat_rparam_32(d), bmat_param_32(a)) { bmat_sum_n(d,a,16); }
void bmat_sum_64(bmat_rparam_64(d), bmat_param_64(a)) { bmat_sum_n(d,a,64); }


// D = M+I  (restrict on impl & not in forward decl on purpose)
void bmat_add_unit_8(bmat_rparam_8(d), bmat_param_8(m))
{
  d[0] = m[0] ^ bmat_main_diagonal_mask_8;
}

void bmat_add_unit_16(bmat_rparam_16(d), bmat_param_16(m))
{
  uint64_t u = bmat_main_diagonal_mask_16;
  
  d[0] = m[0] ^ u; u <<= 4;
  d[1] = m[1] ^ u; u <<= 4;
  d[2] = m[2] ^ u; u <<= 4;
  d[3] = m[3] ^ u;
}

void bmat_add_unit_32(bmat_rparam_32(d), bmat_param_32(m))
{
  uint64_t u = bmat_main_diagonal_mask_32;

  for(uint32_t i=0; i<16; i++) {
    d[i] = m[i] ^ u; u <<= 2;
  }
}

void bmat_add_unit_64(bmat_rparam_64(d), bmat_param_64(m))
{
  uint64_t u = 1;

  for(uint32_t i=0; i<64; i++) {
    d[i] = m[i] ^ u; u <<= 1;
  }
}


// D = MJ
void bmat_flip_h_8(bmat_param_8 (d), bmat_param_8(m)) { d[0] = bit_reverse_8x8 (m[0]); }

// D = JM
void bmat_flip_v_8(bmat_param_8(d),  bmat_param_8(m)) { d[0] = byteswap_64(m[0]); }

#if defined(SWAR_AVX2_H)
void bmat_flip_h_16(bmat_param_16(d), bmat_param_16(m))
{
  u256_t r[1]; bmat_load_256xn(r,m,1); r[0] = bit_reverse_16x16(r[0]);  bmat_store_256xn(d,r,1);
}

void bmat_flip_h_32(bmat_param_32(d), bmat_param_32(m))
{
  u256_t r[4]; bmat_load_256xn(r,m,4); for_range(i,0,4) r[i] = bit_reverse_32x8(r[i]);  bmat_store_256xn(d,r,4);
}

void bmat_flip_h_64(bmat_param_64(d), bmat_param_64(m))
{
  u256_t r[16]; bmat_load_256xn(r,m,16); for_range(i,0,16) r[i] = bit_reverse_64x4(r[i]);  bmat_store_256xn(d,r,16);
}

void bmat_flip_v_16(bmat_param_16(d), bmat_param_16(m))
{
  u256_t r[1];

  bmat_load_256xn(r,m,1);
  r[0] = u16_swap_256(r[0]);
  bmat_store_256xn(d,r,1);
}

void bmat_flip_v_32(bmat_param_32(d), bmat_param_32(m))
{
  u256_t r[4];
  bmat_load_256xn(r,m,4);

  u256_t t0,t1;
  
  t0 = r[0]; t1 = r[3]; r[0] = u32_swap_256(t1); r[3] = u32_swap_256(t0);
  t0 = r[1]; t1 = r[2]; r[1] = u32_swap_256(t1); r[2] = u32_swap_256(t0);

  bmat_store_256xn(d,r,4);
}

void bmat_flip_v_64(bmat_param_64(d), bmat_param_64(m))
{
  u256_t r[16];
  bmat_load_256xn(r,m,16);

  u256_t t0,t1;
  
  for_range(i,0,8) {
    t0      = r[   i];
    t1      = r[15-i];
    r[   i] = u64_swap_256(t1);
    r[15-i] = u64_swap_256(t0);
  }

  bmat_store_256xn(d,r,16);
}

#else

// generic 64-bit hardware versions
void bmat_flip_h_16(bmat_param_16(d), bmat_param_16(m)) { for_range(i,0, 4) d[i] = bit_reverse_16x4(m[i]); }
void bmat_flip_h_32(bmat_param_32(d), bmat_param_32(m)) { for_range(i,0,16) d[i] = bit_reverse_32x2(m[i]); }
void bmat_flip_h_64(bmat_param_64(d), bmat_param_64(m)) { for_range(i,0,64) d[i] = bit_reverse_64  (m[i]); }

void bmat_flip_v_16(bmat_param_16(d), bmat_param_16(s))
{
  uint64_t t0,t1;
  t0 = s[0]; t1=s[3]; d[0] = u16_swap_64(t1); d[3] = u16_swap_64(t0);
  t0 = s[1]; t1=s[2]; d[1] = u16_swap_64(t1); d[2] = u16_swap_64(t0);
}

void bmat_flip_v_32(bmat_param_32(d), bmat_param_32(m))
{
  uint64_t t0,t1;
  for(uint32_t i=0; i<8; i++) {
    t0 = m[i]; t1 = m[15-i]; d[i] = rot_64(t1,32); d[15-i] = rot_64(t0,32);
  }
}

void bmat_flip_v_64_ref(bmat_param_64(d), bmat_param_64(m));
void bmat_flip_v_64(bmat_param_64(d), bmat_param_64(m)) { bmat_flip_v_64_ref(d,m); }

#endif


// VALIDATE AND CHANGE TO THESE
static inline uint32_t bmat_md_sum_i(uint64_t* m, uint64_t k, uint32_t s, uint32_t n)
{
  uint64_t t = k & m[0];

  for(uint32_t i=1; i<n; i++) { k <<= s; t ^= (m[i] & k); }

  return pop_64(t);
}

uint32_t bmat_md_sum_16_(bmat_param_16(m)) { return pop_64(bmat_md_sum_i(m,bmat_main_diagonal_mask_16,4, 4)); }
uint32_t bmat_md_sum_32_(bmat_param_32(m)) { return pop_64(bmat_md_sum_i(m,bmat_main_diagonal_mask_32,2,16)); }
uint32_t bmat_md_sum_64_(bmat_param_64(m)) { return pop_64(bmat_md_sum_i(m,                         1,1,64)); }


// main diagonal (integer) sum (KILL THESE)
uint32_t bmat_md_sum_8 (bmat_param_8 (m)) { return pop_64(m[0] & bmat_main_diagonal_mask_8); }

uint32_t bmat_md_sum_16(bmat_param_16(m))
{
  uint64_t a = bmat_main_diagonal_mask_16;
  uint64_t t = 0;
  
  t  = m[0] & a; a <<= 4;
  t ^= m[1] & a; a <<= 4;
  t ^= m[2] & a; a <<= 4;
  t ^= m[3] & a; 
  
  return pop_64(t);
}

uint32_t bmat_md_sum_32(bmat_param_32(m))
{
  uint64_t a = bmat_main_diagonal_mask_32;
  uint64_t t = 0;

  for(uint32_t i=0; i<16; i++) { t ^= m[i] & a; a <<= 2; }
  
  return pop_64(t);
}

uint32_t bmat_md_sum_64(bmat_param_64(m))
{
  uint64_t t = 0;
  uint64_t a = 1;

  for(uint32_t i=0; i<64; i++) { t ^= m[i] & a; a <<= 1; }
  
  return pop_64(t);
}


uint32_t bmat_trace_8 (bmat_param_8 (m)) { return bmat_md_sum_8 (m) & 1; }
uint32_t bmat_trace_16(bmat_param_16(m)) { return bmat_md_sum_16(m) & 1; }
uint32_t bmat_trace_32(bmat_param_32(m)) { return bmat_md_sum_32(m) & 1; }

// working out better than sum diagonal and popcount
uint32_t bmat_trace_64(bmat_param_64(m))
{
  uint64_t t = 0;
  for (uint64_t i=0; i<64; i++)  t ^= (m[i] >> i);
  return  t & 1;
}

static inline uint64_t bmat_vmul_n(uint64_t* m, uint64_t v, uint32_t n)
{
  uint64_t r = m[0] & v;
  
  for(uint32_t i=1; i<n; i++) { r ^= m[i] & v; }
  
  return r;
}

// sum up packed b-bit values
uint8_t  bvec_hsum_8x8 (uint64_t x) { x ^= (x>>32); x ^= (x>>16); x ^= (x>>8); return (uint8_t) x; }
uint16_t bvec_hsum_16x4(uint64_t x) { x ^= (x>>32); x ^= (x>>16);              return (uint16_t)x; }
uint32_t bvec_hsum_32x2(uint64_t x) { x ^= (x>>32);                            return (uint32_t)x; }



