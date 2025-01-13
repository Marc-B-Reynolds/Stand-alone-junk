// Public Domain under http://unlicense.org, see link for details.
// Marc B. Reynolds, 2022-2025

#include "internal/f32_math_common.h"
#include "f32_utils.h"

// SEE:
// * 

// stripped out testing output of subset of range reduction/kernel combinations
//
// asinpi (classic construction): 
//   | K |RR|ULP|   CR     |     FR   |    2 ULP |   >2 ULP |     CR%  |      FR% |   2 ULP% |  >2 ULP% |
//   |k5 |x0|  4|1032035044|  31570400|   1537527|    210246| 96.872570|  2.963374|  0.144321|  0.019735|
//   |k5 |x1|  3|1032156173|  31586667|   1610375|         2| 96.883940|  2.964901|  0.151159|  0.000000|
//   |k5 |x2|  3|1040648342|  23095092|   1609781|         2| 97.681063|  2.167834|  0.151103|  0.000000|
//   |k6 |x0|  2|1052761039|  12552051|     40127|         0| 98.818028|  1.178206|  0.003767|  0.000000|
//   |k6 |x1|  1|1055324695|  10028522|         0|         0| 99.058667|  0.941333|  0.000000|  0.000000|
//   |k6 |x2|  1|1063825073|   1528144|         0|         0| 99.856560|  0.143440|  0.000000|  0.000000|
//   |k7 |x0|  2|1053857208|  11461428|     34581|         0| 98.920920|  1.075834|  0.003246|  0.000000|
//   |k7 |x1|  1|1056653721|   8699496|         0|         0| 99.183417|  0.816583|  0.000000|  0.000000|
//   |k7 |x2|  1|1065206869|    146348|         0|         0| 99.986263|  0.013737|  0.000000|  0.000000|
//   |k8 |x0|  2|1053891035|  11427570|     34612|         0| 98.924096|  1.072656|  0.003249|  0.000000|
//   |k8 |x1|  1|1056690373|   8662844|         0|         0| 99.186857|  0.813143|  0.000000|  0.000000|
//   |k8 |x2|  1|1065251470|    101747|         0|         0| 99.990449|  0.009551|  0.000000|  0.000000|
//
// asinpi (abs error "spitballs")
//   | K | ~abs error |
//   |a0 |1.356564e-03|
//   |a1 |1.281016e-04|
//   |a2 |1.433492e-05|
//   |a3 |1.795590e-06|
//   |a4 |2.682209e-07|
//   |a5 |7.450581e-08|  correct round abs error ~= 1.490116e-08
//
// cospi: (an = abs error "spitballs" kernels)
//   | K |    ULP   |   CR     |     FR   |    2 ULP |   >2 ULP |     CR%  |      FR% |   2 ULP% |  >2 ULP% | ~abs error |
//   | a0|   1672415|1763516917|  32348103|  11562178| 323279238| 82.766771|  1.518187|  0.542645| 15.172397|2.108824e-02|
//   | a1|    223084|1788031508|  46773361|  12366869| 283534698| 83.917309|  2.195204|  0.580412| 13.307075|1.356602e-03|
//   | a2|     30588|1820812274|  55051337|  12702512| 242140313| 85.455802|  2.583713|  0.596164| 11.364321|1.281500e-04|
//   | a3|      4487|1852015093|  65150815|  12284275| 201256253| 86.920237|  3.057710|  0.576535|  9.445518|1.436472e-05|
//   | a4|       681|1884039738|  76053538|  13127479| 157485681| 88.423243|  3.569405|  0.616109|  7.391243|1.847744e-06|
//   | a5|       107|1919663150|  95488708|  25661795|  89892783| 90.095150|  4.481552|  1.204380|  4.218919|2.980232e-07|
//   | d7|         1|2063138893|  67567543|         0|         0| 96.828867|  3.171133|  0.000000|  0.000000|5.960464e-08|
//   | d8|         1|2121232268|   9474168|         0|         0| 99.555351|  0.444649|  0.000000|  0.000000|5.960464e-08|


float f32_asinpi(float x) { f32_asinpi_x2(&f32_asinpi_k6, x); }
