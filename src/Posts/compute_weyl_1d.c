// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// binary32 approximation of (sqrt(5)-1)/2. Choose
// the rounding that insures the final bit is set.
// RD( (sqrt(5)-1)/2  ) =  10368889*2^-24
static const float    alpha = 0x1.3c6ef2p-1f;
static const uint32_t K24   = 10368889u << (32-24);

// binary32: ensure rounded to even (23-bits)
static const float    alpha23 = 0x1.3c6ef4p-1f; // 5184445 * 2^-23
static const uint32_t K23     = 5184445u << (32-23);

// 32-bit version
//static const uint32_t A = 2654435769;

inline uint32_t uabs(uint32_t a) { return (int32_t)a >=0 ? a : -a; }

// binary32: (i*alpha) when alpha is rounded to odd
void test_closed_form()
{
  float    f = 0;
  uint32_t n = 0, m=1;
  uint32_t i,t,r;

  printf("closed form:\n");
  do {
    f = fmodf((float)n*alpha, 1.f);
    i = K24*n;
    t = (uint32_t)(f*0x1p32);
    r = uabs(i-t) >> 8;

    if (r >= m) {
      float e = (float)i * 0x1p-32f;
      printf("%9u %9f : %f %f \n", n, log2f((float)n), e, f);
      m <<= 1;
    }
    n++;
  } while(m < 0x01000000);
}

// binary32: (si+alpha) when alpha is rounded to odd
void test_recurrent_form()
{
  float    f = 0.f;
  uint32_t i = 0;
  uint32_t n = 0, m=1;
  uint32_t t,r;

  printf("\nadditive method:\n");
  do {
    f = f + alpha; if (f >= 1.f) f -= 1.f;
    i = i + K24;
    t = (uint32_t)(f*0x1p32);
    r = uabs(i-t) >> 8;
    
    if (r >= m) {
      float e = (float)i * 0x1p-32f;
      printf("%9u %9f : %f %f \n", n, log2f((float)n), e, f);
      m <<= 1;
    }
    
    n++;
  } while(m < 0x01000000);
}


// binary32: ensure alpha is even
void test_23()
{
  float    f = 0.f;
  uint32_t n = 0, m=1;
  uint32_t i = 0, t,r;

  printf("\nvalidate round-to-even method:\n");
  do {
    f = f + alpha23; if (f >= 1.f) f -= 1.f;
    i = i + K23;
    t = (uint32_t)(f*0x1p32);
    r = uabs(i-t) >> 8;
    if (r >= m) { printf("%8u : %f %f : %f\n", n, (float)i * 0x1p-32f, f, log2f((float)n)); return; }
    n++;
  } while(i != 0);

  // period is 2^23: n = 2^23 and f=0 at this point since we've walked
  // the full period.
  if (f == 0.0f && n == 0x800000)
    printf("  success\n");
  else
    printf("  you broke something: f=%a n=%x", f, n);
}

int main(void)
{
  test_closed_form();
  test_recurrent_form();
  test_23();
}
