
#include <stdio.h>

#define SOBOL_IMPLEMENTATION
#include "../sobol.h"

#define LEN 100

int error(int spos, int cpos)
{
  fprintf(stderr, "opps (%d,%d)\n",spos,cpos);
  return -1;
}

int main(int argc, char** argv)
{
  sobol_1d_t s1p;
  sobol_2d_t s2p;
  sobol_3d_t s3p;
  sobol_fixed_2d_t s2f;
  sobol_fixed_3d_t s3f;
  sobol_fixed_4d_t s4f;

  argc = argc;
  argv = argv;

  uint32_t i;
  float    f1;
  float    f2[2];
  float    f3[3];
  float    f4[4];

  sobol_1d_init(&s1p, 0);
  sobol_2d_init(&s2p, 0,0);
  sobol_3d_init(&s3p, 0,0,0);
  sobol_fixed_2d_init(&s2f, LEN, 0);
  sobol_fixed_3d_init(&s3f, LEN, 0,0);
  sobol_fixed_4d_init(&s4f, LEN, 0,0,0);


  for(i=0; i<LEN; i++) {
    f1 = sobol_1d_next_f32(&s1p);
    sobol_2d_next_f32(&s2p, f2);
    sobol_3d_next_f32(&s3p, f3);

    if (i < 22) {
      printf("{%f,%f,%f},\n",f3[0],f3[1],f3[2]);
    }

    if (f1 != f2[0] || f1 != f3[0]) { return error(i,0); }

    sobol_fixed_2d_next_f32(&s2f, f2);
    sobol_fixed_3d_next_f32(&s3f, f3);
    sobol_fixed_4d_next_f32(&s4f, f4);
  }
  
  return 0;
}
