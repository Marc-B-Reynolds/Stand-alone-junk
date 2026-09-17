// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2010-2026
// Public Domain under http://unlicense.org, see link for details.

#pragma once
#define INTOPS_EXTRA_H

// binomial.c
extern void     binomial_init(void);
extern uint64_t binomial_u64(uint64_t n, uint64_t k);
extern uint64_t binomial_i64(int64_t  n, int64_t  k);

// prime.c
extern bool is_prime_u32(uint32_t n);
