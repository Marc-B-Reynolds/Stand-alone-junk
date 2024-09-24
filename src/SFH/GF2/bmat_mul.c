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



//*******************************************************************
/// ----------
///
/// ## bmat_mul_*n*(c,a,b)
///
/// Computes $ C = AB $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_mul_8 (bmat_rparam_8 (c), bmat_param_8(a),  bmat_param_8 (b))
/// void bmat_mul_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
/// void bmat_mul_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
/// void bmat_mul_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


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




//*******************************************************************
// 16-bit


#if defined(SWAR_AVX2_H)

// AVX2 or temp hack usage via SIMDe

static inline u256_t bmat_mul_step_16(u256_t a, u256_t b, u256_t cm, u256_t z)
{
  return and_256(broadcast_lo_16x16(b), cmpgt_16x16(and_256(a,cm),z));
}

// 35.20
void bmat_mul_16(bmat_rparam_16(C), bmat_param_16(A), bmat_param_16(B))
{
  u256_t a  = bmat_load_256(A);
  u256_t cm = broadcast_16x16(0x8000);
  u256_t z  = zero_256();
  u256_t c  = z;

  // should be: do nothing, goes nowhere (strict aliasing compat)
  uint16_t b[16];
  bmat_to_array_16(b,B);

  // flip bits of A so we can compare against eq to zero for broadcast
  a = xor_256(a,bit_allset_256());

  // work backwards so we can use add instead of shift (take pressure
  // off of port 5)
  for(int i=15; i>=0; i--) {
    c = xor_256(and_256(broadcast_16x16(b[i]), cmpeq_16x16(and_256(a,cm),z)),c);
    a = add_16x16(a,a);
  }

  bmat_store_256(C,c);
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

//*******************************************************************
// 32-bit

#if defined(SWAR_AVX2_H)

#define BMAT_HAS_MUL_32

void bmat_mul_32_(bmat_rparam_32(C), bmat_param_32(A), bmat_param_32(B))
{
  u256_t cm = broadcast_32x8(0x80000000);
  u256_t z  = zero_256();
  u256_t r;

  u256_t c[4];
  u256_t a[4];
  
  uint32_t b[32];
  bmat_to_array_32(b,B);
  bmat_load_256xn(a,A,4);

  // broadcast the first row
  r = broadcast_32x8(b[31]);

  for(uint32_t j=0; j<4; j++) {
    // complement 'a' so we're checking for zeroes
    // instead of ones. 
    a[j] = xor_256(a[j], bit_allset_256());
    c[j] = and_256(r, cmpeq_32x8(and_256(a[j],cm),z));
    a[j] = add_32x8(a[j],a[j]);
  }
  
  // broadcast the remaining rows
  for(int i=30; i>=0; i--) {
    r = broadcast_32x8(b[i]);
    
    for(uint32_t j=0; j<4; j++) {
      c[j] = xor_256(and_256(r, cmpeq_32x8(and_256(a[j],cm),z)),c[j]);
      a[j] = add_32x8(a[j],a[j]);
    }
  }
  
  bmat_store_256xn(C,c,4);
}


// meh.
static inline void bmat_mul_32_step_avx2(uint32_t i, u256_t r, u256_t* c, u256_t* a, u256_t cm)
{
  u256_t t = loadu_256(a+i);
  u256_t z = zero_256();

  t = and_256(r, cmpeq_32x8(and_256(t,cm),z));
  
  storeu_256(c+i, t);
}    

void bmat_mul_32(bmat_rparam_32(C), bmat_param_32(A), bmat_param_32(B))
{
#if 0
  u256_t cm = broadcast_32x8(0x80000000);
  u256_t z  = zero_256();
  u256_t r;
  u256_t c[4];


#if 1
  u256_t a[4];
  bmat_load_256xn(a,A,4);
#else
  u256_t* a = (u256_t*)A;
#endif  
  
  uint32_t b[32];
  bmat_to_array_32(b,B);

  // broadcast the last row
  r = broadcast_32x8(b[31]);

  for(uint32_t j=0; j<4; j++) {
    // complement 'a' so we're checking for zeroes
    // instead of ones.
    a[j] = xor_256(a[j], bit_allset_256());
    c[j] = and_256(r, cmpeq_32x8(and_256(a[j],cm),z));
    a[j] = add_32x8(a[j],a[j]);
  }
  
  // broadcast the remaining rows
  for(int i=30; i>=0; i--) {
    r = broadcast_32x8(b[i]);
    
    for(uint32_t j=0; j<4; j++) {
      c[j] = xor_256(and_256(r, cmpeq_32x8(and_256(a[j],cm),z)),c[j]);
      a[j] = add_32x8(a[j],a[j]);
    }
  }

  bmat_store_256xn(C,c,4);

#else
  u256_t cm = broadcast_32x8(0x80000000);
  u256_t z  = zero_256();
  u256_t r;
  u256_t c0,c1,c2,c3;
  u256_t a0,a1,a2,a3;

  uint32_t b[32];
  bmat_to_array_32(b,B);

  a0 = bmat_load_256(A   );
  a1 = bmat_load_256(A+ 4);
  a2 = bmat_load_256(A+ 8);
  a3 = bmat_load_256(A+12);
  
  // broadcast the last row
  r = broadcast_32x8(b[31]);

  // complement 'a' so we're checking for zeroes
  // instead of ones.
  a0 = xor_256(a0, bit_allset_256());
  c0 = and_256(r, cmpeq_32x8(and_256(a0,cm),z));
  a0 = add_32x8(a0,a0);

  a1 = xor_256(a1, bit_allset_256());
  c1 = and_256(r, cmpeq_32x8(and_256(a1,cm),z));
  a1 = add_32x8(a1,a1);

  a2 = xor_256(a2, bit_allset_256());
  c2 = and_256(r, cmpeq_32x8(and_256(a2,cm),z));
  a2 = add_32x8(a2,a2);

  a3 = xor_256(a3, bit_allset_256());
  c3 = and_256(r, cmpeq_32x8(and_256(a3,cm),z));
  a3 = add_32x8(a3,a3);
  
  // broadcast the remaining rows
  for(int i=30; i>=0; i--) {
    r  = broadcast_32x8(b[i]);

    c0 = xor_256(and_256(r, cmpeq_32x8(and_256(a0,cm),z)),c0);
    a0 = add_32x8(a0,a0);

    c1 = xor_256(and_256(r, cmpeq_32x8(and_256(a1,cm),z)),c1);
    a1 = add_32x8(a1,a1);

    c2 = xor_256(and_256(r, cmpeq_32x8(and_256(a2,cm),z)),c2);
    a2 = add_32x8(a2,a2);

    c3 = xor_256(and_256(r, cmpeq_32x8(and_256(a3,cm),z)),c3);
    a3 = add_32x8(a3,a3);
  }

  bmat_storex_256x4(C,c0,c1,c2,c3);
#endif  
}


#else
// not motived. with only two rows/register doesn't seem
// worth the effort ATM.
#endif

//*******************************************************************
// 64-bit

#if defined(SWAR_AVX2_H)

// compiling like garbage? super slow. using matrix/vector fallback
// is faster. need to examine. strat seems reasonable. humm...
// bad timing? check for line tearing. clean up defs for this.

void bmat_mul_64_avx2(bmat_rparam_64(C), bmat_param_64(A), bmat_param_64(B))
{
  u256_t cm = broadcast_64x4(0x8000000000000000);
  u256_t z  = zero_256();
  u256_t r;

  u256_t c[16];
  u256_t a[16];
  
  uint64_t b[64];
  bmat_to_array_64(b,B);
  bmat_load_256xn(a,A,16);

  r = broadcast_64x4(b[63]);
  
  for(uint32_t j=0; j<16; j++) {
    a[j] = xor_256(a[j], bit_allset_256());
    c[j] = and_256(r, cmpeq_64x4(and_256(a[j],cm),z));
    a[j] = add_64x4(a[j],a[j]);
  }
      
  for(int i=62; i>=0; i--) {
    r = broadcast_64x4(b[i]);
    
    for(uint32_t j=0; j<16; j++) {
      c[j] = xor_256(and_256(r, cmpeq_64x4(and_256(a[j],cm),z)),c[j]);
      a[j] = add_64x4(a[j],a[j]);
    }
  }
  
  bmat_store_256xn(C,c,16);
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

// one element per register: use reference version
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

  // there are multiple possible strategies so this
  // could quite possible be improved. below the
  // permutes (*) are greedy reordering to handle
  // the pack ops working in each 128-bit lane. there's
  // probably a better one time (instead of 3)
  // reordering. A different method of attack would
  // be to peform one xorshift by 4, AND mask the
  // nibble and pshufb. This gives all of the byte
  // info step as single bit per byte. This would
  // replace the first 4 xorshifts below. Couldn't
  // come up with a faster overall colapse from there.
  // Seems promising but brain fail.
  
  m[0] = xor_256(m[0], srli_32x8(m[0],16));
  m[1] = xor_256(m[1], srli_32x8(m[1],16));
  m[2] = xor_256(m[2], srli_32x8(m[2],16));
  m[3] = xor_256(m[3], srli_32x8(m[3],16));

  m[0] = packus_32x8(and_256(m[0],mask), and_256(m[1],mask));
  m[1] = packus_32x8(and_256(m[2],mask), and_256(m[3],mask));
  m[0] = permute_64x4(m[0], SSE_MM_SHUFFLE(3,1,2,0));  // *
  m[1] = permute_64x4(m[1], SSE_MM_SHUFFLE(3,1,2,0));  // *
  
  m[0] = xor_256(m[0], srli_16x16(m[0],8));
  m[1] = xor_256(m[1], srli_16x16(m[1],8));
  mask = broadcast_16x16(0xff);

  m[0] = packus_16x16(and_256(m[0],mask), and_256(m[1],mask));
  m[0] = bit_parity_mask_8x32(m[0]);
  m[0] = permute_64x4(m[0], SSE_MM_SHUFFLE(3,1,2,0));  // *
  
  return movemask_8x32(m[0]);
}

// 69.50
uint64_t bmat_mulv_64(bmat_param_64(M), uint64_t V)
{
  u256_t v = broadcast_64x4(V);
  u256_t c = broadcast_8x32(1);
  u256_t z = zero_256();
  u256_t m[16];
  
  bmat_load_256xn(m,M,16);

  // The 64-bit version does start by performing a PSHUFB
  // byte parity computation (see 32-bit notes). This fits
  // nicely with the next step of using SAD to sum them up.
  // After that I lacked motionation/inspiration and just
  // kicked the bits into place with almost certainly too
  // much work. Also (like a ton of the code here) it's
  // putting WAY too much pressure on port-5. 
  // 1) after the SAD each register is mostly empty.
  //    the bottom of each 64-bit element has 5 bits
  //    that may not be zero and only the low bit needs
  //    to be preserved.
  
  for(uint32_t i=0; i<16; i+=2) {
    m[i   ] = sad_8x32(bit_parity_8x32(and_256(m[i  ],v)),z);
    m[i+1 ] = sad_8x32(bit_parity_8x32(and_256(m[i+1],v)),z);

    // start of not-great
    m[i>>1] = packus_16x16(m[i],m[i+1]);
    m[i>>1] = permute_64x4(m[i>>1], SSE_MM_SHUFFLE(3,1,2,0));
  }

  // continued: not-great
  for(uint32_t i=0; i<8; i+=2) {
    m[i>>1] = packus_16x16(m[i],m[i+1]);
    m[i>>1] = permute_64x4(m[i>>1], SSE_MM_SHUFFLE(3,1,2,0));
  }

  for(uint32_t i=0; i<4; i+=2) {
    m[i>>1] = packus_16x16(m[i],m[i+1]);
    m[i>>1] = permute_64x4(m[i>>1], SSE_MM_SHUFFLE(3,1,2,0));
  }
  
  uint32_t t0 = movemask_8x32(negate_8x32(and_256(m[ 0],c)));
  uint32_t t1 = movemask_8x32(negate_8x32(and_256(m[ 1],c)));
  
  return ((uint64_t)t1<<32)|t0;
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

// one element per register: use reference version
extern uint64_t bmat_mulv_64_ref(bmat_param_64(M), uint64_t v);

uint64_t bmat_mulv_64(bmat_param_64(m), uint64_t v)
{
  return bmat_mulv_64_ref(m,v);
}
#endif



//*******************************************************************
// default expansions: AB
//

#ifndef BMAT_HAS_MUL_32
#ifndef BMAT_MUL_DEF_REF_32

void bmat_mul_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
{
  uint32_t m[32];
  uint32_t r[32];

  bmat_to_array_32(m,a);

  for(uint32_t i=0; i<32; i++)
    r[i] = bmat_vmul_32(m[i],b);

  array_to_bmat_32(c,r);
}

#else
extern void bmat_mul_32_ref(bmat_param_32(C), bmat_param_32(MA), bmat_param_32(MB));
void bmat_mul_32(bmat_param_32(c), bmat_param_32(a), bmat_param_32(b)) { bmat_mul_32_ref(c,a,b); }
#endif
#endif


#ifndef BMAT_HAS_MUL_64
#ifndef BMAT_MUL_DEF_REF_64

void bmat_mul_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  uint64_t m[64];
  uint64_t r[64];

  bmat_to_array_64(m,a);

  for(uint32_t i=0; i<64; i++)
    r[i] = bmat_vmul_64(m[i],b);

  array_to_bmat_64(c,r);
}

#else

extern void bmat_mul_64_ref(bmat_param_64(c), bmat_param_64(a), bmat_param_64(b));

void bmat_mul_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  bmat_mul_64_ref(c,a,b);
}
#endif
#endif




//*******************************************************************
/// ----------
///
/// ## bmat_mult_*n*(c,a,b)
///
/// Computes $ C = AB^T $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_mult_8 (bmat_rparam_8 (c), bmat_param_8(a),  bmat_param_8 (b))
/// void bmat_mult_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
/// void bmat_mult_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
/// void bmat_mult_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>


// default expansions: AB^T
//
// assuming porting then building a specialized vector/matrix products
// should probably happen before jumping into full matrix product


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


// for a default 16-bit using the transpose could be reasonable
#ifndef BMAT_HAS_MULT_16
#ifndef BMAT_MULT_TRANSPOSE_16
void bmat_mult_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
{
  uint16_t m[16];
  uint16_t r[16];

  bmat_to_array_16(m,a);

  for(uint32_t i=0; i<16; i++)
    r[i] = bmat_mulv_16(b,m[i]);

  array_to_bmat_16(c,r);
}
#else
void bmat_mult_16(bmat_param_16(c), bmat_param_16(a), bmat_param_16(b))
{
  bmat_def_16(t);
  bmat_transpose_16(t,b);
  bmat_mul_16(c,a,t);
}
#endif
#endif

#ifndef BMAT_HAS_MULT_32
void bmat_mult_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
{
  uint32_t m[32];
  uint32_t r[32];

  bmat_to_array_32(m,a);

  for(uint32_t i=0; i<32; i++)
    r[i] = bmat_mulv_32(b,m[i]);

  array_to_bmat_32(c,r);
}
#endif

#ifndef BMAT_HAS_MULT_64
void bmat_mult_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  uint64_t m[64];
  uint64_t r[64];

  bmat_to_array_64(m,a);

  for(uint32_t i=0; i<64; i++)
    r[i] = bmat_mulv_64(b,m[i]);

  array_to_bmat_64(c,r);
}
#endif


//*******************************************************************
/// ----------
///
/// ## bmat_sq_*n*(c,a)
///
/// Computes $ C = A^2 $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_sq_8 (bmat_rparam_8 (c), bmat_param_8 (a))
/// void bmat_sq_16(bmat_rparam_16(c), bmat_param_16(a))
/// void bmat_sq_32(bmat_rparam_32(c), bmat_param_32(a))
/// void bmat_sq_64(bmat_rparam_64(c), bmat_param_64(a))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

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
// 8x8 "standard" functions wrap inner routines that work directly
// on 64-bit integers.

void bmat_mul_8(bmat_rparam_8(c), bmat_param_8(a), bmat_param_8(b))
{
  c[0] = bmat_mul_8_i(a[0],b[0]);
}

void bmat_mult_8(bmat_rparam_8(c), bmat_param_8(a), bmat_param_8(b))
{
  c[0] = bmat_mult_8_i(a[0],b[0]);
}



//*******************************************************************
/// ----------
///
/// ## bmat_madd_*n*(c,a,b)
///
/// Computes $ D = AB+C $
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_madd_8 (bmat_rparam_8 (d), bmat_param_8(a),  bmat_param_8 (b), bmat_param_8 (c))
/// void bmat_madd_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b), bmat_param_16(c))
/// void bmat_madd_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b), bmat_param_32(c))
/// void bmat_madd_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b), bmat_param_64(c))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

// D = AB+C

void bmat_madd_8(bmat_rparam_8(d), bmat_param_8(a), bmat_param_8(b), bmat_param_8(c))
{
  bmat_mul_8(d,a,b);
  bmat_sum_8(d,c);
}

void bmat_madd_16(bmat_rparam_16(d), bmat_param_16(a), bmat_param_16(b), bmat_param_16(c))
{
  bmat_mul_16(d,a,b);
  bmat_sum_16(d,c);
}

void bmat_madd_32(bmat_rparam_32(d), bmat_param_32(a), bmat_param_32(b), bmat_param_32(c))
{
  bmat_mul_32(d,a,b);
  bmat_sum_32(d,c);
}

void bmat_madd_64(bmat_rparam_64(d), bmat_param_64(a), bmat_param_64(b), bmat_param_64(c))
{
  bmat_mul_64(d,a,b);
  bmat_sum_64(d,c);
}


//*******************************************************************
/// ----------
///
/// ## bmat_bracket_*n*(c,a,b)
///
/// Computes $ C = \left[ A,B \right] = AB+BA $
///
/// Calling this the [commutator](https://en.wikipedia.org/wiki/Commutator#Ring_theory) seems a bit akward since the *anticommutator* is the same operation. It makes sense since $M=-M$ and $A+B=A-B$ but still.
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// void bmat_bracket_8 (bmat_rparam_8 (c), bmat_param_8(a),  bmat_param_8 (b))
/// void bmat_bracket_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
/// void bmat_bracket_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
/// void bmat_bracket_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

void bmat_bracket_8(bmat_rparam_8(c), bmat_param_8(a), bmat_param_8(b))
{
  bmat_def_8(t);
  bmat_mul_8(t,a,b);
  bmat_madd_8(c,b,a,t);
}

void bmat_bracket_16(bmat_rparam_16(c), bmat_param_16(a), bmat_param_16(b))
{
  bmat_def_16(t);
  bmat_mul_16(t,a,b);
  bmat_madd_16(c,b,a,t);
}

void bmat_bracket_32(bmat_rparam_32(c), bmat_param_32(a), bmat_param_32(b))
{
  bmat_def_32(t);
  bmat_mul_32(t,a,b);
  bmat_madd_32(c,b,a,t);
}

void bmat_bracket_64(bmat_rparam_64(c), bmat_param_64(a), bmat_param_64(b))
{
  bmat_def_64(t);
  bmat_mul_64(t,a,b);
  bmat_madd_64(c,b,a,t);
}


