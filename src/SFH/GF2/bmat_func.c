// Marc B. Reynolds, 2022-2025
// Public Domain under http://unlicense.org, see link for details.

#include "bmat_i.h"

///
/// Functions and stuff (make a real title)
/// ==============================================================
/// whatever


/// ## bmat_from_ufunc_*n*(m,f)
///
/// 
///
/// <details markdown="1"><summary>function list:</summary>
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
/// uint8_t  bmat_from_ufunc_8 (bmat_param_8(m), uint8_t  (*f)(uint8_t))
/// uint16_t bmat_from_ufunc_16(bmat_param_8(m), uint16_t (*f)(uint16_t))
/// uint32_t bmat_from_ufunc_32(bmat_param_8(m), uint32_t (*f)(uint32_t))
/// uint64_t bmat_from_ufunc_64(bmat_param_8(m), uint64_t (*f)(uint64_t))
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
/// </details>

uint8_t bmat_from_ufunc_8(bmat_param_8(a), uint8_t (*f)(uint8_t))
{
  uint8_t m[8];

  uint8_t t = f(0);
  uint8_t p = 1;

  for(uint32_t i=0; i<8; i++) { m[i]=0; }
    
  do {
    uint8_t r = f(p) ^ t;
    uint8_t b = 1;

    for(int j=0; j<8; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_8(a,m);
  
  return t;
}

uint16_t bmat_from_ufunc_16(bmat_param_16(a), uint16_t (*f)(uint16_t))
{
  uint16_t m[16];

  uint16_t t = f(0);
  uint16_t p = 1;

  for(uint32_t i=0; i<16; i++) { m[i]=0; }
    
  do {
    uint16_t r = f(p) ^ t;
    uint16_t b = 1;

    for(int j=0; j<16; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_16(a,m);
  
  return t;
}



// assumes 'f' can represented by a 32x32 matrix M:
//   y=f(x) -> y = Mx
// or y = Mx+t
//   where 't' is the returned result
uint32_t bmat_from_ufunc_32(bmat_param_32(a), uint32_t (*f)(uint32_t))
{
  uint32_t m[32];

  // an input of zero should yield a zero result. If not
  // work on the assumption that it's 'f' is actually
  // affine.
  uint32_t t = f(0);
  uint32_t p = 1;

  for(uint32_t i=0; i<32; i++) { m[i]=0; }
    
  // build up matrix using single bit inputs
  do {
    uint32_t r = f(p) ^ t;
    uint32_t b = 1;

    for(int j=0; j<32; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_32(a,m);
  
  return t;
}

// allow the logical unary function 'f' be a f(x,k) for input constant 'k'
uint32_t bmat_from_ufunc_p_32(bmat_param_32(a), uint32_t (*f)(uint32_t,uint32_t), uint32_t k)
{
  uint32_t m[32];

  uint32_t t = f(0,k);
  uint32_t p = 1;

  for(uint32_t i=0; i<32; i++) { m[i]=0; }
    
  do {
    uint32_t r = f(p,k) ^ t;
    uint32_t b = 1;

    for(int j=0; j<32; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_32(a,m);

  return t;
}


uint64_t bmat_from_ufunc_64(bmat_param_64(a), uint64_t (*f)(uint64_t))
{
  uint64_t m[64];

  uint64_t t = f(0);
  uint64_t p = 1;

  for(uint32_t i=0; i<64; i++) { m[i]=0; }

  do {
    uint64_t r = f(p) ^ t;
    uint64_t b = 1;

    for(int j=0; j<64; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_64(a,m);
  
  return t;
}

uint64_t bmat_from_ufunc_p_64(bmat_param_64(a), uint64_t (*f)(uint64_t,uint64_t), uint64_t k)
{
  uint64_t m[64];

  uint64_t t = f(0,k);
  uint64_t p = 1;

  for(uint32_t i=0; i<64; i++) { m[i]=0; }

  do {
    uint64_t r = f(p,k) ^ t;
    uint64_t b = 1;

    for(int j=0; j<64; j++) {
      m[j]  ^= ((r & b) != 0) ? p : 0;
      b    <<= 1;
    }
    
    p <<= 1;
  } while(p);

  array_to_bmat_64(a,m);

  return t;
}



// ## bmat_ufunc_validate_*n*(m,f)
//
// 
//
// <details markdown="1"><summary>function list:</summary>
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ c
// uint32_t bmat_ufunc_validate_32(
// uint64_t bmat_ufunc_validate_64(
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
// </details>


//************************************************
// add beefed up vesion that (statistically) catch invalid functions

// assuming f0 & f1 are both solely bitwise functions then will return
// zero if they return identical results for all inputs. Specifically
// they must both be representable by Mx+t where M is a 32x32 binary matrix.
uint32_t f2_ufunc_validate_32_i(uint32_t (*f0)(uint32_t), uint32_t (*f1)(uint32_t))
{
  uint32_t r = f0(0) ^ f1(0);
  uint32_t x = 1;
  
  do { r  |= f0(x) ^ f1(x); x <<= 1; } while(x);
  
  return r;
}

uint32_t f2_ufunc_p_validate_32_i(uint32_t (*f0)(uint32_t), uint32_t (*f1)(uint32_t,uint32_t), uint32_t p)
{
  uint32_t r = f0(0) ^ f1(0,p);
  uint32_t x = 1;
  
  do { r  |= f0(x) ^ f1(x,p); x <<= 1; } while(x);
  
  return r;
}

uint32_t f2_ufunc_p2_validate_32_i(uint32_t (*f0)(uint32_t,uint32_t), uint32_t (*f1)(uint32_t,uint32_t), uint32_t p)
{
  uint32_t r = f0(0,p) ^ f1(0,p);
  uint32_t x = 1;
  
  do { r  |= f0(x,p) ^ f1(x,p); x <<= 1; } while(x);
  
  return r;
}

uint64_t f2_ufunc_validate_64_i(uint64_t (*f0)(uint64_t), uint64_t (*f1)(uint64_t))
{
  uint64_t r = f0(0) ^ f1(0);
  uint64_t x = 1;
  
  do { r  |= f0(x) ^ f1(x); x <<= 1; } while(x);
  
  return r;
}

uint64_t f2_ufunc_p2_validate_64_i(uint64_t (*f0)(uint64_t,uint64_t), uint64_t (*f1)(uint64_t,uint64_t), uint64_t p)
{
  uint64_t r = f0(0,p) ^ f1(0,p);
  uint64_t x = 1;
  
  do { r  |= f0(x,p) ^ f1(x,p); x <<= 1; } while(x);
  
  return r;
}
