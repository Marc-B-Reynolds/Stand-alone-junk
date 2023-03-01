// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2023

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "f32_util.h"
#include "internal/f32_math_common.h"

// cube root and reciprocal cube root

// SEE:
// * https://gist.github.com/Marc-B-Reynolds/739a46f55c2a9ead54f4d0629ee5e417
// * "Fast Calculation of Cube and Inverse Cube Roots Using a Magic Constant
//   and Its Implementation on Microcontrollers", Moroz, Samotyy, Walczyk,
//   Cieslinski, 2021 (algorithm 5 (A5) and algorithm 6)
//   https://www.mdpi.com/1996-1073/14/4/1058


