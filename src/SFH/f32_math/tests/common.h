#ifndef __COMMON_H__
#define __COMMON_H__

typedef struct {
  float    abs;
  uint32_t max;
  uint32_t ulp[4];
} func_error_t;

// tracks total error data across multiple intervals
func_error_t func_error[LENGTHOF(func_table)] = {{0}};


// add error data from current interval to the totals
void error_to_totals(func_error_t* e)
{
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++, e++) {
    func_error[fi].ulp[0] += e->ulp[0];
    func_error[fi].ulp[1] += e->ulp[1];
    func_error[fi].ulp[2] += e->ulp[2];
    func_error[fi].ulp[3] += e->ulp[3];

    if (e->max > func_error[fi].max) func_error[fi].max = e->max;
    if (e->abs > func_error[fi].abs) func_error[fi].abs = e->abs;
  }
}

void error_dump_i(func_error_t* e)
{
  printf("|%15s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%12s|\n",
	 "func", "max ULP", "CR", "FR", "2 ULP", "> 2 ULP",
	 "CR%", "FR%", "2 ULP%","> 2 ULP%", "abs");

  printf("|%15s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%12s|\n",
	 "---:", "---:", "---:", "---:", "---:", "---:",
	 "---:", "---:", "---:","---:", "---:");

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++, e++) {
    uint32_t u0 = e->ulp[0];
    uint32_t u1 = e->ulp[1];
    uint32_t u2 = e->ulp[2];
    uint32_t u3 = e->ulp[3];
    uint32_t t  = (u0+u1+u2+u3);
    double   s  = 100.0/(double)t;

    printf("|%15s|%10u|%10u|%10u|%10u|%10u|%10f|%10f|%10f|%10f|%e|\n",
	   func_table[fi].name, e->max,
	   u0, u1, u2, u3,
	   s*u0, s*u1, s*u2, s*u3, e->abs
	   );
  }
}

void error_dump(void)
{
  printf("\nTOTAL: %s\n", func_name);
  error_dump_i(func_error);
}

// mostly usless ATM. need to add flags to func
const f32_pair_t test_vector_odd_data[] ={
  {.x= 0.f,     .y= 0.f},
  {.x=-0.f,     .y=-0.f},
//{.x= f32_inf, .y= f32_inf},
//{.x=-f32_inf, .y=-f32_inf}
};

const f32_pair_t test_vector_even_data[] ={
  {.x= 0.f,     .y= 0.f},
  {.x=-0.f,     .y= 0.f},
//{.x= f32_inf, .y= f32_inf},
//{.x=-f32_inf, .y= f32_inf}
};



static inline void test_error_add(func_error_t* error, float e, float a)
{
  float d = fabsf(e-a);

  if (d == 0.f) { error->ulp[0]++; return; };

  uint32_t ulp = f32_ulp_dist(e,a);

  if (ulp > error->max) { error->max = ulp; }
  if (d   > error->abs) { error->abs = d;   }
  
  if (ulp > 3) ulp = 3;
  
  error->ulp[ulp]++;
}

// for when signs of e and a can (reasonably) be different
static inline void test_error_add_ds(func_error_t* error, float e, float a)
{
  float d = fabsf(e-a);

  if (d == 0.f) { error->ulp[0]++; return; };

  uint32_t ulp = f32_ulp_dist(e,a);

  if (ulp > error->max) { error->max = ulp; }
  if (d   > error->abs) { error->abs = d;   }
  
  if (ulp > 3) ulp = 3;
  
  error->ulp[ulp]++;
}

void test_force(uint32_t x0, uint32_t x1)
{
  if (x1 < x0) { uint32_t t = x0; x0=x1; x1=t; }

  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e]\n", func_name, x0,x1,f0,f1);

  func_error_t error[LENGTHOF(func_table)] = {{0}};
  
  for(uint32_t ix=x0; ix<=x1; ix++) {
    float x  = f32_from_bits(ix);
    float cr = cr_func(x);
    
    for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
      float r = func_table[fi].f(x);
      test_error_add(error+fi, cr,r);
    }
  }
  
  error_to_totals(error);
  error_dump_i(error);
}


// correctly rounded f(x) = RN(kx) on [x0,x1]
void test_linear_range(uint32_t x0, uint32_t x1, float k)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};
  
  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {linear result range %a*x}\n", func_name, x0,x1,f0,f1,k);
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x  = f32_from_bits(xi);
      float r  = func_table[fi].f(x);
      float cr = k*x;
      test_error_add(error+fi,cr,r);
    }
  }
  error_to_totals(error);
  error_dump_i(error);
}


// correctly rounded f(x) = k on [x0,x1]
void test_const_range(uint32_t x0, uint32_t x1, float k)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};
  
  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);

  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {constant result range %a}\n", func_name, x0,x1,f0,f1,k);
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x = f32_from_bits(xi);
      float r = func_table[fi].f(x);
      test_error_add(error+fi,k,r);
    }
  }
  error_to_totals(error);
  error_dump_i(error);
}

// correctly rounded f(x) = RN(RN(kx,53),24) on [x0,x1]
void test_linear_range_dp_up(uint32_t x0, uint32_t x1, const f64_pair_t k)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};
  
  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {linear result range}\n", func_name, x0,x1,f0,f1);
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x  = f32_from_bits(xi);
      float r  = func_table[fi].f(x);
      float cr = (float)f64_up_mul(k, (double)x);
      test_error_add(error+fi,cr,r);
    }
  }
  error_to_totals(error);
  error_dump_i(error);
}

// correctly rounded f(x) = x on [x0,x1]
void test_identity_range(uint32_t x0, uint32_t x1)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};

  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {result = input range}\n", func_name, x0,x1,f0,f1);

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x = f32_from_bits(xi);
      float r = func_table[fi].f(x);
      test_error_add(error+fi,x,r);
    }
  }

  error_to_totals(error);
  error_dump_i(error);
}

// wip: broken. only first being recorded...what?
void test_sample_positive_finite(uint32_t count)
{
  printf("\nchecking: %s via LDS sampling of positive finite (n=%08x)\n", func_name, count);

  func_error_t error[LENGTHOF(func_table)] = {{0}};

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    // use an additive recurrence to cover the range cheaply
    static const uint32_t A = 2654435769;
    uint32_t u = A;
    uint32_t n = count;

    while (n > 0) {
      // skip infinites and nans
      if ((u & 0x7f800000) != 0x7f800000) {
	uint32_t ix = u;
	float    x  = f32_from_bits(ix);
	float    r  = func_table[fi].f(x);
	float    cr = cr_func(x);
	test_error_add(error+fi, cr,r);
	n--;
      }
      u += A;
    }
  }
  
  error_to_totals(error);
  error_dump_i(error);
}

// test all denormal inputs
void test_di(void)
{
  uint32_t x0 = f32_to_bits(0x1.0p-149f);
  uint32_t x1 = f32_to_bits(0x1.0p-126f)-1;
  test_force(x0,x1);
}

// test a full power-of-two interval starting from x. (need not start on a boundary)
void test_1pot(float x)
{
  uint32_t x0 = f32_to_bits(x);
  uint32_t x1 = f32_to_bits(2.f*x)-1;
  test_force(x0,x1);
}

// test a full power-of-two interval starting from x. (need not start on a boundary)
// [x,2x) & (-2x,-x]
void test_1pot_pn(float x)
{
  uint32_t x0 = f32_to_bits(x);
  uint32_t x1 = f32_to_bits(2.f*x)-1;
  test_force(x0,x1);
  x0 = f32_to_bits(-x);
  x1 = f32_to_bits(-2.f*x);
  test_force(x0,x1);
}


void test_sanity_nan(void)
{
  float x = 0.f/0.f;
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    float r0 = func_table[fi].f( x);
    
    if (r0 == r0) {
      printf("  %s : FAILED NaN-> f(%a) = %a\n",
	     func_table[fi].name, x,r0);
      break;
    }
  }
}


void test_vector(const f32_pair_t* data, uint32_t n)
{
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for (uint32_t i=0; i<n; i++) {
      float x  = data[i].x;
      float r0 = func_table[fi].f(x);
      float r1 = data[i].y;
      
      if (!(r0 == r1)) {
	if ((r0 == r0)||(r1 == r1)) {
	  printf(" * %10s : f(%a) = %a expected %a (%08x %08x)\n",
		 func_table[fi].name, x,r0,r1, f32_to_bits(r0), f32_to_bits(r1));
	}
      }
    }
  }
}

static inline void test_specials(void)
{
  static const float specials[] =
  {
    0.f, -0.f, f32_nan, -f32_nan, f32_inf, -f32_inf
  };

  for (uint32_t i=0; i<LENGTHOF(specials); i++) {
    float    x  = specials[i];
    float    cr = cr_func(x);
    uint32_t ic = f32_to_bits(cr);
    
    for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
      float    r  = func_table[fi].f(x);
      uint32_t ir = f32_to_bits(r);

      if (ir != ic) {
	if ((r == r)||(cr == cr)) {
	    printf(" * %10s : f(%a) = %a expected %a (%08x %08x)\n",
		   func_table[fi].name, x, r, cr, ir,ic);
	}
      }
    }
  }
}




// for functions that expect f(-x) = -f(x) {exactly}
//   expects {and note about range once reworked}
void test_sanity_odd(void)
{
  test_vector(test_vector_odd_data, LENGTHOF(test_vector_odd_data));

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    
    for(float x=0.f; x <= 1.f; x += (1.f/1024.f)) {
      float r0 = func_table[fi].f( x);
      float r1 = func_table[fi].f(-x);
      
      if (r0 != -r1) {
	printf("  %s : FAILED ODD -> f(%a) = %a, f(-%a)=%a\n",
	       func_table[fi].name, x,r0,x,r1);
	break;
      }
    }
  }
}


// for functions that expect f(-x) = f(x) {exactly}
//   expects {and note about range once reworked}
void test_sanity_even(void)
{
  test_vector(test_vector_even_data, LENGTHOF(test_vector_even_data));
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    
    for(float x=0.f; x <= 1.f; x += (1.f/1024.f)) {
      float r0 = func_table[fi].f( x);
      float r1 = func_table[fi].f(-x);
      
      if (r0 != r1) {
	printf("  %s : FAILED EVEN -> f(%a) = %a, f(-%a)=%a\n",
	       func_table[fi].name, x,r0,x,r1);
	break;
      }
    }
  }
}

#include <unistd.h>
#include <time.h>
#include "timehack.h"

void test_all(void);
void test_spot(void);
void test_sanity(void);

int test_run(int argc, char** argv)
{
  bool lol    = false;
  bool all    = true;
  bool sanity = true;

  // hacky command line options
  int  sid    = 1;
  
  while (argc > sid) {
    if (argv[sid][0] == '-') {
      switch(argv[sid][1]) {
      case 's' : all = false; break;
      case 't' : lol = true;  break;
	
      default  :
	fprintf(stderr, "error: unknown option %c\n", argv[sid][1]);
	return -1;
      }
    }
    else {
      // here to add limiting functions tested
      printf("skipping: %s\n", argv[sid]);
    }

    sid++;
  }

  if (lol) {
    timing_test(func_table, LENGTHOF(func_table));
    return 0;
  }
  // add range reduction & special input related sanity test here
  
  //
  
  printf("<details markdown=\"1\"><summary>click for range breakdown</summary>\n\n");

  if (sanity) {
    printf("\nrunning: minimal sanity check\n");
    //test_sanity_nan();
#if !defined(NO_TEST_SPECIALS)    
    test_specials();
#endif    
    test_sanity();
  }
  
  if (all)
    test_all();
  else
    test_spot();

  printf("\n</details>\n\n");

  // dump summary
  error_dump();

  return 0;
}


#endif
