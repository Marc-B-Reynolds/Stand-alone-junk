// WIP

// for including a core-math function and suppressing the warnings. only
// handles GCC & clang like compilers ATM
// SEE: https://core-math.gitlabpages.inria.fr
//
// #define  CORE_MATH_FUNC sinpi
// #include "core_math_expand.h"

#if defined(__GNUC__)

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
#include STRINGIFY(CORE_MATH_FUNC)                                        
#pragma GCC diagnostic pop

#else
#include STRINGIFY(CORE_MATH_FUNC)                                        
#endif
