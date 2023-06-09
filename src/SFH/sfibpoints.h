// Marc B. Reynolds, 2018-2023
// Public Domain under http://unlicense.org, see link for details.

#ifndef SF_WALK_H
#define SF_WALK_H

// Spherical Fibonacci point set for testing
// http://marc-b-reynolds.github.io/math/2018/06/21/SFPoints4ET.html

// requires: stdint.h, math.h (for fma & sqrt) and vec3_t defined

// constant turning rate: 
//   TX = cos(2pi K)
//   TY = sin(2pi K) 
//   K  = frac(phi) = 1/phi = (sqrt(5)-1)/2

#if defined(_MSC_VER)
static const double SF_WALK_TX = -0.73736887807831985597317725478205829858779907226562;
static const double SF_WALK_TY = -0.67549029426152362720614519275841303169727325439453;
#else
static const double SF_WALK_TX = -0x1.798869e0de834p-1;
static const double SF_WALK_TY = -0x1.59d9dd253cc11p-1;
#endif

typedef struct {
  double z,dz;    // incremental height on cap
  double x0,y0;   // incremental point on unit circle p(n)
  double x1,y1;   //   and p(n-1)
} sf_walk_t;

// internal worker init routine
static inline void sf_walk_zone_init_i(sf_walk_t* w, uint64_t n, double z0, double z1)
{
  w->x0 = 1.0;
  w->y0 = 0.0;
  w->x1 =  SF_WALK_TX;
  w->y1 = -SF_WALK_TY;
  w->z  = z0;
  w->dz = (z0-z1)/(double)n;
}

// spherical zone walk
// n  = number of points to generate
// z0 = first z coordinate of zone
// z1 = last  z coordinate of zone
static inline void sf_walk_zone_init(sf_walk_t* w, uint64_t n, float z0, float z1)
{
  sf_walk_zone_init_i(w,n,(double)z0,(double)z1);
}

// spherical cap walk (center of cap at +Z)
// n = number of points to generate
// h = height of cap (ex: half-sphere=1, full-sphere=2)
static inline void sf_walk_cap_init(sf_walk_t* w, uint64_t n, float h)
{
  sf_walk_zone_init_i(w, n, 1.0, 1.0-(double)h);
}

// full sphere walk
// n = number of points to generate
static inline void sf_walk_init(sf_walk_t* w, uint32_t n)
{
  sf_walk_cap_init(w, n, 2.f);
}

// return the next point in the set
static inline vec3_t sf_walk_next(sf_walk_t* w)
{
  static const double M = 2.0*SF_WALK_TX;
  
  double x0=w->x0, y0=w->y0;
  double x1=w->x1, y1=w->y1;
  
  double ct,st;

  // current cylinder to cap mapping values
  ct   = w->z; 
  st   = sqrt((1.0-ct)*(1.0+ct));
  
  // output current point in set
  vec3_t v;
  
  v.x = (float)(st*x0);
  v.y = (float)(st*y0);
  v.z = (float)(ct);
  
  // update point on circle: turn by 2pi*K
  // via reverse Goertzel Algorithm
  w->x0 = fma(M, x0, -x1);
  w->y0 = fma(M, y0, -y1);
  w->x1 = x0;
  w->y1 = y0;

  // update height in cap position
  w->z -= w->dz;

  return v;
}

#endif
