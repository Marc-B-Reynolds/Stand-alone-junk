// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_acospif) version:
// ************************************
//
// The CORE-MATH routine fall under:
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


// compile with (or equiv).
// clang -O3 -march=native -Wall -Wextra -Wconversion -Wpedantic -Wno-unused-function -fno-math-errno -ffp-contract=off acospif.c -o acospif -lm

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "util.h"

#if defined(__GNUC__)
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif



//**********************************************************************

// could do fma version (pair) of product by 1/pi instead. meh.
float libm(float x) { return acosf(x)/((float)M_PI); }


#include "internal/f32_acospi_sb.h"



//******************************************************************************

float f32_acospi_a0p(float x)
{
  return 0.5f * sqrtf(1.f-x);
}


// expand variants restricted to positive input: f(0) result relaxed
float f32_acospi_a1p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p1, x); }
float f32_acospi_a2p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p2, x); }
float f32_acospi_a3p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p3, x); }
float f32_acospi_a4p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p4, x); }
float f32_acospi_a5p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p5, x); }
float f32_acospi_a6p(float x) { return f32_acospi_sb_pp(&f32_acospi_sb_p6, x); }


// fake polynomial for initial version
static inline float f32_acospi_sbe_p0(UNUSED float x) { return 0.5f; }

// expand full range variants: f(0) result relaxed
float f32_acospi_r1(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p1, x); }
float f32_acospi_r2(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p2, x); }
float f32_acospi_r3(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p3, x); }
float f32_acospi_r4(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p4, x); }
float f32_acospi_r5(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p5, x); }
float f32_acospi_r6(float x) { return f32_acospi_sb_p(&f32_acospi_sb_p6, x); }

// expand full range variants: f(0) result exact
float f32_acospi_a0(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p0, x); }
float f32_acospi_a1(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p1, x); }
float f32_acospi_a2(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p2, x); }
float f32_acospi_a3(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p3, x); }
float f32_acospi_a4(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p4, x); }
float f32_acospi_a5(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p5, x); }
float f32_acospi_a6(float x) { return f32_acospi_sb_p(&f32_acospi_sbe_p6, x); }


//**********************************************************************
// SEE: https://core-math.gitlabpages.inria.fr
// and license info at top of file.

// oh my!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wimplicit-float-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include <stdint.h>
//#include <errno.h>
#include <fenv.h>

typedef union {float f; uint32_t u;} b32u32_u;

float cr_acospif(float x){
  float ax = __builtin_fabsf(x);
  double az = ax, z = x;
  b32u32_u t = {.f = x};
  int e = (t.u>>23)&0xff;
  if(__builtin_expect(e>=127, 0)){
    if(x == 1.0f) return 0.0f;
    if(x ==-1.0f) return 1.0f;
    if(e==0xff && (t.u<<9)) return x; // nan
    //errno = EDOM;
    feraiseexcept(FE_INVALID);
    return __builtin_nanf("1");
  }
  int s = 146 - e, i = 0;
  if(__builtin_expect(s<32, 1)) i = ((t.u&(~0u>>9))|1<<23)>>s;
  static const double ch[][8] = {
    {0x1.45f306dc9c882p-2, 0x1.b2995e7b7dc2fp-5, 0x1.8723a1cf50c7ep-6, 0x1.d1a4591d16a29p-7,
     0x1.3ce3aa68ddaeep-7, 0x1.d3182ab0cc1bfp-8, 0x1.62b379a8b88e3p-8, 0x1.6811411fcfec2p-8},
    {0x1.ffffffffd3cd9p-2, -0x1.17cc1b3355fd5p-4, 0x1.d067a1e8d5a99p-6, -0x1.08e16fb09314ap-6,
    0x1.5eed43d42dcb2p-7, -0x1.f58baca7acc71p-8, 0x1.5dab64e2dcf15p-8, -0x1.59270e30797acp-9},
    {0x1.fffffff7c4622p-2, -0x1.17cc149ded3a2p-4, 0x1.d0654d4cb2c1ap-6, -0x1.08c3ba713d33ap-6,
    0x1.5d2053481079cp-7, -0x1.e485ebc545e7ep-8, 0x1.303baca167dddp-8, -0x1.dee8d16d06b38p-10},
    {0x1.ffffffa749848p-2, -0x1.17cbe7155935p-4, 0x1.d05a312269adfp-6, -0x1.0862b3ee617d7p-6,
    0x1.5920708db2a73p-7, -0x1.cb0463b3862c3p-8, 0x1.02b82478f95d7p-8, -0x1.52a7b8579e729p-10},
    {0x1.fffffe1f92bb5p-2, -0x1.17cb3e74c64e3p-4, 0x1.d03af67311cbfp-6, -0x1.079441cbfc7ap-6,
    0x1.52b4287805a61p-7, -0x1.ac3286d604a98p-8, 0x1.b2f1210d9701bp-9, -0x1.e740ddc25afd6p-11},
    {0x1.fffff92beb6e2p-2, -0x1.17c986fe9518bp-4, 0x1.cff98167c9a5ep-6, -0x1.0638b591eae52p-6,
    0x1.4a0803828959ep-7, -0x1.8adeca229f11dp-8, 0x1.6b9a7ba05dfcep-9, -0x1.640521a43b2dp-11},
    {0x1.ffffeccee5bfcp-2, -0x1.17c5f1753f5eap-4, 0x1.cf874e4fe258fp-6, -0x1.043e6cf77b256p-6,
    0x1.3f7db42227d92p-7, -0x1.691a6fa2a2882p-8, 0x1.2f6543162bc61p-9, -0x1.07d5da05822b6p-11},
    {0x1.ffffd2f64431dp-2, -0x1.17bf8208c10c1p-4, 0x1.ced7487cdb124p-6, -0x1.01a0d30932905p-6,
    0x1.3388f99b254dap-7, -0x1.4844e245c65bdp-8, 0x1.fa777150197c6p-10, -0x1.8c1ecf16a05c8p-12},
    {0x1.ffffa36d1712ep-2, -0x1.17b523971bd4ep-4, 0x1.cddee26de2deep-6, -0x1.fccb00abaaabcp-7,
    0x1.269afc3622342p-7, -0x1.2933152686752p-8, 0x1.a76d4956cc9a3p-10, -0x1.2ce7d6dc651cep-12},
    {0x1.ffff5402ab3a1p-2, -0x1.17a5ba85da77ap-4, 0x1.cc96894e05c02p-6, -0x1.f532143cb832ep-7,
    0x1.19180b660ff09p-7, -0x1.0c57417a78b3cp-8, 0x1.62e26cbd7bb1ep-10, -0x1.ce28d33fe1df3p-13},
    {0x1.fffed8d639751p-2, -0x1.1790349f3ae76p-4, 0x1.caf9a4fd1b398p-6, -0x1.ec986b111342ep-7,
    0x1.0b53c3ad4baa4p-7, -0x1.e3c2282eeace4p-9, 0x1.2a55369f55bbep-10, -0x1.667fe48c396e8p-13},
    {0x1.fffe24b714161p-2, -0x1.177394fbcb719p-4, 0x1.c90652d920ebdp-6, -0x1.e3239197bddf1p-7,
    0x1.fb2188525b025p-8, -0x1.b3aadd451afc7p-9, 0x1.f74020f31fdabp-11, -0x1.18b0cb246768dp-13},
    {0x1.fffd298bec9e2p-2, -0x1.174efbfd34648p-4, 0x1.c6bcfe48ea92bp-6, -0x1.d8f9f2a16157cp-7,
    0x1.e0044f56c8864p-8, -0x1.883e2347fe76cp-9, 0x1.a9f0e3c1b7af5p-11, -0x1.bb5acc0e60825p-14},
    {0x1.fffbd8b784c4dp-2, -0x1.1721abdd3722ep-4, 0x1.c41fee756d4bp-6, -0x1.ce40bccf8065fp-7,
    0x1.c59b684b70ef9p-8, -0x1.6133d027996b3p-9, 0x1.69cad01106397p-11, -0x1.60f8e45494156p-14},
    {0x1.fffa23749cf88p-2, -0x1.16eb0a8285c06p-4, 0x1.c132d762e1b0dp-6, -0x1.c31a959398f4ep-7,
    0x1.ac1c5b46bc8ap-8, -0x1.3e34f1abe51dcp-9, 0x1.346738737c0b9p-11, -0x1.1b227a3f5c75p-14},
    {0x1.fff7fb25bb407p-2, -0x1.16aaa14d7564p-4, 0x1.bdfa75fca5ff2p-6, -0x1.b7a6e260d079cp-7,
    0x1.93ab06911033cp-8, -0x1.1ee5560967fd5p-9, 0x1.07d31060838bfp-11, -0x1.c96f33a283115p-15},
  };
  const double *c = ch[i];
  double z2 = z*z, z4 = z2*z2;
  if(__builtin_expect(!i, 0)){
    double c0 = c[0] + z2*c[1];
    double c2 = c[2] + z2*c[3];
    double c4 = c[4] + z2*c[5];
    double c6 = c[6] + z2*c[7];
    c0 += c2*z4;
    c4 += c6*z4;
    c0 += c4*(z4*z4);
    return 0.5 - z*c0;
  } else {
    double f = __builtin_sqrt(1-az);
    double c0 = c[0] + az*c[1];
    double c2 = c[2] + az*c[3];
    double c4 = c[4] + az*c[5];
    double c6 = c[6] + az*c[7];
    c0 += c2*z2;
    c4 += c6*z2;
    c0 += c4*z4;
    static const double o[] = {0,1};
    double r = o[t.u>>31] + c0*__builtin_copysign(f, x);
    return r;
  }
}

#pragma GCC diagnostic pop

//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
#if 1
  ENTRY(f32_acospi_a0),
  ENTRY(f32_acospi_a1),
  ENTRY(f32_acospi_a2),
  ENTRY(f32_acospi_a3),
  ENTRY(f32_acospi_a4),
  ENTRY(f32_acospi_a5),
  ENTRY(f32_acospi_a6),
#endif
  
#if 1
  ENTRY(f32_acospi_r1),
  ENTRY(f32_acospi_r2),
  ENTRY(f32_acospi_r3),
  ENTRY(f32_acospi_r4),
  ENTRY(f32_acospi_r5),
  ENTRY(f32_acospi_r6),
#endif  
};

const char* func_name = "acospi";

float cr_func(float x) { return cr_acospif(x); }

#include "common.h"

//********************************************************


// acospi(x) = 1/2 on [-0x1.921fb4p-24, 0x1.921fb4p-25]
void brute_all()
{
  static const uint32_t xa = 0x33490fda;  // f32_to_bits( 0x1.921fb4p-25f);
  static const uint32_t xb = 0xb3c90fda;  // f32_to_bits(-0x1.921fb4p-24f);

  uint32_t x1 = f32_to_bits(1.0f);

  // handle constant output range
  brute_const_range(0x00000000,xa, 0.5f);
  brute_const_range(0x80000000,xb, 0.5f);

  brute_force(xa,x1);
  brute_force(xb,x1 ^ 0x80000000);
}


// validating range that f(x)=1/2
void scan_half() {
  uint32_t ix = f32_to_bits(0x1.921fp-25f);
  float    r;

  do {
    float x = f32_from_bits(++ix);
    r = cr_acospif(x);
  } while(r == 0.5f);

  uint32_t x0 = ix-1;
  ix = f32_to_bits(-0x1.921fp-24f);
  
  do {
    float x = f32_from_bits(++ix);
    r = cr_acospif(x);
  } while(r == 0.5f);

  uint32_t x1 = ix-1;

  printf("f(x) = 1/2 on [%a,%a]\n", f32_from_bits(x0), f32_from_bits(x1));
}

int main(void)
{
  
#if 1
  brute_all();
#else
#if 0  
  brute_1pot_pn(1.f/32.f);
  brute_1pot_pn(1.f/16.f);
  brute_1pot_pn(1.f/ 8.f);
  brute_1pot_pn(1.f/ 4.f);
#endif  
  brute_1pot_pn(1.f/ 2.f);
#endif  

  error_dump();

  return 0;
}
