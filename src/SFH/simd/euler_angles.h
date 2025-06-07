// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2016-2025
// Public Domain under http://unlicense.org, see link for details.

// quick hack from old version. clean-up (maybe...it's euler angles)
// but anyway..some explict FMAs would be reasonable

// OLD NOTES:
// Tait-Bryan: yaw-pitch-roll (ZYX)
// storage of values in struct swap x&y given bit allocation
// strategy used in this file (not this file...old comment).
vec3f_t quatf_to_zyx(quat_t q)
{
  double x  = q[0], y=q[1], z=q[2], w=q[3];
  double t0 = (x+z)*(x-z);
  double t1 = (w+y)*(w-y);
  double xx = 0.5*(t0+t1);
  double xy = x*y+w*z;
  double xz = w*y-x*z;
  double yz = 2.0*(y*z+w*x);
  double t  = xx*xx+xy*xy;

  vec3f_t v;
  
  v[2] = (float)atan2(xy, xx);
  v[0] = (float)atan(xz/sqrt(t));
  
  if (t != 0)
    v[1] = (float)atan2(yz, t1-t0);
  else 
    v[1] = (float)(2.0*atan2(x,w) - sgnd(xz)*v->z);
}


quatf_t quatf_from_zyx(vec3f_t v)
{
  float hy = 0.5f*v[2];
  float hp = 0.5f*v[0];
  float hr = 0.5f*v[1];
  float ys = sinf(hy), yc = cosf(hy);
  float ps = sinf(hp), pc = cosf(hp);
  float rs = sinf(hr), rc = cosf(hr);

  quatf_t q;

  q->w = rc*pc*yc + rs*ps*ys;
  q->x = rs*pc*yc - rc*ps*ys;
  q->y = rc*ps*yc + rs*pc*ys;
  q->z = rc*pc*ys - rs*ps*yc;

  return q;
}

