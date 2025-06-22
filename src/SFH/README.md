# Public domain single header (and not) file libraries

http://unlicense.org/



High level overview.  `?` indicates if it's really single header functionality.


| **file**      | **description**                                  | ? |
| :-------      |:-------------------------------------------------|---|
| bitops.h      |                                                  | ✓ |
| bitset.h      | minimal bitvector ops                            | ✓ |
| carryless.h   | $\mathbb{F}_2$ operations                        |   |
| f32_horner.h  | FMA horner's method helpers                      | ✓ |
| f32_horner2.h | FMA horner's method helpers (order 2)            | ✓ |
| f32_util.h    |                                                  |   |
| f32_quant.h   | scalar uniform quantization                      | ✓ |
| f64_horner.h  | FMA horner's method helpers                      | ✓ |
| f64_horner2.h | FMA horner's method helpers (order 2)            | ✓ |
| f64_util.h    |                                                  |   |
| f64_pair.h    | "pair" extended precision methods                | ✓ |
| intops.h      |                                                  |   |
| lcgs.h        | random access pseudo random numbers (LCG based)  |   |
| prns.h        | random access pseudo random numbers (Weyl based) |   |
| Sobol.h       | sobol sequences                                  | ✓ |
| quat.h        |                                                  |   |
| sfibpoints.h  | Spherical Fibbonacci points (for test set)       | ✓ |
| simd.h        |                                                  | ✓ |
| simd_2d3d.h   |                                                  | ✓ |
| swar.h        |                                                  |   |
| f64_util.h    |                                                  |   |
| int_util.h    |                                                  |   |
| vec2.h        |                                                  |   |
| vec3.h        |                                                  |   |
| welford.h     | streaming computation of mean, variance, std dev | ✓ |



| **directory** | **description**                                   |
| :-------      |:--------------------------------------------------|
|  doc          | normally there's be some documentation            | 
|  examples     | ditto but for examples                            | 
|  GF2          | small square matrices of $\mathbb{F}_2$           | 
|  report       |                                                   | 
|  simd         | extra functionality for `simd.h` and `simd2d3d.h` | 
