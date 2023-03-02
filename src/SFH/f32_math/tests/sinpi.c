// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_sinpif) version:
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "internal/f32_sincospi.h"

#include "util.h"

//**********************************************************************

// doesn't exist yet
float libm(float x) { return sinf(((float)M_PI)*x); }


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

//#include <errno.h>  // MBR hack. LOL: errno
#include <fenv.h>

typedef union {float f; uint32_t u;} b32u32_u;

float cr_sinpif(float x){
  static const double sn[] = { 0x1.921fb54442d0fp-37, -0x1.4abbce6102b94p-112,  0x1.4669fa3c58463p-189};
  static const double cn[] = {-0x1.3bd3cc9be45cfp-74, 0x1.03c1f08088742p-150, -0x1.55d1e5eff55a5p-228};
  static const double S[] =
    {0x0p+0, 0x1.91f65f10dd814p-5, 0x1.917a6bc29b42cp-4, 0x1.2c8106e8e613ap-3, 0x1.8f8b83c69a60bp-3, 0x1.f19f97b215f1bp-3,
    0x1.294062ed59f06p-2, 0x1.58f9a75ab1fddp-2, 0x1.87de2a6aea963p-2, 0x1.b5d1009e15ccp-2, 0x1.e2b5d3806f63bp-2,
    0x1.073879922ffeep-1, 0x1.1c73b39ae68c8p-1, 0x1.30ff7fce17035p-1, 0x1.44cf325091dd6p-1, 0x1.57d69348cecap-1,
    0x1.6a09e667f3bcdp-1, 0x1.7b5df226aafafp-1, 0x1.8bc806b151741p-1, 0x1.9b3e047f38741p-1, 0x1.a9b66290ea1a3p-1,
    0x1.b728345196e3ep-1, 0x1.c38b2f180bdb1p-1, 0x1.ced7af43cc773p-1, 0x1.d906bcf328d46p-1, 0x1.e212104f686e5p-1,
    0x1.e9f4156c62ddap-1, 0x1.f0a7efb9230d7p-1, 0x1.f6297cff75cbp-1, 0x1.fa7557f08a517p-1, 0x1.fd88da3d12526p-1,
    0x1.ff621e3796d7ep-1,0x1p+0, 0x1.ff621e3796d7ep-1, 0x1.fd88da3d12526p-1, 0x1.fa7557f08a517p-1, 0x1.f6297cff75cbp-1,
    0x1.f0a7efb9230d7p-1, 0x1.e9f4156c62ddap-1, 0x1.e212104f686e5p-1, 0x1.d906bcf328d46p-1, 0x1.ced7af43cc773p-1,
    0x1.c38b2f180bdb1p-1, 0x1.b728345196e3ep-1, 0x1.a9b66290ea1a3p-1, 0x1.9b3e047f38741p-1, 0x1.8bc806b151741p-1,
    0x1.7b5df226aafafp-1, 0x1.6a09e667f3bcdp-1, 0x1.57d69348cecap-1, 0x1.44cf325091dd6p-1, 0x1.30ff7fce17035p-1,
    0x1.1c73b39ae68c8p-1, 0x1.073879922ffeep-1, 0x1.e2b5d3806f63bp-2, 0x1.b5d1009e15ccp-2, 0x1.87de2a6aea963p-2,
    0x1.58f9a75ab1fddp-2, 0x1.294062ed59f06p-2, 0x1.f19f97b215f1bp-3, 0x1.8f8b83c69a60bp-3, 0x1.2c8106e8e613ap-3,
    0x1.917a6bc29b42cp-4, 0x1.91f65f10dd814p-5,0x0p+0, -0x1.91f65f10dd814p-5, -0x1.917a6bc29b42cp-4, -0x1.2c8106e8e613ap-3,
    -0x1.8f8b83c69a60bp-3, -0x1.f19f97b215f1bp-3, -0x1.294062ed59f06p-2, -0x1.58f9a75ab1fddp-2, -0x1.87de2a6aea963p-2,
    -0x1.b5d1009e15ccp-2, -0x1.e2b5d3806f63bp-2, -0x1.073879922ffeep-1, -0x1.1c73b39ae68c8p-1, -0x1.30ff7fce17035p-1,
    -0x1.44cf325091dd6p-1, -0x1.57d69348cecap-1, -0x1.6a09e667f3bcdp-1, -0x1.7b5df226aafafp-1, -0x1.8bc806b151741p-1,
    -0x1.9b3e047f38741p-1, -0x1.a9b66290ea1a3p-1, -0x1.b728345196e3ep-1, -0x1.c38b2f180bdb1p-1, -0x1.ced7af43cc773p-1,
    -0x1.d906bcf328d46p-1, -0x1.e212104f686e5p-1, -0x1.e9f4156c62ddap-1, -0x1.f0a7efb9230d7p-1, -0x1.f6297cff75cbp-1,
    -0x1.fa7557f08a517p-1, -0x1.fd88da3d12526p-1, -0x1.ff621e3796d7ep-1,-0x1p+0, -0x1.ff621e3796d7ep-1,
    -0x1.fd88da3d12526p-1, -0x1.fa7557f08a517p-1, -0x1.f6297cff75cbp-1, -0x1.f0a7efb9230d7p-1, -0x1.e9f4156c62ddap-1,
    -0x1.e212104f686e5p-1, -0x1.d906bcf328d46p-1, -0x1.ced7af43cc773p-1, -0x1.c38b2f180bdb1p-1, -0x1.b728345196e3ep-1,
    -0x1.a9b66290ea1a3p-1, -0x1.9b3e047f38741p-1, -0x1.8bc806b151741p-1, -0x1.7b5df226aafafp-1, -0x1.6a09e667f3bcdp-1,
    -0x1.57d69348cecap-1, -0x1.44cf325091dd6p-1, -0x1.30ff7fce17035p-1, -0x1.1c73b39ae68c8p-1, -0x1.073879922ffeep-1,
    -0x1.e2b5d3806f63bp-2, -0x1.b5d1009e15ccp-2, -0x1.87de2a6aea963p-2, -0x1.58f9a75ab1fddp-2, -0x1.294062ed59f06p-2,
    -0x1.f19f97b215f1bp-3, -0x1.8f8b83c69a60bp-3, -0x1.2c8106e8e613ap-3, -0x1.917a6bc29b42cp-4, -0x1.91f65f10dd814p-5};

  b32u32_u ix = {.f = x};
  int32_t e = (ix.u>>23)&0xff;
  if(__builtin_expect(e == 0xff, 0)){
    if(!(ix.u << 9)){
      //errno = EDOM;                // MBR: once upon a time...there was no IEEE-754 and the people were sad
      //feraiseexcept (FE_INVALID);  // MBR: not needed for testing
      return __builtin_nanf("inf");
    }
    return x;
  }
  int32_t m = (ix.u&~0u>>9)|1<<23, sgn = ix.u; sgn >>= 31;
  m = (m^sgn) - sgn;
  int32_t s = 143 - e;
  if(__builtin_expect(s<0, 0)){
    if(__builtin_expect(s<-6, 0)) return __builtin_copysignf(0.0f, x);
    int32_t iq = m<<(-s-1);
    iq &= 127;
    if(iq==0||iq==64) return __builtin_copysignf(0.0f, x);
    return S[iq];
  } else if(__builtin_expect(s>30, 0)){
    double z = x, z2 = z*z;
    return z*(0x1.921fb54442d18p+1 + z2*(-0x1.4abbce625be53p+2));
  }
  int32_t si = 25 - s;
  if(__builtin_expect(si>=0&&(m<<si)==0,0)) return __builtin_copysignf(0.0f, x);

  int32_t k = m<<(31-s);
  double z = k, z2 = z*z;
  double fs = sn[0] + z2*(sn[1] + z2*sn[2]);
  double fc = cn[0] + z2*(cn[1] + z2*cn[2]);
  uint32_t iq = m>>s; iq = (iq + 1)>>1;
  uint32_t is = iq&127, ic = (iq + 32)&127;
  double ts = S[is], tc = S[ic];
  double r = ts + (ts*z2)*fc + (tc*z)*fs;
  return r;
}

#pragma GCC diagnostic pop

//********************************************************



func_entry_t func_table[] =
{
  ENTRY(libm),
  ENTRY(f32_sinpi_a5),
  ENTRY(f32_sinpi_k5),
  ENTRY(f32_sinpi_a6),
  ENTRY(f32_sinpi_o6),
  ENTRY(f32_sinpi_k6),
  ENTRY(f32_sinpi_k7),
  ENTRY(f32_sinpi_k8),
  ENTRY(f32_sinpi_d6),
};

const char* func_name = "sinpi";

float cr_func(float x) { return cr_sinpif(x); }

#include "common.h"

//********************************************************

void scan_linear() {
  uint32_t ix = 0;
  float    r,cr;

  do {
    float x = f32_from_bits(++ix);
    r = (float)f64_up_mul(&f64_mul_k_pi, (double)x);
    cr = cr_sinpif(x);
  } while(r == cr);

  ix--;

  printf("f(x) = pi*x up to %a %08x\n", f32_from_bits(ix),ix);
}

// up to 1/4
void test_spot()
{
  test_1pot(1.f/32.f);
  test_1pot(1.f/16.f);
  test_1pot(1.f/ 8.f);
}

void test_all()
{
  // sinpi(x) = RN(pi*x) up to 0x1.f82a6ep-24 0x33fc1537
  static const uint32_t cut = 0x33fc1537;
  test_linear_range_dp_up(0, cut, &f64_mul_k_pi);  
  test_force(cut+1, f32_to_bits(1.f/32.f));
  
  // partition the rest into POT intervals
  test_spot();
}

void test_sanity()
{
  test_sanity_odd();
}

int main(int argc, char** argv)
{
  return test_run(argc, argv);
}


