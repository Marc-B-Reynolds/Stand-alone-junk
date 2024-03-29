#ifndef __COMMON_H__
#define __COMMON_H__

typedef struct {
  float    abs;
  uint32_t max;
  uint32_t ulp[4];
} func_error_t;

// tracks total error data across multiple intervals
func_error_t func_error[LENGTHOF(func_table)] = {{0}};

static const uint32_t f32_sign_bit_k = 0x80000000;

static inline uint32_t f32_ulp_dist(float a, float b)
{
  uint32_t ua = f32_to_bits(a);
  uint32_t ub = f32_to_bits(b);

  if ((int32_t)(ub^ua) >= 0)
    return u32_abs(ua-ub);
  
  return ua+ub+f32_sign_bit_k;
}

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

void error_dump()
{
  printf("\nTOTAL: %s\n", func_name);
  error_dump_i(func_error);
}


static inline void brute_error_add(func_error_t* error, float e, float a)
{
  float d = fabsf(e-a);

  if (d == 0.f) { error->ulp[0]++; return; };

  uint32_t ulp = f32_ulp_dist_ss(e,a);

  if (ulp > error->max) { error->max = ulp; }
  if (d   > error->abs) { error->abs = d;   }
  
  if (ulp > 3) ulp = 3;
  
  error->ulp[ulp]++;
}

// for when signs of e and a can be different
static inline void brute_error_add_ds(func_error_t* error, float e, float a)
{
  float d = fabsf(e-a);

  if (d == 0.f) { error->ulp[0]++; return; };

  uint32_t ulp = f32_ulp_dist(e,a);

  if (ulp > error->max) { error->max = ulp; }
  if (d   > error->abs) { error->abs = d;   }
  
  if (ulp > 3) ulp = 3;
  
  error->ulp[ulp]++;
}

void brute_force(uint32_t x0, uint32_t x1)
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
      brute_error_add(error+fi, cr,r);
    }
  }
  
  error_to_totals(error);
  error_dump_i(error);
}


// correctly rounded f(x) = k on [x0,x1]
void brute_const_range(uint32_t x0, uint32_t x1, float k)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};
  
  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {constant result range %a}\n", func_name, x0,x1,f0,f1,k);
  
  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x = f32_from_bits(xi);
      float r = func_table[fi].f(x);
      brute_error_add(error+fi,k,r);
    }
  }
  error_to_totals(error);
  error_dump_i(error);
}

// correctly rounded f(x) = x on [x0,x1]
void brute_identity_range(uint32_t x0, uint32_t x1)
{
  func_error_t error[LENGTHOF(func_table)] = {{0}};

  float f0 = f32_from_bits(x0);
  float f1 = f32_from_bits(x1);
  
  printf("\nchecking: %s on [%08x,%08x] [%e,%e] : {result = input range} \n", func_name, x0,x1,f0,f1);

  for(uint32_t fi=0; fi < LENGTHOF(func_table); fi++) {
    for(uint32_t xi=x0; xi<=x1; xi++) {
      float x = f32_from_bits(xi);
      float r = func_table[fi].f(x);
      brute_error_add(error+fi,x,r);
    }
  }

  error_to_totals(error);
  error_dump_i(error);
}

void brute_di()
{
  uint32_t x0 = f32_to_bits(0x1.0p-149f);
  uint32_t x1 = f32_to_bits(0x1.0p-126f);
  brute_force(x0,x1);
}

void brute_1pot(float x)
{
  uint32_t x0 = f32_to_bits(x);
  uint32_t x1 = f32_to_bits(2.f*x);
  brute_force(x0,x1);
}

void brute_1pot_pn(float x)
{
  uint32_t x0 = f32_to_bits(x);
  uint32_t x1 = f32_to_bits(2.f*x);
  brute_force(x0,x1);
  x0 = f32_to_bits(-x);
  x1 = f32_to_bits(-2.f*x);
  brute_force(x0,x1);
}



#endif
