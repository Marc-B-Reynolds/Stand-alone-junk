// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_cbrtf) version:
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
#include "util.h"

//**********************************************************************

float libm(float x) { return cbrtf(x); }


//**********************************************************************


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

#define INEXACTFLAG 0

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;


float cr_cbrtf (float x){
  static const double escale[3] = {1.0, 0x1.428a2f98d728bp+0/* 2^(1/3) */, 0x1.965fea53d6e3dp+0/* 2^(2/3) */};
#if INEXACTFLAG!=0
  volatile uint32_t flag = _mm_getcsr(); /* store MXCSR Control/Status Register */
#endif
  b32u32_u cvt0 = {.f = x};
  uint32_t hx = cvt0.u, ix = 0x7fffffff&hx, e = ix>>23, mant = hx&0x7fffff;
  long sign = hx>>31;
  if(__builtin_expect(((e+1)&0xff)<2, 0)){
    if(e==0xff||ix==0) return x + x; /* 0, inf, nan */
    int nz = __builtin_clz(ix) - 8;  /* denormals */
    mant <<= nz;
    mant &= 0x7fffff;
    e -= nz - 1;
  }
  e += 899;
  b64u64_u cvt1 = {.u = (uint64_t)mant<<29|(0x3fful<<52)};
  uint32_t et = e/3, it = e%3;
  uint64_t isc = ((const uint64_t*)escale)[it];
  isc += (long)(et - 342)<<52;
  isc |= sign<<63;
  b64u64_u cvt2 = {.u = isc};
  static const double c[] = {0x1.1b0babccfef9cp-1, 0x1.2c9a3e94d1da5p-1, -0x1.4dc30b1a1ddbap-3, 0x1.7a8d3e4ec9b07p-6};
  const double u0 = 0x1.5555555555555p-2, u1 = 0x1.c71c71c71c71cp-3, u2 = 0x1.61f9add3c0ca4p-3;
  double z = cvt1.f, r = 1/z, z2 = z*z;
  double c0 = c[0] + z*c[1], c2 = c[2] + z*c[3], y = c0 + z2*c2, y2 = y*y;
  double w0 = y*u0, w1 = y*u1, w2 = y*u2;
  double h = y2*(y*r) - 1, h2 = h*h;
  y -= h*((w0 - w1*h) + w2*h2);
  y *= cvt2.f;
  b64u64_u cvt3 = {.f = y};
  float yf = y;
  long m0 = cvt3.u<<19, m1 = m0>>63;
  if(__builtin_expect((m0^m1)<(1l<<31),0)){
    b64u64_u cvt4 = {.u = (cvt3.u + (1ul<<31))&0xffffffff00000000ul};
    yf = cvt4.f;
#if INEXACTFLAG!=0
    _mm_setcsr(flag); /* restore MXCSR Control/Status Register for exact roots to get rid of the inexact flag if risen inside the function */
#endif
  }
  return yf;
}

#pragma GCC diagnostic pop

//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
};

const char* func_name = "cbrt";

float cr_func(float x) { return cr_cbrtf(x); }

#include "common.h"

//********************************************************

void test_spot()
{
}

void test_all()
{
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
