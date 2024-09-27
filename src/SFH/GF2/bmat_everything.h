// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

// just "include every source file here" + "include this" and VOILA!
// SRLY: for no-build system (no configuration communication), multi-config + easy LTO.
// sure it's evil but (shrug) not a library.

#include "bmat_i.h"
#include "bmat_basics.c"
#include "bmat_set.c"
#include "bmat_toeplitz.c"
#include "bmat_block.c"
#include "bmat_gauss.c"
#include "bmat_mul.c"
#include "bmat_transpose.c"
#include "bmat_random.c"
#include "bmat_func.c"

#include "bmat_ref.c"
#include "bmat_m4ri.c"
#include "bmat_print.c"
