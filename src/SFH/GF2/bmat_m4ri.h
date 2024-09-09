// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#include <m4ri/m4ri_config.h>
#include <m4ri/m4ri.h>

extern mzd_t* m4ri_alloc_8 (void);
extern mzd_t* m4ri_alloc_16(void);
extern mzd_t* m4ri_alloc_32(void);
extern mzd_t* m4ri_alloc_64(void);

extern void m4ri_free(mzd_t*);

extern void bmat_to_m4ri_8 (mzd_t*, bmat_param_8(m));
extern void bmat_to_m4ri_16(mzd_t*, bmat_param_16(m));
extern void bmat_to_m4ri_32(mzd_t*, bmat_param_32(m));
extern void bmat_to_m4ri_64(mzd_t*, bmat_param_64(m));

extern void bmat_from_m4ri_8 (bmat_param_8(m),  mzd_t*);
extern void bmat_from_m4ri_16(bmat_param_16(m), mzd_t*);
extern void bmat_from_m4ri_32(bmat_param_32(m), mzd_t*);
extern void bmat_from_m4ri_64(bmat_param_64(m), mzd_t*);

extern mzd_t* m4ri_make_8 (bmat_param_8 (s));
extern mzd_t* m4ri_make_16(bmat_param_16(s));
extern mzd_t* m4ri_make_32(bmat_param_32(s));
extern mzd_t* m4ri_make_64(bmat_param_64(s));
