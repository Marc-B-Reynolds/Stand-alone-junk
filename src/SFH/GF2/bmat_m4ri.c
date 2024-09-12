// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

/// M4RI bridge functions
///==============================================================
///
/// The headers for these functions are in `bmat_m4ri.h`.  If any
/// of these routines are used then M4RI must be installed and
/// the library linked.
/// 
/// Only the functions:
/// * `bmat_to_m4ri_n`
/// * `bmat_from_m4ri_n`
/// 
/// provide any functionality. Everything else is just wrappers
/// that seem convenient to me. A fair amount of heavy weight
/// functions for testing convenience.

//Any function that contains `test`
// is solely designed for testing purposes (it probably does
// the *thing* in a super expensive way)


// OPTIONAL BRIDGE TO M4RI. It's all hacked ATM
// (should have M4RIE as well as an additional option)
// testing specific stuff should be moved to testing
// directory. Stuff here should be for supplemental
// support only.

#include "bmat_i.h"
#include "bmat_m4ri.h"

/// ## m4ri_alloc_n(n)
///
/// allocates a M4RI structure for a $n \times n$ matrix.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// mzd_t* m4ri_alloc_n(uint32_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>
mzd_t* m4ri_alloc_n(uint32_t n)
{
  return mzd_init((rci_t)n,(rci_t)n);
}

/// ## m4ri_alloc_*n*(void)
///
/// allocates a M4RI structure for a $n \times n$ matrix.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// mzd_t* m4ri_alloc_8 (void)
/// mzd_t* m4ri_alloc_16(void)
/// mzd_t* m4ri_alloc_32(void)
/// mzd_t* m4ri_alloc_64(void)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

mzd_t* m4ri_alloc_8 (void) { return m4ri_alloc_n(8);  }
mzd_t* m4ri_alloc_16(void) { return m4ri_alloc_n(16); }
mzd_t* m4ri_alloc_32(void) { return m4ri_alloc_n(32); }
mzd_t* m4ri_alloc_64(void) { return m4ri_alloc_n(64); }


/// ## m4ri_make_*n*(m)
///
/// allocates a M4RI structure for a $n \times n$ matrix and
/// sets it to `m`.
///
/// <details markdown="1"><summary>function list:</summary>
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// mzd_t* m4ri_make_8 (bmat_param_8 (m))
/// mzd_t* m4ri_make_16(bmat_param_8 (m))
/// mzd_t* m4ri_make_32(bmat_param_8 (m))
/// mzd_t* m4ri_make_64(bmat_param_8 (m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>
mzd_t* m4ri_make_8 (bmat_param_8 (s)) { mzd_t* m = m4ri_alloc_8 (); bmat_to_m4ri_8 (m,s); return m; }
mzd_t* m4ri_make_16(bmat_param_16(s)) { mzd_t* m = m4ri_alloc_16(); bmat_to_m4ri_16(m,s); return m; }
mzd_t* m4ri_make_32(bmat_param_32(s)) { mzd_t* m = m4ri_alloc_32(); bmat_to_m4ri_32(m,s); return m; }
mzd_t* m4ri_make_64(bmat_param_64(s)) { mzd_t* m = m4ri_alloc_64(); bmat_to_m4ri_64(m,s); return m; }


/// ## m4ri_free(mzd_t* m)
///
/// Just a wrapper of `mzd_free` to be name consistent with
/// `m4ri_alloc_n` functions.

void m4ri_free(mzd_t* m) { mzd_free(m); }


/// ## bmat_to_m4ri_*n*(d,m)
///
/// copies the matrix `m` into the M4RI structure `d`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_to_m4ri_8 (mzd_t* d, bmat_param_8(m))
/// void bmat_to_m4ri_16(mzd_t* d, bmat_param_16(m))
/// void bmat_to_m4ri_32(mzd_t* d, bmat_param_32(m))
/// void bmat_to_m4ri_64(mzd_t* d, bmat_param_64(m))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_to_m4ri_8(mzd_t* d, bmat_param_8(m))
{
  uint8_t s[8];
  bmat_to_array_8(s,m);

  const rci_t D = 8;
  
  for(rci_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(rci_t c=0; c<D; c++) {
      mzd_write_bit(d,r,c, row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_m4ri_16(mzd_t* d, bmat_param_16(m))
{
  uint16_t s[16];
  bmat_to_array_16(s,m);

  const rci_t D = 16;
  
  for(rci_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(rci_t c=0; c<D; c++) {
      mzd_write_bit(d,r,c, row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_m4ri_32(mzd_t* d, bmat_param_32(m))
{
  uint32_t s[32];
  bmat_to_array_32(s,m);

  const rci_t D = 32;
  
  for(rci_t r=0; r<D; r++) {
    uint32_t row = s[r];
    for(rci_t c=0; c<D; c++) {
      mzd_write_bit(d,r,c, row & 1);
      row >>= 1;
    }
  }
}

void bmat_to_m4ri_64(mzd_t* d, bmat_param_64(m))
{
  uint64_t s[64];
  bmat_to_array_64(s,m);

  const rci_t D = 64;
  
  for(rci_t r=0; r<D; r++) {
    uint64_t row = s[r];
    for(rci_t c=0; c<D; c++) {
      mzd_write_bit(d,r,c, row & 1);
      row >>= 1;
    }
  }
}

/// ## bmat_from_m4ri_*n*(d,m)
///
/// copies the matrix `m` into the M4RI structure `d`
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_from_m4ri_8 (bmat_param_8(m)), mzd_t* d)
/// void bmat_from_m4ri_16(bmat_param_16(m), mzd_t* d)
/// void bmat_from_m4ri_32(bmat_param_32(m), mzd_t* d)
/// void bmat_from_m4ri_64(bmat_param_64(m), mzd_t* d)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_from_m4ri_8(bmat_param_8(m), mzd_t* s)
{
  uint8_t d[8];

  const rci_t D = 8;
  
  for(rci_t i=0; i<D; i++)
    d[i] = (uint8_t)mzd_read_bits(s,i,0,D);

  array_to_bmat_8(m,d);
}

void bmat_from_m4ri_16(bmat_param_16(m), mzd_t* s)
{
  uint16_t d[16];

  const rci_t D = 16;
  
  for(rci_t i=0; i<D; i++)
    d[i] = (uint16_t)mzd_read_bits(s,i,0,D);

  array_to_bmat_16(m,d);
}

void bmat_from_m4ri_32(bmat_param_32(m), mzd_t* s)
{
  uint32_t d[32];

  const rci_t D = 32;
  
  for(rci_t i=0; i<D; i++)
    d[i] = (uint32_t)mzd_read_bits(s,i,0,D);

  array_to_bmat_32(m,d);
}

void bmat_from_m4ri_64(bmat_param_64(m), mzd_t* s)
{
  uint64_t d[64];

  const rci_t D = 64;

  for(rci_t i=0; i<D; i++)
    d[i] = mzd_read_bits(s,i,0,D);

  array_to_bmat_64(m,d);
}

void m4ri_set_rv_n(mzd_t* v, uint64_t x, rci_t n)
{
  for(rci_t i=0; i<n; i++) {
    mzd_write_bit(v,0,i, x & 1);
    x >>= 1;
  }
}

void m4ri_set_rv_8 (mzd_t* v, uint8_t  x) { m4ri_set_rv_n(v,x, 8); }
void m4ri_set_rv_16(mzd_t* v, uint16_t x) { m4ri_set_rv_n(v,x,16); }
void m4ri_set_rv_32(mzd_t* v, uint32_t x) { m4ri_set_rv_n(v,x,32); }
void m4ri_set_rv_64(mzd_t* v, uint64_t x) { m4ri_set_rv_n(v,x,64); }

uint64_t m4ri_get_rv_n(mzd_t* v, rci_t n) { return mzd_read_bits(v,0,0,n); }

uint32_t m4ri_get_rv_8 (mzd_t* v) { return (uint32_t)m4ri_get_rv_n(v, 8); }
uint32_t m4ri_get_rv_16(mzd_t* v) { return (uint32_t)m4ri_get_rv_n(v,16); }
uint32_t m4ri_get_rv_32(mzd_t* v) { return (uint32_t)m4ri_get_rv_n(v,32); }
uint64_t m4ri_get_rv_64(mzd_t* v) { return (uint64_t)m4ri_get_rv_n(v,64); }

mzd_t* m4ri_alloc_rv_8(void)      { return mzd_init((rci_t)1,(rci_t) 8); }
mzd_t* m4ri_alloc_rv_16(void)     { return mzd_init((rci_t)1,(rci_t)16); }
mzd_t* m4ri_alloc_rv_32(void)     { return mzd_init((rci_t)1,(rci_t)32); }
mzd_t* m4ri_alloc_rv_64(void)     { return mzd_init((rci_t)1,(rci_t)64); }

mzd_t* m4ri_make_rv_8 (uint8_t  v) { mzd_t* m = m4ri_alloc_rv_8 (); m4ri_set_rv_8 (m,v); return m; }
mzd_t* m4ri_make_rv_16(uint16_t v) { mzd_t* m = m4ri_alloc_rv_16(); m4ri_set_rv_16(m,v); return m; }
mzd_t* m4ri_make_rv_32(uint32_t v) { mzd_t* m = m4ri_alloc_rv_32(); m4ri_set_rv_32(m,v); return m; }
mzd_t* m4ri_make_rv_64(uint64_t v) { mzd_t* m = m4ri_alloc_rv_64(); m4ri_set_rv_64(m,v); return m; }


//*****************************************************
// heavy weight convenience functions: alloc/convert/op/convert/free

void m4ri_wrap_mm_8(bmat_param_8(C), bmat_param_8(A), bmat_param_8(B))
{
  mzd_t* a = m4ri_make_8(A);
  mzd_t* b = m4ri_make_8(B);
  mzd_t* c = _mzd_mul_naive(0,a,b,1);
  bmat_from_m4ri_8(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c);
}

void m4ri_wrap_mm_16(bmat_param_16(C), bmat_param_16(A), bmat_param_16(B))
{
  mzd_t* a = m4ri_make_16(A);
  mzd_t* b = m4ri_make_16(B);
  mzd_t* c = _mzd_mul_naive(0,a,b,1);
  bmat_from_m4ri_16(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c);
}

void m4ri_wrap_mm_32(bmat_param_32(C), bmat_param_32(A), bmat_param_32(B))
{
  mzd_t* a = m4ri_make_32(A);
  mzd_t* b = m4ri_make_32(B);
  mzd_t* c = _mzd_mul_naive(0,a,b,1);
  bmat_from_m4ri_32(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c);
}

void m4ri_wrap_mm_64(bmat_param_64(C), bmat_param_64(A), bmat_param_64(B))
{
  mzd_t* a = m4ri_make_64(A);
  mzd_t* b = m4ri_make_64(B);
  mzd_t* c = _mzd_mul_naive(0,a,b,1);
  bmat_from_m4ri_64(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c);
}

void m4ri_wrap_mt_8(bmat_param_8(C), bmat_param_8(A), bmat_param_8(B))
{
  mzd_t* a = m4ri_make_8(A);
  mzd_t* b = m4ri_make_8(B);
  mzd_t* t = mzd_transpose(0,b);
  mzd_t* c = _mzd_mul_naive(0,a,t,1);
  bmat_from_m4ri_8(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c); mzd_free(t);
}

void m4ri_wrap_mt_16(bmat_param_16(C), bmat_param_16(A), bmat_param_16(B))
{
  mzd_t* a = m4ri_make_16(A);
  mzd_t* b = m4ri_make_16(B);
  mzd_t* t = mzd_transpose(0,b);
  mzd_t* c = _mzd_mul_naive(0,a,t,1);
  bmat_from_m4ri_16(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c); mzd_free(t);
}

void m4ri_wrap_mt_32(bmat_param_32(C), bmat_param_32(A), bmat_param_32(B))
{
  mzd_t* a = m4ri_make_32(A);
  mzd_t* b = m4ri_make_32(B);
  mzd_t* t = mzd_transpose(0,b);
  mzd_t* c = _mzd_mul_naive(0,a,t,1);
  bmat_from_m4ri_32(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c); mzd_free(t);
}

void m4ri_wrap_mt_64(bmat_param_64(C), bmat_param_64(A), bmat_param_64(B))
{
  mzd_t* a = m4ri_make_64(A);
  mzd_t* b = m4ri_make_64(B);
  mzd_t* t = mzd_transpose(0,b);
  mzd_t* c = _mzd_mul_naive(0,a,t,1);
  bmat_from_m4ri_64(C,c);
  mzd_free(a); mzd_free(b); mzd_free(c); mzd_free(t);
}

uint32_t m4ri_wrap_vm_8(uint32_t V, bmat_param_8(M))
{
  mzd_t*   m = m4ri_make_8(M);
  mzd_t*   v = m4ri_make_rv_8((uint8_t)V);
  mzd_t*   r = m4ri_alloc_rv_8();
  _mzd_mul_va(r,v,m,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_8(r)) & 0xff;
  mzd_free(m); mzd_free(v); mzd_free(r);
  return R;
}

uint32_t m4ri_wrap_vm_16(uint32_t V, bmat_param_16(M))
{
  mzd_t*   m = m4ri_make_16(M);
  mzd_t*   v = m4ri_make_rv_16((uint16_t)V);
  mzd_t*   r = m4ri_alloc_rv_16();
  _mzd_mul_va(r,v,m,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_16(r)) & 0xffff;
  mzd_free(m); mzd_free(v); mzd_free(r);
  return R;
}

uint32_t m4ri_wrap_vm_32(uint32_t V, bmat_param_32(M))
{
  mzd_t*   m = m4ri_make_32(M);
  mzd_t*   v = m4ri_make_rv_32(V);
  mzd_t*   r = m4ri_alloc_rv_32();
  _mzd_mul_va(r,v,m,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_32(r));
  mzd_free(m); mzd_free(v); mzd_free(r);
  return R;
}

uint64_t m4ri_wrap_vm_64(uint64_t V, bmat_param_64(M))
{
  mzd_t*   m = m4ri_make_64(M);
  mzd_t*   v = m4ri_make_rv_64(V);
  mzd_t*   r = m4ri_alloc_rv_64();
  _mzd_mul_va(r,v,m,1);
  uint64_t R = ((uint64_t)m4ri_get_rv_64(r));
  mzd_free(m); mzd_free(v); mzd_free(r);
  return R;
}


uint32_t m4ri_wrap_mv_8(bmat_param_8(M), uint32_t V)
{
  mzd_t* m = m4ri_make_8(M);
  mzd_t* v = m4ri_make_rv_8((uint8_t)V);
  mzd_t* r = m4ri_alloc_rv_8();
  mzd_t* t = mzd_transpose(0,m);
  _mzd_mul_va(r,v,t,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_8(r)) & 0xff;
  mzd_free(m); mzd_free(v); mzd_free(r); mzd_free(t);
  return R;
}

uint32_t m4ri_wrap_mv_16(bmat_param_16(M), uint32_t V)
{
  mzd_t* m = m4ri_make_16(M);
  mzd_t* v = m4ri_make_rv_16((uint16_t)V);
  mzd_t* r = m4ri_alloc_rv_16();
  mzd_t* t = mzd_transpose(0,m);
  _mzd_mul_va(r,v,t,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_16(r)) & 0xffff;
  mzd_free(m); mzd_free(v); mzd_free(r); mzd_free(t);
  return R;
}

uint32_t m4ri_wrap_mv_32(bmat_param_32(M), uint32_t V)
{
  mzd_t* m = m4ri_make_32(M);
  mzd_t* v = m4ri_make_rv_32(V);
  mzd_t* r = m4ri_alloc_rv_32();
  mzd_t* t = mzd_transpose(0,m);
  _mzd_mul_va(r,v,t,1);
  uint32_t R = ((uint32_t)m4ri_get_rv_32(r));
  mzd_free(m); mzd_free(v); mzd_free(r); mzd_free(t);
  return R;
}

uint64_t m4ri_wrap_mv_64(bmat_param_64(M), uint64_t V)
{
  mzd_t* m = m4ri_make_64(M);
  mzd_t* v = m4ri_make_rv_64(V);
  mzd_t* r = m4ri_alloc_rv_64();
  mzd_t* t = mzd_transpose(0,m);
  _mzd_mul_va(r,v,t,1);
  uint64_t R = ((uint64_t)m4ri_get_rv_64(r));
  mzd_free(m); mzd_free(v); mzd_free(r); mzd_free(t);
  return R;
}


//*****************************************************


// add comments
uint32_t m4ri_full_echelon(mzd_t* m)
{
  return (uint32_t)mzd_echelonize(m,1);
}

// not super useful since not unique
uint32_t m4ri_partial_echelon(mzd_t* m)
{
  return (uint32_t)mzd_echelonize(m,0);
}

void m4ri_zero(mzd_t* m) { mzd_add(m,m,m); }



/// ## m4ri_pow(m,n)
///
/// $$ M' = M^n $$
///
/// in-place computation (result is in `m`). Straight
/// forward software implementation.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void m4ri_pow(mzd_t* m, uint64_t n)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// temp hack
inline mzd_t* mat_mul(mzd_t* r, mzd_t* a, mzd_t* b) { return mzd_mul(r,a,b,0); }
// Does M4RI have this and I'm just not finding it?
void m4ri_pow(mzd_t* m, uint64_t n)
{
  mzd_t* s = mzd_copy(0,m);
  mzd_t* t = mzd_copy(0,m);
  
  mzd_set_ui(m,1);

  while(n != 0) {
    if (n & 1) {
      t = mzd_copy(t,m);
      m = mat_mul(m,t,s);
    }
    
    mat_mul(t,s,s);
    mzd_copy(s,t);
    n >>= 1;
  }
  
  mzd_free(t);
  mzd_free(s);
}


/// ## m4ri_mulv_*n*(d,m)
///
/// $$ r = Mv $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


/// ## m4ri_vmul_*n*(d,m)
///
/// $$ r = vM $$
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>






#if 0

// allocate and copy

//-------------------------------------------------------------
// testing routines

#define COPY1_8()  bmat_to_m4ri_8 (ma,a);
#define COPY1_16() bmat_to_m4ri_16(ma,a);
#define COPY1_32() bmat_to_m4ri_32(ma,a);
#define COPY1_64() bmat_to_m4ri_64(ma,a);
#define COPY2_8()  bmat_to_m4ri_8 (ma,a); bmat_to_m4ri_8 (mb,b);
#define COPY2_16() bmat_to_m4ri_16(ma,a); bmat_to_m4ri_16(mb,b);
#define COPY2_32() bmat_to_m4ri_32(ma,a); bmat_to_m4ri_32(mb,b);
#define COPY2_64() bmat_to_m4ri_64(ma,a); bmat_to_m4ri_64(mb,b);
#define COPY3_8()  bmat_to_m4ri_8 (ma,a); bmat_to_m4ri_8 (mb,b); bmat_to_m4ri_8(mc,c);
#define COPY3_16() bmat_to_m4ri_16(ma,a); bmat_to_m4ri_16(mb,b); bmat_to_m4ri_16(mc,c);
#define COPY3_32() bmat_to_m4ri_32(ma,a); bmat_to_m4ri_32(mb,b); bmat_to_m4ri_32(mc,c);
#define COPY3_64() bmat_to_m4ri_64(ma,a); bmat_to_m4ri_64(mb,b); bmat_to_m4ri_64(mc,c);

#define LOOP(I,S,E) for(I=S;I<E;I++)

uint32_t m4ri_test_mul_n(uint64_t* c, uint64_t* b, uint64_t* a, uint32_t w, uint32_t n)
{
  mzd_t* mr = m4ri_alloc(w);
  mzd_t* mc = m4ri_alloc(w);
  mzd_t* ma = m4ri_alloc(w);
  mzd_t* mb = m4ri_alloc(w);

  uint32_t error = 0;

  // add check 'w'

  switch(w) {
    case 8:  for (uint32_t i=0; i<n; i++) {COPY_3_8();}
    case 16: for (uint32_t i=0; i<n; i++) {COPY_3_8();}
    case 32: for (uint32_t i=0; i<n; i++) {COPY_3_8();}
    case 64: for (uint32_t i=0; i<n; i++) {COPY_3_8();}
      
    default: // do unreachable
      error = 1;
      break;
  }
  
  mzd_free(mr); mzd_free(ma); mzd_free(mb); mzd_free(mc);
  
  return error;
}



//-------------------------------------------------------------


// BELOW HERE IS OLD GARBAGE

// assumes v is 1x32
void m4ri_32_to_row_vector(mzd_t* v, uint32_t x)
{
  
#if !defined(MAT_ASSUME_MEM_LAYOUT)
  for(int i=0; i<32; i++) {
    mzd_write_bit(v,0,i, x & 1);
    x >>= 1;
  }
#else
  ((uint32_t*)(v->data))[0] = x;
#endif
}


// assumes v is 1x32
uint32_t m4ri_row_vector_to_32(mzd_t* v)
{
  //assert();
  
#if !defined(MAT_ASSUME_MEM_LAYOUT)
  uint32_t r = 0;
  
  for(int i=0; i<32; i++) 
    r |= (uint32_t)mzd_read_bit(v,0,i) << i;

  return r;
#else
  return ((uint32_t*)(v->data))[0];
#endif
}


// assumes v is 1x64
void m4ri_64_to_row_vector(mzd_t* v, uint64_t x)
{
#if !defined(MAT_ASSUME_MEM_LAYOUT)
  for(int i=0; i<64; i++) {
    mzd_write_bit(v,0,i, x & 1);
    x >>= 1;
  }
#else
  ((uint64_t*)(v->data))[0] = x;
#endif
}


// assumes v is 1x64
uint64_t m4ri_row_vector_to_64(mzd_t* v)
{
#if !defined(MAT_ASSUME_MEM_LAYOUT)
  uint64_t r = 0;
  
  for(int i=0; i<64; i++) 
    r |= (uint64_t)mzd_read_bit(v,0,i) << i;

  return r;
#else
  return ((uint64_t*)(v->data))[0];
#endif
}


inline mzd_t* m4ri_mul(mzd_t* r, mzd_t* a, mzd_t* b) { return mzd_mul(r,a,b,0); }

#if 0
// M is matrix to invert, I is inverse (out) and returns 1 if invertiable
uint32_t m4ri_inv_k(mzd_t* I, mzd_t* M, m4ri_common_t* data)
{
  mzd_t* id = data->id;
  mzd_t* t  = data->m[MAT_TEMP_1];

  mzd_inv_m4ri(I,M,0);

  m4ri_mul(t,I,M);

  return (uint32_t)mzd_equal(t,id);
}
#endif

//uint32_t m4ri_32_inv(mzd_t* I, mzd_t* M)  { return m4ri_inv_k(I,M, &mat_32_common); }
//uint32_t m4ri_64_inv(mzd_t* I, mzd_t* M)  { return m4ri_inv_k(I,M, &mat_64_common); }
//uint32_t m4ri_32h_inv(mzd_t* I, mzd_t* M) { return m4ri_inv_k(I,M, &mat_32h_common); }
//uint32_t m4ri_64h_inv(mzd_t* I, mzd_t* M) { return m4ri_inv_k(I,M, &mat_64h_common); }


// fill row vector 'v' with 'x' and compute vM. temp result stored in 'r'
uint32_t m4ri_eval_k_32(mzd_t* r, mzd_t* v, mzd_t* M, uint32_t x)
{
  m4ri_32_to_row_vector(v,x);

  _mzd_mul_va(r,v,M,1);

  return m4ri_row_vector_to_32(r);
}


// meh
uint32_t m4ri_eval_32(mzd_t* M, uint32_t x)
{
  // transpose matrix (boo!)
  mzd_t* T = mzd_transpose(0,M);

  // create temporary row vectors (boo!)
  mzd_t* v = mzd_init(1,32);
  mzd_t* r = mzd_init(1,32);

  uint32_t result = m4ri_eval_k_32(r,v,T,x);

  mzd_free(T);
  mzd_free(v);
  mzd_free(r);

  return result;
}

// in-place compute m^n (validate)
// ouch! expensive as is. (doesn't matter ATM)
mzd_t* m4ri_pow(mzd_t* m, uint64_t n)
{
  mzd_t* s = mzd_copy(0,m);
  mzd_t* t = mzd_copy(0,m);
  
  mzd_set_ui(m,1);

  while(n != 0) {
    if (n & 1) {
      t = mzd_copy(t,m);
      m = m4ri_mul(m,t,s);
    }
    
    m4ri_mul(t,s,s);
    mzd_copy(s,t);
    n >>= 1;
  }
  
  mzd_free(t);
  mzd_free(s);

  return m;
}

#endif
