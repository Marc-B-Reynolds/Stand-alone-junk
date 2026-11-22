// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#include "SFH/compiler_hints.h"
#include "SFH/f32_util.h"


// SEE:
// * 

//#include "SFH/f32_math/internal/f32_math_common.h"
//#include "SFH/f32_math/internal/f32_asincos.h"


//────────────────────────────────────────────────────────────────────────────────────
// Hastings 1955 style full domain [-1,1] asin approximations for abs error:
// except forcing the constant term of P so f(±0) & f(±1) are exact
// (except f(-0) = +0 instead of -0)
//
// positive 'x':
//   π/2 + sqrt(1-x)P(x)
//   P(x) = c₀x - π/2      (for a0. one extra term added for each)
// and fold through by product with sign(x) to handle
// negative x inputs (and x's become |x|).
//
// ∙ notice the core is a transformed approximation of acos
// ∙ produces no small magnitude (other than zero) numbers (no denormals).
//
//       abs-error (full report in f32_math/test/reports/asin.md)
//   a0  4.261762e-03
//   a1  4.024506e-04
//   a2  4.506111e-05
//   a3  5.662441e-06
//   a4  8.940697e-07
//   a5  2.980232e-07

hint_small_hot
float f32_asin_a0(float x)
{
  static const float c0 = 0.17539402770089185f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);   // sign(x) * π/2
  float a = fabsf(x);                        // |x|
  float t = sqrtf(1.f-a);                    // sqrt(1-|x|)
  float p = fmaf(x,c0,-k);                   // P(x)
  
  return fmaf(t,p,k);
}

hint_small_hot
float f32_asin_a1(float x)
{
  static const float c1 = -0.05344910528338329f;
  static const float c0 =  0.20744683027439512f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,c1,c0),-k);

  return fmaf(t,p,k);
}

hint_small_hot
float f32_asin_a2(float x)
{
  static const float c2 =  0.021641525571976804f;
  static const float c1 = -0.07798149543591731f;
  static const float c0 =  0.21330134383396135f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,c2,c1),c0),-k);

  return fmaf(t,p,k);
}

hint_small_hot
float f32_asin_a3(float x)
{
  static const float c3 = -0.010044210004718978f; 
  static const float c2 =  0.03822427750032387f;
  static const float c1 = -0.0860213463845701f;
  static const float c0 =  0.2143665611826344f;
  
  float k = f32_mul_by_sign(f32_pi/2.f,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,c3,c2),c1),c0),-k);

  return fmaf(t,p,k);
}

hint_small_hot
float f32_asin_a4(float x)
{
  static const float c4 =  0x1.4bd458p-8f;
  static const float c3 = -0x1.57f016p-6f;
  static const float c2 =  0x1.7ae748p-5f;
  static const float c1 = -0x1.69a5acp-4f;
  static const float c0 =  0x1.b76b9ap-3f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,fmaf(a,c4,c3),c2),c1),c0),-k);

  return fmaf(t,p,k);
}

hint_small_hot
float f32_asin_a5(float x)
{
  static const float c5 = -0x1.64fd5ap-9f;
  static const float c4 =  0x1.941a0ap-7f;
  static const float c3 = -0x1.cce15cp-6f;
  static const float c2 =  0x1.949beap-5f;
  static const float c1 = -0x1.6c11d2p-4f;
  static const float c0 =  0x1.b77e22p-3f;
  
  float h = f32_pi/2.f;
  float k = f32_mul_by_sign(h,x);
  float a = fabsf(x);
  float t = sqrtf(1.f-a);
  float p = fmaf(x,fmaf(a,fmaf(a,fmaf(a,fmaf(a,fmaf(a,c5,c4),c3),c2),c1),c0),-k);

  return fmaf(t,p,k);
}


//────────────────────────────────────────────────────────────────────────────────────
// Hastings style abs-error approximations of acos. Simple computes π/2-asin(x)
// As noted in the asin approximations..they are transformed acos so this could be
// worked back in. However this leads to additional operations: computing both acos(±0)
// correctly introduces more work.
//
//       abs-error (full report in f32_math/test/reports/acos.md)
//   a0  4.261971e-03
//   a1  4.026890e-04
//   a2  4.529953e-05
//   a3  5.722046e-06
//   a4  9.536743e-07
//   a5  4.768372e-07

hint_small_hot hint_flatten float f32_acos_a0(float x) { return f32_pi/2.f - f32_asin_a0(x); }
hint_small_hot hint_flatten float f32_acos_a1(float x) { return f32_pi/2.f - f32_asin_a1(x); }
hint_small_hot hint_flatten float f32_acos_a2(float x) { return f32_pi/2.f - f32_asin_a2(x); }
hint_small_hot hint_flatten float f32_acos_a3(float x) { return f32_pi/2.f - f32_asin_a3(x); }
hint_small_hot hint_flatten float f32_acos_a4(float x) { return f32_pi/2.f - f32_asin_a4(x); }
hint_small_hot hint_flatten float f32_acos_a5(float x) { return f32_pi/2.f - f32_asin_a5(x); }
