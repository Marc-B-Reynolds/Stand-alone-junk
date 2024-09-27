# testing stuff

One file is one executable thing going on. These evil little things just include all the source (via. bmat_everything.h) and lto.

| file               | description |
| --------           |-------------|
| bmat_test.c        | tests basic functions against (internal) reference versions  |
| bmat_test_m4ri.c   | tests (internal) reference versions against [M4RI](https://github.com/malb/m4ri) |
| bmat_test_kernel.c | tests kernel and fixed point                     |
| bmat_timing.c      | completely janky timing test of some functions   |
| ---                | ---                                              |
| dev_*.c            | internal dev testing hacks                       |




