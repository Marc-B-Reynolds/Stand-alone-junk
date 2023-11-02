// ALL WIP AND PROBABLY BROKEN...needs testing

typedef float (f32_horner_t)(float, const float[]);


// compute: c_0 + x(c_1 + x*P(x)) with c_1 as a pair (H,L)
//   C = array of coefficients: {c_0, L, H} | { c_n .. c_2 (hi to lo)}
//   P = function to evaluate P
static inline float f32_hornerx_full(float x, const float* const C, f32_horner_t* P)
{
  float r;

  r = P(x,         C+3);  // P(x)
  r = fma(r,x,    C[1]);  // RN(x*P(x) + L)
  r = fma(x,r,    C[0]);  // RN(x*RN(x*P(x) + L) + c_0)
  r = fma(x,C[2], r);     // RN(x*H + RN(x*RN(x*P(x) + L) + c_0)
                          // x*H + x^2*P(x) + x*L + c_0
  return r;               // c_0 + x(c_1 + x*P(x))
}

//  compute: c_0 + x^2(c_2 + x^2*P(x^2)) with c_2 as pair (H,L)
//  C = {c_0, L, H} { c_n .. c_4 (hi to lo) }
static inline float f32_hornerx_even(float x, float s, const float* const C, f32_horner_t* P)
{
  float r;

  r = P(s,          C+3);  // P(s)
  r = fmaf(r,s,    C[1]);  // RN(s*P(s) + L)
  r = fmaf(x,C[2], x*r);   // RN(x*H + RN(x*RN(s*P(s) + L)))
  r = fmaf(x,r,    C[0]);  // RN(x*RN(x*H + RN(x*RN(s*P(s) + L))) + c_0)
                           // x^2*H + x^2*(s*P(s) + L) + c_0
  return r;                // c_0 + x^2(c_2 + x^2*P(x^2))
}


// same as 'f32_hornerx_even' but removes one product
static inline float f32_hornerx_even_alt(float x, float s, const float* const C, f32_horner_t* horner)
{
  float r;

  r = horner(s,    C+3);   // P(s)
  r = fmaf(s,r,    C[1]);  // RN(s*P(s) + L)
  r = fmaf(s,r,    C[0]);  // RN(s*RN(s*P(s) + L) + c_0)
  r = fmaf(s,C[2], r);     // RN(s*H + RN(s*RN(s*P(s) + L) + c_0))
                           // s*H + s*(s*P(s) + L) + c_0
  return r;                // c_0 + x^2(c_2 + x^2*P(x^2))
}


//  compute: x(c_1 + x^2*P(x^2)) with c_1 as pair (H,L)
//  C   = {L, H} | { remaining: high to low}
static inline float f32_hornerx_odd(float x, float s, const float* const C, f32_horner_t* horner)
{
  float r;

  r = horner(s, C+2);      // P(s) 
  r = fmaf(r,s,C[0]);      // RN(s*P(s) + L)
  r = x*r;                 // RN(x*RN(s*P(s) + L))
  r = fmaf(x, C[1], r);    // RN(x*H + RN(x*RN(s*P(s) + L)))
                           // x*H + x^3*P(x^2) + x*L
  return r;                // x(c_1 + x^2*P(x^2))
}
