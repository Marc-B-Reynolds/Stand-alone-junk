// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

/**
   Textbook matrix multiplication (row times column) is based on
   working with scalars. Since we need to work in parallel we'll
   perform "row broadcasting". Consider the following 4x4 product:

  row   A     B       C   
   0 |1000| |abcd| |abcd|
   1 |1000|.|efgh|=|abcd|
   2 |1000| |ijkl| |abcd|
   3 |1000| |mnop| |abcd|
      0123                 column

   and if A had only 1s in the column-1 then C's rows would be all
   be |efgh|, etc. So:

   column-n of A indicates that row-n of C needs a copy of row-n
   of B. Shit that's poorly written.

   To compute C=AB^T then we can proceed as above by performing
   a bit gather to transform column-n of B into the bits to be
   broadcast.
 */


#if defined(SWAR_AVX2_H)

const u256_data_t bmat_cm_8_256  = {.u64 = BMAT_REP_SHIFT_4(UINT64_C(0x0101010101010101),1) };

// worker with both matrices in register
// 10.00
uint64_t bmat_mul_8_i(uint64_t a, uint64_t B)
{
  u256_t t  = setr_8x32(0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,
			3,3,3,3,3,3,3,3);
  u256_t z  = zero_256();
  u256_t cm = bmat_cm_8_256.p;
  u256_t b  = broadcast_64x4(B);
  u256_t a0 = broadcast_64x4(a);
  u256_t a1 = srli_64x4(a0,4);
  u256_t r0 = pshufb_128x2(b,t);
  u256_t r1 = pshufb_128x2(bsrli_128x2(b,4),t);
  u256_t c0,c1;

  // sum of the masked broadcasts
  c0 = and_256(r0, cmpgt_8x32(and_256(a0,cm),z));
  c1 = and_256(r1, cmpgt_8x32(and_256(a1,cm),z));

  // merge
  c0 = xor_256(bsrli_128x2(c0,8), c0);
  c1 = xor_256(bsrli_128x2(c1,8), c1);
  c0 = xor_256(c0,c1);
  c0 = xor_256(c0, unpackhi_128x2(c0,c0));
  
  return movq_256(c0);
}

#else

// worker with both matrices in register
// 14.75
uint64_t bmat_mul_8_i(uint64_t a, uint64_t b)
{
  // use integer multiplier to broadcast rows of B
  // * rm isolates row  of B
  // * cm isolates cols of A
  const uint64_t rm = UINT64_C(0x00000000000000ff);
  const uint64_t cm = UINT64_C(0x0101010101010101);

  // perform the row broadcast:
  //  * (rm & b) isolates row-0 of B
  //  * (cm & a) isolates col-0 of A which indicates
  //    which rows of C need a copy
  //  * the product performs the (masked) broadcast
  uint64_t r = (cm & a)*(rm & b);

  // rinse and repeat for remaining row/col pairs
  for (uint32_t i=1; i<8; i++) {
    a >>= 1;                        // move to next column
    b >>= 8;                        // next row in place
    r  ^= (cm & a)*(rm & b);
  }

  return r;
}

#endif


void bmat_mul_8(bmat_rparam_8(c), bmat_param_8(a), bmat_param_8(b))
{
  c[0] = bmat_mul_8_i(a[0],b[0]);
}


//*******************************************************************
// 16-bit


#if defined(SWAR_AVX2_H)

// AVX2 or temp hack usage via SIMDe

static inline u256_t bmat_mul_step_16(u256_t a, u256_t b, u256_t cm, u256_t z)
{
  return and_256(broadcast_lo_16x16(b), cmpgt_16x16(and_256(a,cm),z));
}

// 53.75
u256_t bmat_mul_16_i(u256_t a, u256_t b)
{
  u256_t cm = broadcast_16x16(0x1);
  u256_t z  = zero_256();
  u256_t c;
  u256_t r;

  // cookie-cutter written with one long dep chain: let the compiler rework
  r = b;
  c =         bmat_mul_step_16(a,r,cm,z);    a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1);

  r = unpackhi_128x2(b,b);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c); a = srli_16x16(a,1); r = bsrli_128x2(r,2);
  c = xor_256(bmat_mul_step_16(a,r,cm,z),c);

  return c;
}

void bmat_mul_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
{
  bmat_store_256(c, bmat_mul_16_i(bmat_load_256(a),bmat_load_256(b)));
}

#else

// fall-back 64-bit SWAR versions

// 104.25
void bmat_mul_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(B))
{
  const uint64_t rm = UINT64_C(0x000000000000ffff);
  const uint64_t cm = UINT64_C(0x0001000100010001);

  // initialize C to zero
  c[0]=0; c[1]=0; c[2]=0; c[3]=0;

  // grab all of matrix A
  uint64_t a0 = a[0];
  uint64_t a1 = a[1];
  uint64_t a2 = a[2];
  uint64_t a3 = a[3];

  for (uint32_t j=0; j<4; j++) {
    uint64_t b = B[j];                  // grab 4 rows of B
    
    for (uint32_t i=0; i<4; i++) {
      uint64_t r = (rm & b);            // isolated row (4j+i)

      // perform the sum of masked broadcasts
      // and move the next column of A into place
      c[0] ^= (cm & a0)*r; a0 >>= 1;
      c[1] ^= (cm & a1)*r; a1 >>= 1;
      c[2] ^= (cm & a2)*r; a2 >>= 1;
      c[3] ^= (cm & a3)*r; a3 >>= 1;

      // shift the next row into place
      b >>= 16;
    }
  }
}

#endif

// need to write bit_gather_lsb_16x4
#if 0

// gather the values to be broadcast. 
static inline uint64_t bmat_mult_16_row(uint64_t p)
{
  // 0 1 2 3 16 17 18 19 32 33 34 35 48 49 50 51 **
  const uint64_t k = UINT64_C(0x0000001001001001);
  const uint64_t m = UINT64_C(0x000f000f000f000f);

  uint64_t r = k*(b0 & m); r >>= 36; r &= 0xffff;
  
  return r;
}

void bmat_mult_16_(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(B))
{
  const uint64_t cm = UINT64_C(0x0001000100010001);

  c[0]=0; c[1]=0; c[2]=0; c[3]=0;

  // nothing done yet
  uint64_t a0 = a[0], b0 = b[0];
  uint64_t a1 = a[1], b1 = b[1];
  uint64_t a2 = a[2], b2 = b[2];
  uint64_t a3 = a[3], b3 = b[3];

  uint64_t r,t;
  
  for (uint32_t j=0; j<4; j++) {

    r = ((r & (cm*0xf)) * cm);
    r = rol_64(r, 28) & 0xffff; // nope..see above
    
    for (uint32_t i=0; i<4; i++) {
      
      c[0] ^= (cm & a0)*r; a0 >>= 1;
      c[1] ^= (cm & a1)*r; a1 >>= 1;
      c[2] ^= (cm & a2)*r; a2 >>= 1;
      c[3] ^= (cm & a3)*r; a3 >>= 1;
    }
  }
}
#endif

// WIP HACKS


//*******************************************************************
// 32-bit

#if 0
void bmat_mul_32_avx2(bmat_rparam_32(C), bmat_param_32(A), bmat_param_32(B))
{
  bmat_def_64(r);
}
#endif

// wip hack
extern void bmat_mul_32_ref(bmat_param_32(C), bmat_param_32(MA), bmat_param_32(MB));
void bmat_mul_32(bmat_param_32(c), bmat_param_32(a), bmat_param_32(b)) { bmat_mul_32_ref(c,a,b); }


//*******************************************************************
// 64-bit

extern void bmat_mul_64_ref(bmat_param_64(c), bmat_param_64(a), bmat_param_64(b));

void bmat_mul_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  bmat_mul_64_ref(c,a,b);
}


#if 0
void bmat_mult_64_(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  

  for (uint32_t i=0; i<64; i++) {
    uint64_t a = A[i];
    uint64_t r = (-(a & 1)) & B[0];
    
    for (uint32_t j=1; j<64; j++) {
      a >>= 1;
      r ^= (-(a & 1)) & B[j];
    }
    
    S[i] = r;
  }
}
#else
#endif

//*******************************************************************
// default expansions: transpose. default 8 & 16 aren't too bad

#ifndef BMAT_HAS_MULT_8

// 26.38
static inline uint64_t bmat_mult_8_i(uint64_t a, uint64_t b)
{
  const uint64_t cm = UINT64_C(0x0101010101010101);
  
  uint64_t r = 0;

  for (uint32_t i=0; i<8; i++) {
    r ^= bit_gather_lsb_8x8(cm & b) * (cm & a);
    a >>= 1;
    b >>= 1;
  }
  return r;
}
#endif

void bmat_mult_8(bmat_rparam_8(c), bmat_param_8(a), bmat_param_8(b))
{
  c[0] = bmat_mult_8_i(a[0],b[0]);
}


#ifndef BMAT_HAS_MULT_16
void bmat_mult_16(bmat_param_16(c), bmat_param_16(a), bmat_param_16(b))
{
  bmat_def_16(t);
  bmat_transpose_16(t,b);
  bmat_mul_16(c,a,t);
}
#endif

#ifndef BMAT_HAS_MULT_32
void bmat_mult_32(bmat_param_32(c), bmat_param_32(a), bmat_param_32(b))
{
  bmat_def_32(t);
  bmat_transpose_32(t,b);
  bmat_mul_32(c,a,t); }
#endif

#ifndef BMAT_HAS_MULT_64
void bmat_mult_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  bmat_def_64(t);
  bmat_transpose_64(t,b);
  bmat_mul_64(c,a,t);
}
#endif


//*******************************************************************
// default expansions: square

#ifndef BMAT_HAS_SQ_8
void bmat_sq_8(bmat_param_16(c), bmat_param_16(a))
{
  bmat_mul_8(c,a,a);
}
#endif

#ifndef BMAT_HAS_SQ_16
void bmat_sq_16(bmat_param_16(c), bmat_param_16(a))
{
  bmat_mul_16(c,a,a);
}
#endif

#ifndef BMAT_HAS_SQ_32
void bmat_sq_32(bmat_param_32(c), bmat_param_32(a))
{
  bmat_mul_32(c,a,a); }
#endif

#ifndef BMAT_HAS_SQ_64
void bmat_sq_64(bmat_rparam_64(c), bmat_param_64(a))
{
  bmat_mul_64(c,a,a);
}
#endif


//*******************************************************************
// d = vM  : vector/matrix product

/**
 Long variable names and/or copious code comments doesn't seem
 an effective way to explain how these routines work. They are
 pretty much all the same for generic and AVX2 versions and are
 (like the matrix product) based on row broadcasting.

 Given a nxn matrix where we can fit 'i' rows per register and
 using a 4x4 as an example:

            M 
          |abcd|          
   |xyzw|.|efgh|
          |ijkl|
          |mnop|

      3   2    1    0        (e = element index)
0) |....|....|....|....|  r = initial result (. = 0)
1) |1...|.1..|..1.|...1|  c = first 'i' rows of I (all here) to select
2) |wxyx|wzyx|wzyx|wzyx|  v = vector V broadcast to each 'i' element
3) |ponm|lkji|hgfe|dcba|  m = first/next 'i' rows of M (all here)
4) |w...|.z..|..y.|...x|  s = v & c : non-zero if row needs selecting
5) |    |    |    |    |  s = cmp(s,0) : s[e] = s[e]!=0 ~0 : 0 (select mask)
6) |    |    |    |    |  r = r ^ (m & s) (add selected rows to running sum)

7) loop (n/i) times:
   * shift v right by 'i' for next set of rows
   * update load pointer next set of row
   * goto 3
8) complete the result by performing a horiztonal
   add (XOR) of the 'i' partial results (r)

example: V = {1,1,0,1}
1) |1...|.1..|..1.|...1|  c
2) |1.11|1.11|1.11|1.11|  v
4) |1...|....|..1.|...1|  s = v & c
5) |1111|....|1111|1111|  s = cmp(s,0)
   |ponm|....|hgfe|dcba|  m & s <- selected rows

 generic versions using integer products to perform the broadcast
 (32/64 'n' not because 2/1 rows per reg)
 */

// 5.23
uint8_t bmat_vmul_8(uint8_t v, bmat_param_8(M))
{
  uint64_t m = M[0];
  uint64_t s = 0xff*bit_scatter_lsb_8x8(v);
  uint64_t p = m & s;

  // fold the results: change to bmat_hsum
  p ^= (p >> 32);
  p ^= (p >> 16);
  p ^= (p >>  8);

  return (uint8_t)p;
}

#if defined(SWAR_AVX2_H)

// 6.87
uint16_t bmat_vmul_16(uint16_t v, bmat_param_16(M))
{
  u256_t s,r,m;

  // fill s with 16 copies of v and each is masked
  // with the bit of the corresponding row (in m).
  // would like to gt compare the result of that with
  // zero but the last row will be negative (if MSB
  // of v is set). so compare eq with zero and flip
  // the result (xor with all ones). AND that with
  // m and all that's left to do is sum up the
  // results.
  
  m = bmat_load_256(M);
  s = and_256(bmat_md_256_16.p, broadcast_16x16(v));
  s = xor_256(cmpeq_16x16(s, zero_256()), bit_allset_256());
  r = and_256(m,s);

  return bmat_hsum_16x16(r);
}

// 9.67
uint32_t bmat_vmul_32(uint32_t V, bmat_param_32(M))
{
  u256_t c = bmat_md_256_32.p;
  u256_t v = broadcast_32x8(V);
  u256_t z = zero_256();
  u256_t r = z;
  u256_t m,s;
  
  for(uint32_t i=0; i<4; i++) {
    m = bmat_load_256(M);
    s = cmpgt_32x8(and_256(v,c),z);
    r = xor_256(r, and_256(m,s));
    v = srli_32x8(v,8);
    M += 4;
  }  

  return bmat_hsum_32x8(r);
}

// 32.67
uint64_t bmat_vmul_64(uint64_t V, bmat_param_64(M))
{
  u256_t c = bmat_md_256_64.p;
  u256_t v = broadcast_64x4(V);
  u256_t z = zero_256();
  u256_t r = z;
  u256_t m,s;
  
  for(uint32_t i=0; i<16; i++) {
    m = bmat_load_256(M);
    s = cmpgt_64x4(and_256(v,c),z);
    r = xor_256(r, and_256(m,s));
    v = srli_64x4(v,4);
    M += 4;
  }  

  return bmat_hsum_64x4(r);
}

#else

// generic 64-bit hardware versions

// 10.14
uint16_t bmat_vmul_16(uint16_t v, bmat_param_16(M))
{
  uint64_t m = M[0];
  uint64_t s = 0xffff*bit_scatter_lsb_16x4(v);
  uint64_t p = m & s;

  v >>= 4;
  m   = M[1];
  s   = 0xffff*bit_scatter_lsb_16x4(v);
  p  ^= m & s;

  v >>= 4;
  m   = M[2];
  s   = 0xffff*bit_scatter_lsb_16x4(v);
  p  ^= m & s;

  v >>= 4;
  m   = M[3];
  s   = 0xffff*bit_scatter_lsb_16x4(v);
  p  ^= m & s;

  p ^= (p >> 32);
  p ^= (p >> 16);

  return (uint16_t)p;
}

// 24.12
uint32_t bmat_vmul_32(uint32_t v, bmat_param_32(M))
{
  const uint64_t mask = UINT64_C(0xffffffff);
  const uint64_t b0   = UINT64_C(1);
  const uint64_t b1   = UINT64_C(2);
  
  uint64_t m,s0,s1;
  uint64_t r0 = 0;
  uint64_t r1 = 0;
  
  for(uint32_t i=0; i<16; i++) {
    m   = M[i]; 
    s0  = (-(v & b0)) & ( mask);
    s1  = (-(v & b1)) & (~mask);
    r0 ^= s0 & m;
    r1 ^= s1 & m;
    v >>= 2;
  }

  r0 ^= r1 >> 32;
  
  return (uint32_t)r0;
}

extern uint64_t bmat_vmul_64_ref(uint64_t v, bmat_param_64(M));

uint64_t bmat_vmul_64(uint64_t v, bmat_param_64(M))
{
  return bmat_vmul_64_ref(v,M);
}

#endif


//*******************************************************************
// d = Mv matrix/vector product (these blow. quick hack)

// 6.0
uint8_t bmat_mulv_8(bmat_param_8(M), uint8_t V)
{
  uint64_t m = M[0];
  uint64_t v = broadcast_8x8(V);
  uint64_t r = m & v;

  r ^= r >> 4;
  r ^= r >> 2;
  r ^= r >> 1;

  return (uint8_t)bit_gather_lsb_8x8(r);
}


#if defined(SWAR_AVX2_H)

// 5.53
uint16_t bmat_mulv_16(bmat_param_16(M), uint16_t V)
{
  u256_t v = broadcast_16x16(V);
  u256_t m = bmat_load_256(M);

  m = and_256(m,v);
  m = bit_parity_mask_8x32(m);

  uint32_t r = movemask_8x32(m);

  r ^= (r >> 1);

  return (uint16_t)bit_gather_32(r,0x55555555);
}

// 16.00
uint32_t bmat_mulv_32(bmat_param_32(M), uint32_t V)
{
  u256_t v    = broadcast_32x8(V);
  u256_t mask = broadcast_32x8(0xffff);
  
  u256_t m[4];
  
  bmat_load_256xn(m,M,4);

  m[0] = and_256(m[0],v);
  m[1] = and_256(m[1],v);
  m[2] = and_256(m[2],v);
  m[3] = and_256(m[3],v);

  m[0] = xor_256(m[0], srli_32x8(m[0],16));
  m[1] = xor_256(m[1], srli_32x8(m[1],16));
  m[2] = xor_256(m[2], srli_32x8(m[2],16));
  m[3] = xor_256(m[3], srli_32x8(m[3],16));

  m[0] = packus_32x8(and_256(m[0],mask), and_256(m[1],mask));
  m[1] = packus_32x8(and_256(m[2],mask), and_256(m[3],mask));
  m[0] = permute_64x4(m[0], SSE_MM_SHUFFLE(3,1,2,0));
  m[1] = permute_64x4(m[1], SSE_MM_SHUFFLE(3,1,2,0));
  
  m[0] = xor_256(m[0], srli_16x16(m[0],8));
  m[1] = xor_256(m[1], srli_16x16(m[1],8));
  mask = broadcast_16x16(0xff);

  m[0] = packus_16x16(and_256(m[0],mask), and_256(m[1],mask));
  m[0] = bit_parity_mask_8x32(m[0]);
  m[0] = permute_64x4(m[0], SSE_MM_SHUFFLE(3,1,2,0));
  
  return movemask_8x32(m[0]);
}

#else

// 21.89
uint16_t bmat_mulv_16(bmat_param_16(m), uint16_t V)
{
  uint64_t v = broadcast_16x4(V);
  uint64_t r = 0;

  for(uint32_t i=0; i<4; i++) {
    uint64_t t = m[3-i] & v;

    t ^= t >> 8;
    t ^= t >> 4;
    t ^= t >> 2;
    t ^= t >> 1;

    r = bit_gather_lsb_16x4(t) | (r<<4);
  }
  return (uint16_t)r;
}

// 51.00
uint32_t bmat_mulv_32(bmat_param_32(m), uint32_t V)
{
  // only two elements per register. nothing interesting
  // to do here. could use a carryless product but if we
  // have that then I assume we have SIMD registers.
  uint64_t v = (uint64_t)V;
  uint64_t r = 0;
  
  v ^= v << 32;
  
  for(uint32_t i=0; i<16; i++) {
    uint64_t t = m[i] & v;
    
    t  = ((pop_64((uint32_t)t) & 1)|
	  ((pop_64(t>>32) & 1)<<1));
    r ^= t << (i+i);
  }
  
  return (uint32_t)r;
}

#endif


extern uint64_t bmat_mulv_64_ref(bmat_param_64(M), uint64_t v);

uint64_t bmat_mulv_64(bmat_param_64(m), uint64_t v)
{
  return bmat_mulv_64_ref(m,v);
}

