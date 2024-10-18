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

/// ## bmat_equal_*n*(a,b)
///
/// Returns `true` if the inputs are the same matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_equal_8 (bmat_param_8 (a), bmat_param_8 (b))
/// void bmat_equal_16(bmat_param_16(a), bmat_param_16(b))
/// void bmat_equal_32(bmat_param_32(a), bmat_param_32(b))
/// void bmat_equal_64(bmat_param_64(a), bmat_param_64(b))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

bool bmat_equal_8 (bmat_rparam_8 (a), bmat_param_8 (b)) { return a[0] == b[0];         }
bool bmat_equal_16(bmat_rparam_16(a), bmat_param_16(b)) { return bmat_equal_n(a,b, 4); }
bool bmat_equal_32(bmat_rparam_32(a), bmat_param_32(b)) { return bmat_equal_n(a,b,16); }
bool bmat_equal_64(bmat_rparam_64(a), bmat_param_64(b)) { return bmat_equal_n(a,b,64); }


/// ## bmat_is_zero_*n*(m)
///
/// Returns `true` if the input the zero matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_is_zero_8 (bmat_param_8 (m))
/// void bmat_is_zero_16(bmat_param_16(m))
/// void bmat_is_zero_32(bmat_param_32(m))
/// void bmat_is_zero_64(bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

bool bmat_is_zero_8 (bmat_param_8(m))  { return m[0] == 0; }
bool bmat_is_zero_16(bmat_param_16(m)) { return bmat_is_zero_n(m, 4); }
bool bmat_is_zero_32(bmat_param_32(m)) { return bmat_is_zero_n(m,16); }
bool bmat_is_zero_64(bmat_param_64(m)) { return bmat_is_zero_n(m,64); }

/// ## bmat_add_*n*(d,a,b)
///
/// $$ D = A+B $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_add_8 (bmat_param_8 (d), bmat_param_8 (a), bmat_param_8 (b))
/// void bmat_add_16(bmat_param_16(d), bmat_param_16(a), bmat_param_16(b))
/// void bmat_add_32(bmat_param_32(d), bmat_param_32(a), bmat_param_32(b))
/// void bmat_add_64(bmat_param_64(d), bmat_param_64(a), bmat_param_64(b))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// D = A+B  (restrict on impl & not in forward decl on purpose)
void bmat_add_8 (bmat_rparam_8 (d), bmat_param_8 (a), bmat_param_8 (b)) { bmat_add_n(d,a,b, 1); }
void bmat_add_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b)) { bmat_add_n(d,a,b, 4); }
void bmat_add_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b)) { bmat_add_n(d,a,b,16); }
void bmat_add_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b)) { bmat_add_n(d,a,b,64); }


/// ## bmat_sum_*n*(m,n)
///
/// $$ D = D+A $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_sum_8 (bmat_param_8 (d), bmat_param_8 (a))
/// void bmat_sum_16(bmat_param_16(d), bmat_param_16(a))
/// void bmat_sum_32(bmat_param_32(d), bmat_param_32(a))
/// void bmat_sum_64(bmat_param_64(d), bmat_param_64(a))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// D += A 
void bmat_sum_8 (bmat_rparam_8 (d), bmat_param_8 (a)) { bmat_sum_n(d,a, 1); }
void bmat_sum_16(bmat_rparam_16(d), bmat_param_16(a)) { bmat_sum_n(d,a, 4); }
void bmat_sum_32(bmat_rparam_32(d), bmat_param_32(a)) { bmat_sum_n(d,a,16); }
void bmat_sum_64(bmat_rparam_64(d), bmat_param_64(a)) { bmat_sum_n(d,a,64); }


/// ## bmat_add_unit_*n*(m,n)
///
/// $$ D = M+I $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_add_unit_8 (bmat_param_8 (d), bmat_param_8 (m))
/// void bmat_add_uint_16(bmat_param_16(d), bmat_param_16(m))
/// void bmat_add_unit_32(bmat_param_32(d), bmat_param_32(m))
/// void bmat_add_unit_64(bmat_param_64(d), bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// NOTE: forward decl should NOT have restrict (see impl notes)

void bmat_add_unit_8(bmat_rparam_8(d), bmat_param_8(m))
{
  d[0] = m[0] ^ BMAT_MAIN_DIAGONAL_MASK_8;
}

void bmat_add_unit_16(bmat_rparam_16(d), bmat_param_16(m))
{
  uint64_t u = BMAT_MAIN_DIAGONAL_MASK_16;
  
  d[0] = m[0] ^ u; u <<= 4;
  d[1] = m[1] ^ u; u <<= 4;
  d[2] = m[2] ^ u; u <<= 4;
  d[3] = m[3] ^ u;
}

void bmat_add_unit_32(bmat_rparam_32(d), bmat_param_32(m))
{
  uint64_t u = BMAT_MAIN_DIAGONAL_MASK_16;

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

/// ## bmat_row_lshift_*n*(m,n)
///
/// In-place left shifts each row by `n`:
///
/// $$ M' = ML^n
///
/// where $L$ is the left shift matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_row_lshift_8 (bmat_param_8 (m),n)
/// void bmat_row_lshift_16(bmat_param_16(m),n)
/// void bmat_row_lshift_32(bmat_param_32(m),n)
/// void bmat_row_lshift_64(bmat_param_64(m),n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

/// ## bmat_row_lshift_*n*(d,m,s)
///
/// Left shifts each row by `s`:
///
/// $$ D = ML^s $$
///
/// where $L$ is the left shift matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_row_lshift_8 (bmat_param_8 (m), bmat_param_8 (m), s)
/// void bmat_row_lshift_16(bmat_param_16(m), bmat_param_16(m), s)
/// void bmat_row_lshift_32(bmat_param_32(m), bmat_param_32(m), s)
/// void bmat_row_lshift_64(bmat_param_64(m), bmat_param_64(m), s)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// mask to clear 'n' right columns (64-bit package)
// input mask is: 
inline uint64_t bmat_rcolumn_mask(uint64_t mask, uint32_t n)
{
  return ~((mask << n) - mask);
}

// mask to clear 'n' left columns (64-bit package)
// input mask is:
inline uint64_t bmat_lcolumn_mask(uint64_t mask, uint32_t n)
{
  return (mask >> n) + ~mask;
}

void bmat_row_lshift_8(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  s &= 7;
  uint64_t mask = bmat_rcolumn_mask(BMAT_COLUMN_MASK_8,s);
  d[0] = (m[0] << s) & mask;
}

void bmat_row_lshift_16(bmat_rparam_16(d), bmat_param_16(m), uint32_t s)
{
  s &= 0xf;

  uint64_t mask = bmat_rcolumn_mask(BMAT_COLUMN_MASK_16,s);

  d[0] = (m[0] << s) & mask;
  d[1] = (m[1] << s) & mask;
  d[2] = (m[2] << s) & mask;
  d[3] = (m[3] << s) & mask;
}

void bmat_row_lshift_32(bmat_rparam_32(d), bmat_param_32(m), uint32_t s)
{
  s &= 0x1f;

  uint64_t mask = bmat_rcolumn_mask(BMAT_COLUMN_MASK_32,s);

  for(uint32_t i=0; i<16; i++) {
    d[i] = (m[i] << s) & mask;
  }
}

void bmat_row_lshift_64(bmat_rparam_64(d), bmat_param_64(m), uint32_t s)
{
  s &= 0x3f;

  for(uint32_t i=0; i<64; i++)
    d[i] = (m[i] << s);
}


/// ## bmat_row_rshift_*n*(d,m,s)
///
/// Right shifts each row by `s`:
///
/// $$ D = MR^s $$
///
/// where $R$ is the right shift matrix. Input `s` is mod width.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_row_rshift_8 (bmat_param_8 (m), bmat_param_8 (m), s)
/// void bmat_row_rshift_16(bmat_param_16(m), bmat_param_16(m), s)
/// void bmat_row_rshift_32(bmat_param_32(m), bmat_param_32(m), s)
/// void bmat_row_rshift_64(bmat_param_64(m), bmat_param_64(m), s)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_row_rshift_8(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  s &= 7;
  uint64_t mask = bmat_lcolumn_mask(BMAT_COLUMN_MASK_8^1,s);
  d[0] = (m[0] >> s) & mask;
}

void bmat_row_rshift_16(bmat_rparam_16(d), bmat_param_16(m), uint32_t s)
{
  s &= 0xf;

  uint64_t mask = bmat_lcolumn_mask(BMAT_COLUMN_MASK_16^1,s);

  d[0] = (m[0] >> s) & mask;
  d[1] = (m[1] >> s) & mask;
  d[2] = (m[2] >> s) & mask;
  d[3] = (m[3] >> s) & mask;
}

void bmat_row_rshift_32(bmat_rparam_32(d), bmat_param_32(m), uint32_t s)
{
  s &= 0x1f;

  uint64_t mask = bmat_lcolumn_mask(BMAT_COLUMN_MASK_32^1,s);

  for(uint32_t i=0; i<16; i++) {
    d[i] = (m[i] >> s) & mask;
  }
}

void bmat_row_rshift_64(bmat_rparam_64(d), bmat_param_64(m), uint32_t s)
{
  s &= 0x3f;

  for(uint32_t i=0; i<64; i++)
    d[i] = (m[i] >> s);
}


/// ## bmat_row_ushift_*n*(d,m,s)
///
/// Shifts each row up `s`:
///
/// $$ D = L^s~M $$
///
/// where $L$ is the left shift matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_row_dshift_8 (bmat_param_8 (m), bmat_param_8 (m), s)
/// void bmat_row_dshift_16(bmat_param_16(m), bmat_param_16(m), s)
/// void bmat_row_dshift_32(bmat_param_32(m), bmat_param_32(m), s)
/// void bmat_row_dshift_64(bmat_param_64(m), bmat_param_64(m), s)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_row_ushift_8_ref(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  s &= 0x7;
  
  bmat_adup_8(a,m);

  for(uint32_t i=0;   i<8-s; i++) a[i] = a[s+i];
  for(uint32_t i=8-s; i<8;   i++) a[i] = 0;

  array_to_bmat_8(d,a);
}

void bmat_row_ushift_16_ref(bmat_rparam_16(d), bmat_param_16(m), uint32_t s)
{
  s &= 0xf;
  
  bmat_adup_16(a,m);

  for(uint32_t i=0;    i<16-s; i++) a[i] = a[s+i];
  for(uint32_t i=16-s; i<16;   i++) a[i] = 0;

  array_to_bmat_16(d,a);
}

void bmat_row_ushift_32_ref(bmat_rparam_32(d), bmat_param_32(m), uint32_t s)
{
  s &= 0x1f;
  
  bmat_adup_32(a,m);

  for(uint32_t i=0;    i<32-s; i++) a[i] = a[s+i];
  for(uint32_t i=32-s; i<32;   i++) a[i] = 0;

  array_to_bmat_32(d,a);
}

void bmat_row_ushift_64_ref(bmat_rparam_64(d), bmat_param_64(m), uint32_t s)
{
  for(uint32_t i=0;    i<64-s; i++) d[i] = m[s+i];
  for(uint32_t i=64-s; i<64;   i++) d[i] = 0;
}

void bmat_row_ushift_8(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  d[0] = m[0] >> (8*(s&7));
}

void bmat_row_ushift_16(bmat_rparam_16(d), bmat_param_16(m), uint32_t s) { bmat_row_ushift_16_ref(d,m,s); }
void bmat_row_ushift_32(bmat_rparam_32(d), bmat_param_32(m), uint32_t s) { bmat_row_ushift_32_ref(d,m,s); }
void bmat_row_ushift_64(bmat_rparam_64(d), bmat_param_64(m), uint32_t s) { bmat_row_ushift_64_ref(d,m,s); }


/// ## bmat_row_dshift_*n*(d,m,s)
///
/// Shifts each row down `s`:
///
/// $$ D = R^s~M $$
///
/// where $R$ is the right shift matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_row_dshift_8 (bmat_param_8 (m), bmat_param_8 (m), s)
/// void bmat_row_dshift_16(bmat_param_16(m), bmat_param_16(m), s)
/// void bmat_row_dshift_32(bmat_param_32(m), bmat_param_32(m), s)
/// void bmat_row_dshift_64(bmat_param_64(m), bmat_param_64(m), s)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_row_dshift_8_ref(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  s &= 0x7;
  
  bmat_adup_8(a,m);
  uint8_t D[8];

  for(uint32_t i=0; i<s; i++) D[i] = 0;
  for(uint32_t i=s; i<8; i++) D[i] = a[i-s];

  array_to_bmat_8(d,D);
}

void bmat_row_dshift_16_ref(bmat_rparam_16(d), bmat_param_16(m), uint32_t s)
{
  s &= 0xf;
  
  bmat_adup_16(a,m);
  uint16_t D[16];

  for(uint32_t i=0; i<s;  i++) D[i] = 0;
  for(uint32_t i=s; i<16; i++) D[i] = a[i-s];

  array_to_bmat_16(d,D);
}

void bmat_row_dshift_32_ref(bmat_rparam_32(d), bmat_param_32(m), uint32_t s)
{
  s &= 0x1f;
  
  bmat_adup_32(a,m);
  uint32_t D[32];

  for(uint32_t i=0; i<s;  i++) D[i] = 0;
  for(uint32_t i=s; i<32; i++) D[i] = a[i-s];

  array_to_bmat_32(d,D);
}

void bmat_row_dshift_64_ref(bmat_rparam_64(d), bmat_param_64(m), uint32_t s)
{
  s &= 0x3f;
  
  uint64_t D[64];

  for(uint32_t i=0; i<s;  i++) D[i] = 0;
  for(uint32_t i=s; i<64; i++) D[i] = m[i-s];

  array_to_bmat_64(d,D);
}


void bmat_row_dshift_8(bmat_rparam_8(d), bmat_param_8(m), uint32_t s)
{
  d[0] = m[0] << (8*(s&7));
}

void bmat_row_dshift_16(bmat_rparam_16(d), bmat_param_16(m), uint32_t s) { bmat_row_dshift_16_ref(d,m,s); }
void bmat_row_dshift_32(bmat_rparam_32(d), bmat_param_32(m), uint32_t s) { bmat_row_dshift_32_ref(d,m,s); }
void bmat_row_dshift_64(bmat_rparam_64(d), bmat_param_64(m), uint32_t s) { bmat_row_dshift_64_ref(d,m,s); }


/// ## bmat_flip_h_*n*(m,n)
///
/// Reverses each row:
///
/// $$ D = MJ $$
///
/// where $J$ is the exchange matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_flip_h_8 (bmat_param_8 (d), bmat_param_8 (m))
/// void bmat_flip_h_16(bmat_param_16(d), bmat_param_16(m))
/// void bmat_flip_h_32(bmat_param_32(d), bmat_param_32(m))
/// void bmat_flip_h_64(bmat_param_64(d), bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


/// ## bmat_flip_v_*n*(m,n)
///
/// Reverses each column:
///
/// $$ D = JM $$
///
/// where $J$ is the exchange matrix.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_flip_v_8 (bmat_param_8 (d), bmat_param_8 (m))
/// void bmat_flip_v_16(bmat_param_16(d), bmat_param_16(m))
/// void bmat_flip_v_32(bmat_param_32(d), bmat_param_32(m))
/// void bmat_flip_v_64(bmat_param_64(d), bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


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


//*************************************************************************************************
// WARNING: below here implementation need to occur after the callsites where they are used. This
// is because the forward declarations don't have the 'restrict' type qualifier specified. This is
// to hint to the compiler that the output being one of the inputs is legal (and thus no warning)
// but there's no partial overlaps between the output and input(s).  So once some d[i] has been
// written there is no future input (say m[i]) read that requires it's incoming value. 
// Obviously doesn't matter for any external callsites.

// useless defines just to "look the same" as other sizes
void bmat_add_lsk_8(bmat_rparam_8(d), bmat_param_8(m), uint64_t k) { d[0] = m[0] ^ k; }
void bmat_add_rsk_8(bmat_rparam_8(d), bmat_param_8(m), uint64_t k) { d[0] = m[0] ^ k; }

void bmat_add_lsk_16(bmat_rparam_16(d), bmat_param_16(m), uint64_t k)
{ 
  d[0] = m[0] ^ k; k <<= 4;
  d[1] = m[1] ^ k; k <<= 4;
  d[2] = m[2] ^ k; k <<= 4;
  d[3] = m[3] ^ k;
}

void bmat_add_lsk_32(bmat_rparam_32(d), bmat_param_32(m), uint64_t k)
{
  for(uint32_t i=0; i<16; i++) {
    d[i] = m[i] ^ k; k <<= 2;
  }
}

void bmat_add_lsk_64(bmat_rparam_64(d), bmat_param_64(m), uint64_t k)
{
  for(uint32_t i=0; i<64; i++) {
    d[i] = m[i] ^ k; k <<= 1;
  }
}

void bmat_add_rsk_16(bmat_rparam_16(d), bmat_param_16(m), uint64_t k)
{ 
  d[0] = m[0] ^ k; k >>= 4;
  d[1] = m[1] ^ k; k >>= 4;
  d[2] = m[2] ^ k; k >>= 4;
  d[3] = m[3] ^ k;
}

void bmat_add_rsk_32(bmat_rparam_32(d), bmat_param_32(m), uint64_t k)
{
  for(uint32_t i=0; i<16; i++) {
    d[i] = m[i] ^ k; k >>= 2;
  }
}

void bmat_add_rsk_64(bmat_rparam_64(d), bmat_param_64(m), uint64_t k)
{
  for(uint32_t i=0; i<64; i++) {
    d[i] = m[i] ^ k; k >>= 1;
  }
}

// here just because they are similar to the add versions
void bmat_set_lsk_8(bmat_param_8(m), uint64_t k) { m[0] = k; }
void bmat_set_rsk_8(bmat_param_8(m), uint64_t k) { m[0] = k; }

void bmat_set_lsk_16(bmat_param_16(m), uint64_t k)
{ 
  m[0] = k; k <<= 4;
  m[1] = k; k <<= 4;
  m[2] = k; k <<= 4;
  m[3] = k;
}

void bmat_set_lsk_32(bmat_param_32(m), uint64_t k)
{
  for(uint32_t i=0; i<16; i++) {
    m[i] = k; k <<= 2;
  }
}

void bmat_set_lsk_64(bmat_param_64(m), uint64_t k)
{
  for(uint32_t i=0; i<64; i++) {
    m[i] = k; k <<= 1;
  }
}

void bmat_set_rsk_16(bmat_param_16(m), uint64_t k)
{ 
  m[0] = k; k >>= 4;
  m[1] = k; k >>= 4;
  m[2] = k; k >>= 4;
  m[3] = k;
}

void bmat_set_rsk_32(bmat_param_32(m), uint64_t k)
{
  for(uint32_t i=0; i<16; i++) {
    m[i] = k; k >>= 2;
  }
}

void bmat_set_rsk_64(bmat_param_64(m), uint64_t k)
{
  for(uint32_t i=0; i<64; i++) {
    m[i] = k; k >>= 1;
  }
}
