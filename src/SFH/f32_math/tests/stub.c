// -*- coding: utf-8 -*-
// Public Domain under http://unlicense.org, see link for details.
//
//

#if defined(__linux__) || defined(__unix__)
// for thread affinity
#define _GNU_SOURCE 
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "util.h"


// testing test uniform range
//#define SAMPLE_X0 -1.f
//#define SAMPLE_X1  1.f


//────────────────────────────────────────────────────────────────────────────────────

float libm(float x) { return 0; }


//────────────────────────────────────────────────────────────────────────────────────


//────────────────────────────────────────────────────────────────────────────────────
// SEE: https://core-math.gitlabpages.inria.fr for licence

#include "core_math_expand.h"


#pragma GCC diagnostic pop
//────────────────────────────────────────────────────────────────────────────────────


func_entry_t func_table[] =
{
  ENTRY(libm),
};

const char* func_name = "lol";

float cr_func(float x) { return 0.f; }

#include "common.h"


//────────────────────────────────────────────────────────────────────────────────────

// move to common.h

void test_spot(void)
{
}

void test_all(void)
{
}

void test_sanity(void)
{
}

int main(int argc, char** argv)
{
  printf("empty stub\n");
  //return test_run(argc, argv);
}
