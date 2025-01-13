// Marc B. Reynolds, 2017-2025
// Public Domain under http://unlicense.org, see link for details.
//
// This is utter junk example code for some posts.
// quick cut/paste/mod so errors are likely
//
// basic swing/twist factoring and recomposition
// factor:    quat_to_{swing_twist|twist_swing}_{a}{_hq}?_n?
// recompose: {swing_twist|twist_swing}_{a}_to_quat{_hq}?
//   swing_twist = swing then twist 
//   twist_swing = twist then swing
//   a  = axis of twist
//   hq = uses Kahan's ab+cd (1 mul, 2 fma, 1 add) - minimal number of funcs, unlikely to be needed
//   n  = normalize 'w' variant
//
// https://marc-b-reynolds.github.io/quaternions/2017/05/12/HopfCoordConvert.html
// https://marc-b-reynolds.github.io/quaternions/2022/01/31/QuatAxisFactor.html

#ifndef SWING_TWIST_H
#define SWING_TWIST_H

typedef struct {
  float sx,sy,sc;   // swing: sin*(x,y) in plane & cos
  float tc,ts;      // twist: cos & sin about
} swing_twist_t;


//**********************************************************
// factor Q into swing then twist with respect to Z (Q = Qs Qt)
// 
// All the other variants just expand this. At least
// the current clang/gcc (13.0.1/11.2) versions fully expand.

static inline void quat_to_swing_twist_z(swing_twist_t* d, quat_t* q)
{
  static const float THRESHOLD = f32_ulp1;
  
  float x=q->x, y=q->y, z=q->z, w=q->w;
  float t=f32_sqrt(w*w+z*z);
  
  if (t > THRESHOLD) {
    float s = 1.f/t;
    d->sx = s*(w*x-y*z);
    d->sy = s*(w*y+x*z);
    d->sc =   t;
    d->ts = s*z;
    d->tc = s*w;
  }
  else {
    d->sx = x;
    d->sy = y;
    d->sc = 0;
    d->ts = 0;
    d->tc = 1.f;
  }
}

// normalize 'w' to positive. output param 'sgn' stores sign of 'w' for if restore in needed
static inline void quat_to_swing_twist_z_n(swing_twist_t* d, quat_t* q, uint32_t* sgn)
{
  static const float THRESHOLD = f32_ulp1;
  
  float x=q->x, y=q->y, z=q->z, w=q->w;
  
  float    t  = f32_sqrt(w*w+z*z);
  uint32_t sw = 0;

  if (t > THRESHOLD) {
    sw = f32_sign_bit(w);

    float s0 = 1.f/t;
    float s1 = f32_from_bits(f32_to_bits(s0) | sw);

    d->sx = s0*(w*x-y*z);
    d->sy = s0*(w*y+x*z);
    d->sc =   t;
    d->ts = s1*z;
    d->tc = s1*w;
  }
  else {
    d->sx = x;
    d->sy = y;
    d->sc = 0;
    d->ts = 0;
    d->tc = 1.f;
  }
  
  *sgn = sw;
}

static inline void quat_to_swing_twist_z_hq(swing_twist_t* d, quat_t* q)
{
  static const float THRESHOLD = f32_ulp1;
  
  float x=q->x, y=q->y, z=q->z, w=q->w;

  // probably not: no loss of signif. explict contraction though
  float t=f32_sqrt(f32_mma(w,w,z,z));   // mma here is useless
  
  if (t > THRESHOLD) {
    float s = 1.f/t;
    d->sx = s*f32_mms(w,x,y,z);
    d->sy = s*f32_mma(w,y,x,z);
    d->sc =   t;
    d->ts = s*z;
    d->tc = s*w;
  }
  else {
    d->sx = x;
    d->sy = y;
    d->sc = 0;
    d->ts = 0;
    d->tc = 1.f;
  }
}

//**********************************************************
// twist about 'z' inverse transforms

static inline void swing_twist_z_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  float sx=s->sx, sy=s->sy, sc=s->sc;
  float ts=s->ts, tc=s->tc;
  
  tc = f32_from_bits(f32_to_bits(tc) | sw);
  ts = f32_from_bits(f32_to_bits(ts) ^ sw);

  quat_set(q, sx*tc+sy*ts, sy*tc-sx*ts, sc*ts, sc*tc);
}

static inline void swing_twist_z_to_quat(quat_t* q, swing_twist_t* s)
{
  // clang/gcc eliminate so just expand
  swing_twist_z_to_quat_n(q,s,0);
}

static inline void swing_twist_z_to_quat_hq(quat_t* q, swing_twist_t* s)
{
  float sx=s->sx, sy=s->sy, sc=s->sc;
  float ts=s->ts, tc=s->tc;
  
  q->x = f32_mma(sx,tc,sy,ts);
  q->y = f32_mms(sy,tc,sx,ts);
  q->z = sc*ts;
  q->w = sc*tc;
}


//**********************************************************
// expand forward transforms for 'x' & 'y'.

static inline void quat_to_swing_twist_x(swing_twist_t* d, quat_t* q)
{
  quat_t t;
  quat_map_x2z(&t,q);
  quat_to_swing_twist_z(d, &t);
}  

static inline void quat_to_swing_twist_y(swing_twist_t* d, quat_t* q)
{
  quat_t t;
  quat_map_y2z(&t,q);
  quat_to_swing_twist_z(d, &t);
}  

static inline void quat_to_swing_twist_x_n(swing_twist_t* d, quat_t* q, uint32_t* sw)
{
  quat_t t;
  quat_map_x2z(&t,q);
  quat_to_swing_twist_z_n(d, &t, sw);
}  

static inline void quat_to_swing_twist_y_n(swing_twist_t* d, quat_t* q, uint32_t* sw)
{
  quat_t t;
  quat_map_y2z(&t,q);
  quat_to_swing_twist_z_n(d, &t, sw);
}  


//**********************************************************
// expand inverse transforms for 'x' & 'y'.

static inline void swing_twist_x_to_quat(quat_t* q, swing_twist_t* s)
{
  swing_twist_z_to_quat(q,s);
  quat_map_z2x(q,q);
}

static inline void swing_twist_y_to_quat(quat_t* q, swing_twist_t* s)
{
  swing_twist_z_to_quat(q,s);
  quat_map_z2y(q,q);
}

static inline void swing_twist_x_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  swing_twist_z_to_quat_n(q,s,sw);
  quat_map_z2x(q,q);
}

static inline void swing_twist_y_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  swing_twist_z_to_quat_n(q,s,sw);
  quat_map_z2y(q,q);
}


//**********************************************************
// forward "reverse order" transforms (Q = Qt Qs)

static inline void quat_to_twist_swing_z(swing_twist_t* d, quat_t* q)
{
  QUAT_DEF_SET(t, q->x, q->y, -q->z, q->w);
  quat_to_swing_twist_z(d, &t);
  d->ts = -d->ts;
}  

static inline void quat_to_twist_swing_z_n(swing_twist_t* d, quat_t* q, uint32_t* sw)
{
  QUAT_DEF_SET(t, q->x, q->y, -q->z, q->w);
  quat_to_swing_twist_z_n(d, &t, sw);
  d->ts = -d->ts;
}  

static inline void quat_to_twist_swing_x(swing_twist_t* d, quat_t* q)
{
  quat_t t;
  quat_map_x2z(&t,q);
  quat_to_twist_swing_z(d, &t);
}  

static inline void quat_to_twist_swing_y(swing_twist_t* d, quat_t* q)
{
  quat_t t;
  quat_map_y2z(&t,q);
  quat_to_twist_swing_z(d, &t);
}  

static inline void quat_to_twist_swing_x_n(swing_twist_t* d, quat_t* q, uint32_t* sw)
{
  quat_t t;
  quat_map_x2z(&t,q);
  quat_to_twist_swing_z_n(d, &t, sw);
}  

static inline void quat_to_twist_swing_y_n(swing_twist_t* d, quat_t* q, uint32_t* sw)
{
  quat_t t;
  quat_map_y2z(&t,q);
  quat_to_twist_swing_z_n(d, &t, sw);
}  


//**********************************************************
// inverse "reverse order" transforms (Q = Qt Qs)

static inline void twist_swing_z_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  float sx=s->sx, sy=s->sy, sc=s->sc;
  float ts=s->ts, tc=s->tc;

  tc = f32_from_bits(f32_to_bits(tc) | sw);
  ts = f32_from_bits(f32_to_bits(ts) ^ sw);

  quat_set(q, sx*tc-sy*ts, sy*tc+sx*ts, sc*ts, sc*tc);
}

static inline void twist_swing_z_to_quat(quat_t* q, swing_twist_t* s)
{
  // clang/gcc eliminate so just expand
  twist_swing_z_to_quat_n(q,s,0);
}

static inline void twist_swing_x_to_quat(quat_t* q, swing_twist_t* s)
{
  twist_swing_z_to_quat(q,s);
  quat_map_z2x(q,q);
}

static inline void twist_swing_y_to_quat(quat_t* q, swing_twist_t* s)
{
  twist_swing_z_to_quat(q,s);
  quat_map_z2y(q,q);
}

static inline void twist_swing_x_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  twist_swing_z_to_quat_n(q,s,sw);
  quat_map_z2x(q,q);
}

static inline void twist_swing_y_to_quat_n(quat_t* q, swing_twist_t* s, uint32_t sw)
{
  twist_swing_z_to_quat_n(q,s,sw);
  quat_map_z2y(q,q);
}

#endif
