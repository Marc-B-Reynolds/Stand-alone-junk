# -*- coding: utf-8 -*-
"""
Hacked together.  I know zero python, so this is cut-n-paste city.

1) Couldn't figure out how to show pixel exact output, so this is kinda
useless as is.
2) It's probably buggy


"""

from scipy import fftpack
import numpy as np
import matplotlib.pyplot as py
import scipy.stats as stats
import math as math

showSpace = True
showPSE   = True

# size to sample
half = 256
size = 2*half

# sampling domain offset
offX = -half
offY = -half
 

# For white noise reference 
class XorShiftRng(object):
  def __init__(self, state):
    self.state = np.uint64(state)

  def next(self):
    self.state ^= self.state << np.uint64(21)
    self.state ^= self.state >> np.uint64(35)
    self.state ^= self.state << np.uint64(4)
    return np.uint64(self.state) 
 
xorshift = XorShiftRng(1) 
 
 # uint types to binary32
def u64tosp(v): return float(np.uint64(v) >> np.uint64(40))*2**(-24)
def u32tosp(v): return float(np.uint32(v) >> np.uint32(8))*2**(-24)

# I'm sure this is stupid.
def mask32(v):  return np.uint32(v & 0xFFFFFFFF)
def mul32(a,b): return mask32(a*b)
def add32(a,b): return mask32(a+b) 

# xorshift
def xs(v,s): 
    return v ^ (v >> np.uint32(s))
# xorshift - multiply
def xsm(v,s,m): 
    v = xs(v,s)
    return mul32(m, v)

# multiply - xorshift
def mxs(v,m,s): 
    v = mul32(m, v)
    return xs(v,s)
 
def wang32(seed):
    seed = (seed ^ np.uint32(61)) ^ (seed >> np.uint32(16))
    seed = mxs(seed, 9, 4)
    seed = mxs(seed, 0x27d4eb2d, 15)   
    return seed
 
 # White noise on [0,1)
def whiteNoise(x,y): return u64tosp(xorshift.next()) 

# As per: 
# http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/
# except perform 2D->1D 
def wang(x,y): return u32tosp(wang32(add32(y,wang32(x))))

# https://github.com/ashima/webgl-noise
def mod289(x):
    return x - math.floor(x * (1.0/289.0))*289.0

def permute(x):
    return mod289(((x*34.0)+1.0)*x)
    
def ppoly(x,y):
    return permute(y+permute(x))

# Murmurhash2 bit finalizer 
def mh2bf(x):
    x = xsm(x, 0x5bd1e995, 13)
    x = xs(x, 15)
    return x

# Murmurhash2 step
def mh2s(x):
    x = mxs(x, 0x5bd1e995, 24)
    return mul32(x, 0x5bd1e995)

# Murmurhash2 add x & y to h
def mh2(h,x,y):
    x = mh2s(x)
    y = mh2s(y)
    h ^= x
    h = mul32(h, 0x5bd1e995)
    h ^= y
    return h;

def mh2_std(x,y):
    h = mh2(0x9747b28c,x,y)
    return mh2bf(h)


# http://marc-b-reynolds.github.io/math/2016/03/29/weyl_hash.html
def weyl(x,y):
    x  = mul32(x, 0x3504f333)
    y  = mul32(y, 0xf1bbcdcb)
    x ^= y;
    x  = mul32(x, 741103597)
    return u32tosp(x)

useSubPlot = False

def showSPE(name, func):
    
    # THIS IS COMPLETELY FUBARed: Can't figure out out to show
    # pixel exact figures.
    foo = 2.0*float(size)/96.0;
    py.figure(figsize=(foo,2*(foo+10)), dpi=96) 
   
    samples = np.ones((size, size), dtype=np.float64)
    
    for y in range(size):
      for x in range(size):
        samples[y][x] = func(x-offX, y-offY)
    
    fft1 = fftpack.fft2(samples)        # 
    fft  = fftpack.fftshift(fft1)
    pse  = np.abs(fft)**2

    #if (useSubPlot) 
    py.subplot(1,2,1) 
    #py.figure(figsize=(foo,2*(foo+10)), dpi=96)
    py.axis("off") 
    py.title(name)
    py.xlabel("space")
    py.imshow(samples, cmap=py.cm.Greys, interpolation="nearest")

    py.subplot(1,2,2)
    #py.figure(figsize=(foo,2*(foo+10)), dpi=96)
    py.axis("off") 
    py.imshow(np.log10(pse), cmap=py.cm.Greys, interpolation="nearest")
    py.xlabel("PSE")
    
    #py.hist2d(np.log10(pse), bins=100)
    py.show()
    
    
def showSPE_(name, func):
    
    # THIS IS COMPLETELY FUBARed: Can't figure out out to show
    # pixel exact figures.
    foo = 5.0*float(size)/96.0;
    fig = py.figure(figsize=(foo,2*foo), dpi=96) 
    samples = np.ones((size, size), dtype=np.float64)
    
    for y in range(size):
      for x in range(size):
        samples[y][x] = func(x-offX, y-offY)
    
    fft1 = fftpack.fft2(samples)        # 
    fft  = fftpack.fftshift(fft1)
    pse  = np.abs(fft)**2

    py.subplot(1,2,1)
    py.title(name)
    py.xlabel("space")
    py.imshow(samples, cmap=py.cm.Greys, interpolation="nearest")

    py.subplot(1,2,2)
    py.imshow(np.log10(pse), cmap=py.cm.Greys, interpolation="nearest")
    py.xlabel("PSE")
    #py.hist2d(np.log10(pse), bins=100)
    py.show()
 
showSPE("White noise", whiteNoise)
showSPE("ppoly(y+ppoly(x))", ppoly)
showSPE("wang(y+wang(x))",     wang)
showSPE("M(weyl(W0,x)^weyl(W1,y))", weyl)
showSPE("M(weyl(W0,x)^weyl(W1,y))", mh2_std)


