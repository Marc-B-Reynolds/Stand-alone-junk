// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// just "include every source file here" + "include this" and VOILA!
// SRLY: for no-build system (no configuration communication), multi-config + easy LTO.
// sure it's evil but (shrug) not a library.

// Done on purpose in some places..including everything will
// make it pop-up.
#if defined(__GNUC__)
#if defined(__clang__)
#else
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
#endif

#include "bmat_i.h"
#include "bmat_basics.c"
#include "bmat_set.c"
#include "bmat_toeplitz.c"
#include "bmat_block.c"
#include "bmat_gauss.c"
#include "bmat_charpoly.c"
#include "bmat_mul.c"
#include "bmat_transpose.c"
#include "bmat_random.c"
#include "bmat_func.c"

#include "bmat_ref.c"
#include "bmat_print.c"

#if defined(BMAT_M4RI_ENABLE)
#include "bmat_m4ri.c"
#endif

#if defined(BMAT_FLINT_ENABLE)
#include "bmat_flint.c"
#endif
