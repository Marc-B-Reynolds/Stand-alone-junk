// Marc B. Reynolds, 2010-2015
// Public Domain under http://unlicense.org, see link for details.
//
// Sobol (low-discrepancy) sequence in 1-3D, stratified in 2-4D.
// Classic binary-reflected gray code.
//
// SEE: http://web.maths.unsw.edu.au/~fkuo/sobol/
//

#ifndef SOBOL_H
#define SOBOL_H

// standard sequences
typedef struct { uint32_t i, d0; }       sobol_1d_t;
typedef struct { uint32_t i, d0,d1; }    sobol_2d_t;
typedef struct { uint32_t i, d0,d1,d2; } sobol_3d_t;

// stratified sequences: 1 dimenions lower
typedef struct { uint32_t i, d0;       float r; } sobol_fixed_2d_t;
typedef struct { uint32_t i, d0,d1;    float r; } sobol_fixed_3d_t;
typedef struct { uint32_t i, d0,d1,d2; float r; } sobol_fixed_4d_t;


#if !defined(_MSC_VER)
#define SOBOL_TO_F32 0x1p-24f
#define SOBOL_TO_F64 0x1p-32f

#else
#include <intrin.h>

#define SOBOL_TO_F32 (1.f/(1<<24))
#define SOBOL_TO_F64 (1.0/(1.0*(1<<24)*(1<<8)))

_inline uint32_t __builtin_ctz(uint32_t x) { unsigned long r; _BitScanForward(&r, (unsigned long)x); return (uint32_t)r; }

#endif

#ifndef SOBOL_NTZ
#define SOBOL_NTZ(X) __builtin_ctz(X)
#endif

#ifdef __cplusplus
#define SOBOL_EXTERN extern "C"
#ifdef  _MSC_VER
#define inline _inline
#endif
#else
#define SOBOL_EXTERN extern
#endif

#if defined(SOBOL_IMPLEMENTATION)

uint32_t sobol_table[] =
{
  0x80000000, 0x80000000,
  0xc0000000, 0xc0000000,
  0xa0000000, 0x60000000,
  0xf0000000, 0x90000000,
  0x88000000, 0xe8000000,
  0xcc000000, 0x5c000000,
  0xaa000000, 0x8e000000,
  0xff000000, 0xc5000000,
  0x80800000, 0x68800000,
  0xc0c00000, 0x9cc00000,
  0xa0a00000, 0xee600000,
  0xf0f00000, 0x55900000,
  0x88880000, 0x80680000,
  0xcccc0000, 0xc09c0000,
  0xaaaa0000, 0x60ee0000,
  0xffff0000, 0x90550000,
  0x80008000, 0xe8808000,
  0xc000c000, 0x5cc0c000,
  0xa000a000, 0x8e606000,
  0xf000f000, 0xc5909000,
  0x88008800, 0x6868e800,
  0xcc00cc00, 0x9c9c5c00,
  0xaa00aa00, 0xeeee8e00,
  0xff00ff00, 0x5555c500,
  0x80808080, 0x8000e880,
  0xc0c0c0c0, 0xc0005cc0,
  0xa0a0a0a0, 0x60008e60,
  0xf0f0f0f0, 0x9000c590,
  0x88888888, 0xe8006868,
  0xcccccccc, 0x5c009c9c,
  0xaaaaaaaa, 0x8e00eeee,
  0xffffffff, 0xc5005555
};

void sobol_1d_seek(sobol_1d_t* s, uint32_t off)
{
  uint32_t i  = s->i;
  uint32_t d0 = s->d0;
  uint32_t n  = i + off;
  uint32_t a  = i ^ (i >> 1);
  uint32_t b  = n ^ (n >> 1);
  uint32_t d  = a ^ b;
  uint32_t c  = 0x80000000;
  
  while(d) {
    if (d & 1) {
      d0 ^= c;
    }
    d >>= 1;
    c >>= 1;
  }
  
  s->i  = n;    
  s->d0 = d0;    
}


void sobol_2d_seek(sobol_2d_t* s, uint32_t off)
{
  uint32_t i  = s->i;
  uint32_t d0 = s->d0;
  uint32_t d1 = s->d1;
  uint32_t n  = i + off;
  uint32_t a  = i ^ (i >> 1);
  uint32_t b  = n ^ (n >> 1);
  uint32_t d  = a ^ b;
  uint32_t c  = 0;
  
  while(d) {
    if (d & 1) {
      d0 ^= 0x80000000 >> c;
      d1 ^= sobol_table[2*c];
    }
    d >>= 1;
    c  += 1;
  }
  
  s->i  = n;    
  s->d0 = d0;    
  s->d1 = d1;    
}

void sobol_2d_seek(sobol_2d_t* s, uint32_t off)
{
  uint32_t i  = s->i;
  uint32_t d0 = s->d0;
  uint32_t d1 = s->d1;
  uint32_t d2 = s->d2;
  uint32_t n  = i + off;
  uint32_t a  = i ^ (i >> 1);
  uint32_t b  = n ^ (n >> 1);
  uint32_t d  = a ^ b;
  uint32_t c  = 0;
  
  while(d) {
    if (d & 1) {
      d0 ^= 0x80000000 >> c;
      d1 ^= sobol_table[2*c  ];
      d2 ^= sobol_table[2*c+1];
    }
    d >>= 1;
    c  += 1;
  }
  
  s->i  = n;    
  s->d0 = d0;    
  s->d1 = d1;    
  s->d2 = d2;    
}

#else

extern uint32_t* sobol_table;
SOBOL_EXTERN void sobol_1d_seek(sobol_1d_t* s, uint32_t off);
SOBOL_EXTERN void sobol_2d_seek(sobol_2d_t* s, uint32_t off);
SOBOL_EXTERN void sobol_3d_seek(sobol_3d_t* s, uint32_t off);

#endif


inline void sobol_1d_init(sobol_1d_t* s, uint32_t hash)
{
  s->d0 = hash;
  s->i  = 0;
}

inline void sobol_2d_init(sobol_2d_t* s, uint32_t hash0, uint32_t hash1)
{
  s->d0 = hash0;
  s->d1 = hash1;
  s->i  = 0;
}

inline void sobol_3d_init(sobol_3d_t* s, uint32_t hash0, uint32_t hash1, uint32_t hash2)
{
  s->d0 = hash0;
  s->d1 = hash1;
  s->d2 = hash2;
  s->i  = 0;
}

inline void sobol_fixed_2d_init(sobol_fixed_2d_t* s, uint32_t len, uint32_t hash)
{
  sobol_1d_init((sobol_1d_t*)s, hash);
  s->r = 1.f/len;
}

inline void sobol_fixed_3d_init(sobol_fixed_3d_t* s, uint32_t len, uint32_t hash0, uint32_t hash1)
{
  sobol_2d_init((sobol_2d_t*)s, hash0, hash1);
  s->r = 1.f/len;
}

inline void sobol_fixed_4d_init(sobol_fixed_3d_t* s, uint32_t len, uint32_t hash0, uint32_t hash1, uint32_t hash2)
{
  sobol_3d_init((sobol_3d_t*)s, hash0, hash1, hash2);
  s->r = 1.f/len;
}


// state updates - normally not needed by user.

inline void sobol_1d_update(sobol_1d_t* s)
{
  uint32_t c = SOBOL_NTZ(~(s->i));
  s->d0 ^= 0x80000000 >> c;
  s->i  += 1;    
}

inline void sobol_2d_update(sobol_2d_t* s)
{
  uint32_t c = SOBOL_NTZ(~(s->i));
  s->d0 ^= 0x80000000 >> c;
  s->d1 ^= sobol_table[2*c];
  s->i  += 1;    
}

inline void sobol_3d_update(sobol_3d_t* s)
{
  uint32_t c = SOBOL_NTZ(~(s->i));
  s->d0 ^= 0x80000000 >> c;
  s->d1 ^= sobol_table[2*c];
  s->d2 ^= sobol_table[2*c+1];
  s->i  += 1;    
}


// next float in the sequence
inline float sobol_1d_next_f32(sobol_1d_t* s)
{
  float r = (s->d0 >> 8) * SOBOL_TO_F32;

  sobol_1d_update(s);

  return r;
}

// next 2 floats in the sequence
inline void sobol_2d_next_f32(sobol_2d_t* s, float* d)
{
  d[0] = (s->d0 >> 8) * SOBOL_TO_F32;
  d[1] = (s->d1 >> 8) * SOBOL_TO_F32;

  sobol_2d_update(s);
}

// next 3 floats in the sequence
inline void sobol_3d_next_f32(sobol_3d_t* s, float* d)
{
  d[0] = (s->d0 >> 8) * SOBOL_TO_F32;
  d[1] = (s->d1 >> 8) * SOBOL_TO_F32;
  d[2] = (s->d2 >> 8) * SOBOL_TO_F32;

  sobol_3d_update(s);
}

inline double sobol_1d_next_f64(sobol_1d_t* s)
{
  double r = s->d0 * SOBOL_TO_F64;

  sobol_1d_update(s);

  return r;
}

inline void sobol_2d_next_f64(sobol_2d_t* s, double* d)
{
  d[0] = s->d0 * SOBOL_TO_F64;
  d[1] = s->d1 * SOBOL_TO_F64;

  sobol_2d_update(s);
}

inline void sobol_3d_next_f64(sobol_3d_t* s, double* d)
{
  d[0] = s->d0 * SOBOL_TO_F64;
  d[1] = s->d1 * SOBOL_TO_F64;
  d[2] = s->d2 * SOBOL_TO_F64;

  sobol_3d_update(s);
}

//** 

inline void sobol_fixed_2d_next_f32(sobol_fixed_2d_t* s, float* d)
{
  uint32_t i = s->i;

  d[0] = (s->d0 >> 8) * SOBOL_TO_F32;
  d[1] = (s->r * i);

  sobol_1d_update((sobol_1d_t*)s);

  s->i = i++;
}

inline void sobol_fixed_3d_next_f32(sobol_fixed_3d_t* s, float* d)
{
  uint32_t i = s->i;

  d[0] = (s->d0 >> 8) * SOBOL_TO_F32;
  d[1] = (s->d1 >> 8) * SOBOL_TO_F32;
  d[2] = (s->r * i);

  sobol_2d_update((sobol_2d_t*)s);

  s->i = i++;
}

inline void sobol_fixed_4d_next_f32(sobol_fixed_4d_t* s, float* d)
{
  uint32_t i = s->i;

  d[0] = (s->d0 >> 8) * SOBOL_TO_F32;
  d[1] = (s->d1 >> 8) * SOBOL_TO_F32;
  d[2] = (s->d1 >> 8) * SOBOL_TO_F32;
  d[3] = (s->r * i);

  sobol_3d_update((sobol_3d_t*)s);

  s->i = i++;
}

inline void sobol_fixed_2d_next_f64(sobol_fixed_2d_t* s, double* d)
{
  uint32_t i = s->i;

  d[0] = s->d0 * SOBOL_TO_F64;
  d[1] = s->r  * i;

  sobol_1d_update((sobol_1d_t*)s);

  s->i = i++;
}

inline void sobol_fixed_3d_next_f64(sobol_fixed_3d_t* s, double* d)
{
  uint32_t i = s->i;

  d[0] = s->d0 * SOBOL_TO_F64;
  d[1] = s->d1 * SOBOL_TO_F64;
  d[2] = s->r  * i;

  sobol_2d_update((sobol_2d_t*)s);

  s->i = i++;
}

inline void sobol_fixed_4d_next_f64(sobol_fixed_4d_t* s, double* d)
{
  uint32_t i = s->i;

  d[0] = s->d0 * SOBOL_TO_F64;
  d[1] = s->d1 * SOBOL_TO_F64;
  d[2] = s->d1 * SOBOL_TO_F64;
  d[3] = s->r  * i;

  sobol_3d_update((sobol_3d_t*)s);

  s->i = i++;
}

// position in the sequence
inline uint32_t sobol_tell(void* s)
{
  return ((sobol_1d_t*)s)->i;
}



#endif
