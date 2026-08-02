// goofying around spot checking

// no implementation ATM headers
#include "sfh/compiler_hints.h"

#include "sfh/bitops.h"
#include "sfh/bitops_small.h"
#include "sfh/bitset.h"

#include "sfh/f32_horner.h"
#include "sfh/f64_horner.h"

#include "sfh/f32_quant.h"

#include "sfh/f64_pair.h"

//#include "sfh/shf_macro.h"

#include "sfh/swar.h"

#include "sfh/vec2.h"
#include "sfh/vec3.h"
#include "sfh/quat.h"

#include "welford.h"

// with implementation headers
#define CARRYLESS_IMPLEMENTATION
#define F32_UTIL_IMPLEMENTATION
#define F64_UTIL_IMPLEMENTATION
#define F64_UTIL_IMPLEMENTATION
#define INTOPS_IMPLEMENTATION
#define SOBOL_IMPLEMENTATION
#define SWAR_AVX2_IMPLEMENTATION

#include "sfh/intops.h"
#include "sfh/f32_util.h"
#include "sfh/f64_util.h"
#include "sfh/carryless.h"
#include "sfh/swar_avx2.h"

#include "sfh/sfibpoints.h"
#include "sfh/Sobol.h"

#if !defined(_MSC_VER)
// use extended C features not in MSC. no workaround
#define  SIMD_IMPLEMENTATION
#include "sfh/simd.h"
#include "sfh/simd_2d3d.h"
#endif
