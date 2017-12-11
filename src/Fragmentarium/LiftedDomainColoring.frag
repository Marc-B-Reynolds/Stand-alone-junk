// public domain (unlicence)

// Pole (magnitude) indications is borked. Haven't
// found anything super happy with. 

#include "2D.frag"
//#include "Progressive2D.frag"
#group DomainPlot

// coloring tweekable constants

uniform bool SMOOTH_HSV; checkbox[true]

// enable/disable growth indicators
uniform bool GROW_IND; checkbox[true]

// growth indication constant (>=0). larger is less pronounced.
uniform float GROW_K; slider[0.0,0.9,1.0]

// grid line scale (radial & concentric)
uniform float GRID_K; slider[0.0,0.01,0.2]

// enable/disable radial grid lines
uniform bool RADIAL_GRID; checkbox[true]

// number of radial lines
uniform int RADIAL_LINES;  slider[1,32,64]

// radial: post blended in RGB if defined, otherwise
// via saturation.
uniform bool RADIAL_GRID_POST; checkbox[false]
const vec3 rg_color = vec3(1.0,1.0,1.0);

// limit ouput to magnitude range [0,1]
uniform bool UNIT_ONLY; checkbox[false]

// blend value for radial lines (0,1) 0=strong, 1=none
uniform float RADIAL_LINE_STR; slider[0.0,0.001,1.0]

// concentric grid lines: shown if defined
uniform bool CONCENTRIC_GRID; checkbox[true]

// concentric post blended in RGB if defined, otherwise
// via brightness
uniform bool CONCENTRIC_GRID_POST; checkbox[false]

uniform bool RECT_GRID; checkbox[false]

uniform float RECT_GRID_STR; slider[0.0,0.55,1.0]

#define RECT_GRID_S

// concentric grid color
const vec3 cg_color = vec3(0.0,0.0,0.0);

// rotate hue angle (choose what's zero)
uniform float HUE_ANGLE; slider[0.0,0.55,1.0]

// if defined: darken as point approach zero (south pole)
// and brighten as approachs infinity (north pole)
#define POLE_SHADING

// if defined: show a rectangle grid. Number of per unit distance.
//#define R_GRID 2.0


#define E  2.71828182845
vec2 epowz(vec2 c) 
{
	return vec2(cos(c.y), sin(c.y))*pow(E, c.x);   
}

// the map to visualize
vec2 map(vec2 z)
{
  return z;
}


//----------------------

// derived constants
#define GROW_M  (1.0/(GROW_K+1.0))
#define GRID_IK (1.0/GRID_K)
#define PI (52707180.0/16777216.0)
#define RL (float(RADIAL_LINES)/(2.0*PI))

vec3 hsv2rgb(vec3 c)
{
  const vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// Fabrice Neyret's smoothed conversion
vec3 fn_hsv2rgb(vec3 c)
{
  return c.z*(1.-c.y*smoothstep(2.,1.,abs(mod(c.x*6.+vec3(0,4,2),6.) -3.)));
}

float grid_step(float t)
{
  t = min(t, 1.0-t);
  return smoothstep(0.0, GRID_K, t);
}

// lifted domain plot color for point 'c'
vec3 domainPlot(vec2 c)
{
  float d     = dot(c,c);
  float l2d   = log2(d); 
  float theta = atan(c.y,c.x);
  
  // growth indication
  float b1 = fract(0.5*l2d);   // frac part of log2(||c||)
  float b0 = b1+b1;
  
  if (b0 >= 1.0) b0 -= 1.0;
    
  float b = GROW_M*(b0+GROW_K);
    
  // concentric grid covers branch-point
  if (b1 < 0.5) b = 1.0;    
    
  if (!GROW_IND) b = 1.0;

  // saturation and value
  float sat = 1.0;
  float val = 1.0;
  
  // magnitude indication: needs work
  if (d < 1.0) {
    val = 2.0/(1.0-log2(d));
    val = 1.0;
  // val = sqrt(2.0*atan(1.0/sqrt(d)));
  }
  else if (d > 2.0) {
    sat = 2.0/l2d;
  //sat = sqrt(2.0*atan(1.0/sqrt(d)));
  }
  
  // rectangular grid
  if (RECT_GRID ){
    float x = floor(c.x);
    float y = floor(c.y);
    if (mod(x+y,2.0)==1.0)  sat*=RECT_GRID_STR;
  }
    
  // radial grid (integrated)
  if  (RADIAL_GRID && !RADIAL_GRID_POST) {
    sat = mix(RADIAL_LINE_STR, sat, grid_step(fract(theta*RL)));   
  }
  
  // concentric grid (integrated)
if (CONCENTRIC_GRID && !CONCENTRIC_GRID_POST) {
   b = mix(0.0, b, grid_step(b0));
}
  
  // convert to RGB
  float hue = theta*(1.0/(2.0*PI))-HUE_ANGLE;
  vec3  v;

  if  (SMOOTH_HSV)
    v  =fn_hsv2rgb(vec3(hue,sat,val));
  else
   v =  hsv2rgb(vec3(hue,sat,val));
    
  v *= b;

  // radial grid (as post effect)
  if  (RADIAL_GRID && RADIAL_GRID_POST) {
    v = mix(rg_color, v, grid_step(fract(theta*RL)));
  }
   
  // concentric grid (as post effect)
  if (CONCENTRIC_GRID && CONCENTRIC_GRID_POST) {
    v = mix(cg_color, v, grid_step(b0));
  } 
    
  return v;
}

vec3 color(vec2 z)
{ 
  if (UNIT_ONLY && dot(z,z)>1.0) return vec3(0.0);
 
   return domainPlot(map(z));
}

#preset Default
Center = 0,0
Zoom = 1
AntiAliasScale = 2
AntiAlias = 15
SMOOTH_HSV = true
GROW_IND = true
GROW_K = 0.9
GRID_K = 0.01
RADIAL_GRID = true
RADIAL_GRID_POST = false
RADIAL_LINES = 32
UNIT_ONLY = false
RADIAL_LINE_STR = 0.001
CONCENTRIC_GRID = true
CONCENTRIC_GRID_POST = false
RECT_GRID = false
RECT_GRID_STR = 0.55
HUE_ANGLE = 0
#endpreset