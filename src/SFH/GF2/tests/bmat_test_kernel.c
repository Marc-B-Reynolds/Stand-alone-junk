
#include <stdint.h>
#include <stdbool.h>

#include "../bmat_everything.h"
#include "bmat_test.h"

//-----------------------------------------------------------------
// runs through the 'n' vectors 'k' and checks that Mk = 0

bool test_kernel_8(bmat_param_8(m), bmat_array_8(k), uint32_t n)
{
  for (uint32_t i=0; i<n; i++) {
    uint8_t v = k[i];
    uint8_t r = bmat_mulv_8(m,v);

    if (r == 0) continue;
    printf("  got: nullity = %u, v(%u) = %02x : Mv = %02x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_kernel_16(bmat_param_16(m), bmat_array_16(k), uint32_t n)
{
  for (uint32_t i=0; i<n; i++) {
    uint16_t v = k[i];
    uint16_t r = bmat_mulv_16(m,v);

    if (r == 0) continue;
    printf("  got: nullity = %u, v(%u) = %04x : Mv = %04x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_kernel_32(bmat_param_32(m), bmat_array_32(k), uint32_t n)
{
  for (uint32_t i=0; i<n; i++) {
    uint32_t v = k[i];
    uint32_t r = bmat_mulv_32(m,v);

    if (r == 0) continue;
    printf("  got: nullity = %u, v(%u) = %08x : Mv = %08x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_kernel_64(bmat_param_64(m), bmat_array_64(k), uint32_t n)
{
  for (uint32_t i=0; i<n; i++) {
    uint64_t v = k[i];
    uint64_t r = bmat_mulv_64(m,v);

    if (r == 0) continue;
    printf(" got: nullity = %u, v(%u) = %016lx : Mv = %016lx\n", n,i,v,r);
    return false;
  }
  
  return true;
}

//-----------------------------------------------------------------
// runs through the 'n' vectors 'k' and checks that Mk = k

bool test_fixed_points_8(bmat_param_8(m), bmat_array_8(k), uint32_t n)
{

  for (uint32_t i=0; i<n; i++) {
    uint8_t v = k[i];
    uint8_t r = bmat_mulv_8(m,v);

    if (r == v) continue;
    printf("  got: nullity = %u, v(%u) = %02x : Mv = %02x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_fixed_points_16(bmat_param_16(m), bmat_array_16(k), uint32_t n)
{

  for (uint32_t i=0; i<n; i++) {
    uint16_t v = k[i];
    uint16_t r = bmat_mulv_16(m,v);

    if (r == v) continue;
    printf("  got: nullity = %u, v(%u) = %04x : Mv = %04x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_fixed_points_32(bmat_param_32(m), bmat_array_32(k), uint32_t n)
{

  for (uint32_t i=0; i<n; i++) {
    uint32_t v = k[i];
    uint32_t r = bmat_mulv_32(m,v);

    if (r == v) continue;
    printf("  got: nullity = %u, v(%u) = %08x : Mv = %08x\n", n,i,v,r);
    return false;
  }
  
  return true;
}

bool test_fixed_points_64(bmat_param_64(m), bmat_array_64(k), uint32_t n)
{

  for (uint32_t i=0; i<n; i++) {
    uint64_t v = k[i];
    uint64_t r = bmat_mulv_64(m,v);

    if (r == v) continue;
    printf("  got: nullity = %u, v(%u) = %016lx : Mv = %016lx\n", n,i,v,r);
    return false;
  }
  
  return true;
}

//-----------------------------------------------------------------

int main(void)
{
  prng_t prng;

  prng.state[0] = __rdtsc();
  prng.state[1] = UINT64_C(0x3ba0d900b9aaf028);
  prng.state[2] = UINT64_C(0xab7837b9aa423d86);
  prng_u64(&prng);

  // big enough for all
  bmat_def_64(m);

  uint32_t trials = 0x3ffff;

  {
    printf(" 8-bit: kernel\n");
    uint8_t  k[8];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_8(m, &prng);
      uint32_t n = bmat_kernel_8(m,k);
      if (test_kernel_8(m,k,n)) continue;
      if (error++ < 10)         continue;
      break;
    }
  }

  {  
    printf("16-bit: kernel\n");
    uint16_t k[16];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_16(m, &prng);
      uint32_t n = bmat_kernel_16(m,k);
      if (test_kernel_16(m,k,n)) continue;
      if (error++ < 10)          continue;
      break;
    }
  }

  {
    printf("32-bit: kernel\n");
    uint32_t k[32];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_32(m, &prng);
      uint32_t n = bmat_kernel_32(m,k);
      if (test_kernel_32(m,k,n)) continue;
      if (error++ < 10)          continue;
      break;
    }
  }

  {
    printf("64-bit: kernel\n");
    uint64_t k[64];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_64(m, &prng);
      uint32_t n = bmat_kernel_64(m,k);
      if (test_kernel_64(m,k,n)) continue;
      if (error++ < 10)          continue;
      break;
    }
  }

  //--------------------------------------------
  
  {
    printf(" 8-bit: fixed points\n");
    uint8_t  k[8];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_8(m, &prng);
      uint32_t n = bmat_fixed_points_8(m,k);
      if (test_fixed_points_8(m,k,n)) continue;
	if (error++ < 10)               continue;
	break;
    }
  }
  
  {
    printf("16-bit: fixed points\n");
    uint16_t k[16];
    uint32_t error = 0;
    
    for(uint32_t i=0; i<trials; i++) {
      bmat_random_16(m, &prng);
      uint32_t n = bmat_fixed_points_16(m,k);
      if (test_fixed_points_16(m,k,n)) continue;
      if (error++ < 10)                continue;
      break;
    }
  }
  
  {
    printf("32-bit: fixed points\n");
    uint32_t k[32];
      uint32_t error = 0;
      
      for(uint32_t i=0; i<trials; i++) {
	bmat_random_32(m, &prng);
	uint32_t n = bmat_fixed_points_32(m,k);
	if (test_fixed_points_32(m,k,n)) continue;
	if (error++ < 10)                continue;
	break;
      }
  }
  
  {
    printf("64-bit: fixed points\n");
    uint64_t k[64];
      uint32_t error = 0;
      
      for(uint32_t i=0; i<trials; i++) {
	bmat_random_64(m, &prng);
	uint32_t n = bmat_fixed_points_64(m,k);
	if (test_fixed_points_64(m,k,n)) continue;
	if (error++ < 10)                continue;
	break;
      }
  }
}



